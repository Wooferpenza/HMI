#include "variable.h"
#include <cstdint>

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

uint16_t Variable::integerDigit() const
{
    return m_integerDigit;
}

void Variable::setIntegerDigit(uint16_t newIntegerDigit)
{
    if (m_integerDigit == newIntegerDigit)
        return;
    m_integerDigit = newIntegerDigit;
    emit integerDigitChanged();
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
    if (qFuzzyCompare(m_minimum, newMinimum))
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
    if (qFuzzyCompare(m_maximum, newMaximum))
        return;
    m_maximum = newMaximum;
    emit maximumChanged();
}

uint16_t Variable::rowValueSize()
{
    switch (m_type) {
    case DataType::Bit:
    case DataType::UWord:
    case DataType::SWord:
        mRowValueSize = 1;
        break;
    case DataType::UDWord:
    case DataType::SDWord:
    case DataType::Float:
        mRowValueSize = 2;
        break;
    default:
        mRowValueSize = 1;
        break;
    }
    return mRowValueSize;
}

void Variable::valueToRowValue()
{
    QVector<uint16_t> vec;
    switch (m_type) {
    case DataType::Bit:
    case DataType::UWord:
    {
        const uint32_t v = mValue.toUInt();
        vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
        break;
    }
    case DataType::SWord:
    {
        const quint32 v = mValue.toUInt();
        vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
        break;
    }
    case DataType::UDWord:
    case DataType::SDWord:
    {
        const quint32 v = mValue.toUInt();
        vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
        vec.push_back(static_cast<uint16_t>(v >> 16));
        break;
    }
    case DataType::Float:
    {
        vec = packFloatLowWordFirst(mValue.toFloat());
        break;
    }

    default:

        break;
    }

    mRowValue=vec;
}

void Variable::setRowData(QVector<uint16_t> row)
{
    mRowValue = row;
    QVariant decoded;
    switch (m_type)
    {
    case DataType::Bit:
    {
        decoded = (row.value(0) & 1u) != 0;
        break;
    }
    case DataType::UWord:
    {
        if (row.size()>=1)
        {
            decoded = row.value(0);
        }
        break;
    }
    case DataType::SWord:
    {
        if (row.size()>=1)
        {
            decoded = int16_t(row.value(0));
        }
        break;
    }
    case DataType::UDWord:
        if (row.size() >= 2)
        {
            const uint32_t dw = (static_cast<uint32_t>(row[1]) << 16) | row[0];
            decoded = dw;
        }
        break;
    case DataType::SDWord:
    {
        if (row.size() >= 2)
        {
            const int32_t dw = (static_cast<int32_t>(row[1]) << 16) | row[0];
            decoded = dw;
        }
        break;
    }
    case DataType::Float:
    {
        if (row.size() >= 2)
        {
            decoded = unpackFloatLowWordFirst(row[0], row[1]);
        }
        break;
    }
    default:
        break;

    }
    mValue = decoded;
    emit valueChanged(mValue);

}
void Variable::setValue(QVariant val)
{
    mValue = val;
    emit valueChanged(mValue);
    valueToRowValue();
    emit rowValueChanged(mRowValue);
}


