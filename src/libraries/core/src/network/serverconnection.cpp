/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   Copyright (C) 2015 by Renaud Guezennec                              *
 *                                                                       *
 *   https://rolisteam.org/                                           *
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
#include "network/serverconnection.h"
#include "network/channel.h"

#include "worker/playermessagehelper.h"
#include <QHostAddress>
#include <QThread>

ServerConnection::ServerConnection(QTcpSocket* socket, QObject* parent)
    : TreeItem(parent), m_socket(socket), m_isAdmin(false), m_player(new Player)
{
    m_remainingData= 0;
    m_headerRead= 0;
    qRegisterMetaType<ServerConnection::ConnectionEvent>();
}
ServerConnection::~ServerConnection()
{
    if(nullptr != m_stateMachine)
    {
        delete m_stateMachine;
        m_stateMachine= nullptr;
    }
}

void ServerConnection::resetStateMachine()
{
    if(nullptr == m_socket)
        return;

    m_heartBeat.reset(new HeartBeatSender);
    connect(m_heartBeat.get(), &HeartBeatSender::sendOff, this,
            [this](NetworkMessageWriter* msg) { sendMessage(msg, false); });

    m_stateMachine= new QStateMachine();

    connect(m_stateMachine, &QStateMachine::started, this, &ServerConnection::isReady);
    m_incomingConnection= new QState();
    m_controlConnection= new QState();
    m_authentificationServer= new QState();
    m_disconnected= new QState();

    m_connected= new QStateMachine();
    m_inChannel= new QState();
    m_wantToGoToChannel= new QState();

    m_stateMachine->addState(m_incomingConnection);
    m_stateMachine->setInitialState(m_incomingConnection);
    m_stateMachine->addState(m_controlConnection);
    m_stateMachine->addState(m_authentificationServer);
    m_stateMachine->addState(m_connected);
    m_stateMachine->addState(m_disconnected);

    m_connected->addState(m_inChannel);
    m_connected->setInitialState(m_inChannel);
    m_connected->addState(m_wantToGoToChannel);

    m_stateMachine->start();

    connect(m_incomingConnection, &QState::activeChanged, this,
            [=](bool b)
            {
                qDebug() << "incomming state" << b;
                if(b)
                {
                    m_currentState= m_incomingConnection;
                }
            });
    connect(m_controlConnection, &QState::activeChanged, this,
            [=](bool b)
            {
                qDebug() << "control state" << b;
                if(b)
                {
                    m_currentState= m_controlConnection;
                    emit checkServerAcceptClient(this);
                }
            });

    connect(m_authentificationServer, &QState::activeChanged, this,
            [=](bool b)
            {
                qDebug() << "authentification state" << b;
                if(b)
                {
                    m_currentState= m_authentificationServer;
                    if(m_knownUser)
                    {
                        qDebug() << "checkserver password" << m_knownUser;
                        emit checkServerPassword(this);
                    }
                    else
                    {
                        qDebug() << "Waiting for data" << m_knownUser;
                        m_waitingData= true;
                    }
                }
            });
    connect(m_wantToGoToChannel, &QState::activeChanged, this,
            [=](bool b)
            {
                if(b)
                {
                    m_currentState= m_wantToGoToChannel;
                }
            });

    connect(m_disconnected, &QState::activeChanged, this,
            [=](bool b)
            {
                if(b)
                {
                    m_currentState= m_disconnected;
                    m_forwardMessage= false;
                    m_heartBeat->stop();
                    closeConnection();
                }
            });

    connect(m_connected, &QState::activeChanged, this,
            [=](bool b)
            {
                if(b)
                {
                    m_heartBeat->start();
                    m_forwardMessage= true;
                }
            });

    m_incomingConnection->addTransition(this, &ServerConnection::checkSuccess, m_controlConnection);
    m_incomingConnection->addTransition(this, &ServerConnection::checkFail, m_disconnected);
    m_incomingConnection->addTransition(this, &ServerConnection::protocolViolation, m_disconnected);

    m_controlConnection->addTransition(this, &ServerConnection::controlSuccess, m_authentificationServer);
    m_controlConnection->addTransition(this, &ServerConnection::controlFail, m_disconnected);
    m_controlConnection->addTransition(this, &ServerConnection::protocolViolation, m_disconnected);

    m_authentificationServer->addTransition(this, &ServerConnection::serverAuthSuccess, m_connected);
    m_authentificationServer->addTransition(this, &ServerConnection::serverAuthFail, m_disconnected);
    m_authentificationServer->addTransition(this, &ServerConnection::protocolViolation, m_disconnected);

    m_connected->addTransition(this, &ServerConnection::socketDisconnection, m_disconnected);
    m_connected->addTransition(this, &ServerConnection::protocolViolation, m_disconnected);

    m_wantToGoToChannel->addTransition(this, &ServerConnection::channelAuthFail, m_inChannel);
    m_wantToGoToChannel->addTransition(this, &ServerConnection::channelAuthSuccess, m_inChannel);
    m_inChannel->addTransition(this, &ServerConnection::moveChannel, m_wantToGoToChannel);

    emit socketInitiliazed();
}

