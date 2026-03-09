#include "mainwindow.h"
#include "modbusmanager.h"
#include "modbusmodel.h"
#include "modbussettings.h"
#include "modbussettingsdialog.h"
#include "numericdisplay.h"
#include "numpaddialog.h"
#include "ui_mainwindow.h"
#include "variable.h"
#include <QSettings>

namespace {

// Привязка виджет → Modbus: имя виджета (objectName в UI), имя переменной, адрес регистра.
// Опционально: тип, формат, лимиты — задаются только если нужны не по умолчанию.
struct VariableBinding {
    const char *widgetName;
    const char *varName;
    quint16 address;
    DataType type = DataType::UWord;
    float min = 0.0f;
    float max = 100.0f;
    int fractional = 0;
    bool hasVariableConfig = false; // true — применить type/min/max/fractional
};

const VariableBinding kVariableBindings[] = {
    {"lineEdit", "Temp", 100, DataType::UWord, -65539, 655350, 2, true},
    {"lineEditCounter", "Counter", 102, DataType::Float, -10.0f, 65535.0f, 2, true},
};

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto displays = findChildren<NumericDisplay*>();
    for (NumericDisplay *display : displays) {
        if (!display)
            continue;
        connect(display, &NumericDisplay::clicked, this, &MainWindow::showNumPad);
    }

    QSettings s;
    ModbusSettings modbusCfg = ModbusSettings::load(s);

    model = new ModbusModel(this);

    for (const VariableBinding &b : kVariableBindings) {
        auto *display = findChild<NumericDisplay *>(QLatin1String(b.widgetName));
        if (!display)
            continue;
        display->variable.setName(QLatin1String(b.varName));
        if (b.hasVariableConfig) {
            display->variable.setType(b.type);
            display->variable.setMinimum(b.min);
            display->variable.setMaximum(b.max);
            display->variable.setFractional(static_cast<uint16_t>(b.fractional));
        }
        model->addVar(&display->variable, b.address);
    }

    manager = new ModbusManager(model, this);
    connect(manager, &ModbusManager::connectionStateChanged, this, [this](const QString &stateText) {
        ui->statusbar->showMessage(stateText, 0);
    });
    connect(manager, &ModbusManager::lastError, this, [this](const QString &errorText) {
        ui->statusbar->showMessage(tr("Ошибка Modbus: %1").arg(errorText), 5000);
    });
    manager->applySettings(modbusCfg);

    connect(ui->actionModbusSettings, &QAction::triggered, this, &MainWindow::showModbusSettings);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNumPad()
{
    auto *display = qobject_cast<NumericDisplay *>(sender());
    if (!display) return;
    NumpadDialog npd(this,display->variable.type(),display->variable.minimum(),display->variable.maximum());
    connect(&npd, &NumpadDialog::enter, display,&NumericDisplay::inputData);
    npd.exec();
}

void MainWindow::showModbusSettings()
{
    QSettings s;
    ModbusSettingsDialog dlg(ModbusSettings::load(s), this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const ModbusSettings newSettings = dlg.settings();
    newSettings.save(s);
    manager->applySettings(newSettings);
}


