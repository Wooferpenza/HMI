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

ModbusVar *ModbusModel::addVar(const QString &name, uint16_t address, size_t size)
{
    if (findVariable(name))
        return nullptr;
    auto *var = new ModbusVar(name, address, size, this);
    m_vars.append(var);
    return var;
}

void ModbusModel::addVar(Variable *variable, uint16_t address)
{
    ModbusVar *var = addVar(variable->name(), address, variable->rowValueSize());
    if (!var)
        return;
    connect(var, &ModbusVar::valueChanged, variable, &Variable::setRowData);
    connect(variable, &Variable::rowValueChanged, this, [this, variable](const QVector<uint16_t> &val) {
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
    for (ModbusVar *var : m_vars) {
        if (var->name == name)
            return var;
    }
    return nullptr;
}
