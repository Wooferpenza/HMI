#include "modbussettingsdialog.h"
#include "ui_modbussettingsdialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>

// Реализация ModbusSettings::load/save (ключи и значения по умолчанию)

namespace {

constexpr const char kModbusIp[] = "modbus/ip";
constexpr const char kModbusPort[] = "modbus/port";
constexpr const char kModbusUnitId[] = "modbus/unitId";
constexpr const char kModbusPollIntervalMs[] = "modbus/pollIntervalMs";
constexpr const char kDefaultIp[] = "192.168.1.5";
constexpr int kDefaultPort = 502;
constexpr int kDefaultUnitId = 1;
constexpr int kDefaultPollIntervalMs = 1000;

} // namespace

ModbusSettings ModbusSettings::load(QSettings &s)
{
    ModbusSettings out;
    out.ip = s.value(kModbusIp, kDefaultIp).toString();
    out.port = s.value(kModbusPort, kDefaultPort).toInt();
    out.unitId = s.value(kModbusUnitId, kDefaultUnitId).toInt();
    out.pollIntervalMs = s.value(kModbusPollIntervalMs, kDefaultPollIntervalMs).toInt();
    return out;
}

void ModbusSettings::save(QSettings &s) const
{
    s.setValue(kModbusIp, ip);
    s.setValue(kModbusPort, port);
    s.setValue(kModbusUnitId, unitId);
    s.setValue(kModbusPollIntervalMs, pollIntervalMs);
}

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
