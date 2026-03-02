#ifndef NUMERICDISPLAY_H
#define NUMERICDISPLAY_H

#include <QLineEdit>
#include <QObject>

class NumericDisplay : public QLineEdit
{
    Q_OBJECT
private:
    float mValue, mMinimumValue, mMaximumValue;
    void cheсkRange();
public:
    NumericDisplay(QWidget *parent = nullptr);
    void setValue(float val);
    void setMinimum(float min);
    void setMaximum(float max);
    float value();
    float minimumValue();
    float maximumValue();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent *event) override {
        QLineEdit::mouseReleaseEvent(event); // Вызываем стандартную обработку
        emit clicked(); // Испускаем сигнал
    }

};

#endif // NUMERICDISPLAY_H
