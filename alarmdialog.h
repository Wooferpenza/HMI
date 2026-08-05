#ifndef ALARMDIALOG_H
#define ALARMDIALOG_H

#include <QDialog>
#include <QStringList>
namespace Ui {
class AlarmDialog;
}

class AlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmDialog(QWidget *parent = nullptr);
    ~AlarmDialog();

public slots:
    void on_alarmChanged(QStringList);
private slots:
    void on_pushButton_clicked();

private:
    Ui::AlarmDialog *ui;
};

#endif // ALARMDIALOG_H
