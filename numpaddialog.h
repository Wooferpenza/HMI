#ifndef NUMPADDIALOG_H
#define NUMPADDIALOG_H

#include <QDialog>
#include "variable.h"
namespace Ui {
class NumpadDialog;
}

class NumpadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit   NumpadDialog(QWidget *parent = nullptr,DataType type=DataType::UWord,float min=0, float max=100);
    ~NumpadDialog();
    void setRange(float min, float max);
    void setType(DataType);
signals:
    void enterInt(int);
    void enterFloat(float);
    void enter(QVariant);
private slots:
    void handleNumberButton();
    void handleClearButton();
    void handleDeleteButton();
    void handleLeftButton();
    void handleRightButton();
    void handleMinusButton();
    void handleEnterButton();

private:
    Ui::NumpadDialog *ui;
    float mMinimum;
    float mMaximum;
    DataType mType;
};

#endif // NUMPADDIALOG_H
