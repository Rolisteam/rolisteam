/*************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                              *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
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
#include "channel.h"
#include "tcpclient.h"

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
void Channel::readFromJson(QJsonObject& json)
{
    m_password= QByteArray::fromBase64(json["password"].toString().toUtf8());
    m_name= json["title"].toString();
    m_description= json["description"].toString();
    m_usersListed= json["usersListed"].toBool();
    m_memorySize= static_cast<quint64>(json["memorySize"].toInt());
    m_id= json["id"].toString();
    m_locked= json["locked"].toBool();

    QJsonArray array= json["children"].toArray();
    for(auto channelJson : array)
    {
        QJsonObject obj= channelJson.toObject();
        TreeItem* item= nullptr;
        if(obj["type"] == "channel")
        {
            Channel* chan= new Channel();
            item= chan;
        }
        else
        {
            TcpClient* tcpItem= new TcpClient(nullptr, nullptr);
            item= tcpItem;
            if(obj["gm"].toBool())
            {
                setCurrentGM(tcpItem);
            }
        }
        item->readFromJson(obj);
        item->setParentItem(this);
        m_child.append(item);
    }
}

void Channel::writeIntoJson(QJsonObject& json)
{
    json["password"]= QString::fromUtf8(m_password.toBase64());
    json["title"]= m_name;
    json["description"]= m_description;
    json["usersListed"]= m_usersListed;
    json["id"]= m_id;
    json["memorySize"]= static_cast<int>(m_memorySize);
    json["type"]= "channel";
    json["locked"]= m_locked;

    QJsonArray array;
    for(int i= 0; i < m_child.size(); ++i)
    {
        if(m_child.at(i)->isLeaf())
        {
            TreeItem* item= m_child.at(i);
            // Channel* item = dynamic_cast<Channel*>(m_child.at(i));
            if(nullptr != item)
            {
                QJsonObject jsonObj;
                item->writeIntoJson(jsonObj);
                jsonObj["gm"]= (item == m_currentGm);
                array.append(jsonObj);
            }
        }
    }
    json["children"]= array;
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
void Channel::sendMessage(NetworkMessage* msg, TcpClient* emitter, bool mustBeSaved)
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
void Channel::sendToMany(NetworkMessage* msg, TcpClient* tcp, bool deleteMsg)
{
    auto const recipient= msg->getRecipientList();
    int i= 0;
    for(auto& client : m_child)
    {
        TcpClient* other= dynamic_cast<TcpClient*>(client.data());

        if((nullptr != other) && (other != tcp) && (recipient.contains(other->getId())))
        {
            bool b= false;
            if(i + 1 == recipient.size())
                b= deleteMsg;
            QMetaObject::invokeMethod(
                other, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg), Q_ARG(bool, b));
            ++i;
        }
    }
}

void Channel::sendToAll(NetworkMessage* msg, TcpClient* tcp, bool deleteMsg)
{
    int i= 0;
    for(auto& client : m_child)
    {
        TcpClient* other= dynamic_cast<TcpClient*>(client.data());
        if((nullptr != other) && (other != tcp))
        {
            bool b= false;
            if(i + 1 == m_child.size())
                b= deleteMsg;

            QMetaObject::invokeMethod(
                other, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg), Q_ARG(bool, b));
        }

        ++i;
    }
}

int Channel::addChild(TreeItem* item)
{
    if(nullptr == item)
        return -1;

    m_child.append(item);
    item->setParentItem(this);

    auto result= m_child.size();

    if(item->isLeaf())
    {
        TcpClient* tcp= dynamic_cast<TcpClient*>(item);
        if(nullptr == tcp)
            return result;

        connect(tcp, &TcpClient::clientSaysGoodBye, this, [=] {
            m_child.removeAll(tcp);
            qInfo() << QStringLiteral("Client left from channel");
            auto message= new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::DelPlayerAction);
            message->string8(tcp->getPlayerId());
            sendToAll(message, tcp, true);
        });
        if(tcp->isGM())
        {
            if(m_currentGm == nullptr)
                setCurrentGM(tcp);
            sendOffGmStatus(tcp);
        }
        updateNewClient(tcp);
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

void Channel::updateNewClient(TcpClient* newComer)
{
    NetworkMessageWriter* msg1= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::ClearTable);
    msg1->string8(newComer->getId());
    QMetaObject::invokeMethod(
        newComer, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg1), Q_ARG(bool, true));
    // Sending players infos
    for(auto& child : m_child)
    {
        if(child->isLeaf())
        {
            TcpClient* tcpConnection= dynamic_cast<TcpClient*>(child.data());
            if(nullptr != tcpConnection)
            {
                if(tcpConnection != newComer && tcpConnection->isFullyDefined())
                {
                    NetworkMessageWriter* msg
                        = new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
                    tcpConnection->fill(msg);
                    QMetaObject::invokeMethod(
                        newComer, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg), Q_ARG(bool, true));

                    NetworkMessageWriter* msg2
                        = new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
                    newComer->fill(msg2);
                    QMetaObject::invokeMethod(tcpConnection, "sendMessage", Qt::QueuedConnection,
                        Q_ARG(NetworkMessage*, msg2), Q_ARG(bool, true));
                }
            }
        }
    }
    // resend all previous data received
    for(auto& msg : m_dataToSend)
    {
        // tcp->sendMessage(msg);
        QMetaObject::invokeMethod(
            newComer, "sendMessage", Qt::QueuedConnection, Q_ARG(NetworkMessage*, msg), Q_ARG(bool, false));
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
        if(!hasRightToKick && item->getId() == sourceId)
        {
            TcpClient* source= dynamic_cast<TcpClient*>(toKick.data());
            if(source)
                hasRightToKick= source->isGM();
        }
        if(item->getId() == str)
        {
            toKick= item;
        }
    }

    if(!hasRightToKick || toKick.isNull())
        return;

    if(m_child.removeAll(toKick) == 0)
        return;

    emit itemChanged();

    TcpClient* client= dynamic_cast<TcpClient*>(toKick.data());
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
        if(item->getId() == id)
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

TcpClient* Channel::getPlayerById(QString id)
{
    TcpClient* result= nullptr;
    for(auto& item : m_child)
    {
        if(nullptr == item)
            continue;

        if(item->isLeaf())
        {
            auto client= dynamic_cast<TcpClient*>(item.data());
            if(client->getPlayerId() == id)
            {
                result= client;
            }
        }
        else
        {
            auto channel= dynamic_cast<Channel*>(item.data());
            if(nullptr != channel)
            {
                result= channel->getPlayerById(id);
            }
        }
        if(result != nullptr)
            break;
    }
    return result;
}
void Channel::fill(NetworkMessageWriter& msg)
{
    msg.string8(m_id);
    msg.string16(m_name);
    msg.string16(m_description);
}
void Channel::read(NetworkMessageReader& msg)
{
    m_id= msg.string8();
    m_name= msg.string16();
    m_description= msg.string16();
}

bool Channel::removeClient(TcpClient* client)
{
    // must be the first line
    int i= m_child.removeAll(client);
    if(i == 0)
    {
        return false;
    }

    disconnect(client);

    // notify all remaining chan member to remove former player
    NetworkMessageWriter* message= new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::DelPlayerAction);
    message->string8(client->getPlayerId());
    sendMessage(message, nullptr, false);

    if(hasNoClient())
    {
        clearData();
    }
    else if((m_currentGm != nullptr) && (m_currentGm == client))
    {
        findNewGM();
    }

    emit itemChanged();
    return (0 < i);
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
        removeClient(static_cast<TcpClient*>(itm));
        return true;
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

void Channel::sendOffGmStatus(TcpClient* client)
{
    if(nullptr == client)
        return;

    bool isRealGM= (m_currentGm == client);

    NetworkMessageWriter* message= new NetworkMessageWriter(NetMsg::AdministrationCategory, NetMsg::GMStatus);
    QStringList idList;
    idList << client->getPlayerId();
    message->setRecipientList(idList, NetworkMessage::OneOrMany);
    message->int8(static_cast<qint8>(isRealGM));
    sendToMany(message, nullptr);
}
void Channel::findNewGM()
{
    auto result= std::find_if(m_child.begin(), m_child.end(), [=](QPointer<TreeItem>& item) {
        auto client= dynamic_cast<TcpClient*>(item.data());
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
        setCurrentGM(dynamic_cast<TcpClient*>(result->data()));

    sendOffGmStatus(m_currentGm);
}

TcpClient* Channel::currentGM() const
{
    return m_currentGm.data();
}
void Channel::setCurrentGM(TcpClient* currentGM)
{
    if(currentGM == m_currentGm)
        return;
    m_currentGm= currentGM;
    emit currentGMChanged();
}

QString Channel::getCurrentGmId()
{
    if(m_currentGm)
        return m_currentGm->getPlayerId();
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
    auto player= getPlayerById(id);
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