void ServerConnection::startReading()
{
    m_socket= new QTcpSocket();
    connect(m_socket, &QTcpSocket::disconnected, this, &ServerConnection::socketDisconnection);
    connect(m_socket, &QTcpSocket::connected, this, &ServerConnection::receivingData);
    connect(m_socket, &QTcpSocket::readyRead, this, &ServerConnection::receivingData);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &ServerConnection::receivingData);
    connect(m_socket, &QTcpSocket::stateChanged, this, &ServerConnection::receivingData);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), this,
            &ServerConnection::connectionError);

    m_socket->setSocketDescriptor(getSocketHandleId());
    resetStateMachine();
}

qintptr ServerConnection::getSocketHandleId() const
{
    return m_socketHandleId;
}

void ServerConnection::setSocketHandleId(const qintptr& socketHandleId)
{
    m_socketHandleId= socketHandleId;
}

bool ServerConnection::isAdmin() const
{
    return m_isAdmin;
}

void ServerConnection::setIsAdmin(bool isAdmin)
{
    m_isAdmin= isAdmin;
}

bool ServerConnection::isGM() const
{
    if(m_player == nullptr)
        return false;
    return m_player->isGM();
}

QString ServerConnection::playerId() const
{
    if(nullptr != m_player)
        return m_player->uuid();

    return {};
}

void ServerConnection::setPlayerId(const QString& id)
{
    if(m_player)
        m_player->setUuid(id);
}

QString ServerConnection::playerName() const
{
    if(m_player)
        return m_player->name();

    return {};
}

void ServerConnection::setInfoPlayer(NetworkMessageReader* msg)
{
    if(nullptr == msg && nullptr == m_player)
        return;

    if(PlayerMessageHelper::readPlayer(*msg, m_player.get()))
    {
        setName(m_player->name());
        setUuid(m_player->uuid());
        m_knownUser= true;
        emit playerInfoDefined();
    }
}

void ServerConnection::fill(NetworkMessageWriter* msg)
{
    if(nullptr != m_player)
    {
        // m_player->fill(*msg);
        PlayerMessageHelper::writePlayerIntoMessage(*msg, m_player.get());
    }
}

bool ServerConnection::isFullyDefined()
{
    if(nullptr != m_player)
    {
        return m_player->isFullyDefined();
    }
    return false;
}

void ServerConnection::closeConnection()
{
    if(nullptr != m_socket)
    {
        m_socket->disconnectFromHost();
    }
    // Notify all others that player has left.
    emit clientSaysGoodBye(playerId());
}

