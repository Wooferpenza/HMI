#include "momentarybutton.h"

MomentaryButton::MomentaryButton(QWidget *parent)
    : QPushButton(parent)
    , m_writeVariable(new Variable(this))
{
    m_writeVariable->setType(Variable::DataFormat::Bit);
    m_writeVariable->setMinimum(0);
    m_writeVariable->setMaximum(1);
    connect(this, &MomentaryButton::writeAddressChanged, m_writeVariable, &Variable::setAddressStr);
    connect(this, &QPushButton::pressed, this, [this]() {
        m_writeVariable->setValue(true);
    });
    connect(this, &QPushButton::released, this, [this]() {
        m_writeVariable->setValue(false);
    });
}

Variable *MomentaryButton::writeVariable() const
{
    return m_writeVariable;
}