#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include "variable.h"

class ToggleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ToggleButton(QWidget *parent = nullptr);
    Variable *variable() const;

private slots:
    void onModbusValueChanged(const QVariant &val);
    void onClicked();

private:
    Variable *m_variable = nullptr;
};

#endif // TOGGLEBUTTON_H
