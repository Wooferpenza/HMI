#ifndef MODBUSMODEL_H
#define MODBUSMODEL_H

#include <QAbstractListModel>
#include "modbuscommon.h"

class ModbusModel : public QAbstractListModel {
    Q_OBJECT
public:
    int rowCount(const QModelIndex & = QModelIndex()) const override { return m_vars.size(); }
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || role != Qt::DisplayRole) return {};
        return m_vars[index.row()].value;
    }

    void updateVariable(const QString &name, const QVariant &val) {
        for (int i = 0; i < m_vars.size(); ++i) {
            if (m_vars[i].name == name) {
                m_vars[i].value = val;
                emit dataChanged(index(i), index(i));
                emit variableUpdated(name, val);
                break;
            }
        }
    }

    void addVar(QString n, uint16_t a, VarType t) { m_vars << ModbusVar{n, a, t, 0}; }

signals:
    void variableUpdated(const QString &name, QVariant value);

public:
    QVector<ModbusVar> m_vars;
};
#endif
