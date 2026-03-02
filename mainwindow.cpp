#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLineEdit>
#include <QString>
#include <QDebug>
#include "numpaddialog.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lineEdit,&NumericDisplay::clicked,this,&MainWindow::showNumPad);

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::showNumPad()
{
    qDebug() <<"click!";
    NumericDisplay *display = qobject_cast<NumericDisplay*>(sender());
    if (display)
    {
        NumpadDialog npd;
        npd.setRange(display->minimumValue(),display->maximumValue());
        connect(&npd,&NumpadDialog::enterFloat,display,&NumericDisplay::setValue);
        npd.show();
        npd.exec();
    }
}


