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
#include "chatroom.h"

#include "filteredplayermodel.h"
#include "messagemodel.h"

namespace InstantMessaging
{

ChatRoom::ChatRoom(ChatRoomType type, const QString& id, QObject* parent)
    : QObject(parent), m_recipiants(new FilteredPlayerModel), m_messageModel(new MessageModel), m_type(type), m_uuid(id)
{
    connect(m_messageModel.get(), &MessageModel::localIdChanged, this, &ChatRoom::localIdChanged);
}

ChatRoom::~ChatRoom()= default;

MessageModel* ChatRoom::messageModel() const
{
    return m_messageModel.get();
}

bool ChatRoom::unreadMessage() const
{
    return m_unreadMessage;
}

QString ChatRoom::uuid() const
{
    return m_uuid;
}

QString ChatRoom::localId() const
{
    return m_messageModel->localId();
}

FilteredPlayerModel* ChatRoom::recipiants() const
{
    return m_recipiants.get();
}

QString ChatRoom::title() const
{
    return m_title;
}

ChatRoom::ChatRoomType ChatRoom::type() const
{
    return m_type;
}

int ChatRoom::recipiantCount() const
{
    return m_recipiants->rowCount();
}

void ChatRoom::setUuid(const QString& id)
{
    if(id == m_uuid)
        return;
    m_uuid= id;
    emit uuidChanged(m_uuid);
}

void ChatRoom::setTitle(const QString& title)
{
    if(m_title == title)
        return;
    m_title= title;
    emit titleChanged(m_title);
}

void ChatRoom::setUnreadMessage(bool b)
{
    if(b == m_unreadMessage)
        return;
    m_unreadMessage= b;
    emit unreadMessageChanged(m_unreadMessage);
}

void ChatRoom::addMessage(const QString& text)
{
    m_messageModel->addMessage(text, QDateTime::currentDateTime(), localId(), MessageInterface::Text);
}

void ChatRoom::addMessageInterface(MessageInterface* message)
{
    m_messageModel->addMessageInterface(message);
}

void ChatRoom::setLocalId(const QString& id)
{
    m_messageModel->setLocalId(id);
}
bool ChatRoom::hasRecipiant(const QString& id)
{
    return m_recipiants->hasRecipiant(id);
}
} // namespace InstantMessaging
