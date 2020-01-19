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

#include <QList>
#include <QTcpServer>

#include <QState>
#include <QStateMachine>
#include <QTimer>

//#include "connectiondialog.h"
//#include "connectionretrydialog.h"
#include "heartbeatsender.h"
#include "network/networklink.h"
#include "network/networkmessagewriter.h"
#include "preferences/preferencesmanager.h"
#include "userlist/playermodel.h"

class Player;
class ConnectionProfile;

/**
 * @brief hold the list of socket (NetworkLink).
 * On startup displays the configDialog.
 */
class ClientManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(ConnectionState connectionStateChanged READ connectionState NOTIFY connectionStateChanged)
public:
    enum ConnectionState
    {
        UNREADY,
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        AUTHENTIFIED
    };
    Q_ENUM(ConnectionState)
    /**
     * @brief NetworkManager
     */
    ClientManager();
    /**
     * @brief ~NetworkManager
     */
    virtual ~ClientManager();
    ConnectionState connectionState() const;

    bool ready() const;

    static NetworkLink* getLinkToServer();
public slots:
    /**
     * @brief startConnection try to connect to the server or to start it.
     * @return true everything goes fine, otherwise false.
     */
    void connectTo(const QString& host, int port);

    void disconnectAndClose();
    void reset();

signals:
    void sendData(char* data, quint32 size, NetworkLink* but);

    void linkAdded(NetworkLink* link);
    void dataReceived(quint64, quint64);
    void stopConnectionTry();
    void connectionStateChanged(ClientManager::ConnectionState);
    void notifyUser(QString);
    void errorOccur(QString);
    void gameMasterStatusChanged(bool status);

    // State signal
    void readyChanged();
    void isAuthentified();
    void isConnectedSig();
    void isConnecting();
    void isDisconnected();
    void connectedToServer();
    void clearData();
    void moveToAnotherChannel();

private slots:
    void setConnectionState(ClientManager::ConnectionState);
    void endingNetworkLink();
    void setReady(bool ready);

private:
    static NetworkLink* m_networkLinkToServer;
    ConnectionState m_connectionState= UNREADY;
    bool m_ready= false;
    bool m_isAdmin;

    PreferencesManager* m_preferences= nullptr;
    PlayerModel* m_playersList= nullptr;

    QState* m_connecting= nullptr;
    QState* m_connected= nullptr;
    QState* m_authentified= nullptr;
    QState* m_error= nullptr;
    QState* m_disconnected= nullptr;
    QStateMachine m_states;
};

#endif
