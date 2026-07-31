#include "exitdialog.h"
#include "ui_exitdialog.h"

exitDialog::exitDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::exitDialog)
{
    ui->setupUi(this);
}

exitDialog::~exitDialog()
{
    delete ui;
}



void exitDialog::on_pushButton_clicked()
{
    close();
    parentWidget()->close();
}


void exitDialog::on_pushButton_2_clicked()
{
    close();
    parentWidget()->close();

}


void exitDialog::on_pushButton_3_clicked()
{
    close();
    parentWidget()->close();
}

void exitDialog::on_pushButton_4_clicked()
{
    close();

}