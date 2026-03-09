#include "momentarybutton.h"

MomentaryButton::MomentaryButton(QWidget *parent)
    : QPushButton(parent)
    , m_variable(new Variable(this))
{
    m_variable->setType(DataType::Bit);
    m_variable->setMinimum(0);
    m_variable->setMaximum(1);

    connect(this, &QPushButton::pressed, this, [this]() {
        m_variable->setValue(true);
    });
    connect(this, &QPushButton::released, this, [this]() {
        m_variable->setValue(false);
    });
}

Variable *MomentaryButton::variable() const
{
    return m_variable;
}
