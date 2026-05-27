#include "modbusmanager.h"
#include "modbusmodel.h"
#include <QVariant>
#include <QModbusReply>
#include <QModbusDataUnit>
#include <QtCore/QtNumeric>

namespace {

constexpr int kDefaultRequestTimeoutMs = 5000;
constexpr int kMinRequestTimeoutMs = 500;
constexpr int kMinReconnectIntervalMs = 2000;
constexpr int kModbusUnitIdMin = 1;
constexpr int kModbusUnitIdMax = 247;
constexpr int kMinPollIntervalMs = 50;
constexpr uint16_t kMaxAddressGap = 4;

} // namespace

ModbusManager::ModbusManager(ModbusModel *model, QObject *parent) : QObject(parent), m_model(model) {
    m_client = new QModbusTcpClient(this);
    m_client->setTimeout(kDefaultRequestTimeoutMs);

    connect(m_client, &QModbusClient::stateChanged, this, &ModbusManager::onClientStateChanged);

    if (m_model) {
        connect(m_model, &ModbusModel::requestWrite, this, &ModbusManager::writeVariable);
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
    if (!m_client->connectDevice())
        emit lastError(tr("Не удалось подключиться: %1").arg(m_client->errorString()));
}

void ModbusManager::applySettings(const ModbusSettings &settings)
{
    setUnitId(settings.unitId);
    setPollIntervalMs(settings.pollIntervalMs);
    connectTo(settings.ip, settings.port);
}

void ModbusManager::disconnectDevice()
{
    m_reconnectTimer->stop();
    m_pollTimer->stop();
    m_connectionIp.clear();
    if (m_client->state() != QModbusDevice::UnconnectedState)
        m_client->disconnectDevice();
}

void ModbusManager::stopPolling()
{
    m_pollTimer->stop();
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
    if (!m_client->connectDevice())
        emit lastError(tr("Переподключение не удалось: %1").arg(m_client->errorString()));
}

void ModbusManager::triggerPoll() {
    if (m_busy || m_client->state() != QModbusDevice::ConnectedState)
        return;

    if (m_model->variableCount() == 0)
        return;

    clearPendingReads();

    QList<ModbusVar *> &vars = m_model->readTable();

    int i = 0;
    while (i < vars.size()) {
        const ModbusVar *first = vars.at(i);
        uint16_t start = first->address;
        uint16_t end = static_cast<uint16_t>(start + first->size);

        int j = i + 1;
        while (j < vars.size()) {
            const ModbusVar *next = vars.at(j);
            const uint16_t nextEnd = static_cast<uint16_t>(next->address + next->size);
            if (next->address <= end + kMaxAddressGap) {
                if (nextEnd > end)
                    end = nextEnd;
                ++j;
            } else {
                break;
            }
        }

        const uint16_t count = static_cast<uint16_t>(end - start);
        m_queue.enqueue({ModbusRequest::Read, start, count, {}});
        i = j;
    }

    processQueue();
}

void ModbusManager::writeVariable(const uint16_t &address, const QVector<uint16_t> &value) {
    m_queue.prepend({ModbusRequest::Write, address, static_cast<uint16_t>(value.size()), value});
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
            } else {
                emit writeCompleted(m_inFlightName);
            }
        } else {
            const QString err = reply->errorString();
            if (m_inFlightType == ModbusRequest::Write)
                emit writeFailed(m_inFlightName, err);
            else
                emit lastError(err);
        }
        reply->deleteLater();
    }

    m_busy = false;
    processQueue();
}

void ModbusManager::clearPendingReads() {
    QQueue<ModbusRequest> kept;
    while (!m_queue.isEmpty()) {
        auto req = m_queue.dequeue();
        if (req.type != ModbusRequest::Read)
            kept.enqueue(std::move(req));
    }
    m_queue = std::move(kept);
}

void ModbusManager::parseReadData(const QModbusDataUnit &res) {
    const uint16_t start = static_cast<uint16_t>(res.startAddress());
    const uint16_t valuesCount = static_cast<uint16_t>(res.valueCount());
    const auto vars = m_model->variablesInRange(start, valuesCount);
    for ( ModbusVar *v : vars) {
        const uint16_t offset = static_cast<uint16_t>(v->address - start);
        const uint16_t endIndex = static_cast<uint16_t>(offset + v->size);
        if (endIndex > valuesCount)
            continue;

        QVector<uint16_t> val;
        val.reserve(v->size);
        for (int j = 0; j < v->size; ++j)
            val.append(res.value(offset + j));

        m_model->updateVariable(v, val);
    }
}
