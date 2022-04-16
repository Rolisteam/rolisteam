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
#include "chatroomfactory.h"
#include "core/model/playermodel.h"
#include "diceparser/diceroller.h"
#include "messageinterface.h"

namespace InstantMessaging
{
namespace
{
bool isClosable(ChatRoom* chatroom)
{
    return (chatroom->type() == ChatRoom::EXTRA
            || (chatroom->type() == ChatRoom::GLOBAL && chatroom->uuid() != QStringLiteral("global")));
}
} // namespace
InstantMessagingModel::InstantMessagingModel(PlayerModel* playerModel, QObject* parent)
    : QAbstractListModel(parent), m_personModel(playerModel)
{
}

InstantMessagingModel::~InstantMessagingModel()= default;

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

    std::set<int> map({TitleRole, ChatRole, RecipiantCountRole, IdRole, HasUnreadMessageRole, ClosableRole});

    if(map.find(item) == map.end())
        return {};

    QVariant var;
    auto chatroom= m_chats[static_cast<std::size_t>(index.row())].get();

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
    case HasUnreadMessageRole:
        var= chatroom->unreadMessage();
        break;
    case RecipiantCountRole:
        var= chatroom->recipiantCount();
        break;
    case ClosableRole:
        var= isClosable(chatroom);
        break;
    }

    return var;
}

QHash<int, QByteArray> InstantMessagingModel::roleNames() const
{
    return QHash<int, QByteArray>({{TitleRole, "title"},
                                   {ChatRole, "chatroom"},
                                   {IdRole, "id"},
                                   {ClosableRole, "closable"},
                                   {HasUnreadMessageRole, "unread"},
                                   {RecipiantCountRole, "recipiantCount"}});
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

bool InstantMessagingModel::unread() const
{
    return std::any_of(std::begin(m_chats), std::end(m_chats),
                       [](const std::unique_ptr<ChatRoom>& room) { return room->unreadMessage(); });
}

void InstantMessagingModel::insertGlobalChatroom(const QString& title, const QString& uuid)
{
    addChatRoom(ChatRoomFactory::createChatRoom(title, QStringList(), uuid, InstantMessaging::ChatRoom::GLOBAL,
                                                localId(), m_personModel, this),
                !uuid.isEmpty());
}

void InstantMessagingModel::insertIndividualChatroom(const QString& playerId, const QString& playerName)
{
    addChatRoom(ChatRoomFactory::createChatRoom(playerName, {playerId, localId()}, playerId,
                                                InstantMessaging::ChatRoom::SINGLEPLAYER, localId(), m_personModel,
                                                this));
}

void InstantMessagingModel::insertExtraChatroom(const QString& title, const QStringList& playerIds, bool remote,
                                                const QString& uuid)
{
    addChatRoom(ChatRoomFactory::createChatRoom(title, playerIds, uuid, InstantMessaging::ChatRoom::EXTRA, localId(),
                                                m_personModel, this),
                remote);
}

void InstantMessagingModel::addChatRoom(ChatRoom* room, bool remote)
{
    if(!room)
        return;

    std::unique_ptr<ChatRoom> chatroom(room);
    connect(this, &InstantMessagingModel::localIdChanged, chatroom.get(), &ChatRoom::setLocalId);
    connect(room, &ChatRoom::unreadMessageChanged, this, [room, this]() {
        auto it= std::find_if(m_chats.begin(), m_chats.end(),
                              [room](const std::unique_ptr<ChatRoom>& chatRoom) { return room == chatRoom.get(); });
        auto idx= static_cast<int>(std::distance(m_chats.begin(), it));
        emit dataChanged(index(idx, 0, QModelIndex()), index(idx, 0, QModelIndex()), {HasUnreadMessageRole, TitleRole});
        emit unreadChanged();
    });
    chatroom->setLocalId(localId());
    chatroom->setDiceParser(m_diceParser);

    auto size= static_cast<int>(m_chats.size());
    beginInsertRows(QModelIndex(), size, size);
    m_chats.push_back(std::move(chatroom));
    endInsertRows();

    emit chatRoomCreated(room, remote);
}

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
    /*  auto idx = std::distance(m_chats.begin(), it);
      emit dataChanged(index(idx, 0, QModelIndex()), index(idx, 0, QModelIndex()),
          {HasUnreadMessageRole, TitleRole});*/
}

void InstantMessagingModel::removePlayer(const QString& id)
{
    auto it= std::find_if(m_chats.begin(), m_chats.end(), [id](const std::unique_ptr<ChatRoom>& chatRoom) {
        return (chatRoom->uuid() == id && ChatRoom::SINGLEPLAYER && chatRoom->type());
    });

    if(it == m_chats.end())
        return;

    auto idx= static_cast<int>(std::distance(m_chats.begin(), it));

    beginRemoveRows(QModelIndex(), idx, idx);
    m_chats.erase(it);
    endRemoveRows();
}

void InstantMessagingModel::setDiceParser(DiceRoller* diceParser)
{
    m_diceParser= diceParser;
    std::for_each(m_chats.begin(), m_chats.end(),
                  [this](const std::unique_ptr<ChatRoom>& chatRoom) { chatRoom->setDiceParser(m_diceParser); });
}
} // namespace InstantMessaging
