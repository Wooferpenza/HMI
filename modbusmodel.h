#ifndef MODBUSMODEL_H
#define MODBUSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QList>
#include "modbuscommon.h"

class ModbusModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit ModbusModel(QObject *parent = nullptr)
        : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex & = QModelIndex()) const override { return m_vars.size(); }
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || role != Qt::DisplayRole) return {};
        return m_vars[index.row()]->value;
    }

    void updateVariable(const QString &name, const QVariant &val) {
        ModbusVar *var = m_varByName.value(name, nullptr);
        if (!var)
            return;

        var->value = val;

        const int row = m_vars.indexOf(var);
        if (row >= 0)
            emit dataChanged(index(row), index(row));

        emit variableUpdated(name, val);
        emit var->valueChanged(val);
    }

    void addVar(const QString &n, uint16_t a, VarType t, int bitIndex = -1)
    {
        if (m_varByName.contains(n))
            return;

        int insertPos = 0;
        while (insertPos < m_vars.size() && m_vars[insertPos]->address < a)
            ++insertPos;

        beginInsertRows(QModelIndex(), insertPos, insertPos);
        auto *var = new ModbusVar(n, a, t, this, bitIndex);
        m_vars.insert(insertPos, var);
        m_varByName.insert(n, var);
        endInsertRows();
    }

    // Новый API доступа к переменным
    int variableCount() const { return m_vars.size(); }

    const ModbusVar &variableAt(int index) const { return * m_vars[index] ; }

    ModbusVar *findVariable(const QString &name) const {
        return m_varByName.value(name, nullptr);
    }

signals:
    void variableUpdated(const QString &name, QVariant value);

private:
    QList<ModbusVar*> m_vars;
    QHash<QString, ModbusVar*> m_varByName;
};
#endif
