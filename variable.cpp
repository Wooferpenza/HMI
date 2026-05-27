#include "variable.h"
#include "registerutils.h"
Variable::Variable(QObject *parent)
    : QObject{parent}
{}

DataType Variable::type() const
{
    return m_type;
}

void Variable::setType(DataType newType)
{
    if (m_type == newType)
        return;
    m_type = newType;

    switch (m_type) {
    case DataType::UDWord:
    case DataType::SDWord:
    case DataType::Float:
        m_rawValueSize = 2;
        break;
    default:
        m_rawValueSize = 1;
        break;
    }

    emit typeChanged();
}

QString Variable::name() const
{
    return m_name;
}

void Variable::setName(const QString &newName)
{
    if (m_name == newName)
        return;
    m_name = newName;
    emit nameChanged();
}

uint16_t Variable::readAddressBit() const
{
    return m_readAddressBit;
}

void Variable::setReadAddressBit(uint16_t newBitIndex)
{
    if (newBitIndex > 15)
        newBitIndex = 15;
    if (m_readAddressBit == newBitIndex)
        return;
    m_readAddressBit = newBitIndex;
    emit readAddressBitChanged();
}

uint16_t Variable::fractional() const
{
    return m_fractional;
}

void Variable::setFractional(uint16_t newFractional)
{
    if (m_fractional == newFractional)
        return;
    m_fractional = newFractional;
    emit fractionalChanged();
}

float Variable::minimum() const
{
    return m_minimum;
}

void Variable::setMinimum(float newMinimum)
{
    if (qFuzzyIsNull(m_minimum - newMinimum))
        return;
    m_minimum = newMinimum;
    emit minimumChanged();
}

float Variable::maximum() const
{
    return m_maximum;
}

void Variable::setMaximum(float newMaximum)
{
    if (qFuzzyIsNull(m_maximum - newMaximum))
        return;
    m_maximum = newMaximum;
    emit maximumChanged();
}

uint16_t Variable::rawValueSize() const
{
    return m_rawValueSize;
}

void Variable::valueToRawValue()
{
    QVector<uint16_t> vec;
    switch (m_type) {
    case DataType::Bit:
    {
        uint16_t word = m_rawValue.value(0, 0);
        const uint16_t mask = static_cast<uint16_t>(1u << m_readAddressBit);
        if (m_value.toBool())
            word |= mask;
        else
            word &= ~mask;
        vec.push_back(word);
        break;
    }
    case DataType::UWord:
    {
        const uint32_t v = m_value.toUInt();
        vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
        break;
    }
    case DataType::SWord:
    {
        const int16_t v = static_cast<int16_t>(m_value.toInt());
        vec.push_back(static_cast<uint16_t>(v));
        break;
    }
    case DataType::UDWord:
    {
        const quint32 v = m_value.toUInt();
        vec = packDWordLowWordFirst(v);
        break;
    }
    case DataType::SDWord:
    {
        const quint32 v = static_cast<quint32>(m_value.toInt());
        vec = packDWordLowWordFirst(v);
        break;
    }
    case DataType::Float:
    {
        vec = packFloatLowWordFirst(m_value.toFloat());
        break;
    }
    default:
        break;
    }

    m_rawValue = vec;
}

void Variable::setRawData(const QVector<uint16_t> &raw)
{
    m_rawValue = raw;
    QVariant decoded;
    switch (m_type)
    {
    case DataType::Bit:
    {
        if (raw.size() >= 1)
            decoded = (raw.value(0) >> m_readAddressBit & 1u) != 0;
        break;
    }
    case DataType::UWord:
    {
        if (raw.size() >= 1)
            decoded = raw.value(0);
        break;
    }
    case DataType::SWord:
    {
        if (raw.size() >= 1)
            decoded = static_cast<int16_t>(raw.value(0));
        break;
    }
    case DataType::UDWord:
    {
        if (raw.size() >= 2)
        {
            const uint32_t dw = (static_cast<uint32_t>(raw[1]) << 16) | raw[0];
            decoded = dw;
        }
        break;
    }
    case DataType::SDWord:
    {
        if (raw.size() >= 2)
        {
            const int32_t dw = (static_cast<int32_t>(raw[1]) << 16) | raw[0];
            decoded = dw;
        }
        break;
    }
    case DataType::Float:
    {
        if (raw.size() >= 2)
            decoded = unpackFloatLowWordFirst(raw[0], raw[1]);
        break;
    }
    default:
        break;
    }
    m_value = decoded;
    emit valueChanged(m_value);
}

void Variable::setValue(const QVariant &val)
{
    m_value = val;
    emit valueChanged(m_value);
    valueToRawValue();
    emit rawValueChanged(m_rawValue);
}

uint16_t Variable::writeAddress() const
{
    return m_writeAddress;
}

void Variable::setWriteAddress(uint16_t newWriteAddress)
{
    if (m_writeAddress == newWriteAddress)
        return;
    m_writeAddress = newWriteAddress;
    emit writeAddressChanged();
}

uint16_t Variable::writeAddressBit() const
{
    return m_writeAddressBit;
}

void Variable::setWriteAddressBit(uint16_t newWriteAddressBit)
{
    if (m_writeAddressBit == newWriteAddressBit)
        return;
    m_writeAddressBit = newWriteAddressBit;
    emit writeAddressBitChanged();
}

uint16_t Variable::readAddress() const
{
    return m_readAddress;
}

void Variable::setReadAddress(uint16_t newReadAddress)
{
    if (m_readAddress == newReadAddress)
        return;
    m_readAddress = newReadAddress;
    emit readAddressChanged();
}
