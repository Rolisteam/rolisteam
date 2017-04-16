/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                                *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyrigth (C) 2010 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
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


#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QTcpServer>
#include <QList>

#include <QStateMachine>
#include <QTimer>
#include <QState>


//#include "connectiondialog.h"
//#include "connectionretrydialog.h"
#include "preferences/preferencesmanager.h"
#include "userlist/playersList.h"
#include "heartbeatsender.h"
#include "network/networkmessagewriter.h"
#include "network/networklink.h"

class Player;
class ConnectionProfile;

/**
 * @brief hold the list of socket (NetworkLink).
 * On startup displays the configDialog.
 */
class ClientManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(ConnectionState)

public:
    enum ConnectionState {DISCONNECTED,CONNECTING,CONNECTED,AUTHENTIFIED};
    /**
     * @brief NetworkManager
     */
    ClientManager(ConnectionProfile* connection);
   // static ClientManager *getInstance();
	/**
	 * @brief ~NetworkManager
	 */
    virtual ~ClientManager();
    /**
     * @brief emettreDonnees
     * @param donnees
     * @param taille
     * @param sauf
     */
    void sendMessage(char *donnees, quint32 taille, NetworkLink *sauf);

    /**
     * @brief ajouterNetworkLink
     * @param NetworkLink
     */
    void addNetworkLink(NetworkLink *NetworkLink);

    /**
     * @brief isServer
     * @return
     */
    bool isServer() const;
    /**
     * @brief getLocalPlayer
     * @return
     */
    Player* getLocalPlayer();


    /**
     * @brief isConnected
     * @return
     */
    bool isConnected() const;
    NetworkLink* getLinkToServer();
    void setConnectionProfile(ConnectionProfile*);
public slots:
    void setConnectionState(ConnectionState);
    void disconnectAndClose();
    /**
     * @brief startConnection try to connect to the server or to start it.
     * @return true everything goes fine, otherwise false.
     */
    bool startConnection();

signals :
    void sendData(char* data, quint32 size, NetworkLink* but);

    void linkAdded(NetworkLink * link);
    void linkDeleted(NetworkLink * link);
    void dataReceived(quint64,quint64);
    void stopConnectionTry();
    void connectionStateChanged(ClientManager::ConnectionState);
    void notifyUser(QString);
    void errorOccur(QString);

    void isReady();

private slots :
    //void newClientConnection();
    void endingNetworkLink(NetworkLink * link);
    void startConnectionToServer();
    //bool startListening();
   // void socketStateChanged(QAbstractSocket::SocketState state);

private:
    NetworkLink* m_networkLinkToServer;
    QTimer* m_reconnect;
    Player* m_localPlayer;

    bool m_disconnectAsked;
    PreferencesManager* m_preferences;
    PlayersList* m_playersList;
    ConnectionState m_connectionState;
    bool m_isClient;
    ConnectionProfile* m_connectionProfile;
    QList<QThread*> m_threadList;
    heartBeatSender* m_hbSender;
 //   static ClientManager* m_singleton;


    QState* m_connecting;
    QState* m_connected;
    QState* m_authentified;
    QState* m_error;
    QState* m_disconnected;
    QState* m_ready;
    QStateMachine m_states;
};

#endif
