#include "togglebutton.h"

ToggleButton::ToggleButton(QWidget *parent)
    : QPushButton(parent)
    , m_readVariable(new Variable(this)), m_writeVariable(new Variable(this))
{
    setCheckable(true);
    m_readVariable->setType(DataType::Bit);
    m_readVariable->setMinimum(0);
    m_readVariable->setMaximum(1);
    m_writeVariable->setType(DataType::Bit);
    m_writeVariable->setMinimum(0);
    m_writeVariable->setMaximum(1);
    connect(this, &ToggleButton::readAddressChanged, m_readVariable, &Variable::setAddress);
    connect(this, &ToggleButton::readAddressBitChanged, m_readVariable, &Variable::setAddressBit);
    connect(this, &ToggleButton::writeAddressChanged, m_writeVariable, &Variable::setAddress);
    connect(this, &ToggleButton::writeAddressBitChanged, m_writeVariable, &Variable::setAddressBit);

    connect(m_readVariable, &Variable::valueChanged, this, &ToggleButton::onModbusValueChanged);
    connect(this, &QPushButton::clicked, this, &ToggleButton::onClicked);
    connect(this, &QPushButton::toggled, this, &ToggleButton::refreshStateText);

    refreshStateText();
}

QStringList ToggleButton::stateTexts() const
{
    return m_stateTexts;
}

void ToggleButton::setStateTexts(const QStringList &texts)
{
    m_stateTexts = texts;
    ensureStateTextListLength();
    refreshStateText();
    emit stateTextsChanged();
}

QString ToggleButton::stateTextFallback() const
{
    return m_stateTextFallback;
}

void ToggleButton::setStateTextFallback(const QString &text)
{
    if (m_stateTextFallback == text)
        return;
    m_stateTextFallback = text;
    refreshStateText();
    emit stateTextFallbackChanged();
}


void ToggleButton::onModbusValueChanged(const QVariant &val)
{
    const bool state = val.toBool();
    if (isChecked() != state)
        setChecked(state);
}

void ToggleButton::onClicked()
{
    m_writeVariable->setValue(isChecked());
}

void ToggleButton::refreshStateText()
{
    if (m_stateTexts.isEmpty() && m_stateTextFallback.isEmpty())
        return;

    const int idx = isChecked() ? 1 : 0;
    QString label = m_stateTexts.value(idx);
    if (label.isEmpty())
        label = m_stateTextFallback;
    setText(label);
}

void ToggleButton::mousePressEvent(QMouseEvent *event)
{
  // event->ignore();
    QPushButton::mousePressEvent(event);
}

void ToggleButton::ensureStateTextListLength()
{
    while (m_stateTexts.size() < 2)
        m_stateTexts.append(QString());
    while (m_stateTexts.size() > 2)
        m_stateTexts.removeLast();
}

uint16_t ToggleButton::readAddress() const
{
    return m_readAddress;
}

void ToggleButton::setReadAddress(uint16_t newReadAddress)
{
    if (m_readAddress == newReadAddress)
        return;
    m_readAddress = newReadAddress;
    emit readAddressChanged(m_readAddress);
}

uint16_t ToggleButton::readAddressBit() const
{
    return m_readAddressBit;
}

void ToggleButton::setReadAddressBit(uint16_t newReadAddressBit)
{
    if (m_readAddressBit == newReadAddressBit)
        return;
    m_readAddressBit = newReadAddressBit;
    emit readAddressBitChanged(m_readAddressBit);
}

uint16_t ToggleButton::writeAddress() const
{
    return m_writeAddress;
}

void ToggleButton::setWriteAddress(uint16_t newWriteAddress)
{
    if (m_writeAddress == newWriteAddress)
        return;
    m_writeAddress = newWriteAddress;
    emit writeAddressChanged(m_writeAddress);
}

uint16_t ToggleButton::writeAddressBit() const
{
    return m_writeAddressBit;
}

void ToggleButton::setWriteAddressBit(uint16_t newWriteAddressBit)
{
    if (m_writeAddressBit == newWriteAddressBit)
        return;
    m_writeAddressBit = newWriteAddressBit;
    emit writeAddressBitChanged(m_writeAddressBit);
}

Variable *ToggleButton::readVariable() const
{
    return m_readVariable;
}

Variable *ToggleButton::writeVariable() const
{
    return m_writeVariable;
}
