#ifndef MOMENTARYBUTTON_H
#define MOMENTARYBUTTON_H

#include <QPushButton>
#include "variable.h"
#include "variableproperty.h"
class MomentaryButton : public QPushButton, public VariableProperty <MomentaryButton>
{
    Q_OBJECT
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, writeAddress);
public:
    explicit MomentaryButton(QWidget *parent = nullptr);
    Variable *writeVariable() const;
signals:

private:
    Variable *m_writeVariable = nullptr;
};

#endif // MOMENTARYBUTTON_H