void ServerConnection::receivingData()
{
    if(m_socket.isNull())
    {
        return;
    }
    quint32 dataRead= 0;

    while(!m_socket.isNull() && m_socket->bytesAvailable())
    {
        if(!m_receivingData)
        {
            qint64 readDataSize= 0;
            char* tmp= reinterpret_cast<char*>(&m_header);

            // To do only if there is enough data
            readDataSize= m_socket->read(tmp + m_headerRead, static_cast<qint64>(sizeof(NetworkMessageHeader))
                                                                 - static_cast<qint64>(m_headerRead));

            if(readDataSize != static_cast<qint64>(sizeof(NetworkMessageHeader))
               && readDataSize + static_cast<qint64>(m_headerRead) != static_cast<qint64>(sizeof(NetworkMessageHeader)))
            {
                m_headerRead+= static_cast<quint64>(readDataSize);
                continue;
            }
            else
            {
                m_headerRead= 0;
            }
            m_buffer= new char[m_header.dataSize];
            m_remainingData= m_header.dataSize;
            emit readDataReceived(m_header.dataSize, m_header.dataSize);
        }
        // To do only if there is enough data
        dataRead
            = m_socket->read(&(m_buffer[static_cast<int>(static_cast<quint64>(m_header.dataSize) - m_remainingData)]),
                             static_cast<qint64>(m_remainingData));
        m_dataReceivedTotal+= dataRead;

        if(dataRead < m_remainingData)
        {
            m_remainingData-= dataRead;
            m_receivingData= true;
            emit readDataReceived(m_remainingData, m_header.dataSize);
        }
        else
        {
            m_headerRead= 0;
            dataRead= 0;
            m_dataReceivedTotal= 0;
            emit readDataReceived(0, 0);
            m_receivingData= false;
            m_remainingData= 0;
            forwardMessage();
        }
    }
}
bool ServerConnection::isCurrentState(QState* state)
{
    return state == m_currentState;
}

QString ServerConnection::getChannelPassword() const
{
    return m_channelPassword;
}

QString ServerConnection::getAdminPassword() const
{
    return m_adminPassword;
}

QString ServerConnection::getServerPassword() const
{
    return m_serverPassword;
}
void ServerConnection::forwardMessage()
{
    qDebug() << "forward message";
    QByteArray array(reinterpret_cast<char*>(&m_header), sizeof(NetworkMessageHeader));
    array.append(m_buffer, static_cast<int>(m_header.dataSize));
    if(isCurrentState(m_disconnected))
        return;

    if(m_header.category == NetMsg::AdministrationCategory)
    {
        qDebug() << "read admin message";
        NetworkMessageReader msg;
        msg.setData(array);
        if(readAdministrationMessages(msg))
            emit dataReceived(array);
    }
    else if(!m_forwardMessage)
    {
        delete[] m_buffer;
        emit protocolViolation();
    }
    else
    {
        emit dataReceived(array);
    }
}

void ServerConnection::sendMessage(NetworkMessage* msg, bool deleteMsg)
{
    if((nullptr != m_socket) && (m_socket->isWritable()))
    {
        NetworkMessageHeader* data= msg->buffer();
        qint64 dataSend= m_socket->write(reinterpret_cast<char*>(data), data->dataSize + sizeof(NetworkMessageHeader));
        if(-1 == dataSend)
        {
            if(m_socket->state() != QAbstractSocket::ConnectedState)
            {
                emit socketDisconnection();
            }
        }
    }
    if(deleteMsg)
    {
        delete msg;
    }
}
void ServerConnection::connectionError(QAbstractSocket::SocketError error)
{
    emit socketError(error);
    if(nullptr != m_socket)
    {
        qWarning() << m_socket->errorString() << error;
    }
}

