#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QObject>
#include <QVariant>

class NumericDisplay : public QLineEdit
{
    Q_OBJECT
private:
    float mValue, mMinimumValue, mMaximumValue;
    void checkRange();

public:
    NumericDisplay(QWidget *parent = nullptr);
   // void setValue(float val);
    void setMinimum(float min);
    void setMaximum(float max);
    float value();
    float minimumValue();
    float maximumValue();

signals:
    void clicked();
public slots:
    void setValue(float val);
    void setValue(const QVariant &value);
protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        QLineEdit::mousePressEvent(event); // Вызываем стандартную обработку
        emit clicked();                      // Испускаем сигнал
    }

};

#endif // NUMERICDISPLAY_H
