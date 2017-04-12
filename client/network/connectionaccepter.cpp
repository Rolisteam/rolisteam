#include "connectionaccepter.h"

ConnectionAccepter::ConnectionAccepter()
    : m_next(nullptr)
{

}

void ConnectionAccepter::setNext(ConnectionAccepter* next)
{
    m_next = next;
}

bool ConnectionAccepter::isActive() const
{
    return m_isActive;
}

void ConnectionAccepter::setIsActive(bool isActive)
{
    m_isActive = isActive;
}
