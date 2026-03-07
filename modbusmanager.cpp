#include "modbusmanager.h"
#include "modbusmodel.h"
#include <QModbusReply>
#include <QModbusDataUnit>
#include <cstdint>
#include <cstring>

static QVector<uint16_t> packFloatLowWordFirst(float f)
{
    uint32_t r = 0;
    std::memcpy(&r, &f, sizeof(float));
    return {static_cast<uint16_t>(r & 0xFFFFu), static_cast<uint16_t>(r >> 16)};
}

static float unpackFloatLowWordFirst(uint16_t low, uint16_t high)
{
    uint32_t r = (uint32_t(high) << 16) | low;
    float f = 0.0f;
    std::memcpy(&f, &r, sizeof(float));
    return f;
}

static QVector<uint16_t> packDWordLowWordFirst(quint32 v)
{
    return {static_cast<uint16_t>(v & 0xFFFFu), static_cast<uint16_t>(v >> 16)};
}

static quint32 unpackDWordLowWordFirst(uint16_t low, uint16_t high)
{
    return (quint32(high) << 16) | low;
}

ModbusManager::ModbusManager(ModbusModel *model, QObject *parent) : QObject(parent), m_model(model) {
    m_client = new QModbusTcpClient(this);
    m_client->setTimeout(5000);

    connect(m_client, &QModbusClient::stateChanged, this, &ModbusManager::onClientStateChanged);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &ModbusManager::tryReconnect);
}

void ModbusManager::setUnitId(int unitId)
{
    if (unitId < 1)
        unitId = 1;
    if (unitId > 247)
        unitId = 247;
    m_unitId = unitId;
}

void ModbusManager::setRequestTimeoutMs(int ms)
{
    if (ms < 500)
        ms = 500;
    m_client->setTimeout(ms);
}

void ModbusManager::setReconnectIntervalMs(int ms)
{
    if (ms < 2000)
        ms = 2000;
    m_reconnectIntervalMs = ms;
}

void ModbusManager::connectTo(const QString &ip, int port) {
    m_connectionIp = ip;
    m_connectionPort = port;
    if (m_client->state() != QModbusDevice::UnconnectedState)
        m_client->disconnectDevice();
    m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    m_client->connectDevice();
}

void ModbusManager::onClientStateChanged(QModbusDevice::State state)
{
    switch (state) {
    case QModbusDevice::UnconnectedState:
        emit connectionStateChanged(tr("Отключено"));
        m_reconnectTimer->start(m_reconnectIntervalMs);
        break;
    case QModbusDevice::ConnectingState:
        emit connectionStateChanged(tr("Подключение..."));
        m_reconnectTimer->stop();
        break;
    case QModbusDevice::ConnectedState:
        emit connectionStateChanged(tr("Подключено"));
        m_reconnectTimer->stop();
        break;
    case QModbusDevice::ClosingState:
        emit connectionStateChanged(tr("Отключение..."));
        break;
    }
}

void ModbusManager::tryReconnect()
{
    if (m_connectionIp.isEmpty())
        return;
    emit connectionStateChanged(tr("Переподключение..."));
    m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_connectionIp);
    m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_connectionPort);
    m_client->connectDevice();
}

void ModbusManager::triggerPoll() {
    if (m_busy || m_client->state() != QModbusDevice::ConnectedState)
        return;

    if (m_model->variableCount() == 0)
        return;

    // Группируем переменные в непрерывные диапазоны адресов,
    // чтобы уменьшить количество Modbus-запросов.
    int i = 0;
    while (i < m_model->variableCount()) {
        const ModbusVar &first = m_model->variableAt(i);
        uint16_t start = first.address;
        uint16_t count = static_cast<uint16_t>(first.regCount());

        int j = i + 1;
        while (j < m_model->variableCount()) {
            const ModbusVar &next = m_model->variableAt(j);
            uint16_t nextStart = next.address;
            uint16_t nextCount = static_cast<uint16_t>(next.regCount());

            // Если следующий регистр начинается сразу после текущего диапазона — расширяем диапазон.
            if (nextStart == static_cast<uint16_t>(start + count)) {
                count = static_cast<uint16_t>(count + nextCount);
                ++j;
            } else {
                break;
            }
        }

        m_queue.enqueue({ModbusRequest::Read, start, count, {}});
        i = j;
    }

    processQueue();
}

