/************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2010 by Joseph Boudou                                 *
 *                                                                       *
 *     https://rolisteam.org/                                         *
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

#include "network/clientmanager.h"

#include <QDebug>
#include <QTcpSocket>

#include "data/person.h"
#include "data/player.h"
#include "network/connectionprofile.h"
#include "network/networklink.h"

#define second 1000
NetworkLink* ClientManager::m_networkLinkToServer= nullptr;
/*****************
 * ClientManager *
 *****************/
ClientManager::ClientManager() : QObject()
{
    qRegisterMetaType<NetworkLink*>("NetworkLink*");
    qRegisterMetaType<char*>("char*");

    m_networkLinkToServer= new NetworkLink();

    // m_playersList= PlayersList::instance();

    m_connecting= new QState();
    m_connected= new QState();
    m_authentified= new QState();
    m_error= new QState();
    m_disconnected= new QState();

    connect(m_connected, &QAbstractState::entered, this, [this]() {
        qDebug() << "client connected state";
        setConnectionState(CONNECTED);
        emit connectedToServer();
    });
    connect(m_connecting, &QAbstractState::entered, this, [this]() {
        qDebug() << "client connecting state";
        setConnectionState(CONNECTING);
        emit isConnecting();
    });
    connect(m_disconnected, &QAbstractState::entered, this, [this]() {
        qDebug() << "client disconnected state";
        setConnectionState(DISCONNECTED);
        setReady(true);
        emit isDisconnected();
    });
    connect(m_authentified, &QAbstractState::entered, this, [this]() {
        qDebug() << "client authentified state";
        setConnectionState(AUTHENTIFIED);
        emit isAuthentified();
    });

    connect(m_error, &QAbstractState::entered, this, [=]() {
        qDebug() << "Error state";
        setConnectionState(DISCONNECTED);
        emit errorOccur(tr("Connection Error"));
    });

    m_states.addState(m_connecting);
    m_states.addState(m_connected);
    m_states.addState(m_authentified);
    m_states.addState(m_disconnected);
    m_states.addState(m_error);
    m_states.setInitialState(m_disconnected);

    m_disconnected->addTransition(m_networkLinkToServer, &NetworkLink::connecting, m_connecting);
    m_connecting->addTransition(m_networkLinkToServer, &NetworkLink::connected, m_connected);
    m_connected->addTransition(m_networkLinkToServer, &NetworkLink::authentificationSuccessed, m_authentified);

    m_authentified->addTransition(m_networkLinkToServer, &NetworkLink::disconnected, m_disconnected);
    m_connected->addTransition(m_networkLinkToServer, &NetworkLink::disconnected, m_disconnected);
    m_connecting->addTransition(m_networkLinkToServer, &NetworkLink::disconnected, m_disconnected);

    m_connecting->addTransition(m_networkLinkToServer, &NetworkLink::authentificationFail, m_disconnected);
    m_connected->addTransition(m_networkLinkToServer, &NetworkLink::authentificationFail, m_disconnected);

    m_error->addTransition(m_networkLinkToServer, &NetworkLink::connecting, m_connecting);
    m_connecting->addTransition(m_networkLinkToServer, &NetworkLink::error, m_error);

    connect(m_networkLinkToServer, &NetworkLink::disconnected, this, &ClientManager::endingNetworkLink);
    connect(m_networkLinkToServer, &NetworkLink::readDataReceived, this, &ClientManager::dataReceived);
    connect(m_networkLinkToServer, &NetworkLink::errorMessage, this, &ClientManager::errorOccur);
    connect(m_networkLinkToServer, &NetworkLink::clearData, this, &ClientManager::clearData);
    connect(m_networkLinkToServer, &NetworkLink::gameMasterStatusChanged, this,
            &ClientManager::gameMasterStatusChanged);
    connect(m_networkLinkToServer, &NetworkLink::moveToAnotherChannel, this, &ClientManager::moveToAnotherChannel);

    connect(&m_states, &QStateMachine::started, this, [this]() { setReady(true); });

    m_states.start();
}

ClientManager::~ClientManager() {}

void ClientManager::connectTo(const QString& host, int port)
{
    m_networkLinkToServer->connectTo(host, port);
}

void ClientManager::endingNetworkLink()
{
    setConnectionState(DISCONNECTED);
    emit notifyUser(tr("Connection with the Remote Server has been lost."));
}
void ClientManager::disconnectAndClose()
{
    m_networkLinkToServer->disconnectAndClose();
    emit notifyUser(tr("Connection to the server has been closed."));
    setConnectionState(DISCONNECTED);
}

ClientManager::ConnectionState ClientManager::connectionState() const
{
    return m_connectionState;
}

bool ClientManager::ready() const
{
    return (m_states.isRunning() & (m_connectionState == DISCONNECTED));
}

void ClientManager::setConnectionState(ConnectionState state)
{
    if(m_connectionState == state)
        return;

    m_connectionState= state;
    emit connectionStateChanged(m_connectionState);
}
NetworkLink* ClientManager::getLinkToServer()
{
    return m_networkLinkToServer;
}

void ClientManager::setReady(bool ready)
{
    if(ready == m_ready)
        return;
    m_ready= ready;
    emit readyChanged();
}

void ClientManager::reset()
{
    // auto const connection= new QMetaObject::Connection;
    // connect(this, &ClientManager::isDisconnected, this, [this]() { emit isReady(); });

    m_networkLinkToServer->reset();
}
