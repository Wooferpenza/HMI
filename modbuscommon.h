#ifndef MODBUSCOMMON_H
#define MODBUSCOMMON_H

#include <QObject>
#include <QVector>

class ModbusVar : public QObject {
    Q_OBJECT
public:
    explicit ModbusVar(uint16_t addr, uint16_t sz, QObject *parent = nullptr)
        : QObject(parent), address(addr), size(sz)
    {
        value.resize(size);
    }
    uint16_t address;
    uint16_t size;
    QVector<uint16_t> value;

signals:
    void valueChanged(const QVector<uint16_t> &value);
};

struct ModbusRequest {
    enum Type { Read, Write } type;
    uint16_t startAddress;
    uint16_t count;
    QVector<uint16_t> data;
};

#endif
