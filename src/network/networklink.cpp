/************************************************************************
*   Copyright (C) 2007 by Romain Campioni                               *
*   Copyright (C) 2009 by Renaud Guezennec                              *
*   Copyright (C) 2011 by Joseph Boudou                                 *
*                                                                       *
*     http://www.rolisteam.org/                                         *
*                                                                       *
*   Rolisteam is free software; you can redistribute it and/or modify   *
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
#include <QTcpSocket>

#include "network/networklink.h"

#include "map/map.h"
#include "map/mapframe.h"
#include "network/networkmanager.h"
#include "map/charactertoken.h"
#include "Image.h"
#include "data/person.h"
#include "userlist/playersList.h"
#include "network/receiveevent.h"


NetworkLink::NetworkLink(QTcpSocket *socket)
    : QObject(NULL),m_mainWindow(NULL)
{
    m_mainWindow = MainWindow::getInstance();
    m_networkManager = m_mainWindow->getNetWorkManager();
    m_socketTcp = socket;
    m_receivingData = false;
    m_headerRead= 0;
    setSocket(socket);

    if(!m_networkManager->isServer())
    {
        m_networkManager->addNetworkLink(this);
    }
}

NetworkLink::NetworkLink(ConnectionProfile* connection)
    : m_host("localhost"),m_port(6660)
{
    m_mainWindow = MainWindow::getInstance();
    m_networkManager = m_mainWindow->getNetWorkManager();
    setConnection(connection);
    m_socketTcp = new QTcpSocket(this);
    initialize();
    m_receivingData = false;
    m_headerRead= 0;
}
void NetworkLink::initialize()
{
    makeSignalConnection();
}

NetworkLink::~NetworkLink()
{
    if(NULL!=m_socketTcp)
    {
        delete m_socketTcp;
        m_socketTcp=NULL;
    }
}
void NetworkLink::makeSignalConnection()
{
    connect(m_socketTcp, SIGNAL(readyRead()),this, SLOT(receivingData()));
    connect(m_socketTcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(m_socketTcp, SIGNAL(disconnected()), this, SLOT(p_disconnect()));
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    connect(m_socketTcp,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SIGNAL(connnectionStateChanged(QAbstractSocket::SocketState)));

    connect(this,SIGNAL(dataToSend(char*,quint32,NetworkLink*)),this,SLOT(sendData(char*,quint32,NetworkLink*)));
}


void NetworkLink::connectionError(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur);
    if(NULL==m_socketTcp)
    {
        return;
    }
    errorMessage(m_socketTcp->errorString());
}

void NetworkLink::p_disconnect()
{
    qWarning("Emit disconneted signal : s");
    emit disconnected(this);
}


void NetworkLink::sendData(char* data, quint32 size, NetworkLink* but)
{
    if(NULL==m_socketTcp)
    {
        qDebug() << "sendData is Null";
        return;
    }
    if (but != this)
    {
        // Emission des donnees
       #ifdef DEBUG_MODE
        NetworkMessageHeader header;
        memcpy((char*)&header,data,sizeof(NetworkMessageHeader));
        qDebug() << "header: cat" << header.category << "act:"<< header.action << "datasize:" << header.dataSize <<  "size"<<size << (int)data[0]
                 << "socket" << m_socketTcp;
        #endif

        int t = m_socketTcp->write(data, size);

        if (t < 0)
        {
            qWarning() << "Tranmission error :" << m_socketTcp->errorString();
        }
    }
}
void NetworkLink::sendData(NetworkMessage* msg)
{
    qDebug() << "sendData message socket" << m_socketTcp;
    if(NULL==m_socketTcp)
    {
        qDebug() << "sendData is Null";
        return;
    }
    //if (but != this)
    {
        int t = m_socketTcp->write((char*)msg->buffer(), msg->getSize());

        if (t < 0)
        {
            qWarning() << "Tranmission error :" << m_socketTcp->errorString();
        }
    }
}

void NetworkLink::sendDataSlot(char *data, quint32 size, NetworkLink *but)
{
    #ifdef DEBUG_MODE
     NetworkMessageHeader header;
     memcpy((char*)&header,data,sizeof(NetworkMessageHeader));
     qDebug() << "sendDataSlot header: cat" << header.category << "act:"<< header.action << "datasize:" << header.dataSize <<  "size"<<size << (int)data[0];
     #endif
    emit dataToSend(data,size, but);
}

void NetworkLink::receivingData()
{
    if(NULL==m_socketTcp)
    {
        return;
    }
    quint32 readData=0;

    while (m_socketTcp->bytesAvailable())
    {
        if (!m_receivingData)
        {
            int readDataSize = 0;
            char* tmp = (char *)&m_header;
            readDataSize = m_socketTcp->read(tmp+m_headerRead, sizeof(NetworkMessageHeader)-m_headerRead);
            readDataSize+=m_headerRead;

            qDebug() << "Read DataSize" << readDataSize << m_header.dataSize << m_header.category << m_header.action << m_socketTcp->bytesAvailable();
            if((readDataSize!=sizeof(NetworkMessageHeader)))//||(m_header.category>=NetMsg::LastCategory)
            {
                m_headerRead=readDataSize;
                return;
            }
            else
            {
               m_headerRead=0;
            }
            m_buffer = new char[m_header.dataSize];
            m_remainingData = m_header.dataSize;
            emit readDataReceived(m_header.dataSize,m_header.dataSize);

        }

        readData = m_socketTcp->read(&(m_buffer[m_header.dataSize-m_remainingData]), m_remainingData);

        if (readData < m_remainingData)
        {
            m_remainingData -= readData;
            m_receivingData = true;
            emit readDataReceived(m_remainingData,m_header.dataSize);
        }
        else
        {
            QApplication::alert(m_mainWindow);
            emit readDataReceived(0,0);

            // Send event
            if (ReceiveEvent::hasReceiverFor(m_header.category, m_header.action))
            {
                ReceiveEvent * event = new ReceiveEvent(m_header, m_buffer, this);
                event->postToReceiver();
            }
            NetworkMessageReader data(m_header,m_buffer);
            if (ReceiveEvent::hasNetworkReceiverFor((NetMsg::Category)m_header.category))
            {

                QList<NetWorkReceiver*> tmpList = ReceiveEvent::getNetWorkReceiverFor((NetMsg::Category)m_header.category);

                foreach(NetWorkReceiver* tmp, tmpList)
                {
                    forwardMessage(tmp->processMessage(&data));
                }
            }
            //emit receivedMessage(data,this);

            switch(data.category())
            {
                case NetMsg::PlayerCategory :
                    processPlayerMessage(&data);
                    break;
                case NetMsg::SetupCategory :
                    processSetupMessage(&data);
                    break;
            }
            delete[] m_buffer;
            m_receivingData = false;
        }

    }

}
void NetworkLink::processPlayerMessage(NetworkMessageReader* msg)
{
    if(NetMsg::PlayerCategory==msg->category())
    {
        if(NetMsg::PlayerConnectionAction == msg->action())
        {
            m_networkManager->addNetworkLink(this);

            NetworkMessageHeader header;
            header.category = NetMsg::SetupCategory;
            header.action = NetMsg::EndConnectionAction;
            header.dataSize = 0;
            sendData((char *)&header, sizeof(NetworkMessageHeader));
        }
        else if(NetMsg::AddPlayerAction == msg->action())
        {

        }
        else if(NetMsg::DelPlayerAction == msg->action())
        {
            forwardMessage(NetWorkReceiver::AllExceptSender);
        }
        else if(NetMsg::ChangePlayerNameAction == msg->action())
        {
           forwardMessage(NetWorkReceiver::AllExceptSender);
        }
        else if(NetMsg::ChangePlayerColorAction == msg->action())
        {
           forwardMessage(NetWorkReceiver::AllExceptSender);
        }
    }
}
void NetworkLink::processSetupMessage(NetworkMessageReader* msg)
{
    if (msg->action() == NetMsg::AddFeatureAction)
    {
        forwardMessage(NetWorkReceiver::AllExceptSender);
    }
}
void NetworkLink::forwardMessage( NetWorkReceiver::SendType type)
{
    if(NetWorkReceiver::NONE == type)
    {
        return;
    }
    if (m_networkManager->isServer())
    {
        char *donnees = new char[m_header.dataSize + sizeof(NetworkMessageHeader)];
        memcpy(donnees, &m_header, sizeof(NetworkMessageHeader));
        memcpy(&(donnees[sizeof(NetworkMessageHeader)]), m_buffer, m_header.dataSize);
        if (NetWorkReceiver::ALL == type)
        {
            m_networkManager->sendMessage(donnees,m_header.dataSize + sizeof(NetworkMessageHeader),NULL);
        }
        else if(NetWorkReceiver::AllExceptSender == type)
        {
            m_networkManager->sendMessage(donnees,m_header.dataSize + sizeof(NetworkMessageHeader),this);
        }
        delete[] donnees;
    }
}

ConnectionProfile *NetworkLink::getConnection() const
{
    return m_connection;
}

void NetworkLink::setConnection(ConnectionProfile* value)
{
    m_connection = value;
    if(NULL!=m_connection)
    {
        m_host = m_connection->getAddress();
        m_port = m_connection->getPort();
    }
}
void NetworkLink::disconnectAndClose()
{
    if(NULL!=m_socketTcp)
    {
        m_socketTcp->close();
        delete m_socketTcp;
        m_socketTcp=NULL;
    }
}
void NetworkLink::setSocket(QTcpSocket* socket, bool makeConnection)
{
    m_socketTcp=socket;
    if(makeConnection)
    {
        makeSignalConnection();
    }

}
void NetworkLink::insertNetWortReceiver(NetWorkReceiver* receiver,NetMsg::Category cat)
{
    m_receiverMap.insert(cat,receiver);
}
void NetworkLink::connectTo()
{
    qDebug() << "connect To thread"<<m_host << m_port;
    m_socketTcp->connectToHost(m_host, m_port);
}
