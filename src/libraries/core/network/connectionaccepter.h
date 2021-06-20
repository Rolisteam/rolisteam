#ifndef CONNECTIONACCEPTER_H
#define CONNECTIONACCEPTER_H

#include <QMap>
#include <QString>
#include <QVariant>

/**
 * @brief The ConnectionAccepter class abstract class to implement chain of responsability
 */
class ConnectionAccepter
{
public:
    ConnectionAccepter();
    virtual ~ConnectionAccepter();
    void setNext(ConnectionAccepter* next);

    bool runAccepter(const QMap<QString, QVariant>& data) const;

    bool isActive() const;
    void setIsActive(bool isActive);

protected:
    virtual bool isValid(const QMap<QString, QVariant>& data) const= 0;

protected:
    ConnectionAccepter* m_next= nullptr;
    bool m_isActive;
};

#endif // CONNECTIONACCEPTER_H
