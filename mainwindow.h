#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ModbusModel;
class ModbusManager;

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
    void showModbusSettings();

    void on_actionClose_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    ModbusModel *model = nullptr;
    ModbusManager *manager = nullptr;
    void contextMenuEvent(QContextMenuEvent *event) override;

};

#endif // MAINWINDOW_H
