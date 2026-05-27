#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include <QStringList>
#include "variable.h"

class ToggleButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variable *variable READ variable FINAL)
    Q_PROPERTY(QStringList stateTexts READ stateTexts WRITE setStateTexts NOTIFY stateTextsChanged FINAL)
    Q_PROPERTY(QString stateTextFallback READ stateTextFallback WRITE setStateTextFallback NOTIFY stateTextFallbackChanged FINAL)

public:
    explicit ToggleButton(QWidget *parent = nullptr);

    QStringList stateTexts() const;
    void setStateTexts(const QStringList &texts);

    QString stateTextFallback() const;
    void setStateTextFallback(const QString &text);

    Variable *variable() const;

signals:
    void stateTextsChanged();
    void stateTextFallbackChanged();

private slots:
    void onModbusValueChanged(const QVariant &val);
    void onClicked();
    void refreshStateText();

private:
    void ensureStateTextListLength();

    Variable *m_variable = nullptr;
    QStringList m_stateTexts;
    QString m_stateTextFallback;
};

#endif // TOGGLEBUTTON_H
