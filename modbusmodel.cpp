#include "modbusmodel.h"

ModbusModel::ModbusModel(QObject *parent)
    : QObject(parent)
{}

void ModbusModel::updateVariable(const QString &name, const QVector<uint16_t> &val)
{
    ModbusVar *var = findVariable(name);
    if (!var)
        return;
    var->value = val;
    emit var->valueChanged(val);
}

ModbusVar *ModbusModel::addVar(const QString &name, uint16_t address, uint16_t size)
{
    if (findVariable(name))
        return nullptr;
    auto *var = new ModbusVar(name, address, size, this);
    m_vars.append(var);
    m_varsByName.insert(name, var);
    return var;
}

void ModbusModel::addVar(Variable *variable, uint16_t address)
{
    ModbusVar *var = addVar(variable->name(), address, variable->rawValueSize());
    if (!var)
        return;
    connect(var, &ModbusVar::valueChanged, variable, &Variable::setRawData);
    connect(variable, &Variable::rawValueChanged, this, [this, variable](const QVector<uint16_t> &val) {
        if (!val.isEmpty())
            emit requestWrite(variable->name(), val);
    });
}

ModbusVar *ModbusModel::variableAt(int index) const
{
    if (index < 0 || index >= m_vars.size())
        return nullptr;
    return m_vars.at(index);
}

ModbusVar *ModbusModel::findVariable(const QString &name) const
{
    return m_varsByName.value(name, nullptr);
}

QVector<ModbusVar *> ModbusModel::variablesInRange(uint16_t start, uint16_t count) const
{
    const uint16_t end = static_cast<uint16_t>(start + count);
    QVector<ModbusVar *> result;
    for (ModbusVar *var : m_vars) {
        if (var->address >= start && var->address < end)
            result.append(var);
    }
    return result;
}
