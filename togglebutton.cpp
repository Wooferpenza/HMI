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
