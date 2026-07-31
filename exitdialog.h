#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>

namespace Ui {
class exitDialog;
}

class exitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit exitDialog(QWidget *parent = nullptr);
    ~exitDialog();

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::exitDialog *ui;
};

#endif // EXITDIALOG_H
