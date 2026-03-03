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
    model = new ModbusModel();
    model->addVar("Temp", 100, VarType::Float);
    model->addVar("Temp1", 102, VarType::Float);
   // model->addVar("Fan", 105, VarType::Bool);
    QMap<QString, std::function<void(QVariant)>> bindings;
    bindings["Temp"] = [this](QVariant v) { ui->lineEdit->setValue(v.toDouble()); };
   // bindings["Fan"]  = [this](QVariant v) { ui->chkFan->setChecked(v.toBool()); };
    manager = new ModbusManager(model);
    manager->connectTo("192.168.1.5", 502);
    connect(model, &ModbusModel::variableUpdated, [bindings](const QString &name, QVariant val) {
        if (bindings.contains(name)) bindings[name](val);
    });


    connect(ui->lineEdit, &NumericDisplay::clicked, this, &MainWindow::showNumPad);
    timer = new QTimer();
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
        npd.show();
        npd.exec();
    }
}
