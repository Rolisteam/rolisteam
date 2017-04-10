/************************************************************************
*   Copyright (C) 2007 by Romain Campioni                               *
*   Copyright (C) 2009 by Renaud Guezennec                              *
*   Copyright (C) 2010 by Joseph Boudou                                 *
*                                                                       *
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


#include "network/networkmanager.h"

#include <QTcpSocket>
//#include <QMessageBox>
#include "network/networklink.h"
#include "data/person.h"
#include "data/player.h"
#include "network/connectionprofile.h"

#define second 1000
NetworkManager* NetworkManager::m_singleton = nullptr;
/*****************
 * NetworkManager *
 *****************/
NetworkManager::NetworkManager()
    : QObject(), m_server(NULL),m_networkLinkToServer(NULL),m_disconnectAsked(false),m_connectionState(DISCONNECTED),m_localPlayer(NULL)
{
    m_reconnect = new QTimer(this);
    //m_preferences =  PreferencesManager::getInstance();
   // m_dialog = new ConnectionRetryDialog();
    m_playersList = PlayersList::instance();


}


NetworkManager* NetworkManager::getInstance()
{
    if(nullptr == m_singleton)
    {
        m_singleton = new NetworkManager();
    }
    return m_singleton;
}

NetworkManager::~NetworkManager()
{
   /* if(NULL!=m_dialog)
    {
        delete m_dialog;
        m_dialog = NULL;
    }*/

    delete m_reconnect;
}
void NetworkManager::setValueConnection(QString portValue,QString hostnameValue,QString username,QString roleValue)
{
    m_portStr = portValue;
    m_host = hostnameValue;
    m_role = roleValue;
    m_username = username;
    m_commandLineValue = true;
}
Player* NetworkManager::getLocalPlayer()
{
    return m_localPlayer;
}
bool NetworkManager::startConnection()
{
    // Server setup
    if (m_connectionProfile->isServer())
    {
        if(startListening())
        {
            setConnectionState(LISTENING);
        }
        else
        {
            setConnectionState(DISCONNECTED);
            return false;
        }
        m_port = m_connectionProfile->getPort();
    }
    else
    {
        m_port = m_connectionProfile->getPort();
        m_address = m_connectionProfile->getAddress();
        startConnectionToServer();
    }
    return true;
}
bool  NetworkManager::startListening()
{
    if (m_server == NULL)
    {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(newClientConnection()));
    }
    if (m_server->listen(QHostAddress::Any, m_connectionProfile->getPort()))
    {
        emit notifyUser(tr("Server is on."));
        connect(this, SIGNAL(linkDeleted(NetworkLink *)), m_playersList, SLOT(delPlayerWithLink(NetworkLink *)));
        return true;
    }
    else
    {
       /* QMessageBox errorDialog(QMessageBox::Warning, tr("Error"), tr("Can not establish the connection."));
        errorDialog.setInformativeText(m_server->errorString());
        errorDialog.exec();
        return false;*/
    }

}

void NetworkManager::socketStateChanged(QAbstractSocket::SocketState state)
{
   // qDebug() << "socket State Changed" << state;
    switch (state)
    {
    case QAbstractSocket::ClosingState:
    case QAbstractSocket::UnconnectedState:
        setConnectionState(DISCONNECTED);
        break;
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
        setConnectionState(CONNECTING);
        break;
    case QAbstractSocket::ListeningState:
        setConnectionState(LISTENING);
        break;
    case QAbstractSocket::ConnectedState:
        setConnectionState(CONNECTED);
        break;
    default:
        break;
    }
}

void NetworkManager::startConnectionToServer()
{
    if(NULL==m_networkLinkToServer)
    {
        m_networkLinkToServer = new NetworkLink(m_connectionProfile,this);
        addNetworkLink(m_networkLinkToServer);
        connect(m_networkLinkToServer,SIGNAL(errorMessage(QString)),this,SIGNAL(errorOccur(QString)));
        connect(m_networkLinkToServer,SIGNAL(connnectionStateChanged(QAbstractSocket::SocketState)),this,SLOT(socketStateChanged(QAbstractSocket::SocketState)));

        m_networkLinkToServer->connectTo();
    }
    else
    {
        m_networkLinkToServer->setConnection(m_connectionProfile);
        //QMetaObject::invokeMethod(m_networkLinkToServer,"connectTo",Qt::QueuedConnection);
        m_networkLinkToServer->connectTo();
    }
}

void NetworkManager::sendMessage(char* data, quint32 size, NetworkLink* but)
{
    NetworkMessageHeader header;
    memcpy((char*)&header,data,sizeof(NetworkMessageHeader));

    //client
    if(NULL!=m_networkLinkToServer)
    {
        m_networkLinkToServer->sendDataSlot(data,size,but);
    }
    else//server
    {
        emit sendData(data, size, but);
    }
}


