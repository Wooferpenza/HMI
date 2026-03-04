#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modbusmanager.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void showNumPad();
    void displayUpdate(QString name,QVariant val);
private:
    Ui::MainWindow *ui;
    ModbusModel *model;
    ModbusManager *manager;
    QTimer *timer;
    void connectDisplay();
};

#endif // MAINWINDOW_H
