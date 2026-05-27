#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>
#include <QVariant>
#include <QVector>
enum  DataType { Bit, UWord, SWord, UDWord, SDWord, Float, String };
class Variable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(DataType type READ type WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(uint16_t fractional READ fractional WRITE setFractional NOTIFY fractionalChanged FINAL)
    Q_PROPERTY(float minimum READ minimum WRITE setMinimum NOTIFY minimumChanged FINAL)
    Q_PROPERTY(float maximum READ maximum WRITE setMaximum NOTIFY maximumChanged FINAL)
    Q_PROPERTY(uint16_t readAddress READ readAddress WRITE setReadAddress NOTIFY readAddressChanged FINAL)
    Q_PROPERTY(uint16_t readAddressBit READ readAddressBit WRITE setReadAddressBit NOTIFY readAddressBitChanged FINAL)
    Q_PROPERTY(uint16_t writeAddress READ writeAddress WRITE setWriteAddress NOTIFY writeAddressChanged FINAL)
    Q_PROPERTY(uint16_t writeAddressBit READ writeAddressBit WRITE setWriteAddressBit NOTIFY writeAddressBitChanged FINAL)
public:

    explicit Variable(QObject *parent = nullptr);

    DataType type() const;
    void setType(DataType newType);

    QString name() const;
    void setName(const QString &newName);

    uint16_t readAddressBit() const;
    void setReadAddressBit(uint16_t newBitIndex);

    uint16_t fractional() const;
    void setFractional(uint16_t newFractional);

    float minimum() const;
    void setMinimum(float newMinimum);

    float maximum() const;
    void setMaximum(float newMaximum);

    uint16_t rawValueSize() const;

    uint16_t writeAddress() const;
    void setWriteAddress(uint16_t newWriteAddress);

    uint16_t writeAddressBit() const;
    void setWriteAddressBit(uint16_t newWriteAddressBit);

    uint16_t readAddress() const;
    void setReadAddress(uint16_t newReadAddress);

signals:
    void typeChanged();
    void nameChanged();
    void readAddressBitChanged();
    void fractionalChanged();
    void minimumChanged();
    void maximumChanged();
    void valueChanged(const QVariant &value);
    void rawValueChanged(const QVector<uint16_t> &rawValue);
    void writeAddressChanged();
    void writeAddressBitChanged();

    void readAddressChanged();

private:
    DataType m_type = DataType::UWord;
    QString m_name;
    uint16_t m_readAddressBit = 0;
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

public slots:
    void setRawData(const QVector<uint16_t> &raw);
    void setValue(const QVariant &val);
};

#endif // VARIABLE_H
