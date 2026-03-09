#ifndef MODBUSCOMMON_H
#define MODBUSCOMMON_H

#include <QObject>
#include <QString>
#include <QVector>
//#include <cstdint>

class ModbusVar : public QObject {
    Q_OBJECT
public:
    explicit ModbusVar(const QString &nam, uint16_t addr, uint16_t sz, QObject *parent = nullptr)
        : QObject(parent), name(nam), address(addr), size(sz)
    {
        value.resize(size);
    }
    QString name;
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
    QString name;
};

#endif
