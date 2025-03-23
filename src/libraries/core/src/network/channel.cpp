/*************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                              *
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

#include "network/channel.h"
#include "network/serverconnection.h"

Channel::Channel() {}

Channel::Channel(QString str)
{
    m_name= str;
}

Channel::~Channel() {}

QByteArray Channel::password() const
{
    return m_password;
}

void Channel::setPassword(const QByteArray& password)
{
    m_password= password;
}

int Channel::childCount() const
{
    return m_child.size();
}

int Channel::indexOf(TreeItem* child)
{
    return m_child.indexOf(child);
}

QString Channel::description() const
{
    return m_description;
}

void Channel::setDescription(const QString& description)
{
    m_description= description;
}

bool Channel::usersListed() const
{
    return m_usersListed;
}

void Channel::setUsersListed(bool usersListed)
{
    m_usersListed= usersListed;
}

bool Channel::isLeaf() const
{
    return false;
}

TreeItem* Channel::getChildAt(int row)
{
    if(m_child.isEmpty())
        return nullptr;
    if(m_child.size() > row)
    {
        return m_child.at(row);
    }
    return nullptr;
}

const QList<QPointer<TreeItem>> Channel::childrenItem() const
{
    return m_child;
}

void Channel::sendMessage(NetworkMessage* msg, ServerConnection* emitter, bool mustBeSaved)
{
    if(msg->getRecipientMode() == NetworkMessage::All)
    {
        sendToAll(msg, emitter);
        if(mustBeSaved)
        {
            m_dataToSend.append(msg);
            setMemorySize(m_memorySize + msg->getSize());
        }
    }
    else if(msg->getRecipientMode() == NetworkMessage::OneOrMany)
    {
        sendToMany(msg, emitter);
    }
}
void Channel::sendToMany(NetworkMessage* msg, ServerConnection* tcp, bool deleteMsg)
{
    auto const recipient= msg->getRecipientList();
    int i= 0;
    for(auto& client : m_child)
    {
        auto other= dynamic_cast<ServerConnection*>(client.data());

        if((nullptr != other) && (other != tcp) && (recipient.contains(other->uuid())))
        {
            bool b= false;
            if(i + 1 == recipient.size())
                b= deleteMsg;

            QMetaObject::invokeMethod(other, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg),
                                      Q_ARG(bool, b));
            ++i;
        }
    }
}

void Channel::sendToAll(NetworkMessage* msg, ServerConnection* tcp, bool deleteMsg)
{
    int i= 0;
    for(auto& client : m_child)
    {
        ServerConnection* other= dynamic_cast<ServerConnection*>(client.data());
        if((nullptr != other) && (other != tcp))
        {
            bool b= false;
            if(i + 1 == m_child.size())
                b= deleteMsg;

            QMetaObject::invokeMethod(other, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg),
                                      Q_ARG(bool, b));
        }
        ++i;
    }
}

bool Channel::contains(QString id)
{
    auto dupplicate= std::find_if(m_child.begin(), m_child.end(), [id](TreeItem* item) { return item->uuid() == id; });

    return dupplicate != m_child.end();
}

int Channel::addChild(TreeItem* item)
{
    if(nullptr == item)
        return -1;

    m_child.append(item);
    item->uuid();
    item->setParentItem(this);

    auto result= m_child.size();

    if(item->isLeaf())
    {
        QPointer<ServerConnection> tcp= dynamic_cast<ServerConnection*>(item);
        QPointer<TreeItem> itemp= item;
        if(tcp.isNull())
            return result;

        connect(tcp, &ServerConnection::clientSaysGoodBye, this,
                [this, itemp, tcp](const QString& playerId)
                {
                    Q_UNUSED(playerId);
                    if(m_child.isEmpty() || itemp.isNull())
                        return;
                    m_child.removeAll(itemp);
                    if(m_child.isEmpty())
                        return;
                    qInfo() << QStringLiteral("Client left from channel");
                    removeClient(tcp);
                });

        // TODO make this connection as oneshot
        connect(tcp, &ServerConnection::playerInfoDefined, this, [this, tcp]() { updateNewClient(tcp); });
        if(tcp->isGM())
        {
            if(m_currentGm == nullptr)
                setCurrentGM(tcp);
            sendOffGmStatus(tcp);
        }
    }
    else
    {
        auto chan= dynamic_cast<Channel*>(item);
        if(nullptr == chan)
            return result;

        connect(chan, &Channel::memorySizeChanged, this, &Channel::memorySizeChanged);
    }
    return result;
}

bool Channel::addChildInto(QString id, TreeItem* child)
{
    if(m_id == id)
    {
        addChild(child);
        return true;
    }
    else
    {
        for(auto& item : m_child)
        {
            if(item->addChildInto(id, child))
            {
                return true;
            }
        }
    }
    return false;
}

void Channel::updateNewClient(ServerConnection* newComer)
{
    NetworkMessageWriter* msg1= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::ClearTable);
    msg1->string8(newComer->uuid());
    QMetaObject::invokeMethod(newComer, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg1),
                              Q_ARG(bool, true));
    // Sending players infos
    for(auto& child : m_child)
    {
        if(!child->isLeaf())
            continue;

        ServerConnection* tcpConnection= dynamic_cast<ServerConnection*>(child.data());
        if(nullptr == tcpConnection)
            continue;

        if(tcpConnection == newComer || !tcpConnection->isFullyDefined())
            continue;

        NetworkMessageWriter* msg= new NetworkMessageWriter(NetMsg::UserCategory, NetMsg::PlayerConnectionAction);
        tcpConnection->fill(msg);
        QMetaObject::invokeMethod(newComer, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg),
                                  Q_ARG(bool, true));

        NetworkMessageWriter* msg2= new NetworkMessageWriter(NetMsg::UserCategory, NetMsg::PlayerConnectionAction);
        newComer->fill(msg2);
        QMetaObject::invokeMethod(tcpConnection, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg2),
                                  Q_ARG(bool, true));
    }
    // resend all previous data received
    for(auto& msg : m_dataToSend)
    {
        // tcp->sendMessage(msg);
        QMetaObject::invokeMethod(newComer, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg),
                                  Q_ARG(bool, false));
    }
}

void Channel::kick(const QString& str, bool isAdmin, const QString& sourceId)
{
    if(!isAdmin && sourceId.isEmpty())
        return;

    bool hasRightToKick= isAdmin;
    QPointer<TreeItem> toKick;
    for(auto& item : m_child)
    {
        if(item == nullptr)
            continue;
        if(!hasRightToKick && item->uuid() == sourceId)
        {
            ServerConnection* source= dynamic_cast<ServerConnection*>(toKick.data());
            if(source)
                hasRightToKick= source->isGM();
        }
        if(item->uuid() == str)
        {
            toKick= item;
        }
    }

    if(!hasRightToKick || toKick.isNull())
        return;

    if(m_child.removeAll(toKick) == 0)
        return;

    emit itemChanged();

    ServerConnection* client= dynamic_cast<ServerConnection*>(toKick.data());
    if(nullptr == client)
        return;

    removeClient(client);
    QMetaObject::invokeMethod(client, "closeConnection", Qt::QueuedConnection);

    // NOTE - could be useless
    for(auto& item : m_child)
    {
        item->kick(str, isAdmin, sourceId);
    }
}

void Channel::clear()
{
    for(auto& item : m_child)
    {
        item->clear();
    }
    qDeleteAll(m_child);
    m_child.clear();
}
TreeItem* Channel::getChildById(QString id)
{
    for(auto& item : m_child)
    {
        if(item.isNull())
            continue;

        if(item->uuid() == id)
        {
            return item;
        }
        else
        {
            auto itemChild= item->getChildById(id);
            if(nullptr != itemChild)
            {
                return itemChild;
            }
        }
    }
    return nullptr;
}

ServerConnection* Channel::getClientById(QString id)
{
    ServerConnection* result= nullptr;
    for(auto& item : m_child)
    {
        if(nullptr == item)
            continue;

        if(item->isLeaf())
        {
            auto client= dynamic_cast<ServerConnection*>(item.data());
            if(client->uuid() == id)
            {
                result= client;
            }
        }
        else
        {
            auto channel= dynamic_cast<Channel*>(item.data());
            if(nullptr != channel)
            {
                result= channel->getClientById(id);
            }
        }
        if(result != nullptr)
            break;
    }
    return result;
}

bool Channel::removeClient(ServerConnection* client)
{
    if(!client)
        return false;

    disconnect(client);

    // notify all remaining chan member to remove former player
    NetworkMessageWriter* message= new NetworkMessageWriter(NetMsg::UserCategory, NetMsg::DelPlayerAction);
    message->string8(client->playerId());
    sendMessage(message, nullptr, false);

    if(hasNoClient())
    {
        clearData();
    }
    if((m_currentGm != nullptr) && (m_currentGm == client))
    {
        findNewGM();
    }

    emit itemChanged();
    return true;
}

void Channel::clearData()
{
    m_dataToSend.clear();
    setMemorySize(0);
}

bool Channel::removeChild(TreeItem* itm)
{
    if(itm->isLeaf())
    {
        return removeClient(static_cast<ServerConnection*>(itm));
    }
    else
    {
        if(itm->childCount() == 0)
        {
            m_child.removeAll(itm);
            return true;
        }
    }
    return false;
}

bool Channel::hasNoClient()
{
    bool hasNoClient= true;
    for(auto& child : m_child)
    {
        if(child->isLeaf())
        {
            hasNoClient= false;
        }
    }
    return hasNoClient;
}

void Channel::sendOffGmStatus(ServerConnection* client)
{
    if(nullptr == client)
        return;

    bool isRealGM= (m_currentGm == client);

    NetworkMessageWriter* message= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::GMStatus);
    QStringList idList;
    idList << client->playerId();
    message->setRecipientList(idList, NetworkMessage::OneOrMany);
    message->int8(static_cast<qint8>(isRealGM));
    sendToMany(message, nullptr);
}
void Channel::findNewGM()
{
    auto result= std::find_if(m_child.begin(), m_child.end(),
                              [=](QPointer<TreeItem>& item)
                              {
                                  auto client= dynamic_cast<ServerConnection*>(item.data());
                                  if(nullptr != client)
                                  {
                                      if(client->isGM())
                                      {
                                          return true;
                                      }
                                  }
                                  return false;
                              });

    if(result == m_child.end())
        setCurrentGM(nullptr);
    else
        setCurrentGM(dynamic_cast<ServerConnection*>(result->data()));

    sendOffGmStatus(m_currentGm);
}

ServerConnection* Channel::currentGM() const
{
    return m_currentGm.data();
}
void Channel::setCurrentGM(ServerConnection* currentGM)
{
    if(currentGM == m_currentGm)
        return;
    m_currentGm= currentGM;
    emit currentGMChanged();
}

QString Channel::getCurrentGmId()
{
    if(m_currentGm)
        return m_currentGm->playerId();
    return {};
}
quint64 Channel::memorySize() const
{
    return m_memorySize;
}
void Channel::setMemorySize(quint64 size)
{
    if(size == m_memorySize)
        return;
    m_memorySize= size;
    emit memorySizeChanged(m_memorySize, this);
}
void Channel::renamePlayer(const QString& id, const QString& name)
{
    auto player= getClientById(id);
    if(nullptr == player)
        return;
    player->setName(name);
}
bool Channel::locked() const
{
    return m_locked;
}
void Channel::setLocked(bool locked)
{
    if(locked == m_locked)
        return;
    m_locked= locked;
    emit lockedChanged();
}
bool Channel::removeChildById(const QString& id)
{
    auto dupplicate= std::find_if(m_child.begin(), m_child.end(), [id](TreeItem* item) { return item->uuid() == id; });

    if(dupplicate == m_child.end())
        return false;

    m_child.erase(dupplicate);
    return true;
}
