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
//#include "variable.h"
#include <QButtonGroup>
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
    uint fractional = 0;
    bool readOnly = false;
};

const VariableBinding kVariableBindings[] = {
    {"lineEdit", "Temp", 100, DataType::UWord, 0, 65535, 1,  false},
    {"lineEditCounter", "Counter", 102, DataType::Float, -10.0f, 65535.0f, 2,  false},
    {"xAbsDisplay", "xAbs", 104, DataType::Float, -10000.0f, 10000.0f, 1,  true},
    {"xRelDisplay", "xRel", 106, DataType::Float, -10000.0f, 10000.0f, 1,  true},
    {"yAbsDisplay", "yAbs", 108, DataType::Float, -10000.0f, 10000.0f, 1,  true},
    {"yRelDisplay", "yRel", 110, DataType::Float, -10000.0f, 10000.0f, 1,  true},
    };


struct ButtonBinding {
    const char *widgetName;
    const char *varName;
    quint16 address;
    uint16_t bitIndex = 0;
};

const ButtonBinding kButtonBindings[] = {
    {"StartButton", "Start", 200, 0},
    {"xRstButton", "xRst", 200, 1},
    {"yRstButton", "yRst", 200, 2},
    };

const ButtonBinding kToggleBindings[] = {
    {"toggleButtonEnable", "Enable", 200, 1},
    };

struct MultiStateBinding {
    const char *widgetName;
    const char *varName;
    quint16 address;
};

const MultiStateBinding kMultiStateBindings[] = {
    {"multiStateMode", "Mode", 202},
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
        display->variable()->setName(QLatin1String(b.varName));
        display->variable()->setType(b.type);
        display->variable()->setMinimum(b.min);
        display->variable()->setMaximum(b.max);
        display->variable()->setFractional(b.fractional);
       // display->setReadOnly(b.readOnly);
        model->addVar(display->variable(), b.address);
    }

    for (const ButtonBinding &b : kButtonBindings) {
        auto *btn = findChild<MomentaryButton *>(QLatin1String(b.widgetName));
        if (!btn)
            continue;
        btn->variable()->setName(QLatin1String(b.varName));
        btn->variable()->setBitIndex(b.bitIndex);
        model->addVar(btn->variable(), b.address);
    }

    for (const ButtonBinding &b : kToggleBindings) {
        auto *btn = findChild<ToggleButton *>(QLatin1String(b.widgetName));
        if (!btn)
            continue;
        btn->variable()->setName(QLatin1String(b.varName));
        btn->variable()->setBitIndex(b.bitIndex);
        model->addVar(btn->variable(), b.address);
    }

    for (const MultiStateBinding &b : kMultiStateBindings) {
        auto *btn = findChild<MultiStateButton *>(QLatin1String(b.widgetName));
        if (!btn)
            continue;
        btn->variable()->setName(QLatin1String(b.varName));
        btn->variable()->setType(DataType::UWord);
        btn->variable()->setMinimum(0.0f);
        btn->variable()->setMaximum(static_cast<float>(btn->stateCount() - 1));
        model->addVar(btn->variable(), b.address);
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


