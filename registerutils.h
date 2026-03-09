#ifndef REGISTERUTILS_H
#define REGISTERUTILS_H

#include <QVector>
#include <cstdint>
#include <cstring>

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

#endif // REGISTERUTILS_H
