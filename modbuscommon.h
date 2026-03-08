#ifndef MODBUSCOMMON_H
#define MODBUSCOMMON_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <cstdint>
#include <cstring>

namespace ModbusUtils {

inline QVector<uint16_t> packFloatLowWordFirst(float f)
{
    uint32_t r = 0;
    std::memcpy(&r, &f, sizeof(float));
    return {static_cast<uint16_t>(r & 0xFFFFu), static_cast<uint16_t>(r >> 16)};
}

inline float unpackFloatLowWordFirst(uint16_t low, uint16_t high)
{
    uint32_t r = (uint32_t(high) << 16) | low;
    float f = 0.0f;
    std::memcpy(&f, &r, sizeof(float));
    return f;
}

inline QVector<uint16_t> packDWordLowWordFirst(quint32 v)
{
    return {static_cast<uint16_t>(v & 0xFFFFu), static_cast<uint16_t>(v >> 16)};
}

inline quint32 unpackDWordLowWordFirst(uint16_t low, uint16_t high)
{
    return (quint32(high) << 16) | low;
}

} // namespace ModbusUtils

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
    uint16_t size;
    QVector<uint16_t> value;
signals:
    void valueChanged(const  QVector<uint16_t> &value);
};

struct ModbusRequest {
    enum Type { Read, Write } type;
    uint16_t startAddress;
    uint16_t count;
    QVector<uint16_t> data;
};

#endif
