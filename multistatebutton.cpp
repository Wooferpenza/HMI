#include "multistatebutton.h"
#include <QRegularExpression>
#include <QtGlobal>
#include <QResizeEvent>

MultiStateButton::MultiStateButton(QWidget *parent)
    : QPushButton(parent)
    , m_readVariable(new Variable(this)), m_writeVariable(new Variable(this))
{
    connect(this, &MultiStateButton::readAddressChanged, m_readVariable, &Variable::setAddressStr);
    connect(this, &MultiStateButton::writeAddressChanged, m_writeVariable, &Variable::setAddressStr);

    connect(m_readVariable, &Variable::valueChanged,this, &MultiStateButton::onModbusValueChanged);
    connect(this, &QPushButton::clicked, this, &MultiStateButton::onClicked);
    ensureListLengths();
    applyStateVisuals();
}

int MultiStateButton::stateCount() const
{
    return m_stateCount;
}

void MultiStateButton::setStateCount(int count)
{
    count = qBound(2, count, 32);
    if (m_stateCount == count)
        return;

    m_stateCount = count;
    ensureListLengths();
    m_readVariable->setMaximum(static_cast<float>(m_stateCount - 1));

    if (m_currentState >= m_stateCount)
        m_currentState = m_stateCount - 1;

    applyStateVisuals();
    emit stateCountChanged();
}

QStringList MultiStateButton::stateIconPaths() const
{
    return m_iconPaths;
}

void MultiStateButton::setStateIconPaths(const QStringList &paths)
{
    m_iconPaths = paths;
    ensureListLengths();
    applyStateVisuals();
    emit stateIconPathsChanged();
}

QStringList MultiStateButton::stateTexts() const
{
    return m_stateTexts;
}

void MultiStateButton::setStateTexts(const QStringList &texts)
{
    m_stateTexts = texts;
    ensureListLengths();
    applyStateVisuals();
    emit stateTextsChanged();
}

QString MultiStateButton::stateTextFallback() const
{
    return m_stateTextFallback;
}

void MultiStateButton::setStateTextFallback(const QString &text)
{
    if (m_stateTextFallback == text)
        return;
    m_stateTextFallback = text;
    applyStateVisuals();
    emit stateTextFallbackChanged();
}

void MultiStateButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    const int side = qMax(16, qMin(width(), height()) - 8);
    setIconSize(QSize(side, side));
}

void MultiStateButton::onModbusValueChanged(const QVariant &val)
{
    const uint n = val.toUInt();
    const int idx = static_cast<int>(qBound(0u, n, static_cast<uint>(m_stateCount - 1)));
    if (idx == m_currentState)
        return;

    m_currentState = idx;
    applyStateVisuals();
    emit currentStateChanged(m_currentState);
}

void MultiStateButton::onClicked()
{
    int currentState = (m_currentState + 1) % m_stateCount;
    //applyStateVisuals();
    m_writeVariable->setValue(static_cast<uint>(currentState));
   // emit currentStateChanged(m_currentState);
}

void MultiStateButton::applyStateVisuals()
{
    const QString path = m_iconPaths.value(m_currentState);
    setIcon(path.isEmpty() ? QIcon() : QIcon(path));

    if (!m_stateTexts.isEmpty() || !m_stateTextFallback.isEmpty()) {
        QString label = m_stateTexts.value(m_currentState);
        if (label.isEmpty())
            label = m_stateTextFallback;
        setText(label);
    }
}

void MultiStateButton::ensureListLengths()
{
    while (m_iconPaths.size() < m_stateCount)
        m_iconPaths.append(QString());
    while (m_iconPaths.size() > m_stateCount)
        m_iconPaths.removeLast();

    while (m_stateTexts.size() < m_stateCount)
        m_stateTexts.append(QString());
    while (m_stateTexts.size() > m_stateCount)
        m_stateTexts.removeLast();
}

QString MultiStateButton::readAddress() const
{
    return m_readAddress;
}


QString MultiStateButton::writeAddress() const
{
    return m_writeAddress;
}


Variable *MultiStateButton::readVariable() const
{
    return m_readVariable;
}

Variable *MultiStateButton::writeVariable() const
{
    return m_writeVariable;
}


