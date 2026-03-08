#include "modbusmanager.h"
#include "modbusmodel.h"
#include <QModbusReply>
#include <QModbusDataUnit>
#include <QtCore/QtNumeric>
#include <algorithm>

namespace {

constexpr int kDefaultRequestTimeoutMs = 5000;
constexpr int kMinRequestTimeoutMs = 500;
constexpr int kMinReconnectIntervalMs = 2000;
constexpr int kModbusUnitIdMin = 1;
constexpr int kModbusUnitIdMax = 247;
constexpr int kMinPollIntervalMs = 50;

} // namespace

ModbusManager::ModbusManager(ModbusModel *model, QObject *parent) : QObject(parent), m_model(model) {
    m_client = new QModbusTcpClient(this);
    m_client->setTimeout(kDefaultRequestTimeoutMs);

    connect(m_client, &QModbusClient::stateChanged, this, &ModbusManager::onClientStateChanged);

    if (m_model) {
        connect(m_model, &ModbusModel::requestWrite,
                this, &ModbusManager::writeVariable);
    }

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &ModbusManager::tryReconnect);

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, this, &ModbusManager::triggerPoll);
}

void ModbusManager::setUnitId(int unitId)
{
    m_unitId = qBound(kModbusUnitIdMin, unitId, kModbusUnitIdMax);
}

void ModbusManager::setRequestTimeoutMs(int ms)
{
    m_client->setTimeout(qMax(ms, kMinRequestTimeoutMs));
}

void ModbusManager::setReconnectIntervalMs(int ms)
{
    m_reconnectIntervalMs = qMax(ms, kMinReconnectIntervalMs);
}

void ModbusManager::setPollIntervalMs(int ms)
{
    m_pollIntervalMs = qMax(ms, kMinPollIntervalMs);
    m_pollTimer->setInterval(m_pollIntervalMs);
    m_pollTimer->start();
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

void ModbusManager::applySettings(const ModbusSettings &settings)
{
    setUnitId(settings.unitId);
    setPollIntervalMs(settings.pollIntervalMs);
    connectTo(settings.ip, settings.port);
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

    // Копируем переменные в локальный список и сортируем по адресу,
    // чтобы группировка диапазонов регистров была корректной.
    QVector<const ModbusVar*> vars;
    vars.reserve(m_model->variableCount());
    for (int i = 0; i < m_model->variableCount(); ++i) {
        const ModbusVar *v = m_model->variableAt(i);
        if (!v)
            continue;
        vars.append(v);
    }

    if (vars.isEmpty())
        return;

    std::sort(vars.begin(), vars.end(), [](const ModbusVar *a, const ModbusVar *b) {
        return a->address < b->address;
    });

    // Группируем переменные в непрерывные диапазоны адресов,
    // чтобы уменьшить количество Modbus-запросов.
    int i = 0;
    while (i < vars.size()) {
        const ModbusVar *first = vars.at(i);
        uint16_t start = first->address;
        uint16_t count = first->size;

        int j = i + 1;
        while (j < vars.size()) {
            const ModbusVar *next = vars.at(j);
            uint16_t nextStart = next->address;
            uint16_t nextCount = next->size;

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

void ModbusManager::writeVariable(const QString &name, QVector<uint16_t> value) {
    const ModbusVar *var = m_model->findVariable(name);
    if (!var)
        return;
    m_queue.prepend({ModbusRequest::Write, var->address, var->size, value});
    processQueue();
}

void ModbusManager::processQueue() {
    if (m_queue.isEmpty() || m_busy) return;
    m_busy = true;
    auto req = m_queue.dequeue();
    m_inFlightType = req.type;
    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, req.startAddress, req.count);

    QModbusReply *reply = nullptr;
    if (req.type == ModbusRequest::Read) {
        reply = m_client->sendReadRequest(unit, m_unitId);
    } else {
        const int valueCount = unit.valueCount();
        const int toCopy = valueCount < req.data.size() ? valueCount : req.data.size();
        for (int i = 0; i < toCopy; ++i)
            unit.setValue(i, req.data.at(i));
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
        const ModbusVar *v = m_model->variableAt(i);
        if (!v) continue;
        uint16_t start = res.startAddress();
        uint16_t valuesCount = static_cast<uint16_t>(res.valueCount());

        if (v->address < start || v->address >= static_cast<uint16_t>(start + valuesCount))
            continue;

        uint16_t offset = static_cast<uint16_t>(v->address - start);
        const uint16_t endIndex = static_cast<uint16_t>(offset + v->size);
        if (endIndex > valuesCount || endIndex < offset)
            continue;

        QVector<uint16_t> val;
        for (int j = 0; j < v->size; ++j)
        {
            val.push_back(res.value(offset + j));
        }

        m_model->updateVariable(v->name, val);
    }


}
