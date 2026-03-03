#ifndef NUMPADDIALOG_H
#define NUMPADDIALOG_H

#include <QDialog>

namespace Ui {
class NumpadDialog;
}

class NumpadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NumpadDialog(QWidget *parent = nullptr);
    ~NumpadDialog();
    void setRange(float min, float max);
signals:
    void enterInt(int);
    void enterFloat(float);
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
    float minimum;
    float maximum;
};

#endif // NUMPADDIALOG_H
