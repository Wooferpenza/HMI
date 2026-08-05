#include "alarmdialog.h"
#include "ui_alarmdialog.h"

AlarmDialog::AlarmDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlarmDialog)
{
    ui->setupUi(this);
}

AlarmDialog::~AlarmDialog()
{
    delete ui;
}

void AlarmDialog::on_alarmChanged(QStringList messageList)
{

    QString string;
    foreach (QString str, messageList)
    {
        string+=str+"\n";
    }
    ui->textEdit->clear();
    ui->textEdit->setText(string);
}

void AlarmDialog::on_pushButton_clicked()
{
    this->close();
}

