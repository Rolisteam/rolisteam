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

    virtual bool isValid(const QMap<QString,QVariant>& data) = 0;

    bool isActive() const;
    void setIsActive(bool isActive);

protected:
    ConnectionAccepter* m_next;
    bool m_isActive;
};

#endif // CONNECTIONACCEPTER_H
