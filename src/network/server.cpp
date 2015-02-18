/***************************************************************************
    *   Copyright (C) 2011 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
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

#include "server.h"
#include <QTcpSocket>
#include "message.h"
#include "player.h"
#include "sendertoclient.h"
Server::Server(int port,QObject *parent) :
    QTcpServer(parent),m_port(port)
{
    m_list = new QList<QTcpSocket*>;
    m_messageQueue =new QList<Message*>;
    m_clientSender = new SenderToClient(m_messageQueue,m_list);
    
    
    
    m_clientSender->start();
    
    connect(this,SIGNAL(newConnection()),this,SLOT(incommingTcpConnection()));
    connect(this,SIGNAL(messageToAnalyse()),m_clientSender,SLOT(messageToSend()));
}
void Server::startConnection()
{
    if(!listen(QHostAddress::Any,m_port))
    {
        emit error(tr("Enable to start the server"));
    }
}
void Server::incommingTcpConnection()
{
    if(hasPendingConnections())
    {
        // qDebug() <<  type.size() << "size readAll";
        emit error(tr("size readAll "));
        QTcpSocket* tmp = nextPendingConnection();
        connect(tmp,SIGNAL(readyRead()),this,SLOT(readDataFromClient()));
        m_list->append(tmp);
    }
}
void Server::readDataFromClient()
{
    
    QTcpSocket* tmp = static_cast<QTcpSocket*>(sender());
    /// @todo: base line of server, forward messages to all other clients.
    
    QByteArray type =tmp->readAll();
    qDebug() <<  type.size() << "size readAll";
    emit error(tr("size readAll %1").arg(type.size()));
    QDataStream stream(type);
    stream.setVersion(QDataStream::Qt_4_4);
    
    
    while(!stream.atEnd())
    {
        quint32 cat;
        stream >> cat;
        //stream >> name;
        Message* msg = new Message;
        quint32 size;
        stream >> size;
        msg->setCategory((Network::Category)cat);
        switch(msg->getType())
        {
        case  Network::ChatCategory:
            qDebug() << "Type of tchat";
            emit error(tr("Type of tchat"));
            break;
        case  Network::UsersCategory:
        {
            qDebug() << "Type of User" << size;
            emit error(tr("Type of user %1").arg(size));
            Player* player = new Player();
            stream >> *player;
            qDebug() << player->getColor() << player->getName();

            QByteArray* tmpArray = msg->getDataArray();

            QDataStream msgs(tmpArray,QIODevice::WriteOnly);
            msgs.setVersion(QDataStream::Qt_4_4);
            /// @todo: Clean up this thing

            msg->setCategory(Network::UsersCategory);
            msgs <<  (quint32)0 << (quint32)0 << *player;
        }
            break;
        default:
            //qDebug() << "Type ="<< (int)type[0] << type.size();
            break;
        }
        msg->setSender(tmp);
        m_messageQueue->append(msg);
        emit messageToAnalyse();
    }
}
