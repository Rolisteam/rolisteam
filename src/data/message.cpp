/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "message.h"
#include <QTcpSocket>

#include <QDebug>
Message::Message()
{
    //m_internalData << (quint32)0 << (quint32)0;
}
quint32  Message::getSize()
{
    return m_internalData.size();
}

Network::Category Message::getType()
{
    return m_type;
}

void Message::setCategory(Network::Category type)
{
    m_type = type;
}

void Message::write(QTcpSocket* tmp)
{


   /* QByteArray msg;*/
   QDataStream cout(&m_internalData,QIODevice::WriteOnly);
   cout.setVersion(QDataStream::Qt_4_4);
   cout.device()->seek(0);
   //cout << (quint32)m_type<<((quint32)m_internalData.size() - sizeof(quint32) - sizeof(quint32));
    //quint32 size= m_internalData.size();
    //cou << (quint32)m_type<<(quint32)size << m_internalData;
    //qDebug() <<(quint32)m_type<<(quint32)size;*/


   qDebug() << tmp->write(m_internalData) << "Message::write(QTcpSocket* tmp)";
}

QByteArray* Message::getDataArray()
{
    return &m_internalData;
}
void Message::setSender(QTcpSocket* sender)
{
    m_sender = sender;
}

QTcpSocket* Message::getSender()
{
    return m_sender;
}
