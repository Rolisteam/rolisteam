#include "channel.h"
#include "tcpclient.h"

Channel::Channel()
{

}

Channel::Channel(QString str)
{
    m_name = str;
}

Channel::~Channel()
{

}

QByteArray Channel::password() const
{
    return m_password;
}

void Channel::setPassword(const QByteArray &password)
{
    m_password = password;
}

int Channel::childCount() const
{
    return m_child.size();
}

int Channel::indexOf(TreeItem *child)
{
    return m_child.indexOf(child);
}

QString Channel::description() const
{
    return m_description;
}

void Channel::setDescription(const QString &description)
{
    m_description = description;
}

bool Channel::usersListed() const
{
    return m_usersListed;
}

void Channel::setUsersListed(bool usersListed)
{
    m_usersListed = usersListed;
}

bool Channel::isLeaf() const
{
    return false;
}
void Channel::readFromJson(QJsonObject &json)
{
    m_password=QByteArray::fromBase64(json["password"].toString().toUtf8());
    m_name=json["title"].toString();
    m_description=json["description"].toString();
    m_usersListed=json["usersListed"].toBool();
    m_id=json["id"].toString();

    QJsonArray array = json["channel"].toArray();
    for(auto channelJson : array)
    {
        QJsonObject obj = channelJson.toObject();
        TreeItem* item = nullptr;
        if(obj["type"]=="channel")
        {
            Channel* chan = new Channel();
            item = chan;
        }
        else
        {
            TcpClient* tcpItem = new TcpClient(nullptr,nullptr);
            item = tcpItem;
        }
        item->readFromJson(obj);
        item->setParentItem(this);
        m_child.append(item);
    }
}

void Channel::writeIntoJson(QJsonObject &json)
{
    json["password"]=QString::fromUtf8(m_password.toBase64());
    json["title"]=m_name;
    json["description"]=m_description;
    json["usersListed"]=m_usersListed;
    json["id"]=m_id;
    json["type"]="channel";

    QJsonArray array;
    for (int i = 0; i < m_child.size(); ++i)
    {
        if(m_child.at(i)->isLeaf())
        {
            TreeItem* item = m_child.at(i);
           // Channel* item = dynamic_cast<Channel*>(m_child.at(i));
            if(nullptr != item)
            {
                QJsonObject jsonObj;
                item->writeIntoJson(jsonObj);
                array.append(jsonObj);
            }
        }
    }
    json["channel"]=array;
}

TreeItem *Channel::getChildAt(int row)
{
    if(m_child.isEmpty()) return nullptr;
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
        sendToAll(msg,emitter);
        if(mustBeSaved)
        {
            m_dataToSend.append(msg);
        }
    }
    else if(msg->getRecipientMode() == NetworkMessage::OneOrMany) 
    {
        sendToMany(msg,emitter);
    }

}
void Channel::sendToMany(NetworkMessage* msg, TcpClient* tcp)
{ 
    auto recipient = msg->getRecipientList();
    for(auto client : m_child)
    {
        TcpClient* other = dynamic_cast<TcpClient*>(client);

        if((nullptr != other)&&(other!=tcp)&&(recipient.contains(other->getId())))
        {
            QMetaObject::invokeMethod(other,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg),Q_ARG(bool,false));
        }          
    }
}

void Channel::sendToAll(NetworkMessage* msg, TcpClient* tcp)
{
    for(auto client : m_child)
    {
        TcpClient* other = dynamic_cast<TcpClient*>(client);
        if((nullptr != other)&&(other!=tcp))
        {
            QMetaObject::invokeMethod(other,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg),Q_ARG(bool,false));
        }          
    }
}

int Channel::addChild(TreeItem* item)
{
    if(nullptr != item)
    {
        m_child.append(item);
        item->setParentItem(this);
        TcpClient* tcp = dynamic_cast<TcpClient*>(item);
        if(nullptr!=tcp)
        {
            updateNewClient(tcp);
        }
        return m_child.size();
    }
    return -1;

}

void Channel::updateNewClient(TcpClient* newComer)
{
    NetworkMessageWriter* msg1 = new NetworkMessageWriter(NetMsg::AdministrationCategory,NetMsg::ClearTable);
    msg1->string8(newComer->getId());
    QMetaObject::invokeMethod(newComer,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg1),Q_ARG(bool,true));
    //Sending players infos
    for(auto child : m_child)
    {
        if(child->isLeaf())
        {
            TcpClient* tcpConnection = dynamic_cast<TcpClient*>(child);
            if(nullptr != tcpConnection)
            {
                if((tcpConnection != newComer)&&(tcpConnection->isFullyDefined()))
                {
                    NetworkMessageWriter* msg = new NetworkMessageWriter(NetMsg::PlayerCategory,NetMsg::PlayerConnectionAction);
                    tcpConnection->fill(msg);
                    QMetaObject::invokeMethod(newComer,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg),Q_ARG(bool,true));
                }

            }
        }
    }
    //resend all previous data received
    for(auto msg : m_dataToSend)
    {
        //tcp->sendMessage(msg);
        QMetaObject::invokeMethod(newComer,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg),Q_ARG(bool,false));
    }
}

void Channel::kick(QString str)
{
    bool found = false;
    for(TreeItem* item : m_child)
    {
        if(item->getId() == str)
        {
            //child = item;
            found = true;
            m_child.removeAll(item);
            emit itemChanged();

            TcpClient* client = dynamic_cast<TcpClient*>(item);
            removeClient(client);
            client->closeConnection();
        }
    }
    if(!found)
    {
        for(TreeItem* item : m_child)
        {
            item->kick(str);
        }
    }
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
        for(TreeItem* item : m_child)
        {
            if(item->addChildInto(id, child))
            {
                return true;
            }
        }
    }
    return false;
}

void Channel::clear()
{
    for(TreeItem* item : m_child)
    {
        item->clear();
    }
    m_child.clear();
}
TreeItem* Channel::getChildById(QString id)
{
    for(TreeItem* item : m_child)
    {
        if(item->getId() == id)
        {
            return item;
        }
        else
        {
            auto itemChild = item->getChildById(id);
            if(nullptr != itemChild)
            {
                return itemChild;
            }
        }
    }
    return nullptr;
}

void Channel::insertChildAt(int pos, TreeItem* item)
{
    m_child.insert(pos,item);
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
    m_name = msg.string16();
    m_description = msg.string16();
}

bool Channel::removeClient(TcpClient* client)
{
    //must be the first line
    int i = m_child.removeAll(client);

    //notify all remaining chan member to remove former player
    NetworkMessageWriter* message = new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::DelPlayerAction);
    message->string8(client->getPlayerId());
    sendMessage(message,nullptr,false);

    if(hasNoClient())
    {
        m_dataToSend.clear();
    }

    emit itemChanged();
    return (0 < i);
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
    bool hasNoClient = true;
    for(auto child : m_child)
    {
        if(child->isLeaf())
        {
            hasNoClient = false;
        }
    }
    return hasNoClient;
}
