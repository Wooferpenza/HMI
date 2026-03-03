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
    if (var->type == VarType::Float) {
        float f = value.toFloat();
        uint32_t r;
        memcpy(&r, &f, 4);
        data << (uint16_t)(r >> 16) << (uint16_t)(r & 0xFFFF);
    } else {
        data << (uint16_t)value.toUInt();
    }

    m_queue.prepend({ModbusRequest::Write, var->address, (uint16_t)var->regCount(), data});
    processQueue();
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
    for (int i = 0; i < m_model->variableCount(); ++i) {
        const ModbusVar &v = m_model->variableAt(i);
        uint16_t start = res.startAddress();
        uint16_t valuesCount = static_cast<uint16_t>(res.valueCount());

        if (v.address < start || v.address >= static_cast<uint16_t>(start + valuesCount))
            continue;

        uint16_t offset = static_cast<uint16_t>(v.address - start);

        QVariant val;
        if (v.type == VarType::Float) {
            if (offset + 1 >= valuesCount)
                continue; // защитимся от выхода за границы
            uint32_t r = (uint32_t(res.value(offset)) << 16) | res.value(offset + 1);
            float f;
            memcpy(&f, &r, 4);
            val = f;
        } else {
            val = res.value(offset);
        }

        m_model->updateVariable(v.name, val);
    }
}
