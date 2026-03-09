#ifndef MODBUSMODEL_H
#define MODBUSMODEL_H

#include <QHash>
#include <QList>
#include "modbuscommon.h"
#include "variable.h"

class ModbusModel : public QObject {
    Q_OBJECT
public:
    explicit ModbusModel(QObject *parent = nullptr);
    void updateVariable(const QString &name, const QVector<uint16_t> &val);
    ModbusVar *addVar(const QString &name, uint16_t address, uint16_t size);
    void addVar(Variable *variable, uint16_t address);
    int variableCount() const { return m_vars.size(); }
    ModbusVar *variableAt(int index) const;
    ModbusVar *findVariable(const QString &name) const;
    QVector<ModbusVar *> variablesInRange(uint16_t start, uint16_t count) const;
signals:
    void requestWrite(const QString &name, const QVector<uint16_t> &value);

private:
    QList<ModbusVar *> m_vars;
    QHash<QString, ModbusVar *> m_varsByName;
};

#endif
