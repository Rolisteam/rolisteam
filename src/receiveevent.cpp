#include "receiveevent.h"

#include "datareader.h"

const int ReceiveEvent::Type = QEvent::registerEventType();

ReceiveEvent::ReceiveEvent(quint8 categorie, quint8 action, quint32 bufferSize, const char * buffer)
    : QEvent((QEvent::Type)ReceiveEvent::Type), m_categorie(categorie), m_action(action)
{
    m_data = new DataReader(bufferSize, buffer);
}

ReceiveEvent::~ReceiveEvent()
{
    delete m_data;
}

quint8
ReceiveEvent::categorie()
{
    return m_categorie;
}

quint8
ReceiveEvent::action()
{
    return m_action;
}

DataReader &
ReceiveEvent::data()
{
    return *m_data;
}
