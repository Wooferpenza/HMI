#include "momentarybutton.h"

MomentaryButton::MomentaryButton(QWidget *parent)
    : QPushButton(parent)
    , m_writeVariable(new Variable(this))
    , m_readVariable(new Variable(this))
{
    m_writeVariable->setType(Variable::DataFormat::Bit);
    m_writeVariable->setMinimum(0);
    m_writeVariable->setMaximum(1);
    m_readVariable->setType(Variable::DataFormat::Bit);
    m_readVariable->setMinimum(0);
    m_readVariable->setMaximum(1);
    connect(this, &MomentaryButton::clicked, this, [this](){setChecked(!isChecked());});
    connect(this, &MomentaryButton::writeAddressChanged, m_writeVariable, &Variable::setAddressStr);
    connect(this, &MomentaryButton::readAddressChanged, m_readVariable, &Variable::setAddressStr);
    connect(this, &QPushButton::pressed, this, [this]() { m_writeVariable->setValue(true);});
    connect(this, &QPushButton::released, this, [this]() { m_writeVariable->setValue(false);});
    connect(m_readVariable, &Variable::valueChanged,this, &MomentaryButton::onModbusValueChanged);
    this->setCheckable(true);
}

Variable *MomentaryButton::writeVariable() const
{
    return m_writeVariable;
}
Variable *MomentaryButton::readVariable() const
{
    return m_readVariable;
}

void MomentaryButton::onModbusValueChanged(const QVariant &val)
{
    const bool state = val.toBool();
    setChecked(state);
}
