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
    model->addVar("Temp1", 104, VarType::Float);
    // Привязываем виджеты к именам Modbus‑переменных
    ui->lineEdit->setProperty("modbusVarName", "Temp");
    ui->lineEdit2->setProperty("modbusVarName", "Temp");
  //  connect(model, &ModbusModel::variableUpdated, this, &MainWindow::displayUpdate);
    connectDisplay();
    manager = new ModbusManager(model, this);
    manager->connectTo("192.168.1.5", 502);

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

void MainWindow::displayUpdate(QString name, QVariant val)
{
    QList<NumericDisplay*> displ = this->findChildren<NumericDisplay*>();
    for (auto i=displ.begin();i!=displ.end();i++)
    {
        QString VarName=(*i)->property("modbusVarName").toString();

            if (VarName==name)
            {
                (*i)->setValue(val.toDouble());
            }

    }
}
void MainWindow::connectDisplay()
{
    QList<NumericDisplay*> displ = this->findChildren<NumericDisplay*>();
    for (auto i=displ.begin();i!=displ.end();i++)
    {
        QString VarName=(*i)->property("modbusVarName").toString();
        ModbusVar *var = model->findVariable(VarName);

        connect(var, &ModbusVar::update, (*i), &NumericDisplay::setValue);

    }


}
