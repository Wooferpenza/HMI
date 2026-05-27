#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QVariant>
#include "variable.h"

class NumericDisplay : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly FINAL)
    Q_PROPERTY(Variable *variable READ variable FINAL)
public:
    explicit NumericDisplay(QWidget *parent = nullptr);
   Variable *variable() const;

    bool isReadOnly() const;
    void setReadOnly(bool newReadOnly);

   // Variable *var() const;

signals:
    void clicked();


protected:
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void inputData(const QVariant &data);
    void displayData(const QVariant &val);

private:
    Variable *m_variable = nullptr;
    bool m_readOnly;
 //   Variable *m_var;
};

#endif // NUMERICDISPLAY_H
