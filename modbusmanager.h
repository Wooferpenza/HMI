#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QHash>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QModbusTcpClient>
#include "modbuscommon.h"

class ModbusModel;

class ModbusManager : public QObject {
    Q_OBJECT
public:
    explicit ModbusManager(ModbusModel *model, QObject *parent = nullptr);
    void connectTo(const QString &ip, int port);
    void setUnitId(int unitId);
    void setRequestTimeoutMs(int ms);
    void setReconnectIntervalMs(int ms);
    void writeVariable(const QString &name, QVariant value);

signals:
    void connectionStateChanged(const QString &stateText);
    void lastError(const QString &errorText);

public slots:
    void triggerPoll();

private slots:
    void processQueue();
    void onReplyFinished();
    void onClientStateChanged(QModbusDevice::State state);
    void tryReconnect();

private:
    void parseReadData(const struct QModbusDataUnit &res);
    QModbusTcpClient *m_client;
    ModbusModel *m_model;
    QQueue<ModbusRequest> m_queue;
    QHash<quint16, quint16> m_registerCache;  ///< Кэш регистров для read-modify-write битов
    bool m_busy = false;
    int m_unitId = 1;
    ModbusRequest::Type m_inFlightType = ModbusRequest::Read;
    QString m_connectionIp;
    int m_connectionPort = 502;
    QTimer *m_reconnectTimer = nullptr;
    int m_reconnectIntervalMs = 5000;
};

#endif
