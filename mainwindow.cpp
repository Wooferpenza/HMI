#include "mainwindow.h"
#include "modbusmanager.h"
#include "modbusmodel.h"
#include "modbussettings.h"
#include "modbussettingsdialog.h"
#include "numericdisplay.h"
#include "numpaddialog.h"
#include "ui_mainwindow.h"
#include "exitdialog.h"
#include <QButtonGroup>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include "QDebug"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    alarmDialog = new AlarmDialog(this);
    alarm.set_readAddress("D15");
    connect(&alarm,&Alarm::alarmChanged,this,&MainWindow::on_alarmChanged);
    //connect(&alarm,&Alarm::alarmChanged,this,&MainWindow::on_alarmChanged);
    setAttribute(Qt::WA_AcceptTouchEvents, true);

    auto *navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);
    navGroup->addButton(ui->navButtonOverview, 0);
    navGroup->addButton(ui->navButtonJournal, 1);
   // navGroup->addButton(ui->navButtonService, 2);
    connect(navGroup, &QButtonGroup::idClicked,
            ui->stackedWidgetPages, &QStackedWidget::setCurrentIndex);
    connect(ui->stackedWidgetPages, &QStackedWidget::currentChanged,
            this, [navGroup](int index) {
                if (QAbstractButton *b = navGroup->button(index)) {
                    if (!b->isChecked())
                        b->setChecked(true);
                }
            });

    const auto displays = findChildren<NumericDisplay*>();
    for (NumericDisplay *display : displays) {
        if (!display)
            continue;
        connect(display, &NumericDisplay::clicked, this, &MainWindow::showNumPad);
    }



    QSettings s;
    ModbusSettings modbusCfg = ModbusSettings::load(s);
    model = new ModbusModel(this);

    const auto myWidgets = findChildren<QWidget*>();
    for (QWidget *widget : myWidgets) {
        if (!widget) continue;
        if (widget->property("readVariable").isValid())
        {
            Variable *pVariable=widget->property("readVariable").value<Variable *>();
            auto var = model->addVar(pVariable->address(), pVariable->rawValueSize());
            connect(var, &ModbusVar::valueChanged, pVariable,&Variable::setRawData);
        }

        if (widget->property("writeVariable").isValid())
        {
            Variable *pVariable=widget->property("writeVariable").value<Variable *>();
            connect(pVariable, &Variable::rawValueChanged,this,[this, pVariable](const QVector<uint16_t> &val) {
                if (!val.isEmpty()) emit model->requestWrite(pVariable->address(),val) ;
            });
        }

        if (widget->property("writeReadVariable").isValid())
        {
            Variable *pVariable=widget->property("writeReadVariable").value<Variable *>();
            connect(pVariable, &Variable::rawValueChanged,this,[this, pVariable](const QVector<uint16_t> &val) {
                if (!val.isEmpty()) emit model->requestWrite(pVariable->address(),val) ;
            });
            auto var = model->addVar(pVariable->address(), pVariable->rawValueSize());
            connect(var, &ModbusVar::valueChanged, pVariable,&Variable::setRawData);
        }
    }
    auto var = model->addVar(alarm.readVariable()->address(), alarm.readVariable()->rawValueSize());
    connect(var, &ModbusVar::valueChanged, alarm.readVariable(),&Variable::setRawData);

    manager = new ModbusManager(model, this);
    connect(manager, &ModbusManager::connectionStateChanged, this, [this](const QString &stateText) {
        ui->statusbar->showMessage(stateText, 0);
    });
    connect(manager, &ModbusManager::lastError, this, [this](const QString &errorText) {
        ui->statusbar->showMessage(tr("Ошибка Modbus: %1").arg(errorText), 5000);
    });
    connect(manager, &ModbusManager::writeCompleted, this, [this](const QString &varName) {
        ui->statusbar->showMessage(tr("Записано: %1").arg(varName), 3000);
    });
    connect(manager, &ModbusManager::writeFailed, this, [this](const QString &varName, const QString &err) {
        ui->statusbar->showMessage(tr("Ошибка записи %1: %2").arg(varName, err), 5000);
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
    NumpadDialog npd(this, display->writeVariable()->type(),
                     display->writeVariable()->minimum(), display->writeVariable()->maximum(),
                     display->writeVariable()->fractional());
    npd.setCurrentValue(display->text());
    connect(&npd, &NumpadDialog::enter, display, &NumericDisplay::inputData);
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

void MainWindow::on_actionClose_triggered()
{
    MainWindow::close();
}
void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    event->ignore();
}

void MainWindow::on_pushButton_clicked()
{
    MainWindow::close();
}

void MainWindow::on_alarmChanged(QStringList message)
{
    if (message.count()!=0)
    {
        QString string;
    foreach (QString str, message)
    {
        string+=str;
    }
    qDebug() << string;
    alarmDialog->on_alarmChanged(message);
    alarmDialog->show();
    }
    else {
        alarmDialog->on_alarmChanged(message);
        alarmDialog->close();
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef QT_DEBUG
    event->accept();
    return;
#endif
    exitDialog eDlg(this);
    eDlg.exec();

    // Проверяем, какую кнопку нажал пользователь
    if (eDlg.clickedButtonName() == "CancelButton") {
        // Игнорируем закрытие, программа продолжает работать
        event->ignore();
    }
    else if (eDlg.clickedButtonName() == "ShutdownButton") {
        // Разрешаем программе штатно закрыться
        event->accept();
// Выполняем кроссплатформенный запуск команды выключения
#if defined(Q_OS_WIN)
        QProcess::startDetached("shutdown", QStringList() << "/s" << "/t" << "0");
#elif defined(Q_OS_LINUX)
        QProcess::startDetached("shutdown", QStringList() << "-h" << "now");
#elif defined(Q_OS_MAC)
        QProcess::startDetached("osascript", QStringList() << "-e" << "tell app \"System Events\" to shut down");
#endif
    }
    else if (eDlg.clickedButtonName() == "RestartButton") {
        // Разрешаем программе штатно закрыться
        event->accept();
// Выполняем кроссплатформенный запуск команды выключения
#if defined(Q_OS_WIN)
        QProcess::startDetached("shutdown", QStringList() << "/r" << "/t" << "0");
#elif defined(Q_OS_LINUX)
        QProcess::startDetached("shutdown", QStringList() << "-r" << "now");
#elif defined(Q_OS_MAC)
        QProcess::startDetached("osascript", QStringList() << "-e" << "tell app \"System Events\" to shut down");
#endif
    }
    else if (eDlg.clickedButtonName() == "CloseButton") {
        // Просто закрываем программу
        event->accept();
    }


}