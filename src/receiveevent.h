#ifndef RECEIVE_EVENT_H
#define RECEIVE_EVENT_H

#include <QEvent>
#include <QString>

class DataReader;

class ReceiveEvent
 : public QEvent
{
    public:
        ReceiveEvent(quint8 categorie, quint8 action, quint32 bufferSize, const char * buffer);
        ~ReceiveEvent();

        static const int Type;

        quint8 categorie();
        quint8 action();
        DataReader & data();

    private:
        quint8 m_categorie;
        quint8 m_action;
        DataReader * m_data;
};

#endif
