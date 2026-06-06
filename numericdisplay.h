#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QVariant>
#include "variable.h"
#include "variableproperty.h"

class NumericDisplay : public QLineEdit, public VariableProperty<NumericDisplay>
{
    Q_OBJECT
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, format);
    IMPLEMENT_SHARED_PROPERTY(uint16_t, fractional);
    IMPLEMENT_SHARED_PROPERTY(float, minimum);
    IMPLEMENT_SHARED_PROPERTY(float, maximum);
    IMPLEMENT_SHARED_PROPERTY(QString, readAddress);
    IMPLEMENT_SHARED_PROPERTY(QString, writeAddress);
public:
    enum  TypeData { Bit, UWord, SWord, UDWord, SDWord, Float, String };
    explicit NumericDisplay(QWidget *parent = nullptr);
    Variable *readVariable() const;
    Variable *writeVariable() const;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void inputData(const QVariant &data);
    void displayData(const QVariant &val);

private:
    Variable *m_readVariable = nullptr;
    Variable *m_writeVariable = nullptr;
    bool m_readOnly;
};

#endif // NUMERICDISPLAY_H
