/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                              *
 *   http://www.rolisteam.org                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include "sendertoclient.h"
#include <QTcpSocket>
#include "message.h"

SenderToClient::SenderToClient(QList<Message*>* list,QList<QTcpSocket*>* listsoc ,QObject *parent) :
    m_messageQueue(list),m_clientsList(listsoc),QThread(parent)
{
    QObject::moveToThread(this);
}

void SenderToClient::run()
{
    exec();
}
void SenderToClient::messageToSend()
{
    foreach(Message* tmpMsg, *m_messageQueue)
    {
        foreach(QTcpSocket* soc,*m_clientsList)
        {
            /// @todo add permission management
            if(tmpMsg->getSender()!=soc)
            {
                tmpMsg->write(soc);
            }
        }
    }



}
