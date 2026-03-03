#include "numpaddialog.h"
#include <QMessageBox>
#include <QString>
#include "ui_numpaddialog.h"

NumpadDialog::NumpadDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NumpadDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton0, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton1, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton2, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton3, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton4, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton5, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton6, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton7, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton8, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButton9, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    connect(ui->pushButtonDot, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);

    connect(ui->pushButtonCLR, &QPushButton::clicked, this, &NumpadDialog::handleClearButton);
    connect(ui->pushButtonDEL, &QPushButton::clicked, this, &NumpadDialog::handleDeleteButton);
    connect(ui->pushButtonLeft, &QPushButton::clicked, this, &NumpadDialog::handleLeftButton);
    connect(ui->pushButtonRight, &QPushButton::clicked, this, &NumpadDialog::handleRightButton);
    connect(ui->pushButtonPlusMinus, &QPushButton::clicked, this, &NumpadDialog::handleMinusButton);
    connect(ui->pushButtonENT, &QPushButton::clicked, this, &NumpadDialog::handleEnterButton);
}

NumpadDialog::~NumpadDialog()
{
    delete ui;
}

void NumpadDialog::setRange(float min, float max)
{
    minimum = min;
    maximum = max;
    ui->label->setText(QString::number(minimum) + " - " + QString::number(maximum));
}

void NumpadDialog::handleNumberButton()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        qDebug() << "Нажата кнопка:" << button->text();
        ui->lineEdit->insert(button->text());
    }
}

void NumpadDialog::handleClearButton()
{
    ui->lineEdit->clear();
}

void NumpadDialog::handleDeleteButton()
{
    ui->lineEdit->backspace();
}

void NumpadDialog::handleLeftButton()
{
    ui->lineEdit->cursorBackward(false, 1);
    ui->lineEdit->setFocus();
}

void NumpadDialog::handleRightButton()
{
    ui->lineEdit->cursorForward(false, 1);
    ui->lineEdit->setFocus();
}

void NumpadDialog::handleMinusButton()
{
    QString currentText(ui->lineEdit->text());
    if (currentText.at(0) == "-") {
        currentText.removeFirst();
    } else {
        currentText.push_front("-");
    }
    ui->lineEdit->setText(currentText);
}

void NumpadDialog::handleEnterButton()
{
    QString resultStr = ui->lineEdit->text();
    float result = resultStr.toFloat();
    if ((result < minimum) or (result > maximum)) {
        QMessageBox::warning(this, "Error", "Exceed the limit");
    } else {
        emit enterFloat(result);
        emit enterInt(int(result));
        close();
    }
}
