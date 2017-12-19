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

#include <QCoreApplication>
#include <QTcpSocket>

#include "network/networklink.h"

//#include "network/networkmanager.h"
//#include "map/charactertoken.h"
//#include "Image.h"
//#include "data/person.h"
//#include "userlist/playersList.h"
#include "network/receiveevent.h"
#include "network/networkmessagewriter.h"
#include "network/messagedispatcher.h"

NetworkLink::NetworkLink(QTcpSocket* socket)
    : m_socketTcp(socket)
{
    m_receivingData = false;
    m_headerRead= 0;
    setSocket(socket);
}

NetworkLink::NetworkLink(ConnectionProfile* connection)
{
    setConnection(connection);
    setSocket(new QTcpSocket(this));
    m_receivingData = false;
    m_headerRead= 0;
}
void NetworkLink::initialize()
{
    m_socketTcp->setSocketOption(QAbstractSocket::KeepAliveOption,1);
    makeSignalConnection();
}

NetworkLink::~NetworkLink()
{
    if(nullptr!=m_socketTcp)
    {
        delete m_socketTcp;
        m_socketTcp=nullptr;
    }
}
void NetworkLink::makeSignalConnection()
{
    connect(m_socketTcp, SIGNAL(readyRead()),this, SLOT(receivingData()));
    connect(m_socketTcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connectionError(QAbstractSocket::SocketError)));
    connect(m_socketTcp, SIGNAL(disconnected()), this, SLOT(p_disconnect()));
    connect(m_socketTcp,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    connect(this,SIGNAL(dataToSend(char*,quint32,NetworkLink*)),this,SLOT(sendData(char*,quint32,NetworkLink*)));
}


void NetworkLink::connectionError(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur);
    if(nullptr==m_socketTcp)
    {
        return;
    }
    emit errorMessage(m_socketTcp->errorString());
}

void NetworkLink::p_disconnect()
{
    qWarning("Emit disconneted signal : s");
    emit disconnected(this);
}


void NetworkLink::sendData(char* data, quint32 size, NetworkLink* but)
{
    if(nullptr==m_socketTcp)
    {
        emit errorMessage(tr("Socket is null"));
        return;
    }

    if (but != this)
    {
        // Emission des donnees
       #ifdef DEBUG_MODE
        NetworkMessageHeader header;
        memcpy((char*)&header,data,sizeof(NetworkMessageHeader));
        qDebug() << "send Data Categorie 1:" << MessageDispatcher::cat2String((NetworkMessageHeader*)data) << "Action" << MessageDispatcher::act2String((NetworkMessageHeader*)data);

        qDebug() << "header: cat" << header.category << "act:"<< header.action << "datasize:" << header.dataSize <<  "size"<<size << (int)data[0]
                 << "socket" << m_socketTcp;

        qDebug() << "thread current" << QThread::currentThread() << "thread socket:" << m_socketTcp->thread() << "this thread" << thread();
        #endif

        int t = m_socketTcp->write(data, size);

        if (t < 0)
        {
            emit errorMessage(tr("Tranmission error :")+m_socketTcp->errorString());
        }
    }
}

