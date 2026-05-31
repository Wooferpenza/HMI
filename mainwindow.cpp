#include "mainwindow.h"
#include "modbusmanager.h"
#include "modbusmodel.h"
#include "modbussettings.h"
#include "modbussettingsdialog.h"
#include "numericdisplay.h"
#include "numpaddialog.h"
#include "ui_mainwindow.h"
#include <QButtonGroup>
#include <QSettings>

namespace {

// Привязка виджет → Modbus: имя виджета (objectName в UI), имя переменной, адрес регистра.
// Опционально: тип, формат, лимиты — задаются только если нужны не по умолчанию.
struct VariableBinding {
    const char *widgetName;
    quint16 address;
    Variable::DataFormat type = Variable::DataFormat::UWord;
    float min = 0.0f;
    float max = 100.0f;
    uint fractional = 0;
    bool readOnly = false;
};

const VariableBinding kVariableBindings[] = {
    {"lineEdit", 100, Variable::DataFormat::UWord, 0, 65535, 1,  false},
    {"lineEditCounter", 102, Variable::DataFormat::Float, -10.0f, 65535.0f, 2,  false},
    {"xAbsDisplay", 46,  Variable::DataFormat::Float, -10000.0f, 10000.0f, 2,  true},
    {"xRelDisplay", 48,  Variable::DataFormat::Float, -10000.0f, 10000.0f, 2,  true},
    {"yAbsDisplay", 32,  Variable::DataFormat::Float, -10000.0f, 10000.0f, 2,  true},
    {"yRelDisplay", 34,  Variable::DataFormat::Float, -10000.0f, 10000.0f, 2,  true},
    {"aRelDisplay", 62,  Variable::DataFormat::Float, -10000.0f, 10000.0f, 2,  true},
    {"cutSpeedDisplay", 1100,  Variable::DataFormat::Float, 0.0f, 1000.0f, 0,  false},
    };




} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_AcceptTouchEvents, true);

    auto *navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);
    navGroup->addButton(ui->navButtonOverview, 0);
    navGroup->addButton(ui->navButtonJournal, 1);
    navGroup->addButton(ui->navButtonService, 2);
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

    for (const VariableBinding &b : kVariableBindings) {
        auto *display = findChild<NumericDisplay *>(QLatin1String(b.widgetName));
        if (!display)
            continue;
        display->readVariable()->setType(b.type);
        display->writeVariable()->setType(b.type);
        display->readVariable()->setMinimum(b.min);
        display->writeVariable()->setMinimum(b.min);
        display->readVariable()->setMaximum(b.max);
        display->writeVariable()->setMaximum(b.max);
        display->readVariable()->setFractional(b.fractional);
        display->writeVariable()->setFractional(b.fractional);
        display->setReadOnly(b.readOnly);
    }


    const auto displays1 = findChildren<QWidget*>();
    for (QWidget *display : displays1) {
        if (!display)
            continue;
        if (display->property("readVariable").isValid())
        {
            Variable *dsp=display->property("readVariable").value<Variable *>();
            auto var = model->addVar(dsp->address(), dsp->rawValueSize());
            connect(var, &ModbusVar::valueChanged, dsp,&Variable::setRawData);
        }
        if (display->property("writeVariable").isValid())
        {
            Variable *dsp=display->property("writeVariable").value<Variable *>();
            connect(dsp, &Variable::rawValueChanged,this,[this, dsp](const QVector<uint16_t> &val) {
                if (!val.isEmpty())
                    emit model->requestWrite(dsp->address(),val) ;
            });
        }
    }
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
