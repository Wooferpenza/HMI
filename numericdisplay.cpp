#include "numericdisplay.h"

NumericDisplay::NumericDisplay(QWidget *parent)
    : QLineEdit(parent)
{
    connect(&variable, &Variable::valueChanged, this, &NumericDisplay::displayData);
}

void NumericDisplay::inputData(QVariant data)
{
    variable.setValue(data);
}

void NumericDisplay::displayData(QVariant val)
{
    QString text;
    switch (variable.type())
    {
    case DataType::UWord:
    {
        text = QString::number(val.toUInt());
        break;
    }
    case DataType::SWord:
    {
        text = QString::number(val.toInt());
        break;
    }
    case DataType::UDWord:
    {
        text = QString::number(val.toUInt());
        break;
    }
    case DataType::SDWord:
    {
        text = QString::number(val.toInt());
        break;
    }
    case DataType::Float:
    {
        text = QString::number(val.toFloat(), 'f', variable.fractional());
        break;
    }
    default:
        text ="---";
        break;
    }

    this->setText(text);
}
