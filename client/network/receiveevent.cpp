/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include <QCoreApplication>
#include <QTimer>

#include "network/receiveevent.h"

#include "network/networkmessagereader.h"


/****************
 * ReceiveEvent *
 ****************/

const int ReceiveEvent::Type = QEvent::registerEventType();
QMap<quint16, QObject *> ReceiveEvent::s_receiverMap;
QMap<NetMsg::Category, NetWorkReceiver*> ReceiveEvent::ms_netWorkReceiverMap;

quint16 makeKey(quint8 categorie, quint8 action)
{
    return ((quint16) categorie) + (((quint16) action) * 256);
}


ReceiveEvent::ReceiveEvent(ClientManager* manager, const NetworkMessageHeader & header, const char * buffer, NetworkLink * link)
    : QEvent((QEvent::Type)ReceiveEvent::Type), m_data(manager,header, buffer), m_link(link), m_repost(0)
{}

ReceiveEvent::ReceiveEvent(ClientManager* manager,const ReceiveEvent & other)
    : QEvent((QEvent::Type)ReceiveEvent::Type), m_data(manager,other.m_data), m_link(other.m_link), m_repost(other.m_repost + 1)
{}

ReceiveEvent::~ReceiveEvent()
{}

void ReceiveEvent::postToReceiver()
{
    quint16 key = makeKey(m_data.category(), m_data.action());
    if (s_receiverMap.contains(key))
        QCoreApplication::postEvent(s_receiverMap.value(key), this, Qt::LowEventPriority);
}

void ReceiveEvent::repostLater() const
{
    if (m_repost < 25)
        new DelayReceiveEvent(*this);
}

NetworkLink * ReceiveEvent::link() const
{
    return m_link;
}

NetworkMessageReader & ReceiveEvent::data()
{
    return m_data;
}

bool ReceiveEvent::hasReceiverFor(quint8 categorie, quint8 action)
{
    return s_receiverMap.contains(makeKey(categorie, action));
}

void ReceiveEvent::registerReceiver(NetMsg::Category categorie, NetMsg::Action action, QObject * receiver)
{
    quint16 key = makeKey(categorie, action);
    if (s_receiverMap.contains(key))
    {
        qFatal("A receiver is already registered for (%d,%d)", categorie, action);
    }

    s_receiverMap.insert(key, receiver);
}
void ReceiveEvent::registerNetworkReceiver(NetMsg::Category categorie, NetWorkReceiver* receiver)
{
    ms_netWorkReceiverMap.insertMulti(categorie, receiver);
}

void ReceiveEvent::removeNetworkReceiver(NetMsg::Category categorie, NetWorkReceiver* receiver)
{
    QMap<NetMsg::Category, NetWorkReceiver *>::iterator it = ms_netWorkReceiverMap.find(categorie);
    while (it != ms_netWorkReceiverMap.end() )
    {
        if(it.value()==receiver)
        {
            it = ms_netWorkReceiverMap.erase(it);
        }
        else
        {
            ++it;
        }
    }

}
bool ReceiveEvent::hasNetworkReceiverFor(NetMsg::Category categorie)
{
    return ms_netWorkReceiverMap.contains(categorie);
}

QList<NetWorkReceiver*> ReceiveEvent::getNetWorkReceiverFor(NetMsg::Category categorie)
{
    return ms_netWorkReceiverMap.values(categorie);
}

/*********************
 * DelayReceiveEvent *
 *********************/

DelayReceiveEvent::DelayReceiveEvent(const ReceiveEvent & event)
{
    m_event = new ReceiveEvent(event);
    QTimer::singleShot(200, this, SLOT(postEvent()));
}

DelayReceiveEvent::~DelayReceiveEvent()
{}

void DelayReceiveEvent::postEvent()
{
    m_event->postToReceiver();
    deleteLater();
}
