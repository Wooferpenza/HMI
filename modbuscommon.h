#ifndef MODBUSCOMMON_H
#define MODBUSCOMMON_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <cstdint>

enum class VarType { Bool, Word, DWord, Float };

class ModbusVar : public QObject {
    Q_OBJECT
public:
    explicit ModbusVar(const QString &nam, uint16_t addr, VarType tp, QObject *parent = nullptr, int bitIdx = -1)
        : QObject(parent), name(nam), address(addr), type(tp), bitIndex(bitIdx)
    {}
    QString name;
    uint16_t address;
    VarType type;
    QVariant value;
    int bitIndex = -1;  ///< Для Bool: 0–15 — номер бита в слове; -1 — всё слово (0/ноль)

    int regCount() const { return (type == VarType::DWord || type == VarType::Float) ? 2 : 1; }
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
