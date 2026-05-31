#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>
#include <QVariant>
#include <QVector>
enum  DataType { Bit, UWord, SWord, UDWord, SDWord, Float, String };
class Variable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DataType type READ type WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(uint16_t fractional READ fractional WRITE setFractional NOTIFY fractionalChanged FINAL)
    Q_PROPERTY(float minimum READ minimum WRITE setMinimum NOTIFY minimumChanged FINAL)
    Q_PROPERTY(float maximum READ maximum WRITE setMaximum NOTIFY maximumChanged FINAL)
    Q_PROPERTY(uint16_t Address READ address WRITE setAddress NOTIFY addressChanged FINAL)
    Q_PROPERTY(uint16_t AddressBit READ addressBit WRITE setAddressBit NOTIFY addressBitChanged FINAL)

public:

    explicit Variable(QObject *parent = nullptr);

    DataType type() const;
    void setType(DataType newType);

    uint16_t fractional() const;
    void setFractional(uint16_t newFractional);

    float minimum() const;
    void setMinimum(float newMinimum);

    float maximum() const;
    void setMaximum(float newMaximum);

    uint16_t rawValueSize() const;

    uint16_t address() const;
    void setAddress(uint16_t newReadAddress);

    uint16_t addressBit() const;
    void setAddressBit(uint16_t newBitIndex);

signals:
    void typeChanged();
    void nameChanged();
    void fractionalChanged();
    void minimumChanged();
    void maximumChanged();
    void valueChanged(const QVariant &value);
    void rawValueChanged(const QVector<uint16_t> &rawValue);
    void addressChanged();
    void addressBitChanged();
private:
    DataType m_type = DataType::UWord;
    uint16_t m_fractional = 0;
    float m_minimum = 0.0;
    float m_maximum = 100.0;
    QVector<uint16_t> m_rawValue;
    uint16_t m_rawValueSize = 1;
    QVariant m_value;
    void valueToRawValue();
    uint16_t m_writeAddress=0;
    uint16_t m_writeAddressBit=0;
    uint16_t m_readAddress=0;
    uint16_t m_readAddressBit = 0;
public slots:
    void setRawData(const QVector<uint16_t> &raw);
    void setValue(const QVariant &val);
};

#endif // VARIABLE_H
