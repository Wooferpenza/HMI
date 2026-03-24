#include "multistatebutton.h"

#include <QResizeEvent>

MultiStateButton::MultiStateButton(QWidget *parent)
    : QPushButton(parent)
    , m_variable(new Variable(this))
{
    m_variable->setType(DataType::UWord);
    m_variable->setMinimum(0);
    m_variable->setMaximum(1);
    ensureListLengths();

    connect(m_variable, &Variable::valueChanged,
            this, &MultiStateButton::onModbusValueChanged);
    connect(this, &QPushButton::clicked,
            this, &MultiStateButton::onClicked);

    applyStateVisuals();
}

Variable *MultiStateButton::variable() const
{
    return m_variable;
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
    m_variable->setMaximum(static_cast<float>(m_stateCount - 1));

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

int MultiStateButton::currentState() const
{
    return m_currentState;
}

void MultiStateButton::setCurrentState(int state)
{
    state = qBound(0, state, m_stateCount - 1);
    if (m_currentState == state)
        return;

    m_currentState = state;
    applyStateVisuals();
    m_variable->setValue(static_cast<uint>(m_currentState));
    emit currentStateChanged(m_currentState);
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
    m_currentState = (m_currentState + 1) % m_stateCount;
    applyStateVisuals();
    m_variable->setValue(static_cast<uint>(m_currentState));
    emit currentStateChanged(m_currentState);
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
