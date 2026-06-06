#ifndef MULTISTATEBUTTON_H
#define MULTISTATEBUTTON_H

#include <QPushButton>
#include <QStringList>
#include "variable.h"
#include "variableproperty.h"

class MultiStateButton : public QPushButton, public VariableProperty <MultiStateButton>
{
    Q_OBJECT
    Q_PROPERTY(Variable *readVariable READ readVariable FINAL)
    Q_PROPERTY(Variable *writeVariable READ writeVariable FINAL)
    IMPLEMENT_SHARED_PROPERTY(QString, readAddress);
    IMPLEMENT_SHARED_PROPERTY(QString, writeAddress);
    Q_PROPERTY(int stateCount READ stateCount WRITE setStateCount NOTIFY stateCountChanged FINAL)
    Q_PROPERTY(QStringList stateIconPaths READ stateIconPaths WRITE setStateIconPaths NOTIFY stateIconPathsChanged FINAL)
    Q_PROPERTY(QStringList stateTexts READ stateTexts WRITE setStateTexts NOTIFY stateTextsChanged FINAL)
    Q_PROPERTY(QString stateTextFallback READ stateTextFallback WRITE setStateTextFallback NOTIFY stateTextFallbackChanged FINAL)

public:
    explicit MultiStateButton(QWidget *parent = nullptr);

    int stateCount() const;
    void setStateCount(int count);

    QStringList stateIconPaths() const;
    void setStateIconPaths(const QStringList &paths);

    QStringList stateTexts() const;
    void setStateTexts(const QStringList &texts);

    QString stateTextFallback() const;
    void setStateTextFallback(const QString &text);

    QString readAddress() const;
    void setReadAddress(const QString &newReadAddress);

    QString writeAddress() const;
    void setWriteAddress(const QString &newWriteAddress);



    Variable *readVariable() const;

    Variable *writeVariable() const;

signals:
    void stateCountChanged();
    void stateIconPathsChanged();
    void stateTextsChanged();
    void stateTextFallbackChanged();
    void currentStateChanged(int);

 protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onModbusValueChanged(const QVariant &val);
    void onClicked();

private:
    void applyStateVisuals();
    void ensureListLengths();

    int m_stateCount = 2;
    int m_currentState = 0;
    QStringList m_iconPaths;
    QStringList m_stateTexts;
    QString m_stateTextFallback;
    QString m_readAddress;
    QString m_writeAddress;
    Variable *m_readVariable = nullptr;
    Variable *m_writeVariable = nullptr;
};

#endif // MULTISTATEBUTTON_H
