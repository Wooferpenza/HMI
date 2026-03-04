#ifndef MODBUSCOMMON_H
#define MODBUSCOMMON_H

#include <QString>
#include <QVariant>
#include <QVector>

enum class VarType { Bool, Word, DWord, Float };

struct ModbusVar {
    QString name;
    uint16_t address;
    VarType type;
    QVariant value;
    int regCount() const { return (type == VarType::DWord || type == VarType::Float) ? 2 : 1; }
signals:
   void update();
};

struct ModbusRequest {
    enum Type { Read, Write } type;
    uint16_t startAddress;
    uint16_t count;
    QVector<uint16_t> data;
};

#endif