void ServerConnection::sendEvent(ServerConnection::ConnectionEvent event)
{
    if(nullptr != m_player)
        qDebug() << "server connection to " << m_player->name() << "recieve event:" << event;
    switch(event)
    {
    case CheckSuccessEvent:
        emit checkSuccess();
        break;
    case CheckFailEvent:
        emit checkFail();
        break;
    case ControlFailEvent:
        emit controlFail();
        break;
    case ControlSuccessEvent:
        emit controlSuccess();
        break;
    case ServerAuthDataReceivedEvent:
        emit serverAuthDataReceived();
        break;
    case ServerAuthFailEvent:
        emit serverAuthFail();
        break;
    case ServerAuthSuccessEvent:
        emit serverAuthSuccess();
        break;
    case AdminAuthSuccessEvent:
        emit adminAuthSucceed();
        m_isAdmin= true;
        break;
    case AdminAuthFailEvent:
        emit adminAuthFailed();
        m_isAdmin= false;
        break;
    case ChannelAuthSuccessEvent:
        emit channelAuthSuccess();
        break;
    case ChannelAuthFailEvent:
        emit channelAuthFail();
        break;
    case MoveChanEvent:
        emit moveChannel();
        break;
    case ChannelChanged:
        sendOffChannelChanged();
        break;
    default:
        break;
    }
}
void ServerConnection::sendOffChannelChanged()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::MovedIntoChannel);
    sendMessage(&msg, false);
}
bool ServerConnection::readAdministrationMessages(NetworkMessageReader& msg)
{
    bool res= true;
    qDebug() << "read Admin messages " << msg.action();
    switch(msg.action())
    {
    case NetMsg::ConnectionInfo:
        m_serverPassword= msg.byteArray32();
        setName(msg.string32());
        setUuid(msg.string32());

        qDebug() << name() << uuid() << " << user";
        m_knownUser= true;
        if(m_waitingData)
        {
            emit checkServerPassword(this);
        }
        m_waitingData= false;
        break;
    case NetMsg::ChannelPassword:
        if(isAdmin())
        {
            auto channelId= msg.string8();
            auto passwd= msg.byteArray32();
            emit channelPassword(channelId, passwd);
        }
        break;
    case NetMsg::MoveChannel:
    {
        m_wantedChannel= msg.string32();
        auto passwd= msg.byteArray32();
        emit checkChannelPassword(this, m_wantedChannel, passwd);
    }
    break;
    case NetMsg::AdminPassword:
        m_adminPassword= msg.byteArray32();
        emit checkAdminPassword(this);
        break;
    case NetMsg::HeartbeatAsk:
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::HeartbeatAnswer);
        sendMessage(&msg, false);
        res= false;
    }
    break;
    case NetMsg::HeartbeatAnswer:
    {
        m_heartBeat->receivedAnswer();
        res= false;
    }
    break;
    case NetMsg::Goodbye:
        closeConnection();
        break;
    default:
        break;
    }

    return res;
}

Channel* ServerConnection::getParentChannel() const
{
    return dynamic_cast<Channel*>(getParentItem());
}

void ServerConnection::setParentChannel(Channel* parent)
{
    setParentItem(parent);
}
QTcpSocket* ServerConnection::getSocket()
{
    return m_socket;
}
int ServerConnection::indexOf(TreeItem*)
{
    return -1;
}

void ServerConnection::readFromJson(QJsonObject& json)
{
    setName(json["name"].toString());
    setUuid(json["id"].toString());
    setIsAdmin(json["admin"].toBool());
    auto playerId= json["idPlayer"].toString();
    if(m_player)
        m_player->setUuid(playerId);
}

void ServerConnection::writeIntoJson(QJsonObject& json)
{
    json["type"]= "client";
    json["name"]= m_name;
    json["gm"]= isGM();
    json["admin"]= m_isAdmin;
    json["id"]= m_id;
    json["idPlayer"]= playerId();
}
QString ServerConnection::getIpAddress()
{
    if(nullptr != m_socket)
    {
        return m_socket->peerAddress().toString();
    }
    return {};
}

QString ServerConnection::getWantedChannel()
{
    return m_wantedChannel;
}

bool ServerConnection::isConnected() const
{
    if(!m_socket.isNull())
        return (m_socket->isValid() && (m_socket->state() == QAbstractSocket::ConnectedState));
    else
        return false;
}
