#ifndef EXITDIALOG_H
#define EXITDIALOG_H

#include <QDialog>
#include <QString>
namespace Ui {
class exitDialog;
}

class exitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit exitDialog(QWidget *parent = nullptr);
    ~exitDialog();
    QString clickedButtonName ( ) {return m_clickedButtonName;};
private slots:
     void on_pushButton_clicked();

private:
    Ui::exitDialog *ui;
    QString m_clickedButtonName;
};

#endif // EXITDIALOG_H
