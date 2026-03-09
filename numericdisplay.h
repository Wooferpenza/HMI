#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QVariant>
#include "variable.h"

class NumericDisplay : public QLineEdit
{
    Q_OBJECT
public:
    explicit NumericDisplay(QWidget *parent = nullptr);
    Variable *variable() const;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void inputData(const QVariant &data);
    void displayData(const QVariant &val);

private:
    Variable *m_variable = nullptr;
};

#endif // NUMERICDISPLAY_H
