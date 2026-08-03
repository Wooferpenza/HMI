#ifndef MOMENTARYBUTTON_H
#define MOMENTARYBUTTON_H

#include <QPushButton>
#include <QEvent>
#include "variable.h"
#include "variableproperty.h"
class MomentaryButton : public QPushButton, public VariableProperty <MomentaryButton>
{
    Q_OBJECT
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, writeAddress);
    IMPLEMENT_SHARED_PROPERTY(QString, readAddress);
public:
    explicit MomentaryButton(QWidget *parent = nullptr);
    Variable *writeVariable() const;
    Variable *readVariable() const;

signals:
private slots:
      void onModbusValueChanged(const QVariant &val);
private:
    Variable *m_writeVariable = nullptr;
    Variable *m_readVariable = nullptr;
protected:
 //   bool event(QEvent *event) override;
};

#endif // MOMENTARYBUTTON_H
