#include "exitdialog.h"
#include "ui_exitdialog.h"

exitDialog::exitDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::exitDialog)
{
    ui->setupUi(this);
    connect(ui->CloseButton, &QPushButton::clicked, this, &exitDialog::on_pushButton_clicked);
    connect(ui->ShutdownButton, &QPushButton::clicked, this, &exitDialog::on_pushButton_clicked);
    connect(ui->RestartButton, &QPushButton::clicked, this, &exitDialog::on_pushButton_clicked);
    connect(ui->CancelButton, &QPushButton::clicked, this, &exitDialog::on_pushButton_clicked);
}

exitDialog::~exitDialog()
{
    delete ui;
}



void exitDialog::on_pushButton_clicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button) {
        m_clickedButtonName=button->objectName();
    }
    close();
}