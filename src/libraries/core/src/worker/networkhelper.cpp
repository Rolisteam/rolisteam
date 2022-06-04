/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "worker/networkhelper.h"

#include <QJsonDocument>

#include "media/mediatype.h"
#include "network/channel.h"
#include "network/channelmodel.h"
#include "network/serverconnection.h"

namespace helper
{
namespace network
{
QByteArray jsonObjectToByteArray(const QJsonObject& obj)
{
    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Indented);
}

QJsonObject channelItemToJsonObject(const TreeItem* item)
{
    QJsonObject jsonObj;
    if(item->isLeaf())
    {
        auto client= dynamic_cast<const ServerConnection*>(item);
        if(client)
        {
            jsonObj[Core::jsonNetwork::JSON_TYPE]= Core::jsonNetwork::JSON_TYPE_CLIENT;
            jsonObj[Core::jsonNetwork::JSON_NAME]= client->name();
            jsonObj[Core::jsonNetwork::JSON_ADMIN]= client->isAdmin();
            jsonObj[Core::jsonNetwork::JSON_ID]= client->uuid();
            jsonObj[Core::jsonNetwork::JSON_IDPLAYER]= client->playerId();
        }
    }
    else
    {
        auto channel= dynamic_cast<const Channel*>(item);
        if(channel)
        {

            jsonObj[Core::jsonNetwork::JSON_TYPE]= Core::jsonNetwork::JSON_TYPE_CHANNEL;
            jsonObj[Core::jsonNetwork::JSON_NAME]= channel->name();
            jsonObj[Core::jsonNetwork::JSON_DESCRIPTION]= channel->description();
            jsonObj[Core::jsonNetwork::JSON_PASSWORD]= QString::fromUtf8(channel->password().toBase64());
            jsonObj[Core::jsonNetwork::JSON_USERLISTED]= channel->usersListed();
            jsonObj[Core::jsonNetwork::JSON_MEMORYSIZE]= static_cast<int>(channel->memorySize());
            jsonObj[Core::jsonNetwork::JSON_LOCKED]= channel->locked();
            jsonObj[Core::jsonNetwork::JSON_ID]= channel->uuid();
            QJsonArray array;
            auto const& children= channel->childrenItem();
            for(auto& child : children)
            {
                auto childObj= channelItemToJsonObject(child);
                array.append(childObj);
            }
            jsonObj[Core::jsonNetwork::JSON_CHILDREN]= array;
        }
    }
    return jsonObj;
}

QJsonObject channelModelToJSonObject(ChannelModel* model)
{
    if(!model)
        return {};

    auto channels= model->modelData();
    QJsonObject obj;
    QJsonArray array;
    for(auto& item : channels) // int i = 0; i< m_root->childCount(); ++i)
    {
        if(nullptr == item)
            continue;
        auto jsonObj= channelItemToJsonObject(item);
        array.append(jsonObj);
    }
    obj[Core::jsonNetwork::JSON_CHANNELS]= array;
    return obj;
}

TreeItem* readClient(const QJsonObject& obj)
{
    auto res= new ServerConnection(nullptr, nullptr);

    res->setName(obj[Core::jsonNetwork::JSON_NAME].toString());
    res->setIsAdmin(obj[Core::jsonNetwork::JSON_ADMIN].toBool());
    res->setUuid(obj[Core::jsonNetwork::JSON_ID].toString());
    res->setPlayerId(obj[Core::jsonNetwork::JSON_IDPLAYER].toString());

    return res;
}

TreeItem* readChannel(const QJsonObject& obj)
{
    auto res= new Channel();
    res->setPassword(QByteArray::fromBase64(obj[Core::jsonNetwork::JSON_PASSWORD].toString().toUtf8()));
    res->setName(obj[Core::jsonNetwork::JSON_NAME].toString());
    res->setUsersListed(obj[Core::jsonNetwork::JSON_USERLISTED].toBool());
    res->setMemorySize(obj[Core::jsonNetwork::JSON_MEMORYSIZE].toInt());
    res->setLocked(obj[Core::jsonNetwork::JSON_LOCKED].toBool());
    res->setUuid(obj[Core::jsonNetwork::JSON_ID].toString());
    res->setName(obj[Core::jsonNetwork::JSON_NAME].toString());

    auto children= obj[Core::jsonNetwork::JSON_CHILDREN].toArray();
    for(auto childRef : children)
    {
        auto child= childRef.toObject();
        TreeItem* treeItem= nullptr;
        if(child[Core::jsonNetwork::JSON_TYPE].toString() == Core::jsonNetwork::JSON_TYPE_CHANNEL)
        {
            treeItem= readChannel(child);
        }
        else
        {
            treeItem= readClient(child);
        }
        res->addChild(treeItem);
    }

    return res;
}

void fetchChannelModel(ChannelModel* model, const QJsonObject& obj)
{
    auto array= obj[Core::jsonNetwork::JSON_CHANNELS].toArray();

    QList<TreeItem*> children;
    for(auto itemRef : array)
    {
        auto item= itemRef.toObject();
        QPointer<TreeItem> treeItem;
        if(item[Core::jsonNetwork::JSON_TYPE].toString() == Core::jsonNetwork::JSON_TYPE_CHANNEL)
        {
            treeItem= readChannel(item);
        }
        else
        {
            treeItem= readClient(item);
        }
        children.append(treeItem);
    }

    model->resetData(children);
}

} // namespace network
} // namespace helper
