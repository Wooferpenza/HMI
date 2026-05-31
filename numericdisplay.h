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
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    Q_PROPERTY(TypeData type READ type WRITE settype NOTIFY typeChanged FINAL)
    Q_PROPERTY(uint16_t readAddress READ readAddress WRITE setReadAddress NOTIFY readAddressChanged FINAL)
    Q_PROPERTY(uint16_t readAddressBit READ readAddressBit WRITE setReadAddressBit NOTIFY readAddressBitChanged FINAL)
    Q_PROPERTY(uint16_t writeAddress READ writeAddress WRITE setWriteAddress NOTIFY writeAddressChanged FINAL)
    Q_PROPERTY(uint16_t writeAddressBit READ writeAddressBit WRITE setWriteAddressBit NOTIFY writeAddressBitChanged FINAL)
public:
    enum  TypeData { Bit, UWord, SWord, UDWord, SDWord, Float, String };
    explicit NumericDisplay(QWidget *parent = nullptr);
    Variable *readVariable() const;
    Variable *writeVariable() const;

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

    TypeData type() const;
    void settype(const TypeData &newType);

signals:
    void clicked();


    void readAddressChanged(uint16_t);

    void readAddressBitChanged(uint16_t);

    void writeAddressChanged(uint16_t);

    void writeAddressBitChanged(uint16_t);

    void typeChanged();

protected:
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void inputData(const QVariant &data);
    void displayData(const QVariant &val);

private:
    Variable *m_readVariable = nullptr;
    Variable *m_writeVariable = nullptr;
    bool m_readOnly;
    uint16_t m_readAddress;
    uint16_t m_readAddressBit;
    uint16_t m_writeAddress;
    uint16_t m_writeAddressBit;
    TypeData m_type;
};

#endif // NUMERICDISPLAY_H
