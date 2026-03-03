#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QObject>
#include <QQueue>
#include <QModbusTcpClient>

// Сначала подключаем структуры, чтобы QQueue<ModbusRequest> понимал тип
#include "modbuscommon.h"

// Предварительное объявление класса модели, чтобы не было циклической зависимости
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
    QQueue<ModbusRequest> m_queue; // Теперь ошибка исчезнет
    bool m_busy = false;
};

#endif
