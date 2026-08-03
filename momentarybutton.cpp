#include "momentarybutton.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QTouchEvent>
MomentaryButton::MomentaryButton(QWidget *parent)
    : QPushButton(parent)
    , m_writeVariable(new Variable(this))
    , m_readVariable(new Variable(this))
{
    m_writeVariable->setType(Variable::DataFormat::Bit);
    m_writeVariable->setMinimum(0);
    m_writeVariable->setMaximum(1);
    m_readVariable->setType(Variable::DataFormat::Bit);
    m_readVariable->setMinimum(0);
    m_readVariable->setMaximum(1);
    connect(this, &MomentaryButton::clicked, this, [this](){setChecked(!isChecked());});
    connect(this, &MomentaryButton::writeAddressChanged, m_writeVariable, &Variable::setAddressStr);
    connect(this, &MomentaryButton::readAddressChanged, m_readVariable, &Variable::setAddressStr);
    connect(this, &QPushButton::pressed, this, [this]() { m_writeVariable->setValue(true);});
    connect(this, &QPushButton::released, this, [this]() { m_writeVariable->setValue(false);});
    connect(m_readVariable, &Variable::valueChanged,this, &MomentaryButton::onModbusValueChanged);
    this->setCheckable(true);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
}

Variable *MomentaryButton::writeVariable() const
{
    return m_writeVariable;
}
Variable *MomentaryButton::readVariable() const
{
    return m_readVariable;
}

void MomentaryButton::onModbusValueChanged(const QVariant &val)
{
    const bool state = val.toBool();
    setChecked(state);
}
// bool MomentaryButton::event(QEvent *event) {
//     // 1. Перехватываем сенсорные события Windows
//     if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchEnd) {
//         QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

//         if (!touchEvent->points().isEmpty()) {
//             const QEventPoint &point = touchEvent->points().first();
//             QPointF localPos = point.position();

//             if (event->type() == QEvent::TouchBegin) {
//                 // Имитируем нажатие левой кнопки мыши мгновенно
//                 QMouseEvent *mousePress = new QMouseEvent(
//                     QEvent::MouseButtonPress, localPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier
//                     );
//                 QCoreApplication::postEvent(this, mousePress);
//             }
//             else if (event->type() == QEvent::TouchEnd) {
//                 // Имитируем отпускание левой кнопки мыши мгновенно
//                 QMouseEvent *mouseRelease = new QMouseEvent(
//                     QEvent::MouseButtonRelease, localPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier
//                     );
//                 QCoreApplication::postEvent(this, mouseRelease);
//             }
//         }

//         // Говорим Windows 10, что тач обработан, и оригинальное касание нужно погасить
//         event->accept();
//         return true;
//     }
//     // 2. Блокируем "искусственные" клики мыши, которые Windows 10 пытается создать сама из тача
//     if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease) {
//         QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

//         // Проверяем источник события. Если это синтетический клик от Windows-тача, игнорируем его
// #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//         if (mouseEvent->pointingDevice()->type() == QInputDevice::DeviceType::TouchScreen) {
// #else
//             // Для Qt 5 используем старый синтаксис источника
//         if (mouseEvent->source() == Qt::MouseEventSynthesizedBySystem) {
// #endif
//             event->accept();
//             return true;
//         }
//     }

//     // Настоящая физическая мышь пойдет сюда и отработает штатно
//     return QPushButton::event(event);
// }