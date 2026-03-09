#include "numpaddialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QString>
#include <cmath>
#include "ui_numpaddialog.h"

NumpadDialog::NumpadDialog(QWidget *parent, DataType type, float min, float max, uint16_t fractional)
    : QDialog(parent)
    , ui(new Ui::NumpadDialog)
{
    ui->setupUi(this);
    const QRegularExpression numberButtonRe(QStringLiteral(R"(^pushButton([0-9]|Dot)$)"));
    const auto buttons = findChildren<QPushButton*>();
    for (auto *btn : buttons) {
        if (!btn)
            continue;
        btn->setFocusPolicy(Qt::NoFocus);
        if (numberButtonRe.match(btn->objectName()).hasMatch())
            connect(btn, &QPushButton::clicked, this, &NumpadDialog::handleNumberButton);
    }

    connect(ui->pushButtonCLR, &QPushButton::clicked, this, &NumpadDialog::handleClearButton);
    connect(ui->pushButtonDEL, &QPushButton::clicked, this, &NumpadDialog::handleDeleteButton);
    connect(ui->pushButtonLeft, &QPushButton::clicked, this, &NumpadDialog::handleLeftButton);
    connect(ui->pushButtonRight, &QPushButton::clicked, this, &NumpadDialog::handleRightButton);
    connect(ui->pushButtonPlusMinus, &QPushButton::clicked, this, &NumpadDialog::handleMinusButton);
    connect(ui->pushButtonENT, &QPushButton::clicked, this, &NumpadDialog::handleEnterButton);
    setType(type);
    setFractional(fractional);
    setRange(min, max);
}



NumpadDialog::~NumpadDialog()
{
    delete ui;
}

void NumpadDialog::setRange(float min, float max)
{
    m_minimum = min;
    m_maximum = max;
    ui->label->setText(QString::number(m_minimum) + " ~ " + QString::number(m_maximum));
}

void NumpadDialog::setType(DataType type)
{
    m_type = type;
}

void NumpadDialog::setFractional(uint16_t frac)
{
    m_fractional = frac;
}

void NumpadDialog::setCurrentValue(const QString &text)
{
    ui->lineEdit->setText(text);
    ui->lineEdit->selectAll();
    ui->lineEdit->setFocus();
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
}

void NumpadDialog::handleRightButton()
{
    ui->lineEdit->cursorForward(false, 1);
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
    QVariant result;
    const bool intWithFrac = m_fractional > 0
        && m_type != DataType::Float
        && m_type != DataType::Bit;

    switch (m_type)
    {
    case DataType::Bit:
        if (resultStr == QLatin1String("0") || resultStr == QLatin1String("1")) {
            result = (resultStr == QLatin1String("1"));
            ok = true;
        }
        break;
    case DataType::Float:
        result = resultStr.toFloat(&ok);
        break;
    case DataType::UWord:
    case DataType::SWord:
    case DataType::UDWord:
    case DataType::SDWord:
        if (intWithFrac) {
            const double entered = resultStr.toDouble(&ok);
            if (ok) {
                const double scale = std::pow(10.0, m_fractional);
                const long long raw = static_cast<long long>(std::round(entered * scale));
                result = static_cast<int>(raw);
            }
        } else {
            switch (m_type) {
            case DataType::UWord:  result = resultStr.toUShort(&ok); break;
            case DataType::SWord:  result = resultStr.toShort(&ok);  break;
            case DataType::UDWord: result = resultStr.toUInt(&ok);   break;
            case DataType::SDWord: result = resultStr.toInt(&ok);    break;
            default: break;
            }
        }
        break;
    default:
        break;
    }

    if (!ok || resultStr.isEmpty()) {
        const QString msg = (m_type == DataType::Bit)
            ? tr("Введите 0 или 1")
            : tr("Введите число");
        QMessageBox::warning(this, tr("Ошибка"), msg);
        return;
    }

    const float displayValue = intWithFrac
        ? static_cast<float>(result.toInt() / std::pow(10.0, m_fractional))
        : result.toFloat();

    if (displayValue < m_minimum || displayValue > m_maximum) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Значение вне диапазона"));
    } else {
        emit enter(result);
        close();
    }
}
