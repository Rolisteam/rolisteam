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
#include <QMessageBox>
#include "network/networklink.h"
#include "data/person.h"
#include "data/player.h"
#include "audio/audioPlayer.h"
#include "network/selectconnectionprofiledialog.h"

#define second 1000

/*****************
 * NetworkManager *
 *****************/
NetworkManager::NetworkManager()
	: QObject(), m_server(NULL),m_NetworkLinkToServer(NULL),m_disconnectAsked(false),m_connectionState(false),m_localPlayer(NULL),m_audioPlayer(NULL)
{
    m_reconnect = new QTimer(this);
    m_preferences =  PreferencesManager::getInstance();
    m_dialog = new ConnectionRetryDialog();
    connect(m_dialog,SIGNAL(tryConnection()),this,SLOT(startConnection()));
    connect(m_dialog,SIGNAL(rejected()),this,SIGNAL(stopConnectionTry()));
}


NetworkManager::~NetworkManager()
{
    if(NULL!=m_dialog)
    {
        delete m_dialog;
        m_dialog = NULL;
    }
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

bool NetworkManager::configAndConnect(QString version)
{
    bool isGM =  !m_preferences->value("isPlayer",false).toBool();

	if(m_portStr.isEmpty())
    {
		m_portStr = m_preferences->value("ServerPort",6660).toString();
	}
    if(m_username.isEmpty())
    {
        m_username = m_preferences->value("UserName",tr("UserName")).toString();
    }
	if(m_host.isEmpty())
    {
        m_host = m_preferences->value("ipaddress","").toString();
    }
	if(!m_role.isEmpty())
    {
        isGM = m_role.compare("gm")==0 ? true : false;
	}
}
Player* NetworkManager::getLocalPlayer()
{
    return m_localPlayer;
}

bool NetworkManager::startConnection()
{
    bool cont = true;
    m_playersList = PlayersList::instance();
    while (cont)
    {
        // Server setup
        if (m_connectionProfile->isServer())
        {
            cont = !startListening();
            m_port = m_connectionProfile->getPort();
        }
        else
        {
            m_port = m_connectionProfile->getPort();
            m_address = m_connectionProfile->getAddress();
            if(startConnectionToServer())
            {
                cont = false;
            }
        }
        if(cont)
        {
            m_dialog->startTimer();
           int value = m_dialog->exec();

           if(value == QDialog::Accepted)
           {
               cont = true;
           }
           else
           {
               cont = false;
               return false;
           }
        }
    }

    return true;
}
bool  NetworkManager::startListening()
{
    if (m_server == NULL)
    {
        m_server = new QTcpServer(this);
        connect(m_server, SIGNAL(newConnection()), this, SLOT(nouveauClientConnecte()));
    }
    if (m_server->listen(QHostAddress::Any, m_connectionProfile->getPort()))
    {
        emit notifyUser(tr("Server is on."));
        connect(this, SIGNAL(linkDeleted(NetworkLink *)), m_playersList, SLOT(delPlayerWithLink(NetworkLink *)));
        return true;
    }
    else
    {
        QMessageBox errorDialog(QMessageBox::Warning, tr("Error"), tr("Can not establish the connection."));
        errorDialog.setInformativeText(m_server->errorString());
        errorDialog.exec();
        return false;
    }

}

bool NetworkManager::startConnectionToServer()
{
    ConnectionWaitDialog waitDialog;
    QTcpSocket * socket;

    socket = waitDialog.connectTo(m_connectionProfile->getAddress(), m_connectionProfile->getPort());


        if (socket != NULL)
        {
            //G_client=true;
            if(NULL==m_NetworkLinkToServer)
            {
                m_NetworkLinkToServer = new NetworkLink(socket);
            }
            else
            {
                m_NetworkLinkToServer->setSocket(socket,true);
            }

            m_dialog->hide();
          return true;

        }
        else
        {
            return false;
        }
}

void NetworkManager::sendMessage(char* data, quint32 size, NetworkLink* but)
{
    // Demande d'emission vers toutes les NetworkLinks
    emit sendData(data, size, but);
}


void NetworkManager::ajouterNetworkLink(NetworkLink* networkLink)
{
    if((NULL!=networkLink)&&(!NetworkLinks.contains(networkLink)))
    {
        NetworkLinks.append(networkLink);
        connect(this, SIGNAL(sendData(char *, quint32, NetworkLink *)),networkLink, SLOT(sendData(char *, quint32, NetworkLink *)));
        connect(networkLink, SIGNAL(disconnected(NetworkLink *)),this, SLOT(endingNetworkLink(NetworkLink *)));
        connect(networkLink,SIGNAL(readDataReceived(quint64,quint64)),this,SIGNAL(dataReceived(quint64,quint64)));
        emit linkAdded(networkLink);
    }
}


void NetworkManager::nouveauClientConnecte()
{
    QTcpSocket *socketTcp = m_server->nextPendingConnection();

    new NetworkLink(socketTcp);

}
void NetworkManager::setAudioPlayer(AudioPlayer* audio)
{
    m_audioPlayer = audio;
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
        if(link==m_NetworkLinkToServer)
        {
            setConnectionState(false);
            emit notifyUser(tr("Connection with the Remote Server has been lost."));
            m_NetworkLinkToServer = NULL;
            m_playersList->cleanListButLocal();
        }


        if(!m_disconnectAsked)
        {
            m_dialog->startTimer();
            m_dialog->show();
        }
    }
    else
    {
        if (link == NULL)
        {
			qWarning() << tr("NULL NetworkLink pointer (NetworkManager::finDeNetworkLink).");
            return;
        }

        int i = NetworkLinks.indexOf(link);
        if (i < 0)
        {
			qWarning()<< tr("Unknown thread joined, (finDeNetworkLink - NetworkManager.cpp)");
            return;
        }
        NetworkLinks.removeAt(i);
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
        foreach(NetworkLink * tmp,NetworkLinks)
        {
            tmp->disconnectAndClose();
        }
        NetworkLinks.clear();
    }
    else
    {
        m_NetworkLinkToServer->disconnectAndClose();
        emit notifyUser(tr("Connection to the server has been closed."));
        m_playersList->cleanListButLocal();
        delete m_NetworkLinkToServer;
        m_NetworkLinkToServer=NULL;
    }
    setConnectionState(false);
}
bool NetworkManager::isServer() const
{
    return m_connectionProfile->isServer();
}
bool NetworkManager::isConnected() const
{
    return m_connectionState;
}

void NetworkManager::setConnectionState(bool state)
{
    if(m_connectionState!=state)
    {
        m_connectionState=state;
        emit connectionStateChanged(m_connectionState);
    }
}
NetworkLink* NetworkManager::getLinkToServer()
{
    return m_NetworkLinkToServer;
}
quint16 NetworkManager::getPort() const
{
    return m_port;
}
void NetworkManager::setConnectionProfile(ConnectionProfile* profile)
{
    m_connectionProfile = profile;
}
