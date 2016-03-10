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
    /*ReceiveEvent::registerNetworkReceiver(NetMsg::PictureCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::MapCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::NPCCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::DrawCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::ConnectionCategory,m_mainWindow);
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterPlayerCategory,m_mainWindow);*/
/*#ifndef NULL_PLAYER
    m_audioPlayer = AudioPlayer::getInstance();
    ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory,m_audioPlayer);
#endif*/

    setSocket(socket);

    //if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    if(!m_networkManager->isServer())
    {
		m_networkManager->ajouterNetworkLink(this);
    }
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
}


void NetworkLink::connectionError(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur);
    if(NULL==m_socketTcp)
    {
        return;
    }
    qWarning() << tr("Network error occurs :") << m_socketTcp->errorString();
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
        return;
    }
    if (but != this)
    {
        // Emission des donnees
       #ifdef DEBUG_MODE
        NetworkMessageHeader header;
        memcpy((char*)&header,data,sizeof(NetworkMessageHeader));
        qDebug() << "header:" << (header.category==NetMsg::PictureCategory) << header.action << header.dataSize;
        if(header.category==NetMsg::PictureCategory)
        {
            qDebug() << "test";
        }
        #endif

        int t = m_socketTcp->write(data, size);

        if (t < 0)
        {
            qWarning() << "Tranmission error :" << m_socketTcp->errorString();
        }
    }
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

            //qDebug() << "Read DataSize" << readDataSize << m_header.dataSize << m_header.category << m_header.action << m_socketTcp->bytesAvailable();
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
            m_networkManager->ajouterNetworkLink(this);

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
