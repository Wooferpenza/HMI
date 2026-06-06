#include "numericdisplay.h"
#include <cmath>

NumericDisplay::NumericDisplay(QWidget *parent)
    : QLineEdit(parent)
    , m_readVariable(new Variable(this)), m_writeVariable(new Variable(this))
{
    m_readVariable->setObjectName("read");
    m_writeVariable->setObjectName("write");
    connect(this, &NumericDisplay::formatChanged, m_readVariable, &Variable::setFormat);
    connect(this, &NumericDisplay::formatChanged, m_writeVariable, &Variable::setFormat);
    connect(this, &NumericDisplay::fractionalChanged, m_readVariable, &Variable::setFractional);
    connect(this, &NumericDisplay::fractionalChanged, m_writeVariable, &Variable::setFractional);
    connect(this, &NumericDisplay::minimumChanged, m_readVariable, &Variable::setMinimum);
    connect(this, &NumericDisplay::minimumChanged, m_writeVariable, &Variable::setMinimum);
    connect(this, &NumericDisplay::maximumChanged, m_readVariable, &Variable::setMaximum);
    connect(this, &NumericDisplay::maximumChanged, m_writeVariable, &Variable::setMaximum);
    connect(this, &NumericDisplay::readAddressChanged, m_readVariable, &Variable::setAddressStr);
    connect(this, &NumericDisplay::writeAddressChanged, m_writeVariable, &Variable::setAddressStr);
    connect(m_readVariable, &Variable::valueChanged, this, &NumericDisplay::displayData);
}

Variable *NumericDisplay::readVariable() const
{
    return m_readVariable;
}

Variable *NumericDisplay::writeVariable() const
{
    return m_writeVariable;
}
void NumericDisplay::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
    if (!isReadOnly())
        emit clicked();
}

void NumericDisplay::inputData(const QVariant &data)
{
    m_writeVariable->setValue(data);
}

void NumericDisplay::displayData(const QVariant &val)
{
    const uint16_t frac = m_readVariable->fractional();
    QString text;

    switch (m_readVariable->type())
    {
    case Variable::DataFormat::Bit:
        text = val.toBool() ? QStringLiteral("1") : QStringLiteral("0");
        break;
    case Variable::DataFormat::UWord:
    case Variable::DataFormat::UDWord:
        if (frac > 0) {
            const double scaled = val.toUInt() / std::pow(10.0, frac);
            text = QString::number(scaled, 'f', frac);
        } else {
            text = QString::number(val.toUInt());
        }
        break;
    case Variable::DataFormat::SWord:
    case Variable::DataFormat::SDWord:
        if (frac > 0) {
            const double scaled = val.toInt() / std::pow(10.0, frac);
            text = QString::number(scaled, 'f', frac);
        } else {
            text = QString::number(val.toInt());
        }
        break;
    case Variable::DataFormat::Float:
        text = QString::number(val.toFloat(), 'f', frac);
        break;
    default:
        text = QStringLiteral("---");
        break;
    }

    setText(text);
}
