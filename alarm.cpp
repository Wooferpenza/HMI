#include "alarm.h"
#include "bitset"

Alarm::Alarm(QObject *parent)
    : QObject{parent} , m_readVariable(new Variable(this))
{
    m_readVariable->setType(Variable::DataFormat::UDWord);
    m_readVariable->setMinimum(0);
    m_readVariable->setMaximum(65535);
    connect(this, &Alarm::readAddressChanged, m_readVariable, &Variable::setAddressStr);
    connect(m_readVariable, &Variable::valueChanged,this, &Alarm::onModbusValueChanged);
}

Variable *Alarm::readVariable() const
{
    return m_readVariable;
}

void Alarm::onModbusValueChanged(const QVariant &val)
{
    m_activeAlarm.clear();
    std::bitset<32> bits(val.toUInt());
    for (int i = 0; i < m_allAlarm.count(); ++i)
    {
        if(bits.test(i))
        {
            m_activeAlarm.push_back(m_allAlarm.at(i));
        }
    }
   emit alarmChanged(m_activeAlarm);
}
