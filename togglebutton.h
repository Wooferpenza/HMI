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
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, format);
    IMPLEMENT_SHARED_PROPERTY(uint16_t, fractional);
    IMPLEMENT_SHARED_PROPERTY(float, minimum);
    IMPLEMENT_SHARED_PROPERTY(float, maximum);
    IMPLEMENT_SHARED_PROPERTY(QString, readAddress);
    IMPLEMENT_SHARED_PROPERTY(QString, writeAddress);
    Q_PROPERTY(QStringList stateTexts READ stateTexts WRITE setStateTexts NOTIFY stateTextsChanged FINAL)
    Q_PROPERTY(QString stateTextFallback READ stateTextFallback WRITE setStateTextFallback NOTIFY stateTextFallbackChanged FINAL)

public:
    explicit ToggleButton(QWidget *parent = nullptr);

    QStringList stateTexts() const;
    void setStateTexts(const QStringList &texts);

    QString stateTextFallback() const;
    void setStateTextFallback(const QString &text);

    Variable *variable() const;

    Variable *readVariable() const;

    Variable *writeVariable() const;

signals:
    void stateTextsChanged();
    void stateTextFallbackChanged();

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
    Variable *m_writeVariable = nullptr;
};

#endif // TOGGLEBUTTON_H
