#include "mainwindow.h"
#include "momentarybutton.h"
#include "modbusmanager.h"
#include "togglebutton.h"
#include "multistatebutton.h"
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
    DataType type = DataType::UWord;
    float min = 0.0f;
    float max = 100.0f;
    uint fractional = 0;
    bool readOnly = false;
};

const VariableBinding kVariableBindings[] = {
    {"lineEdit", 100, DataType::UWord, 0, 65535, 1,  false},
    {"lineEditCounter", 102, DataType::Float, -10.0f, 65535.0f, 2,  false},
    {"xAbsDisplay", 46,  DataType::Float, -10000.0f, 10000.0f, 2,  true},
    {"xRelDisplay", 48,  DataType::Float, -10000.0f, 10000.0f, 2,  true},
    {"yAbsDisplay", 32,  DataType::Float, -10000.0f, 10000.0f, 2,  true},
    {"yRelDisplay", 34,  DataType::Float, -10000.0f, 10000.0f, 2,  true},
    {"aRelDisplay", 62,  DataType::Float, -10000.0f, 10000.0f, 2,  true},
    {"cutSpeedDisplay", 1100,  DataType::Float, 0.0f, 1000.0f, 0,  false},

    };


struct ButtonBinding {
    const char *widgetName;
    quint16 address;
    uint16_t bitIndex = 0;
};

const ButtonBinding kButtonBindings[] = {
    {"StartButton", 112, 0},
    {"xRstButton", 38, 0},
    {"yRstButton", 24, 0},
    {"aRstButton", 52, 0},
    {"cutStartButton", 0, 0},
    {"cutStopButton", 0, 1},
    {"clumpDownButton", 0, 2},
    {"clumpUpButton", 0, 3},
    };

const ButtonBinding kToggleBindings[] = {
    {"toggleButtonEnable", 113, 0},
     {"cutRunToggle", 10, 0},

    };

struct MultiStateBinding {
    const char *widgetName;
    const char *varName;
    quint16 address;
};

const MultiStateBinding kMultiStateBindings[] = {
    {"manualAxisSelect", "Mode", 20},
    {"manualStep", "Mode", 21},
    };

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        display->variable()->setName(QLatin1String(b.widgetName));
        display->variable()->setType(b.type);
        display->variable()->setMinimum(b.min);
        display->variable()->setMaximum(b.max);
        display->variable()->setFractional(b.fractional);
        display->variable()->setReadAddress(b.address);
        display->variable()->setWriteAddress(b.address);
        display->setReadOnly(b.readOnly);
    }


    for (const ButtonBinding &b : kButtonBindings) {
        auto *btn = findChild<MomentaryButton *>(QLatin1String(b.widgetName));
        if (!btn)
            continue;
        btn->variable()->setName(QLatin1String(b.widgetName));
        btn->variable()->setReadAddress(b.address);
        btn->variable()->setWriteAddress(b.address);
        btn->variable()->setReadAddressBit(b.bitIndex);
        btn->variable()->setWriteAddressBit(b.bitIndex);
        // auto var = model->addVar(btn->variable()->name(), btn->variable()->readAddress(),btn->variable()->rawValueSize());
        // connect(var, &ModbusVar::valueChanged, btn->variable(),&Variable::setRawData);
        // connect(btn->variable(),&Variable::rawValueChanged,this,[this, btn](const QVector<uint16_t> &val) {
        //     if (!val.isEmpty())
        //         emit model->requestWrite(btn->variable()->writeAddress(),val) ;
        // });
    }

    for (const ButtonBinding &b : kToggleBindings) {
        auto *btn = findChild<ToggleButton *>(QLatin1String(b.widgetName));
        if (!btn)
            continue;
        btn->variable()->setName(QLatin1String(b.widgetName));
        btn->variable()->setReadAddress(b.address);
        btn->variable()->setWriteAddress(b.address);
        btn->variable()->setReadAddressBit(b.bitIndex);
        btn->variable()->setWriteAddressBit(b.bitIndex);
        // auto var = model->addVar(btn->variable()->name(), btn->variable()->readAddress(),btn->variable()->rawValueSize());
        // connect(var, &ModbusVar::valueChanged, btn->variable(),&Variable::setRawData);
        // connect(btn->variable(),&Variable::rawValueChanged,this,[this, btn](const QVector<uint16_t> &val) {
        //     if (!val.isEmpty())
        //         emit model->requestWrite(btn->variable()->writeAddress(),val) ;
        // });
    }

    for (const MultiStateBinding &b : kMultiStateBindings) {
        auto *btn = findChild<MultiStateButton *>(QLatin1String(b.widgetName));
        if (!btn)
            continue;
        btn->variable()->setName(QLatin1String(b.varName));
        btn->variable()->setType(DataType::UWord);
        btn->variable()->setMinimum(0.0f);
        btn->variable()->setMaximum(static_cast<float>(btn->stateCount() - 1));
        btn->variable()->setReadAddress(b.address);
        btn->variable()->setWriteAddress(b.address);
        // auto var = model->addVar(btn->variable()->name(), btn->variable()->readAddress(),btn->variable()->rawValueSize());
        // connect(var, &ModbusVar::valueChanged, btn->variable(),&Variable::setRawData);
        // connect(btn->variable(),&Variable::rawValueChanged,this,[this, btn](const QVector<uint16_t> &val) {
        //     if (!val.isEmpty())
        //         emit model->requestWrite(btn->variable()->writeAddress(),val) ;
        // });
    }
    const auto displays1 = findChildren<QWidget*>();
    for (QWidget *display : displays1) {
        if (!display)
            continue;
        if (display->property("variable").isValid())
        {
            Variable *dsp=display->property("variable").value<Variable *>();
            auto var = model->addVar(dsp->name(), dsp->readAddress(), dsp->rawValueSize());
            connect(var, &ModbusVar::valueChanged, dsp,&Variable::setRawData);
            connect(dsp, &Variable::rawValueChanged,this,[this, dsp](const QVector<uint16_t> &val) {
                if (!val.isEmpty())
                    emit model->requestWrite(dsp->writeAddress(),val) ;
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
    NumpadDialog npd(this, display->variable()->type(),
                     display->variable()->minimum(), display->variable()->maximum(),
                     display->variable()->fractional());
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


