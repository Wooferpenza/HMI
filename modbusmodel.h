#ifndef MODBUSMODEL_H
#define MODBUSMODEL_H

#include <QAbstractListModel>
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
        for (int i = 0; i < m_vars.size(); ++i) {
            if (m_vars[i]->name == name) {
                m_vars[i]->value = val;
                emit dataChanged(index(i), index(i));
                emit variableUpdated(name, val);
                emit m_vars[i]->update();
                break;
            }
        }
    }

    void addVar(QString n, uint16_t a, VarType t) {
        m_vars << new ModbusVar; }

    // Новый API доступа к переменным
    int variableCount() const { return m_vars.size(); }

    const ModbusVar &variableAt(int index) const { return *m_vars[index]; }

    const ModbusVar *findVariable(const QString &name) const {
        for (int i = 0; i < m_vars.size(); ++i) {
            if (m_vars[i]->name == name) {
                return m_vars[i];
            }
        }
        return nullptr;
    }

signals:
    void variableUpdated(const QString &name, QVariant value);

private:
    QList<ModbusVar*> m_vars;
};
#endif
