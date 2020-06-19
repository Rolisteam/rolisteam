/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "instantmessagingmodel.h"

#include <QDebug>
#include <set>

#include "chatroom.h"
#include "messageinterface.h"

namespace InstantMessaging
{
InstantMessagingModel::InstantMessagingModel(QObject* parent) : QAbstractListModel(parent) {}

InstantMessagingModel::~InstantMessagingModel()= default;

QVariant InstantMessagingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
    return QVariant();
}

int InstantMessagingModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_chats.size());
}

QVariant InstantMessagingModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    int item= role;
    if(role == Qt::DisplayRole)
        item= TitleRole;

    std::set<int> map({TitleRole, ChatRole, RecipiantCountRole, IdRole});

    if(map.find(item) == map.end())
        return {};

    QVariant var;
    auto chatroom= m_chats[index.row()].get();

    switch(item)
    {
    case TitleRole:
        var= chatroom->title();
        break;
    case ChatRole:
        var= QVariant::fromValue(chatroom);
        break;
    case IdRole:
        var= chatroom->uuid();
        break;
    case TypeRole:
        var= chatroom->type();
        break;
    case RecipiantCountRole:
        var= chatroom->recipiantCount();
        break;
    }

    return var;
}

QHash<int, QByteArray> InstantMessagingModel::roleNames() const
{
    return QHash<int, QByteArray>(
        {{TitleRole, "title"}, {ChatRole, "chatroom"}, {IdRole, "id"}, {RecipiantCountRole, "recipiantCount"}});
}

InstantMessaging::ChatRoom* InstantMessagingModel::globalChatRoom() const
{
    Q_ASSERT(m_chats.size() > 0);
    return m_chats[0].get();
}

QString InstantMessagingModel::localId() const
{
    return m_localId;
}

void InstantMessagingModel::insertGlobalChatroom(const QString& title, const QString& uuid)
{
    std::unique_ptr<ChatRoom> global(new ChatRoom(InstantMessaging::ChatRoom::GLOBAL));
    connect(this, &InstantMessagingModel::localIdChanged, global.get(), &ChatRoom::setLocalId);
    global->setLocalId(localId());
    global->setTitle(title);

    if(!uuid.isEmpty())
        global->setUuid(uuid);

    emit chatRoomCreated(global.get());

    beginInsertRows(QModelIndex(), m_chats.size(), m_chats.size());
    m_chats.push_back(std::move(global));
    endInsertRows();
}

void InstantMessagingModel::insertIndividualChatroom(const QString& playerId, const QString& playerName)
{
    std::unique_ptr<ChatRoom> singlePlayerIm(new ChatRoom(InstantMessaging::ChatRoom::SINGLEPLAYER));
    singlePlayerIm->setUuid(playerId);
    singlePlayerIm->setTitle(playerName);
    singlePlayerIm->setLocalId(localId());

    emit chatRoomCreated(singlePlayerIm.get());
    connect(this, &InstantMessagingModel::localIdChanged, singlePlayerIm.get(), &ChatRoom::setLocalId);
    beginInsertRows(QModelIndex(), m_chats.size(), m_chats.size());
    m_chats.push_back(std::move(singlePlayerIm));
    endInsertRows();
}

void InstantMessagingModel::insertCustomChatroom(const QStringList& playerIds) {}

void InstantMessagingModel::setLocalId(const QString& id)
{
    if(m_localId == id)
        return;
    m_localId= id;
    emit localIdChanged(m_localId);
}

void InstantMessagingModel::addMessageIntoChatroom(MessageInterface* message, ChatRoom::ChatRoomType type,
                                                   const QString& uuid)
{ // from network
    auto it= std::find_if(m_chats.begin(), m_chats.end(),
                          [type, uuid, message](const std::unique_ptr<ChatRoom>& chatRoom) {
                              bool val= false;
                              if(chatRoom->uuid() == uuid) // global and Extra chatrom
                                  val= true;
                              else if(type == ChatRoom::SINGLEPLAYER && type == chatRoom->type()
                                      && chatRoom->uuid() == message->owner()) // SinglePlayer chat room
                                  val= true;
                              return val;
                          });

    if(it == m_chats.end())
        return;

    (*it)->addMessageInterface(message);
}

void InstantMessagingModel::removePlayer(const QString& id)
{
    auto it= std::find_if(m_chats.begin(), m_chats.end(), [id](const std::unique_ptr<ChatRoom>& chatRoom) {
        return (chatRoom->uuid() == id && ChatRoom::SINGLEPLAYER && chatRoom->type());
    });

    if(it == m_chats.end())
        return;

    auto idx= std::distance(m_chats.begin(), it);

    beginRemoveRows(QModelIndex(), idx, idx);
    m_chats.erase(it);
    endRemoveRows();
    // TODO select behaviour after

    /*std::for_each(m_chats.begin(), m_chats.end(), [id](const std::unique_ptr<ChatRoom>& chatRoom) {
        if(chatRoom->type() == ChatRoom::EXTRA)
            {
                chatRoom->removeParticipant
            }
    });*/
}

} // namespace InstantMessaging
