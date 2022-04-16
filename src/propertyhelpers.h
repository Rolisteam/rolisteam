#ifndef PROPERTY_HELPERS_H
#define PROPERTY_HELPERS_H

#include <QObject>

#define WRITABLE_PROPERTY(type, name)                                           \
protected:                                                                      \
    Q_PROPERTY(type name READ get_##name WRITE set_##name NOTIFY name##Changed) \
    type m_##name;                                                              \
                                                                                \
public:                                                                         \
    type get_##name() const { return m_##name; }                                \
public Q_SLOTS:                                                                 \
    bool set_##name(type name)                                                  \
    {                                                                           \
        bool ret= false;                                                        \
        if((ret= m_##name != name))                                             \
        {                                                                       \
            m_##name= name;                                                     \
            emit name##Changed(m_##name);                                       \
        }                                                                       \
        return ret;                                                             \
    }                                                                           \
Q_SIGNALS:                                                                      \
    void name##Changed(type name);                                              \
                                                                                \
private:

#define READONLY_PROPERTY(type, name)                          \
protected:                                                     \
    Q_PROPERTY(type name READ get_##name NOTIFY name##Changed) \
    type m_##name;                                             \
                                                               \
public:                                                        \
    type get_##name() const { return m_##name; }               \
public Q_SLOTS:                                                \
    bool update_##name(type name)                              \
    {                                                          \
        bool ret= false;                                       \
        if((ret= m_##name != name))                            \
        {                                                      \
            m_##name= name;                                    \
            emit name##Changed(m_##name);                      \
        }                                                      \
        return ret;                                            \
    }                                                          \
Q_SIGNALS:                                                     \
    void name##Changed(type name);                             \
                                                               \
private:

#endif // PROPERTY_HELPERS_H
