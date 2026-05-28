#include "numericdisplay.h"
#include <cmath>

NumericDisplay::NumericDisplay(QWidget *parent)
    : QLineEdit(parent)
    , m_variable(new Variable(this))
{
    connect(m_variable, &Variable::valueChanged, this, &NumericDisplay::displayData);
}

Variable *NumericDisplay::variable() const
{
    return m_variable;
}

void NumericDisplay::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
    if (!isReadOnly())
        emit clicked();
}

void NumericDisplay::inputData(const QVariant &data)
{
    m_variable->setValue(data);
}

void NumericDisplay::displayData(const QVariant &val)
{
    const uint16_t frac = m_variable->fractional();
    QString text;

    switch (m_variable->type())
    {
    case DataType::Bit:
        text = val.toBool() ? QStringLiteral("1") : QStringLiteral("0");
        break;
    case DataType::UWord:
    case DataType::UDWord:
        if (frac > 0) {
            const double scaled = val.toUInt() / std::pow(10.0, frac);
            text = QString::number(scaled, 'f', frac);
        } else {
            text = QString::number(val.toUInt());
        }
        break;
    case DataType::SWord:
    case DataType::SDWord:
        if (frac > 0) {
            const double scaled = val.toInt() / std::pow(10.0, frac);
            text = QString::number(scaled, 'f', frac);
        } else {
            text = QString::number(val.toInt());
        }
        break;
    case DataType::Float:
        text = QString::number(val.toFloat(), 'f', frac);
        break;
    default:
        text = QStringLiteral("---");
        break;
    }

    setText(text);
}

bool NumericDisplay::isReadOnly() const
{
    return m_readOnly;
}

void NumericDisplay::setReadOnly(bool newReadOnly)
{
    m_readOnly = newReadOnly;
}




uint16_t NumericDisplay::readAddress() const
{
    return m_readAddress;
}

void NumericDisplay::setReadAddress(uint16_t newReadAddress)
{
    if (m_readAddress == newReadAddress)
        return;
    m_readAddress = newReadAddress;
    emit readAddressChanged();
}

uint16_t NumericDisplay::readAddressBit() const
{
    return m_readAddressBit;
}

void NumericDisplay::setReadAddressBit(uint16_t newReadAddressBit)
{
    if (m_readAddressBit == newReadAddressBit)
        return;
    m_readAddressBit = newReadAddressBit;
    emit readAddressBitChanged();
}

uint16_t NumericDisplay::writeAddress() const
{
    return m_writeAddress;
}

void NumericDisplay::setWriteAddress(uint16_t newWriteAddress)
{
    if (m_writeAddress == newWriteAddress)
        return;
    m_writeAddress = newWriteAddress;
    emit writeAddressChanged();
}

uint16_t NumericDisplay::writeAddressBit() const
{
    return m_writeAddressBit;
}

void NumericDisplay::setWriteAddressBit(uint16_t newWriteAddressBit)
{
    if (m_writeAddressBit == newWriteAddressBit)
        return;
    m_writeAddressBit = newWriteAddressBit;
    emit writeAddressBitChanged();
}
