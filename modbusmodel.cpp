#include "modbusmodel.h"

ModbusModel::ModbusModel(QObject *parent)
    : QObject(parent)
{}

ModbusVar *ModbusModel::addVar( uint16_t address, uint16_t size)
{
    auto *var = new ModbusVar(address, size, this);
    m_readTable.append(var);
    sortReadTable();
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

QList<ModbusVar *> &ModbusModel::readTable()
{
    if (m_readTable.size())
    return m_readTable;
}

void ModbusModel::sortReadTable()
{
    std::sort(m_readTable.begin(), m_readTable.end(), [](const ModbusVar *a, const ModbusVar *b) {
        return a->address < b->address;
    });
}


