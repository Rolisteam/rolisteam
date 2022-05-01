/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                                *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyrigth (C) 2010 by Joseph Boudou                                 *
 *                                                                       *
 *   https://rolisteam.org/                                           *
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

#include "heartbeatsender.h"
#include "network/clientconnection.h"
#include "network/networkmessage.h"
#include "network/networkmessagewriter.h"
#include "network_global.h"

/**
 * @brief ClientManager manages the state of current client: connected, authentified…
 * and send networkmessage to the server, or notify from new received messages.
 *
 */
class NETWORK_EXPORT ClientManager : public QObject
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

    ClientManager(QObject* parent= nullptr);
    virtual ~ClientManager();
    ConnectionState connectionState() const;

    bool ready() const;
public slots:
    // void processPlayerMessage(NetworkMessageReader* msg);
    // void processSetupMessage(NetworkMessageReader* msg);

    void connectTo(const QString& host, int port);
    void disconnectAndClose();
    void reset();
    void setAuthentificationStatus(bool status);

signals:
    void messageReceived(QByteArray);
    void dataReceived(quint64, quint64);

    void connectionStateChanged(ClientManager::ConnectionState);
    void notifyUser(QString);
    void gameMasterStatusChanged(bool status);

    // State signal
    void connecting();
    void stopConnecting();
    void connectedToServer();
    void authentificationSuccessed();
    void authentificationFailed();

    void readyChanged();
    void clearData();
    void moveToAnotherChannel();

private slots:
    void setConnectionState(ClientManager::ConnectionState);
    void setReady(bool ready);

private:
    std::unique_ptr<ClientConnection> m_networkLinkToServer;
    ConnectionState m_connectionState= UNREADY;
    bool m_ready= false;
    bool m_isAdmin= false;

    QState* m_connecting= nullptr;
    QState* m_connected= nullptr;
    QState* m_waitingData= nullptr;
    QState* m_authentified= nullptr;
    QState* m_error= nullptr;
    QState* m_disconnected= nullptr;
    QStateMachine m_states;
};

#endif
