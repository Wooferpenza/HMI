#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

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
    bool m_busy = false;
};

#endif
