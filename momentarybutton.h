#ifndef MOMENTARYBUTTON_H
#define MOMENTARYBUTTON_H

#include <QPushButton>
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

private:
    Variable *m_writeVariable = nullptr;
    Variable *m_readVariable = nullptr;
};

#endif // MOMENTARYBUTTON_H