void NetworkLink::sendData(NetworkMessage* msg)
{
    if(nullptr==m_socketTcp)
    {
        emit errorMessage(tr("Socket is null"));
        return;
    }
    //if (but != this)
    {
        int t = m_socketTcp->write((char*)msg->buffer(), msg->getSize());


        if (t < 0)
        {
            emit errorMessage(tr("Tranmission error :")+m_socketTcp->errorString());
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
    if(nullptr==m_socketTcp)
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
            //QApplication::alert(m_mainWindow);
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
                QList<NetWorkReceiver*> tmpList = ReceiveEvent::getNetWorkReceiverFor(static_cast<NetMsg::Category>(m_header.category));

                for(NetWorkReceiver* tmp : tmpList)
                {
                    tmp->processMessage(&data);
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
                case NetMsg::AdministrationCategory:
                    processAdminstrationMessage(&data);
                default:
                    break;
            }
            delete[] m_buffer;
            m_remainingData = 0;
            m_receivingData = false;
        }

    }
}
void NetworkLink::processAdminstrationMessage(NetworkMessageReader* msg)
{
    if(NetMsg::heartbeat == msg->action())
    {
        QString id = msg->string8();
        if(!m_hbCount.contains(id))
        {
            m_hbCount[id]=0;
        }
        else
        {
            m_hbCount[id]+=1;
        }
    }
    else if(NetMsg::AuthentificationSucessed == msg->action())
    {
        emit authentificationSuccessed();
    }
    else if(NetMsg::AuthentificationFail == msg->action())
    {
        emit errorMessage(tr("Authentification Fail"));
        emit authentificationFail();
        if(isOpen())
        {
            disconnectAndClose();
        }
    }
    else if(NetMsg::ClearTable == msg->action())
    {
        emit clearData();
    }
    else if(NetMsg::AdminAuthFail == msg->action())
    {
        emit adminAuthFailed();
    }
    else if(NetMsg::AdminAuthSucessed == msg->action())
    {
        emit adminAuthSuccessed();
    }
    else if(NetMsg::NeedPassword == msg->action())
    {
        if(nullptr != m_connection)
        {
            const auto pw = m_connection->getPassword();
            if(pw.isEmpty())
            {
                emit errorMessage(tr("Authentification Fail"));
                emit authentificationFail();
                emit disconnected();
                if(isOpen())
                {
                    disconnectAndClose();
                }
            }
        }
    }
    else if(NetMsg::SetChannelList == msg->action())
    {
    }
    else
        qDebug() <<   "processAdminstrationMessage "<< msg->action();
}

void NetworkLink::processPlayerMessage(NetworkMessageReader* msg)
{
    if(NetMsg::PlayerCategory==msg->category())
    {
        if(NetMsg::PlayerConnectionAction == msg->action())
        {
            //m_networkManager->addNetworkLink(this);

            NetworkMessageHeader header;
            header.category = NetMsg::AdministrationCategory;
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
}

ConnectionProfile *NetworkLink::getConnection() const
{
    return m_connection;
}

void NetworkLink::setConnection(ConnectionProfile* value)
{
    m_connection = value;
}
void NetworkLink::disconnectAndClose()
{
    if(nullptr!=m_socketTcp)
    {
        m_socketTcp->close();
    }
}
void NetworkLink::setSocket(QTcpSocket* socket, bool makeConnection)
{
    m_socketTcp=socket;
    if(makeConnection)
    {
        initialize();
    }

}
void NetworkLink::insertNetWortReceiver(NetWorkReceiver* receiver,NetMsg::Category cat)
{
    m_receiverMap.insert(cat,receiver);
}
void NetworkLink::connectTo()
{
   if(nullptr != m_socketTcp)
   {
       if(nullptr != m_connection)
       {
            m_socketTcp->connectToHost(m_connection->getAddress(), m_connection->getPort());
       }
       else
       {
           emit errorMessage(tr("Connection Profile is not defined"));
       }
   }
}
void NetworkLink::socketStateChanged(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::ClosingState:
    case QAbstractSocket::UnconnectedState:
        emit disconnected();
        break;
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
        emit connecting();//setConnectionState(CONNECTING);
        break;
    case QAbstractSocket::ConnectedState:
    {
        emit connected();
        QString pw = m_connection->getPassword();
        if(!pw.isEmpty())
        {
            NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::Password);
            msg.string32(pw);
            msg.uint8(false);
            msg.sendAll();
        }
        //setConnectionState(CONNECTED);
    }
        break;
    default:
        break;
    }
}
bool NetworkLink::isOpen() const
{
    if(nullptr != m_socketTcp)
    {
        return m_socketTcp->isOpen();
    }
    return false;
}
