#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QHash>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QModbusTcpClient>
#include "modbuscommon.h"
#include "modbussettings.h"

class ModbusModel;

class ModbusManager : public QObject {
    Q_OBJECT
public:
    explicit ModbusManager(ModbusModel *model, QObject *parent = nullptr);
    void connectTo(const QString &ip, int port);
    void setUnitId(int unitId);
    void setRequestTimeoutMs(int ms);
    void setReconnectIntervalMs(int ms);
    void setPollIntervalMs(int ms);
    void applySettings(const ModbusSettings &settings);
    void writeVariable(const QString &name, QVector<uint16_t> value);

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
    void parseReadData(const QModbusDataUnit &res);
    QModbusTcpClient *m_client;
    ModbusModel *m_model;
    QQueue<ModbusRequest> m_queue;
    bool m_busy = false;
    int m_unitId = 1;
    ModbusRequest::Type m_inFlightType = ModbusRequest::Read;
    QString m_connectionIp;
    int m_connectionPort = 502;
    QTimer *m_reconnectTimer = nullptr;
    int m_reconnectIntervalMs = 5000;
    QTimer *m_pollTimer = nullptr;
    int m_pollIntervalMs = 1000;
};

#endif
