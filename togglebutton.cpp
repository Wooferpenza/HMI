#include "togglebutton.h"

ToggleButton::ToggleButton(QWidget *parent)
    : QPushButton(parent)
    , m_readVariable(new Variable(this)), m_writeVariable(new Variable(this))
{
    setCheckable(true);
    connect(this, &ToggleButton::formatChanged, m_readVariable, &Variable::setFormat);
    connect(this, &ToggleButton::formatChanged, m_writeVariable, &Variable::setFormat);
    connect(this, &ToggleButton::fractionalChanged, m_readVariable, &Variable::setFractional);
    connect(this, &ToggleButton::fractionalChanged, m_writeVariable, &Variable::setFractional);
    connect(this, &ToggleButton::minimumChanged, m_readVariable, &Variable::setMinimum);
    connect(this, &ToggleButton::minimumChanged, m_writeVariable, &Variable::setMinimum);
    connect(this, &ToggleButton::maximumChanged, m_readVariable, &Variable::setMaximum);
    connect(this, &ToggleButton::maximumChanged, m_writeVariable, &Variable::setMaximum);
    connect(this, &ToggleButton::readAddressChanged, m_readVariable, &Variable::setAddressStr);
    connect(this, &ToggleButton::writeAddressChanged, m_writeVariable, &Variable::setAddressStr);

    m_readVariable->setType(Variable::DataFormat::Bit);
    m_readVariable->setMinimum(0);
    m_readVariable->setMaximum(1);
    m_writeVariable->setType(Variable::DataFormat::Bit);
    m_writeVariable->setMinimum(0);
    m_writeVariable->setMaximum(1);

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

Variable *ToggleButton::readVariable() const
{
    return m_readVariable;
}

Variable *ToggleButton::writeVariable() const
{
    return m_writeVariable;
}
