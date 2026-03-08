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

    switch (variable.format()) {
    case DataFormat::SignedDecimal: {
        text = QString::number(val.toInt());
        break;
    }
    case DataFormat::UnsignedDecimal:
        text = QString::number(val.toUInt());
        break;
    case DataFormat::Hexadecimal:
        text = "0x" + QString::number(val.toUInt(), 16).toUpper().rightJustified(4, '0');
        break;
    case DataFormat::Binary:
        text = "0b" + QString::number(val.toUInt(), 2).rightJustified(16, '0');
        break;
    case DataFormat::Floating: {
        text = QString::number(val.toFloat(), 'f', variable.fractional());

        break;
    }
    default:
        text ="---";
        break;
    }

    this->setText(text);
}
