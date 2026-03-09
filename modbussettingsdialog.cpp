#include "modbussettingsdialog.h"
#include "ui_modbussettingsdialog.h"
#include <QMessageBox>

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
    const QStringList octets = ip.split(QLatin1Char('.'));
    if (octets.size() != 4) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Неверный формат IP-адреса"));
        return;
    }
    bool valid = true;
    for (const QString &octet : octets) {
        bool ok = false;
        const int val = octet.toInt(&ok);
        if (!ok || val < 0 || val > 255) {
            valid = false;
            break;
        }
    }
    if (!valid) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Неверный формат IP-адреса"));
        return;
    }
    accept();
}
