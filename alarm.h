#ifndef ALARM_H
#define ALARM_H

#include <QObject>
#include <QStringList>
#include "variable.h"
#include "variableproperty.h"

class Alarm : public QObject, public VariableProperty <Alarm>
{
    Q_OBJECT
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, readAddress);
public:
    explicit Alarm(QObject *parent = nullptr);
    Variable *readVariable() const;

signals:
   void alarmChanged(QStringList);
private slots:
   void onModbusValueChanged(const QVariant &val);
private:
    Variable *m_readVariable = nullptr;
    QStringList m_allAlarm={"Авария оси Y","Авария оси X","Авария оси A","Авария привода UZ1","Авария привода UZ3","Ошибка перемещения"};
    QStringList m_activeAlarm;
};

#endif // ALARM_H
