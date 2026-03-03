#include "mainwindow.h"
#include <QDebug>
#include <QLineEdit>
#include <QString>
#include <QTimer>
#include "./ui_mainwindow.h"
#include "numpaddialog.h"
#include "modbusmodel.h"
#include "modbusmanager.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new ModbusModel(this);
    model->addVar("Temp", 100, VarType::Float);
    model->addVar("Temp1", 102, VarType::Float);
   // model->addVar("Fan", 105, VarType::Bool);
    QMap<QString, std::function<void(QVariant)>> bindings;
    bindings["Temp"] = [this](QVariant v) { ui->lineEdit->setValue(v.toDouble()); };
   // bindings["Fan"]  = [this](QVariant v) { ui->chkFan->setChecked(v.toBool()); };
    // Привязываем виджеты к именам Modbus‑переменных
    ui->lineEdit->setProperty("modbusVarName", "Temp");

    manager = new ModbusManager(model, this);
    manager->connectTo("192.168.1.5", 502);
    connect(model, &ModbusModel::variableUpdated, [bindings](const QString &name, QVariant val) {
        if (bindings.contains(name)) bindings[name](val);
    });


    connect(ui->lineEdit, &NumericDisplay::clicked, this, &MainWindow::showNumPad);
    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, manager, &ModbusManager::triggerPoll);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNumPad()
{
    qDebug() << "click!";
    NumericDisplay *display = qobject_cast<NumericDisplay *>(sender());
    if (display) {
        NumpadDialog npd;
        npd.setRange(display->minimumValue(), display->maximumValue());
        connect(&npd, &NumpadDialog::enterFloat, display, &NumericDisplay::setValue);

        // При подтверждении ввода сразу пишем значение в Modbus
        const QString varName = display->property("modbusVarName").toString();
        if (!varName.isEmpty()) {
            connect(&npd, &NumpadDialog::enterFloat, this, [this, varName](float v) {
                manager->writeVariable(varName, v);
            });
        }

        npd.show();
        npd.exec();
    }
}
