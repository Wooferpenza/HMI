#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QVariant>
#include "variable.h"

class NumericDisplay : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly FINAL)
    Q_PROPERTY(Variable *variable READ variable FINAL)
    Q_PROPERTY(uint16_t readAddress READ readAddress WRITE setReadAddress NOTIFY readAddressChanged FINAL)
    Q_PROPERTY(uint16_t readAddressBit READ readAddressBit WRITE setReadAddressBit NOTIFY readAddressBitChanged FINAL)
    Q_PROPERTY(uint16_t writeAddress READ writeAddress WRITE setWriteAddress NOTIFY writeAddressChanged FINAL)
    Q_PROPERTY(uint16_t writeAddressBit READ writeAddressBit WRITE setWriteAddressBit NOTIFY writeAddressBitChanged FINAL)
public:
    explicit NumericDisplay(QWidget *parent = nullptr);
   Variable *variable() const;

    bool isReadOnly() const;
    void setReadOnly(bool newReadOnly);

    uint16_t readAddress() const;
    void setReadAddress(uint16_t newReadAddress);

    uint16_t readAddressBit() const;
    void setReadAddressBit(uint16_t newReadAddressBit);

    uint16_t writeAddress() const;
    void setWriteAddress(uint16_t newWriteAddress);

    uint16_t writeAddressBit() const;
    void setWriteAddressBit(uint16_t newWriteAddressBit);

signals:
    void clicked();


    void readAddressChanged();

    void readAddressBitChanged();

    void writeAddressChanged();

    void writeAddressBitChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void inputData(const QVariant &data);
    void displayData(const QVariant &val);

private:
    Variable *m_variable = nullptr;
    bool m_readOnly;
 //   Variable *m_var;
    uint16_t m_readAddress;
    uint16_t m_readAddressBit;
    uint16_t m_writeAddress;
    uint16_t m_writeAddressBit;
};

#endif // NUMERICDISPLAY_H
