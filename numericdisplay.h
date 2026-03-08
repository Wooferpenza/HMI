#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QObject>
#include <QVariant>
#include "variable.h"
class NumericDisplay : public QLineEdit
{
    Q_OBJECT
public:
    NumericDisplay(QWidget *parent = nullptr);
    Variable variable;
signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        QLineEdit::mousePressEvent(event); // Вызываем стандартную обработку
        emit clicked();                    // Испускаем сигнал
    }
public slots:
    void inputData(QVariant data);
    void  displayData(QVariant);

};

#endif // NUMERICDISPLAY_H
