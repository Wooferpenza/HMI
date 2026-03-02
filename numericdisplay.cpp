#include "numericdisplay.h"

void NumericDisplay::cheсkRange()
{
    if (mValue<mMinimumValue) mValue=mMinimumValue;
    if (mValue>mMaximumValue) mValue=mMaximumValue;
}

NumericDisplay::NumericDisplay(QWidget *parent) : QLineEdit(parent) {
    mValue=0; mMinimumValue=0; mMaximumValue=100;
}

void NumericDisplay::setValue(float val)
{
    mValue=val;
    NumericDisplay::cheсkRange();
    NumericDisplay::setText(QString::number(mValue,'f',2));
}

void NumericDisplay::setMinimum(float min)
{
    mMinimumValue=min;
}

void NumericDisplay::setMaximum(float max)
{
    mMaximumValue=max;
}

float NumericDisplay::value()
{
    return mValue;
}

float NumericDisplay::minimumValue()
{
    return mMinimumValue;
}

float NumericDisplay::maximumValue()
{
    return mMaximumValue;
}
