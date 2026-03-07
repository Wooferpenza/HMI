#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QHash>
#include <QObject>
#include <QQueue>
#include <QModbusTcpClient>
#include "modbuscommon.h"

class ModbusModel;

class ModbusManager : public QObject {
    Q_OBJECT
public:
    explicit ModbusManager(ModbusModel *model, QObject *parent = nullptr);
    void connectTo(const QString &ip, int port);
    void setUnitId(int unitId);
    void writeVariable(const QString &name, QVariant value);

public slots:
    void triggerPoll();

private slots:
    void processQueue();
    void onReplyFinished();

private:
    void parseReadData(const struct QModbusDataUnit &res);
    QModbusTcpClient *m_client;
    ModbusModel *m_model;
    QQueue<ModbusRequest> m_queue;
    QHash<quint16, quint16> m_registerCache;  ///< Кэш регистров для read-modify-write битов
    bool m_busy = false;
    int m_unitId = 1;
    ModbusRequest::Type m_inFlightType = ModbusRequest::Read;
};

#endif
