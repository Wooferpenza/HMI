#ifndef VARIABLE_H
#define VARIABLE_H

#include <QObject>
#include <QVariant>
#include <QVector>
//enum  DataType { Bit,  UWord, SWord, UDWord, SDWord, Float, String };

class Variable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString format WRITE setFormat FINAL)
    Q_PROPERTY(uint16_t fractional READ fractional WRITE setFractional FINAL)
    Q_PROPERTY(float minimum READ minimum WRITE setMinimum FINAL)
    Q_PROPERTY(float maximum READ maximum WRITE setMaximum FINAL)
    Q_PROPERTY(QString addressStr WRITE setAddressStr FINAL)

public:

    enum  DataFormat { Bit, UWord, SWord, UDWord, SDWord, Float, String };
    Q_ENUM(DataFormat)

    explicit Variable(QObject *parent = nullptr);

    DataFormat type() const;
    void setType(DataFormat newType);

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

    void setAddressStr(const QString &newAddressStr);

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
    DataFormat m_type = DataFormat::UWord;
    uint16_t m_fractional = 0;
    float m_minimum = 0.0;
    float m_maximum = 100.0;
    QVector<uint16_t> m_rawValue;
    uint16_t m_rawValueSize = 1;
    QVariant m_value;
    void valueToRawValue();
    uint16_t m_address=0;
    uint16_t m_addressBit=0;

    QString m_format;
    QString m_addressStr;

public slots:
    void setRawData(const QVector<uint16_t> &raw);
    void setValue(const QVariant &val);
    void onSetFormat();
    void setFormat(QString newFormat);
};

#endif // VARIABLE_H
