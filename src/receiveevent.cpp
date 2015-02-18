/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
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

#include <QApplication>

#include "receiveevent.h"

#include "datareader.h"

const int ReceiveEvent::Type = QEvent::registerEventType();
QMap<quint16, QObject *> ReceiveEvent::s_receiverMap;

quint16 makeKey(quint8 categorie, quint8 action)
{
    return ((quint16) categorie) + (((quint16) action) * 256);
}



ReceiveEvent::ReceiveEvent(quint8 categorie, quint8 action, quint32 bufferSize, const char * buffer, Liaison * link)
    : QEvent((QEvent::Type)ReceiveEvent::Type), m_categorie(categorie), m_action(action), m_link(link)
{
    m_data = new DataReader(bufferSize, buffer);
}

ReceiveEvent::~ReceiveEvent()
{
    delete m_data;
}

void ReceiveEvent::postToReceiver()
{
    quint16 key = makeKey(m_categorie, m_action);
    if (s_receiverMap.contains(key))
        QCoreApplication::postEvent(s_receiverMap.value(key), this, Qt::LowEventPriority);
}

quint8 ReceiveEvent::categorie() const
{
    return m_categorie;
}

quint8 ReceiveEvent::action() const
{
    return m_action;
}

Liaison * ReceiveEvent::link() const
{
    return m_link;
}

DataReader & ReceiveEvent::data()
{
    return *m_data;
}

bool ReceiveEvent::hasReceiverFor(quint8 categorie, quint8 action)
{
    return s_receiverMap.contains(makeKey(categorie, action));
}

void ReceiveEvent::registerReceiver(quint8 categorie, quint8 action, QObject * receiver)
{
    quint16 key = makeKey(categorie, action);
    if (s_receiverMap.contains(key))
    {
        qFatal("A receiver is allready registered for (%d,%d)", categorie, action);
    }

    s_receiverMap.insert(key, receiver);
}
