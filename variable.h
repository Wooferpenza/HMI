#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <cstdint>
inline QVector<uint16_t> packFloatLowWordFirst(float f)
{
    uint32_t r = 0;
    std::memcpy(&r, &f, sizeof(float));
    return {static_cast<uint16_t>(r & 0xFFFFu), static_cast<uint16_t>(r >> 16)};
}

inline float unpackFloatLowWordFirst(uint16_t low, uint16_t high)
{
    uint32_t r = (uint32_t(high) << 16) | low;
    float f = 0.0f;
    std::memcpy(&f, &r, sizeof(float));
    return f;
}

inline QVector<uint16_t> packDWordLowWordFirst(quint32 v)
{
    return {static_cast<uint16_t>(v & 0xFFFFu), static_cast<uint16_t>(v >> 16)};
}

inline quint32 unpackDWordLowWordFirst(uint16_t low, uint16_t high)
{
    return (quint32(high) << 16) | low;
}

enum class DataType { Bit,UWord, SWord, UDWord, SDWord, Float, String };
class Variable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(DataType type READ type WRITE setType NOTIFY typeChanged FINAL)

    Q_PROPERTY(uint16_t integerDigit READ integerDigit WRITE setIntegerDigit NOTIFY integerDigitChanged FINAL)
    Q_PROPERTY(uint16_t fractional READ fractional WRITE setFractional NOTIFY fractionalChanged FINAL)
    Q_PROPERTY(float minimum READ minimum WRITE setMinimum NOTIFY minimumChanged FINAL)
    Q_PROPERTY(float maximum READ maximum WRITE setMaximum NOTIFY maximumChanged FINAL)
    // uint16_t mInterDigit;
public:
    explicit Variable(QObject *parent = nullptr);

    DataType type() const;
    void setType(DataType newType);
    QString name() const;
    void setName(const QString &newName);

    uint16_t integerDigit() const;
    void setIntegerDigit(uint16_t newIntegerDigit);

    uint16_t fractional() const;
    void setFractional(uint16_t newFractional);

    float minimum() const;
    void setMinimum(float newMinimum);

    float maximum() const;
    void setMaximum(float newMaximum);

    uint16_t rowValueSize();

signals:
    void typeChanged();
    void formatChanged();

    void nameChanged();

    void integerDigitChanged();

    void fractionalChanged();

    void minimumChanged();

    void maximumChanged();

    void valueChanged(QVariant);
    void userValueChanged(QVariant);
    void rowValueChanged(QVector<uint16_t>);

private:
    DataType m_type = DataType::UWord;
    QString m_name;
    uint16_t m_integerDigit = 0;
    uint16_t m_fractional = 0;
    float m_minimum = 0.0;
    float m_maximum = 100.0;
    QVector<uint16_t> mRowValue;
    uint16_t mRowValueSize = 1;
    QVariant mValue;
    void valueToRowValue();
public slots:
    void setRowData(QVector<uint16_t> row);
    void setValue(QVariant val);
};

#endif // VARIABLE_H
