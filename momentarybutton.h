#ifndef MOMENTARYBUTTON_H
#define MOMENTARYBUTTON_H

#include <QPushButton>
#include "variable.h"

class MomentaryButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    Q_PROPERTY(uint16_t writeAddress READ writeAddress WRITE setWriteAddress NOTIFY writeAddressChanged FINAL)
    Q_PROPERTY(uint16_t writeAddressBit READ writeAddressBit WRITE setWriteAddressBit NOTIFY writeAddressBitChanged FINAL)
public:
    explicit MomentaryButton(QWidget *parent = nullptr);
    Variable *writeVariable() const;

    uint16_t writeAddress() const;
    void setWriteAddress(uint16_t newWriteAddress);

    uint16_t writeAddressBit() const;
    void setWriteAddressBit(uint16_t newWriteAddressBit);

signals:
    void writeAddressChanged(uint16_t);
    void writeAddressBitChanged(uint16_t);

private:
    Variable *m_writeVariable = nullptr;
    uint16_t m_writeAddress;
    uint16_t m_writeAddressBit;
};

#endif // MOMENTARYBUTTON_H
