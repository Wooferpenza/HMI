#include "variable.h"
#include "modbuscommon.h"
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

DataFormat Variable::format() const
{
    return m_format;
}

void Variable::setFormat(DataFormat newFormat)
{
    if (m_format == newFormat)
        return;
    m_format = newFormat;
    emit formatChanged();
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
    case DataType::Word:
        mRowValueSize = 1;
        break;
    case DataType::DWord:
        mRowValueSize = 2;
        break;
    case DataType::QWord:
        mRowValueSize = 4;
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
    case DataType::Word:
    {
        const quint32 v = mValue.toUInt();
        vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
        break;
    }
    case DataType::DWord:
    {
        if (m_format == DataFormat::Floating) {
            vec = ModbusUtils::packFloatLowWordFirst(mValue.toFloat());
        }
        else
        {
            const quint32 v = mValue.toUInt();
            vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
            vec.push_back(static_cast<uint16_t>(v >> 16));
        }
        break;
    }
    case DataType::QWord:
        vec.push_back(mValue.toUInt());
        break;
    default:
        mRowValueSize = 1;
        break;
    }

    mRowValue=vec;
}

void Variable::setRowData(QVector<uint16_t> row)
{
    mRowValue = row;

    QVariant decoded;
    switch (m_type) {
    case DataType::Bit:
        decoded = (row.value(0) & 1u) != 0;
        break;
    case DataType::Word:
        decoded = row.value(0);
        break;
    case DataType::DWord:
        if (row.size() >= 2) {
            if (m_format == DataFormat::Floating) {
                decoded = ModbusUtils::unpackFloatLowWordFirst(row[0], row[1]);
            } else {
                const quint32 dw = (static_cast<quint32>(row[1]) << 16) | row[0];
                decoded = dw;
            }
        } else {
            decoded = row.value(0);
        }
        break;
    case DataType::QWord:
        if (row.size() >= 4) {
            const qulonglong qw =
                (static_cast<qulonglong>(row[3]) << 48) |
                (static_cast<qulonglong>(row[2]) << 32) |
                (static_cast<qulonglong>(row[1]) << 16) |
                static_cast<qulonglong>(row[0]);
            decoded = qw;
        } else {
            decoded = row.value(0);
        }
        break;
    default:
        decoded = row.value(0);
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


