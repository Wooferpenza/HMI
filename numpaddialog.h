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
    explicit NumpadDialog(QWidget *parent = nullptr, Variable::DataFormat type = Variable::DataFormat::UWord,
                          float min = 0, float max = 100, uint16_t fractional = 0);
    ~NumpadDialog() override;
    void setRange(float min, float max);
    void setType(Variable::DataFormat type);
    void setFractional(uint16_t frac);
    void setCurrentValue(const QString &text);

signals:
    void enter(const QVariant &value);

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
    float m_minimum;
    float m_maximum;
    Variable::DataFormat m_type;
    uint16_t m_fractional = 0;
};

#endif // NUMPADDIALOG_H
