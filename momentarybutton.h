#ifndef MOMENTARYBUTTON_H
#define MOMENTARYBUTTON_H

#include <QPushButton>
#include "variable.h"

class MomentaryButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variable *variable READ variable FINAL)
public:
    explicit MomentaryButton(QWidget *parent = nullptr);
    Variable *variable() const;

private:
    Variable *m_variable = nullptr;
};

#endif // MOMENTARYBUTTON_H
