#include "modbusmanager.h"
#include "modbusmodel.h" // Подключаем полную модель только здесь
#include <QModbusReply>
#include <QModbusDataUnit>
ModbusManager::ModbusManager(ModbusModel *model, QObject *parent) : QObject(parent), m_model(model) {
    m_client = new QModbusTcpClient(this);
}

void ModbusManager::connectTo(const QString &ip, int port) {
    m_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ip);
    m_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
    m_client->connectDevice();
}

void ModbusManager::triggerPoll() {
    if (m_busy || m_client->state() != QModbusDevice::ConnectedState) return;
    // Упрощенная группировка: по одному запросу на переменную (для краткости примера)
    for (const auto &v : m_model->m_vars)
        m_queue.enqueue({ModbusRequest::Read, v.address, (uint16_t)v.regCount(), {}});
    processQueue();
}

void ModbusManager::writeVariable(const QString &name, QVariant value) {
    for (const auto &v : m_model->m_vars) {
        if (v.name == name) {
            QVector<uint16_t> data;
            if (v.type == VarType::Float) {
                float f = value.toFloat(); uint32_t r; memcpy(&r, &f, 4);
                data << (uint16_t)(r >> 16) << (uint16_t)(r & 0xFFFF);
            } else data << (uint16_t)value.toUInt();

            m_queue.prepend({ModbusRequest::Write, v.address, (uint16_t)v.regCount(), data});
            processQueue();
            return;
        }
    }
}

void ModbusManager::processQueue() {
    if (m_queue.isEmpty() || m_busy) return;
    m_busy = true;
    auto req = m_queue.dequeue();
    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters, req.startAddress, req.count);

    QModbusReply *reply = nullptr;
    if (req.type == ModbusRequest::Read) reply = m_client->sendReadRequest(unit, 1);
    else {
        for(int i=0; i<req.data.size(); ++i) unit.setValue(i, req.data[i]);
        reply = m_client->sendWriteRequest(unit, 1);
    }

    if (reply) connect(reply, &QModbusReply::finished, this, &ModbusManager::onReplyFinished);
    else { m_busy = false; processQueue(); }
}

void ModbusManager::onReplyFinished() {
    auto *reply = qobject_cast<QModbusReply*>(sender());
    if (reply && reply->error() == QModbusDevice::NoError && reply->type() == QModbusReply::Raw)
        parseReadData(reply->result());

    m_busy = false;
    reply->deleteLater();
    processQueue();
}

void ModbusManager::parseReadData(const QModbusDataUnit &res) {
    for (auto &v : m_model->m_vars) {
        if (v.address == res.startAddress()) {
            QVariant val;
            if (v.type == VarType::Float) {
                uint32_t r = (uint32_t(res.value(0)) << 16) | res.value(1);
                float f; memcpy(&f, &r, 4); val = f;
            } else val = res.value(0);
            m_model->updateVariable(v.name, val);
        }
    }
}
