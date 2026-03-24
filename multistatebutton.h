#ifndef MULTISTATEBUTTON_H
#define MULTISTATEBUTTON_H

#include <QPushButton>
#include <QStringList>
#include "variable.h"

class MultiStateButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(int stateCount READ stateCount WRITE setStateCount NOTIFY stateCountChanged FINAL)
    Q_PROPERTY(QStringList stateIconPaths READ stateIconPaths WRITE setStateIconPaths NOTIFY stateIconPathsChanged FINAL)
    Q_PROPERTY(QStringList stateTexts READ stateTexts WRITE setStateTexts NOTIFY stateTextsChanged FINAL)
    Q_PROPERTY(QString stateTextFallback READ stateTextFallback WRITE setStateTextFallback NOTIFY stateTextFallbackChanged FINAL)
    Q_PROPERTY(int currentState READ currentState WRITE setCurrentState NOTIFY currentStateChanged FINAL)

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

    int currentState() const;
    void setCurrentState(int state);

    Variable *variable() const;

signals:
    void stateCountChanged();
    void stateIconPathsChanged();
    void stateTextsChanged();
    void stateTextFallbackChanged();
    void currentStateChanged(int state);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onModbusValueChanged(const QVariant &val);
    void onClicked();

private:
    void applyStateVisuals();
    void ensureListLengths();

    Variable *m_variable = nullptr;
    int m_stateCount = 2;
    int m_currentState = 0;
    QStringList m_iconPaths;
    QStringList m_stateTexts;
    QString m_stateTextFallback;
};

#endif // MULTISTATEBUTTON_H
