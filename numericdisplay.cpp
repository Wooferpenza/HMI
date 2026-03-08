#include "numericdisplay.h"

NumericDisplay::NumericDisplay(QWidget *parent): QLineEdit(parent) {
    connect(&variable,&Variable::valueChanged,this,&NumericDisplay::displayData);
}

void NumericDisplay::inputData(QVariant data)
{
           variable.setValue(data);

}

void NumericDisplay::displayData(QVariant val)
{
    float value=val.toFloat();
    this->setText(QString::number(value));
}