void ModbusManager::writeVariable(const QString &name, QVariant value) {
    const ModbusVar *var = m_model->findVariable(name);
    if (!var)
        return;

    QVector<uint16_t> data;
    switch (var->type) {
    case VarType::Bool: {
        if (var->bitIndex >= 0 && var->bitIndex <= 15) {
            quint16 reg = m_registerCache.value(var->address, 0);
            if (value.toBool())
                reg |= (1u << var->bitIndex);
            else
                reg &= ~(1u << var->bitIndex);
            data << reg;
            m_registerCache.insert(var->address, reg);
        } else {
            data << (value.toBool() ? 1u : 0u);
        }
        break;
    }
    case VarType::Word:
        data << static_cast<uint16_t>(value.toUInt() & 0xFFFFu);
        break;
    case VarType::DWord:
        data = packDWordLowWordFirst(static_cast<quint32>(value.toUInt()));
        break;
    case VarType::Float:
        data = packFloatLowWordFirst(value.toFloat());
        break;
    }

    m_queue.prepend({ModbusRequest::Write, var->address, static_cast<uint16_t>(var->regCount()), data});
    processQueue();
}

void ModbusManager::processQueue() {
    if (m_queue.isEmpty() || m_busy) return;
    m_busy = true;
    auto req = m_queue.dequeue();
    m_inFlightType = req.type;
    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, req.startAddress, req.count);

    QModbusReply *reply = nullptr;
    if (req.type == ModbusRequest::Read) reply = m_client->sendReadRequest(unit, m_unitId);
    else {
        for(int i=0; i<req.data.size(); ++i) unit.setValue(i, req.data[i]);
        reply = m_client->sendWriteRequest(unit, m_unitId);
    }

    if (reply) {
        connect(reply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
    } else {
        m_busy = false;
        processQueue();
    }
}

void ModbusManager::onReplyFinished() {
    auto *reply = qobject_cast<QModbusReply*>(sender());
    if (reply) {
        if (reply->error() == QModbusDevice::NoError) {
            if (m_inFlightType == ModbusRequest::Read) {
                parseReadData(reply->result());
            }
        } else {
            const QString err = reply->errorString();
            emit lastError(err);
        }
        reply->deleteLater();
    }

    m_busy = false;
    processQueue();
}

void ModbusManager::parseReadData(const QModbusDataUnit &res) {
    for (int i = 0; i < m_model->variableCount(); ++i) {
        const ModbusVar &v = m_model->variableAt(i);
        uint16_t start = res.startAddress();
        uint16_t valuesCount = static_cast<uint16_t>(res.valueCount());

        if (v.address < start || v.address >= static_cast<uint16_t>(start + valuesCount))
            continue;

        uint16_t offset = static_cast<uint16_t>(v.address - start);

        QVariant val;
        switch (v.type) {
        case VarType::Bool:
            if (v.bitIndex >= 0 && v.bitIndex <= 15)
                val = ((res.value(offset) >> v.bitIndex) & 1) != 0;
            else
                val = (res.value(offset) != 0);
            break;
        case VarType::Word:
            val = res.value(offset);
            break;
        case VarType::DWord:
            if (offset + 1 >= valuesCount)
                continue;
            val = unpackDWordLowWordFirst(res.value(offset), res.value(offset + 1));
            break;
        case VarType::Float:
            if (offset + 1 >= valuesCount)
                continue;
            val = unpackFloatLowWordFirst(res.value(offset), res.value(offset + 1));
            break;
        }

        m_model->updateVariable(v.name, val);
    }

    for (int i = 0; i < res.valueCount(); ++i)
        m_registerCache[res.startAddress() + i] = res.value(i);
}
