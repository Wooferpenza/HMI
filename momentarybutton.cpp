#include "momentarybutton.h"

MomentaryButton::MomentaryButton(QWidget *parent)
    : QPushButton(parent)
    , m_writeVariable(new Variable(this))
{
    m_writeVariable->setType(DataType::Bit);
    m_writeVariable->setMinimum(0);
    m_writeVariable->setMaximum(1);
    connect(this, &MomentaryButton::writeAddressChanged, m_writeVariable, &Variable::setAddress);
    connect(this, &MomentaryButton::writeAddressBitChanged, m_writeVariable, &Variable::setAddressBit);
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

uint16_t MomentaryButton::writeAddress() const
{
    return m_writeAddress;
}

void MomentaryButton::setWriteAddress(uint16_t newWriteAddress)
{
    if (m_writeAddress == newWriteAddress)
        return;
    m_writeAddress = newWriteAddress;
    emit writeAddressChanged(m_writeAddress);
}

uint16_t MomentaryButton::writeAddressBit() const
{
    return m_writeAddressBit;
}

void MomentaryButton::setWriteAddressBit(uint16_t newWriteAddressBit)
{
    if (m_writeAddressBit == newWriteAddressBit)
        return;
    m_writeAddressBit = newWriteAddressBit;
    emit writeAddressBitChanged(m_writeAddressBit);
}
