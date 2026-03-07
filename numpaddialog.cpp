#include "numpaddialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QString>
#include "ui_numpaddialog.h"

NumpadDialog::NumpadDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NumpadDialog)
{
    ui->setupUi(this);
    const QRegularExpression numberButtonRe(QStringLiteral(R"(^pushButton([0-9]|Dot)$)"));
    const auto buttons = findChildren<QPushButton*>();
    for (auto *btn : buttons) {
        if (!btn)
            continue;
        if (numberButtonRe.match(btn->objectName()).hasMatch())
            connect(btn, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    }

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
    ui->label->setText(QString::number(minimum) + " ~ " + QString::number(maximum));
}

void NumpadDialog::handleNumberButton()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
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
    QString currentText = ui->lineEdit->text();
    if (currentText.startsWith('-'))
        currentText.remove(0, 1);
    else
        currentText.prepend('-');
    ui->lineEdit->setText(currentText);
}

void NumpadDialog::handleEnterButton()
{
    const QString resultStr = ui->lineEdit->text().trimmed();
    bool ok = false;
    const float result = resultStr.toFloat(&ok);
    if (!ok || resultStr.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите число");
        return;
    }
    if ((result < minimum) || (result > maximum)) {
        QMessageBox::warning(this, "Ошибка", "Значение вне диапазона");
    } else {
        emit enterFloat(result);
        emit enterInt(int(result));
        close();
    }
}
