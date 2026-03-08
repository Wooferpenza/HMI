#include "variable.h"

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

void Variable::setname(const QString &newName)
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

void Variable::setRowData(QVector<uint16_t> row)
{
    rowValue=row;
}

void Variable::setValue(QVariant val)
{
    value=val;
    emit valueChanged(value);
}


