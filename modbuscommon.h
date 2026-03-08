#ifndef MODBUSCOMMON_H
#define MODBUSCOMMON_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <cstdint>

class ModbusVar : public QObject {
    Q_OBJECT
public:
    explicit ModbusVar(const QString &nam, uint16_t addr, size_t sz, QObject *parent = nullptr)
        : QObject(parent), name(nam), address(addr), size(sz)
    {
        value.resize(size);
    }
    QString name;
    uint16_t address;
    size_t size;
    QVector<uint16_t> value;

signals:
    void valueChanged(const QVariant &value);
};

struct ModbusRequest {
    enum Type { Read, Write } type;
    uint16_t startAddress;
    uint16_t count;
    QVector<uint16_t> data;
};

#endif
