#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include <QStringList>
#include <QMouseEvent>
#include "variable.h"

class ToggleButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    Q_PROPERTY(uint16_t readAddress READ readAddress WRITE setReadAddress NOTIFY readAddressChanged FINAL)
    Q_PROPERTY(uint16_t readAddressBit READ readAddressBit WRITE setReadAddressBit NOTIFY readAddressBitChanged FINAL)
    Q_PROPERTY(uint16_t writeAddress READ writeAddress WRITE setWriteAddress NOTIFY writeAddressChanged FINAL)
    Q_PROPERTY(uint16_t writeAddressBit READ writeAddressBit WRITE setWriteAddressBit NOTIFY writeAddressBitChanged FINAL)
    Q_PROPERTY(QStringList stateTexts READ stateTexts WRITE setStateTexts NOTIFY stateTextsChanged FINAL)
    Q_PROPERTY(QString stateTextFallback READ stateTextFallback WRITE setStateTextFallback NOTIFY stateTextFallbackChanged FINAL)

public:
    explicit ToggleButton(QWidget *parent = nullptr);

    QStringList stateTexts() const;
    void setStateTexts(const QStringList &texts);

    QString stateTextFallback() const;
    void setStateTextFallback(const QString &text);

    Variable *variable() const;

    uint16_t readAddress() const;
    void setReadAddress(uint16_t newReadAddress);

    uint16_t readAddressBit() const;
    void setReadAddressBit(uint16_t newReadAddressBit);

    uint16_t writeAddress() const;
    void setWriteAddress(uint16_t newWriteAddress);

    uint16_t writeAddressBit() const;
    void setWriteAddressBit(uint16_t newWriteAddressBit);

    Variable *readVariable() const;

    Variable *writeVariable() const;

signals:
    void stateTextsChanged();
    void stateTextFallbackChanged();

    void readAddressChanged(uint16_t);

    void readAddressBitChanged(uint16_t);

    void writeAddressChanged(uint16_t);

    void writeAddressBitChanged(uint16_t);

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
    uint16_t m_readAddress;
    uint16_t m_readAddressBit;
    uint16_t m_writeAddress;
    uint16_t m_writeAddressBit;
    Variable *m_readVariable = nullptr;
    Variable *m_writeVariable = nullptr;
};

#endif // TOGGLEBUTTON_H
