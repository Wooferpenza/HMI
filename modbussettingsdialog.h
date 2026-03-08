#ifndef MODBUSSETTINGSDIALOG_H
#define MODBUSSETTINGSDIALOG_H

#include "modbussettings.h"
#include <QDialog>

namespace Ui {
class ModbusSettingsDialog;
}

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
