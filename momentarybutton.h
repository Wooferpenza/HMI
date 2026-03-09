#ifndef MOMENTARYBUTTON_H
#define MOMENTARYBUTTON_H

#include <QPushButton>
#include "variable.h"

class MomentaryButton : public QPushButton
{
    Q_OBJECT
public:
    explicit MomentaryButton(QWidget *parent = nullptr);
    Variable *variable() const;

private:
    Variable *m_variable = nullptr;
};

#endif // MOMENTARYBUTTON_H