void NetworkManager::addNetworkLink(NetworkLink* networkLink)
{
    if((NULL!=networkLink)&&(!m_networkLinkList.contains(networkLink)))
    {
        m_networkLinkList.append(networkLink);
        connect(this, SIGNAL(sendData(char *, quint32, NetworkLink *)),networkLink, SLOT(sendData(char *, quint32, NetworkLink *)));
        connect(networkLink, SIGNAL(disconnected(NetworkLink *)),this, SLOT(endingNetworkLink(NetworkLink *)));
        connect(networkLink,SIGNAL(readDataReceived(quint64,quint64)),this,SIGNAL(dataReceived(quint64,quint64)));
        emit linkAdded(networkLink);
    }
}


void NetworkManager::newClientConnection()
{
    QTcpSocket* socketTcp = m_server->nextPendingConnection();
    new NetworkLink(socketTcp,this);

}
void NetworkManager::endingNetworkLink(NetworkLink * link)
{

    if(!m_disconnectAsked)
    {
        link->deleteLater();

        emit linkDeleted(link);
    }


    if (!m_connectionProfile->isServer())
    {
        if(link==m_networkLinkToServer)
        {
            setConnectionState(DISCONNECTED);
            emit notifyUser(tr("Connection with the Remote Server has been lost."));
            m_networkLinkToServer = NULL;
            m_playersList->cleanListButLocal();
        }


       /* if(!m_disconnectAsked)
        {
            m_dialog->startTimer();
            m_dialog->show();
        }*/
    }
    else
    {
        if (link == NULL)
        {
			qWarning() << tr("NULL NetworkLink pointer (NetworkManager::finDeNetworkLink).");
            return;
        }

        int i = m_networkLinkList.indexOf(link);
        if (i < 0)
        {
			qWarning()<< tr("Unknown thread joined, (finDeNetworkLink - NetworkManager.cpp)");
            return;
        }
        m_networkLinkList.removeAt(i);
    }
}
void NetworkManager::disconnectAndClose()
{
    m_disconnectAsked = true;
    if (m_connectionProfile->isServer())
    {
        m_server->close();
        emit notifyUser(tr("Server has been closed."));
        m_playersList->cleanListButLocal();
        foreach(NetworkLink * tmp,m_networkLinkList)
        {
            tmp->disconnectAndClose();
        }
        m_networkLinkList.clear();
    }
    else
    {
        m_networkLinkToServer->disconnectAndClose();
        emit notifyUser(tr("Connection to the server has been closed."));
        m_playersList->cleanListButLocal();
        delete m_networkLinkToServer;
        m_networkLinkToServer=NULL;
    }
    setConnectionState(DISCONNECTED);
}
bool NetworkManager::isServer() const
{
    if(NULL!=m_connectionProfile)
    {
        return m_connectionProfile->isServer();
    }
    return false;
}
bool NetworkManager::isConnected() const
{
    return m_connectionState;
}

void NetworkManager::setConnectionState(ConnectionState state)
{
    if(m_connectionState!=state)
    {
        m_connectionState=state;
        emit connectionStateChanged(m_connectionState);
    }
}
NetworkLink* NetworkManager::getLinkToServer()
{
    return m_networkLinkToServer;
}
quint16 NetworkManager::getPort() const
{
    return m_port;
}
void NetworkManager::setConnectionProfile(ConnectionProfile* profile)
{
    m_connectionProfile = profile;
    if(!m_connectionProfile->isServer())
    {
        m_hbSender = new heartBeatSender(this);
        if(nullptr != m_localPlayer)
        {
            m_hbSender->setIdLocalUser(m_localPlayer->getUuid());
        }
    }
}
//void NetworkManager::messageRecieved(NetworkMessageReader* message,NetworkLink* link)
//{
//    if (ReceiveEvent::hasNetworkReceiverFor((NetMsg::Category)m_header.category))
//    {
//        QList<NetWorkReceiver*> tmpList = ReceiveEvent::getNetWorkReceiverFor((NetMsg::Category)m_header.category);
//        foreach(NetWorkReceiver* tmp, tmpList)
//        {
//            forwardMessage(tmp->processMessage(&data),message,link);
//        }
//    }
//}
//void NetworkLink::forwardMessage( NetWorkReceiver::SendType type,NetworkMessageReader* message,NetworkLink* sender)
//{
//    if(NetWorkReceiver::NONE == type)
//    {
//        return;
//    }
//    if (isServer())
//    {
//        char* donnees = new char[m_header.dataSize + sizeof(NetworkMessageHeader)];
//        memcpy(donnees, &m_header, sizeof(NetworkMessageHeader));
//        memcpy(&(donnees[sizeof(NetworkMessageHeader)]), m_buffer, m_header.dataSize);
//        if (NetWorkReceiver::ALL == type)
//        {
//            sendMessage(donnees,m_header.dataSize + sizeof(NetworkMessageHeader),NULL);
//        }
//        else if(NetWorkReceiver::AllExceptSender == type)
//        {
//            sendMessage(donnees,m_header.dataSize + sizeof(NetworkMessageHeader),sender);
//        }
//        delete[] donnees;
//    }
//}
