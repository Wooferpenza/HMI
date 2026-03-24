#include "togglebutton.h"

ToggleButton::ToggleButton(QWidget *parent)
    : QPushButton(parent)
    , m_variable(new Variable(this))
{
    setCheckable(true);

    m_variable->setType(DataType::Bit);
    m_variable->setMinimum(0);
    m_variable->setMaximum(1);

    connect(m_variable, &Variable::valueChanged,
            this, &ToggleButton::onModbusValueChanged);
    connect(this, &QPushButton::clicked,
            this, &ToggleButton::onClicked);
    connect(this, &QPushButton::toggled,
            this, &ToggleButton::refreshStateText);

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

Variable *ToggleButton::variable() const
{
    return m_variable;
}

void ToggleButton::onModbusValueChanged(const QVariant &val)
{
    const bool state = val.toBool();
    if (isChecked() != state)
        setChecked(state);
}

void ToggleButton::onClicked()
{
    m_variable->setValue(isChecked());
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

void ToggleButton::ensureStateTextListLength()
{
    while (m_stateTexts.size() < 2)
        m_stateTexts.append(QString());
    while (m_stateTexts.size() > 2)
        m_stateTexts.removeLast();
}
