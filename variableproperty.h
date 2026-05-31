#ifndef VARIABLEPROPERTY_H
#define VARIABLEPROPERTY_H

#include <QString>


//
//void setFormat( const QString &form) {
    //      VaribleProperty <MultiStateButton>::setFormat(form, [this,form]() { emit formatChanged(form); });
    //  }


template <typename T>
class VariableProperty {
public:

    QString format() const { return m_format; }
    template <typename NotifyFunc>
    void set_format(QString form, NotifyFunc notify) {
        if (m_format != form) {
            m_format = form;
              notify(); // Просто вызываем переданную лямбду
        }
    }

    uint16_t fractional() const { return m_fractional; }
    template <typename NotifyFunc>
    void set_fractional(uint16_t form, NotifyFunc notify) {
        if (m_fractional != form) {
            m_fractional = form;
            notify(); // Просто вызываем переданную лямбду
        }
    }

    float minimum() const { return m_minimum; }
    template <typename NotifyFunc>
    void set_minimum(float form, NotifyFunc notify) {
        if (m_minimum != form) {
            m_minimum = form;
            notify(); // Просто вызываем переданную лямбду
        }
    }

    float maximum() const { return m_maximum; }
    template <typename NotifyFunc>
    void set_maximum(float form, NotifyFunc notify) {
        if (m_maximum != form) {
            m_maximum = form;
            notify(); // Просто вызываем переданную лямбду
        }
    }

    QString readAddress() const { return m_readAddress; }
    template <typename NotifyFunc>
    void set_readAddress(QString form, NotifyFunc notify) {
        if (m_readAddress != form) {
            m_readAddress = form;
            notify(); // Просто вызываем переданную лямбду
        }
    }

    QString writeAddress() const { return m_writeAddress; }
    template <typename NotifyFunc>
    void set_writeAddress(QString form, NotifyFunc notify) {
        if (m_writeAddress != form) {
            m_writeAddress = form;
            notify(); // Просто вызываем переданную лямбду
        }
    }

private:
    QString m_format;
    QString m_readAddress;
    QString m_writeAddress;
    uint16_t m_fractional;
    float m_minimum;
    float m_maximum;
};

#define IMPLEMENT_SHARED_PROPERTY(Type, Name) \
Q_PROPERTY(Type Name READ Name WRITE set_##Name NOTIFY Name##Changed) \
    public: \
    void set_##Name(Type val) { \
    using BaseStorage = VariableProperty<std::remove_pointer_t<decltype(this)>>; \
        BaseStorage::set_##Name(val, [this, val]() { emit Name##Changed(val); }); \
} \
    public: \
    Q_SIGNAL void Name##Changed(Type);

#endif // VARIABLEPROPERTY_H
