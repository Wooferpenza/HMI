#include "modbusmodel.h"

ModbusModel::ModbusModel(QObject *parent)
    : QObject(parent)
{}

ModbusVar *ModbusModel::addVar(const QString &name, uint16_t address, uint16_t size)
{
    auto *var = new ModbusVar(name, address, size, this);
    m_readTable.append(var);
    return var;
}

ModbusVar *ModbusModel::variableAt(int index) const
{
    if (index < 0 || index >= m_readTable.size())
        return nullptr;
    return m_readTable.at(index);
}

QVector<ModbusVar *> ModbusModel::variablesInRange(uint16_t start, uint16_t count) const
{
    const uint16_t end = static_cast<uint16_t>(start + count);
    QVector<ModbusVar *> result;
    for (ModbusVar *var : m_readTable) {
        if (var->address >= start && var->address < end)
            result.append(var);
    }
    return result;
}

void ModbusModel::updateVariable( ModbusVar *var, const QVector<uint16_t> &val)
{
    if (!var)
        return;
    var->value = val;
    emit var->valueChanged(val);
}

ModbusVar *ModbusModel::findVariable(const QString &name) const
{
    //return m_varsByName.value(name, nullptr);
}

