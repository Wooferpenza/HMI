#ifndef MODBUSMODEL_H
#define MODBUSMODEL_H

//#include <QHash>
#include <QList>
#include "modbuscommon.h"

class ModbusModel : public QObject {
    Q_OBJECT
public:
    explicit ModbusModel(QObject *parent = nullptr);
    ModbusVar *addVar(const QString &name, uint16_t address, uint16_t size);
    int variableCount() const { return m_readTable.size(); }
    ModbusVar *variableAt(int index) const;
    ModbusVar *findVariable(const QString &name) const;
    QVector<ModbusVar *> variablesInRange(uint16_t start, uint16_t count) const;
    void updateVariable(ModbusVar *var, const QVector<uint16_t> &val);
 signals:
    void requestWrite(const uint16_t &address, const QVector<uint16_t> &value);
private:
    QList<ModbusVar *> m_readTable;
   // QHash<QString, ModbusVar *> m_varsByName;
};

#endif
