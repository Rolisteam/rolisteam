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

QString Channel::password() const
{
    return m_password;
}

void Channel::setPassword(const QString &password)
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
    m_password=json["password"].toString();
    m_name=json["title"].toString();
    m_description=json["description"].toString();
    m_usersListed=json["usersListed"].toBool();

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
    json["password"]=m_password;
    json["title"]=m_name;
    json["description"]=m_description;
    json["usersListed"]=m_usersListed;
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
}
void Channel::sendToAll(NetworkMessageReader* msg, TcpClient* tcp)
{
    m_dataToSend.append(msg);
    for(auto client : m_child)
    {
        TcpClient* other = dynamic_cast<TcpClient*>(client);

        if((nullptr != other)&&(other!=tcp))
        {
            qDebug()<< "[server][send to clients]" << other << tcp;

            //other->sendMessage(*msg);
            QMetaObject::invokeMethod(other,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg));
        }

    }
}
int Channel::addChild(TreeItem* item)
{
    m_child.append(item);
    TcpClient* tcp = dynamic_cast<TcpClient*>(item);
    if(nullptr!=tcp)
    {
        tcp->setParentItem(this);
        //resend all previous data received
        for(auto msg : m_dataToSend)
        {
            //tcp->sendMessage(msg);
            QMetaObject::invokeMethod(tcp,"sendMessage",Qt::QueuedConnection,Q_ARG(NetworkMessage*,msg));
        }

    }
    return m_child.size();

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
