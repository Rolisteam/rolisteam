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
#include <QHostAddress>
#include <QDebug>

#include "rclient.h"
#include "messagemanager.h"
#include "readingthread.h"

RClient::RClient(QObject *parent)
    : QObject(parent)
{
    m_client = new QTcpSocket(this);
    m_messageToSendList = new QList<Message*>();
    m_messageReceivedList = new QList<Message*>();

    //m_registedSender = new QMap<Network::Category,MessageManager*>;
    m_reading = new ReadingThread(m_client,m_messageReceivedList);

    m_currentState =DISCONNECTED;
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorOccurs()));
    connect(m_client,SIGNAL(connected()),this,SLOT(connected()));
    connect(m_client,SIGNAL(readyRead()),m_reading,SLOT(readDataFromSocket()));
    connect(m_client,SIGNAL(readyRead()),m_reading,SLOT(readDataFromSocket()));
    connect(m_reading,SIGNAL(messageRecieved()),this,SLOT(dispachRecievedMessage()));
    connect(this,SIGNAL(messageInQueue()),this,SLOT(sendMessage()));
}

RClient::~RClient()
{

}
void RClient::errorOccurs()
{
    m_currentState=ERROR;
    emit stateChanged(m_currentState);
    qDebug() << m_client->errorString();
}

void RClient::startConnection(Connection& connection)
{
    m_connection = connection;
    m_reading->start();
    m_client->connectToHost(m_connection.getAddress(),m_connection.getPort());
}
void RClient::connected()
{
    m_currentState=CONNECTED;
    emit stateChanged(m_currentState);
    emit connectionEstablished();
    qDebug() << "connected established";
}
bool RClient::isConnected()
{
    if(m_currentState==CONNECTED)
        return true;
    return false;
}

void RClient::registerMessageManager(Network::Category cat,MessageManager* manager)
{
    m_registedSender.insert(cat,manager);
}

void RClient::addMessageToSendQueue(Message* m)
{
    m_readingMutex.lock();
    m_messageToSendList->append(m);
    m_readingMutex.unlock();

    emit messageInQueue();
}
void RClient::sendMessage()
{
    while(!m_messageToSendList->empty())
    {

        Message* m= m_messageToSendList->takeFirst();

        qDebug() <<  m->getDataArray()->size() << "RClient::sendMessage()";
        m->write(m_client);

        delete m;
        /*m_client->write(t.getType());
        m_client->write(t.getSize());
        m_client->write(t.getDataArray());*/
    }
}
void RClient::dispachRecievedMessage()
{

    while(!m_messageReceivedList->empty())
    {
        Message* m= m_messageReceivedList->takeFirst();
        /// callback system
        m_registedSender.value(m->getType())->processed(m);

    }

}





/*int RClient::getId(void* t)
{/// @todo a better way should be found. Not network ready the server should give the id instead.
        m_registedSender.append(t);
        return m_registedSender.size();
}*/
