#include "modbussettingsdialog.h"
#include "ui_modbussettingsdialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QRegularExpression>

ModbusSettingsDialog::ModbusSettingsDialog(const ModbusSettings &current, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ModbusSettingsDialog)
{
    ui->setupUi(this);
    ui->lineEditIp->setText(current.ip);
    ui->spinBoxPort->setValue(current.port);
    ui->spinBoxUnitId->setValue(current.unitId);
    ui->spinBoxPollInterval->setValue(current.pollIntervalMs);

    connect(ui->pushButtonApply, &QPushButton::clicked, this, &ModbusSettingsDialog::onApply);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::reject);
}

ModbusSettingsDialog::~ModbusSettingsDialog()
{
    delete ui;
}

ModbusSettings ModbusSettingsDialog::settings() const
{
    ModbusSettings s;
    s.ip = ui->lineEditIp->text().trimmed();
    s.port = ui->spinBoxPort->value();
    s.unitId = ui->spinBoxUnitId->value();
    s.pollIntervalMs = ui->spinBoxPollInterval->value();
    return s;
}

void ModbusSettingsDialog::onApply()
{
    const QString ip = ui->lineEditIp->text().trimmed();
    if (ip.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Введите IP-адрес"));
        return;
    }
    const QRegularExpression ipRe(
        QStringLiteral("^(\\d{1,3}\\.){3}\\d{1,3}$")
    );
    if (!ipRe.match(ip).hasMatch()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Неверный формат IP-адреса"));
        return;
    }
    accept();
}
