#include "mainwindow.h"
#include <QCheckBox>
#include <QSettings>
#include <QTimer>
#include "ui_mainwindow.h"
#include "numericdisplay.h"
#include "numpaddialog.h"
#include "modbussettingsdialog.h"
#include "modbusmodel.h"
#include "modbusmanager.h"

namespace {
constexpr const char kModbusVarNameProp[] = "modbusVarName";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings s;
    const QString ip = s.value("modbus/ip", "192.168.1.5").toString();
    const int port = s.value("modbus/port", 502).toInt();
    const int unitId = s.value("modbus/unitId", 1).toInt();
    int pollMs = s.value("modbus/pollIntervalMs", 1000).toInt();
    if (pollMs < 50)
        pollMs = 50;

    if (!s.contains("modbus/ip")) s.setValue("modbus/ip", ip);
    if (!s.contains("modbus/port")) s.setValue("modbus/port", port);
    if (!s.contains("modbus/unitId")) s.setValue("modbus/unitId", unitId);
    if (!s.contains("modbus/pollIntervalMs")) s.setValue("modbus/pollIntervalMs", pollMs);

    model = new ModbusModel(this);
    model->addVar("Temp", 100, VarType::Float);
    model->addVar("Temp1", 104, VarType::Float);
    model->addVar("Counter", 110, VarType::Word);
    model->addVar("Total", 111, VarType::DWord);
    model->addVar("Flag", 111, VarType::Bool, 0);   // бит 0 регистра 113
    model->addVar("Flag1", 111, VarType::Bool, 1);  // бит 1 регистра 113

    ui->lineEdit->setProperty(kModbusVarNameProp, "Temp");
    ui->lineEdit2->setProperty(kModbusVarNameProp, "Temp1");
    ui->lineEditCounter->setProperty(kModbusVarNameProp, "Counter");
    ui->lineEditTotal->setProperty(kModbusVarNameProp, "Total");
    ui->checkBoxFlag->setProperty(kModbusVarNameProp, "Flag");
    ui->checkBoxFlag1->setProperty(kModbusVarNameProp, "Flag1");

    ui->lineEditCounter->variable.setMinimum(-100);
    ui->lineEditCounter->variable.setMaximum(65535);
    ui->lineEditCounter->variable.setFormat(DataFormat::Floating);
    // ui->lineEditCounter->setDecimals(0);
    // ui->lineEditTotal->setMinimum(0);
    // ui->lineEditTotal->setMaximum(2147483647.0);

    connectDisplay();
    manager = new ModbusManager(model, this);
    manager->setUnitId(unitId);
    connect(manager, &ModbusManager::connectionStateChanged, this, [this](const QString &stateText) {
        ui->statusbar->showMessage(stateText, 0);
    });
    connect(manager, &ModbusManager::lastError, this, [this](const QString &errorText) {
        ui->statusbar->showMessage(tr("Ошибка Modbus: %1").arg(errorText), 5000);
    });
    manager->connectTo(ip, port);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, manager, &ModbusManager::triggerPoll);
    timer->start(pollMs);

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
    NumpadDialog npd(this,display->variable.format(),display->variable.minimum(),display->variable.maximum());
    connect(&npd, &NumpadDialog::enter, display,&NumericDisplay::inputData);
    npd.exec();
}

void MainWindow::showModbusSettings()
{
    QSettings s;
    ModbusSettings current;
    current.ip = s.value("modbus/ip", "192.168.1.5").toString();
    current.port = s.value("modbus/port", 502).toInt();
    current.unitId = s.value("modbus/unitId", 1).toInt();
    current.pollIntervalMs = s.value("modbus/pollIntervalMs", 1000).toInt();

    ModbusSettingsDialog dlg(current, this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const ModbusSettings newSettings = dlg.settings();
    int pollMs = newSettings.pollIntervalMs;
    if (pollMs < 50)
        pollMs = 50;

    s.setValue("modbus/ip", newSettings.ip);
    s.setValue("modbus/port", newSettings.port);
    s.setValue("modbus/unitId", newSettings.unitId);
    s.setValue("modbus/pollIntervalMs", pollMs);

    manager->setUnitId(newSettings.unitId);
    manager->connectTo(newSettings.ip, newSettings.port);

    timer->stop();
    timer->start(pollMs);
}

void MainWindow::connectDisplay()
{
    const auto displays = findChildren<NumericDisplay*>();
    for (auto *display : displays) {
        const QString varName = display->property(kModbusVarNameProp).toString();
        ModbusVar *var = varName.isEmpty() ? nullptr : model->findVariable(varName);
        if (var) {
            //connect(var, &ModbusVar::valueChanged, display, qOverload<const QVariant &>(&NumericDisplay::setValue));
        }
        connect(display, &NumericDisplay::clicked, this, &MainWindow::showNumPad);
    }

    const auto checkBoxes = findChildren<QCheckBox*>();
    for (auto *cb : checkBoxes) {
        const QString varName = cb->property(kModbusVarNameProp).toString();
        if (varName.isEmpty())
            continue;
        ModbusVar *var = model->findVariable(varName);
        if (!var)
            continue;
        connect(var, &ModbusVar::valueChanged, cb, [cb](const QVariant &v) {
            cb->setChecked(v.toBool());
        });
        connect(cb, &QCheckBox::toggled, this, [this, varName](bool checked) {
            manager->writeVariable(varName, checked);
        });
    }
}
