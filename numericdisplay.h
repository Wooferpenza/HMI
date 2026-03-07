#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QMouseEvent>
#include <QObject>
#include <QVariant>

class NumericDisplay : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
private:
    float mValue, mMinimumValue, mMaximumValue;
    int m_decimals = 2;
    void checkRange();

public:
    NumericDisplay(QWidget *parent = nullptr);
   // void setValue(float val);
    void setDecimals(int decimals);
    int decimals() const;
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
