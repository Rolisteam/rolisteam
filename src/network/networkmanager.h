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
#include <QColor>
#include <QTimer>
#include <QDialog>


#include "connectiondialog.h"
#include "connectionretrydialog.h"
#include "preferences/preferencesmanager.h"
#include "userlist/playersList.h"

class Player;
class NetworkLink;
class ConnectionProfile;

/**
 * @brief hold the list of socket (NetworkLink).
 * On startup displays the configDialog.
 */
class NetworkManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(ConnectionState)
public:
    enum ConnectionState {DISCONNECTED,CONNECTING,LISTENING,CONNECTED};
	/**
	 * @brief NetworkManager
	 */
	NetworkManager();
	/**
	 * @brief ~NetworkManager
	 */
	virtual ~NetworkManager();
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
    quint16 getPort() const;
    void setValueConnection(QString portValue,QString hostnameValue,QString username,QString roleValue);
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
    void connectionStateChanged(NetworkManager::ConnectionState);
    void notifyUser(QString);

private slots :
    void newClientConnection();
    void endingNetworkLink(NetworkLink * link);
    void startConnectionToServer();
    bool startListening();
    void socketStateChanged(QAbstractSocket::SocketState state);

private:
    QTcpServer * m_server;
    QList<NetworkLink *> m_networkLinkList;
    NetworkLink* m_networkLinkToServer;
    quint16 m_port;
    quint16 m_listeningPort;
    QString m_address;
    QTimer* m_reconnect;
    Player* m_localPlayer;
    QString  m_localPlayerId;
    bool m_disconnectAsked;
    PreferencesManager* m_preferences;
    ConnectionRetryDialog* m_dialog;
    PlayersList* m_playersList;
    ConnectionState m_connectionState;
    bool m_isClient;
    bool m_commandLineValue;
    QString m_portStr;
    QString m_host;
    QString m_role;
    QString m_username;
    ConnectionProfile* m_connectionProfile;
    ConnectionWaitDialog* m_waitDialog;
    QList<QThread*> m_threadList;
};

#endif
