#ifndef MODBUSSETTINGSDIALOG_H
#define MODBUSSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class ModbusSettingsDialog;
}

struct ModbusSettings {
    QString ip;
    int port = 502;
    int unitId = 1;
    int pollIntervalMs = 1000;
};

class ModbusSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusSettingsDialog(const ModbusSettings &current, QWidget *parent = nullptr);
    ~ModbusSettingsDialog();

    ModbusSettings settings() const;

private slots:
    void onApply();

private:
    Ui::ModbusSettingsDialog *ui;
};

#endif // MODBUSSETTINGSDIALOG_H
