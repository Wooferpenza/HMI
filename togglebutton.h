#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include <QStringList>
#include <QMouseEvent>
#include "variable.h"
#include "variableproperty.h"

class ToggleButton : public QPushButton, public VariableProperty<ToggleButton>
{
    Q_OBJECT
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    Q_PROPERTY(Variable *writeReadVariable READ writeReadVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, readAddress);
    IMPLEMENT_SHARED_PROPERTY(QString, writeReadAddress);
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged FINAL)
    Q_PROPERTY(QStringList stateTexts READ stateTexts WRITE setStateTexts NOTIFY stateTextsChanged FINAL)
    Q_PROPERTY(QString stateTextFallback READ stateTextFallback WRITE setStateTextFallback NOTIFY stateTextFallbackChanged FINAL)

public:
    explicit ToggleButton(QWidget *parent = nullptr);

    QStringList stateTexts() const;
    void setStateTexts(const QStringList &texts);

    QString stateTextFallback() const;
    void setStateTextFallback(const QString &text);

    Variable *readVariable() const;
    Variable *writeReadVariable() const;

    bool readOnly() const;
    void setReadOnly(bool newReadOnly);

signals:
    void stateTextsChanged();
    void stateTextFallbackChanged();

    void readOnlyChanged();

private slots:
    void onModbusValueChanged(const QVariant &val);
    void onClicked();
    void refreshStateText();
protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    void ensureStateTextListLength();

    QStringList m_stateTexts;
    QString m_stateTextFallback;
    Variable *m_readVariable = nullptr;
    Variable *m_writeReadVariable = nullptr;
    bool m_readOnly;
};

#endif // TOGGLEBUTTON_H
