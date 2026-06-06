#include <QMetaEnum>
#include <QRegularExpression>
#include <QtGlobal>
#include "variable.h"
#include "registerutils.h"
Variable::Variable(QObject *parent)
    : QObject{parent}
{}

Variable::DataFormat Variable::type() const
{
    return m_type;
}

void Variable::setType(DataFormat newType)
{
    if (m_type == newType)
        return;
    m_type = newType;

    switch (m_type) {
    case DataFormat::UDWord:
    case DataFormat::SDWord:
    case DataFormat::Float:
        m_rawValueSize = 2;
        break;
    default:
        m_rawValueSize = 1;
        break;
    }

    emit typeChanged();
}

uint16_t Variable::addressBit() const
{
    return m_addressBit;
}

void Variable::setAddressBit(uint16_t newBitIndex)
{
    if (newBitIndex > 15)
        newBitIndex = 15;
    if (m_addressBit == newBitIndex)
        return;
    m_addressBit = newBitIndex;
    emit addressBitChanged();
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
    case DataFormat::Bit:
    {
        uint16_t word = m_rawValue.value(0, 0);
        const uint16_t mask = static_cast<uint16_t>(1u << m_addressBit);
        if (m_value.toBool())
            word |= mask;
        else
            word &= ~mask;
        vec.push_back(word);
        break;
    }
    case DataFormat::UWord:
    {
        const uint32_t v = m_value.toUInt();
        vec.push_back(static_cast<uint16_t>(v & 0xFFFFu));
        break;
    }
    case DataFormat::SWord:
    {
        const int16_t v = static_cast<int16_t>(m_value.toInt());
        vec.push_back(static_cast<uint16_t>(v));
        break;
    }
    case DataFormat::UDWord:
    {
        const quint32 v = m_value.toUInt();
        vec = packDWordLowWordFirst(v);
        break;
    }
    case DataFormat::SDWord:
    {
        const quint32 v = static_cast<quint32>(m_value.toInt());
        vec = packDWordLowWordFirst(v);
        break;
    }
    case DataFormat::Float:
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
    case DataFormat::Bit:
    {
        if (raw.size() >= 1)
            decoded = (raw.value(0) >> m_addressBit & 1u) != 0;
        break;
    }
    case DataFormat::UWord:
    {
        if (raw.size() >= 1)
            decoded = raw.value(0);
        break;
    }
    case DataFormat::SWord:
    {
        if (raw.size() >= 1)
            decoded = static_cast<int16_t>(raw.value(0));
        break;
    }
    case DataFormat::UDWord:
    {
        if (raw.size() >= 2)
        {
            const uint32_t dw = (static_cast<uint32_t>(raw[1]) << 16) | raw[0];
            decoded = dw;
        }
        break;
    }
    case DataFormat::SDWord:
    {
        if (raw.size() >= 2)
        {
            const int32_t dw = (static_cast<int32_t>(raw[1]) << 16) | raw[0];
            decoded = dw;
        }
        break;
    }
    case DataFormat::Float:
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

void Variable::onSetFormat()
{

}

uint16_t Variable::address() const
{
    return m_address;
}

void Variable::setAddress(uint16_t newReadAddress)
{
    if (m_address == newReadAddress)
        return;
    m_address = newReadAddress;
    emit addressChanged();
}

void Variable::setFormat( QString newFormat)
{
    m_format = newFormat;
    int enumIndex = Variable::staticMetaObject.indexOfEnumerator("DataFormat");
    QMetaEnum metaEnum = Variable::staticMetaObject.enumerator(enumIndex);
    bool ok = false;
    int value = metaEnum.keyToValue(newFormat.toUtf8().constData(), &ok);

    if (ok) {
        Variable::DataFormat m_type  = static_cast<Variable::DataFormat>(value);
    } else {
        qFatal("Неверный format %s", this->parent()->objectName().toUtf8().constData());
    }

}

void Variable::setAddressStr(const QString &newAddressStr)
{
    m_addressStr = newAddressStr;
    static const QRegularExpression regex("^D\\d+$|^D\\d+\\.\\d+$");
    if(!regex.match(m_addressStr).hasMatch())
    {qFatal("Неверный %s address %s", this->objectName().toUtf8().constData(),this->parent()->objectName().toUtf8().constData()) ; return; }
    auto adrlist=m_addressStr.sliced(1).split(".");
    if (adrlist.size()>0) m_address = adrlist.at(0).toInt();
    if (adrlist.size()>1) m_addressBit = adrlist.at(1).toInt();
}
