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
#include "instantmessagingupdater.h"

#include "media/mediatype.h"
#include "qmlchat/chatroom.h"
#include "qmlchat/dicemessage.h"
#include "qmlchat/filteredplayermodel.h"
#include "qmlchat/instantmessagingmodel.h"
#include "qmlchat/messagefactory.h"
#include "qmlchat/messageinterface.h"
#include "qmlchat/messagemodel.h"

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/convertionhelper.h"

namespace InstantMessaging
{
InstantMessagingUpdater::InstantMessagingUpdater(QObject* parent) : QObject(parent) {}

void InstantMessagingUpdater::addChatRoom(InstantMessaging::ChatRoom* room)
{
    if(nullptr == room)
        return;

    connect(room, &InstantMessaging::ChatRoom::titleChanged, this,
            [this, room]() { sendOffChatRoomChanges<QString>(room, QStringLiteral("title")); });

    auto model= room->messageModel();
    connect(model, &InstantMessaging::MessageModel::messageAdded, this, [room](MessageInterface* message) {
        auto type= room->type();
        NetworkMessageWriter msg(NetMsg::InstantMessageCategory, NetMsg::InstantMessageAction);
        if(type != ChatRoom::GLOBAL && room->uuid() != QStringLiteral("Global"))
        {
            auto model= room->recipiants();
            if(!model)
                return;

            auto recipiants= model->recipiantIds();
            if(type == ChatRoom::SINGLEPLAYER)
                recipiants.append(room->uuid());

            msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
        }
        msg.uint8(type);
        msg.string8(room->uuid());
        msg.uint8(message->type());
        msg.string16(message->owner());
        msg.string16(message->writer());
        msg.string32(message->text());
        msg.dateTime(message->dateTime());

        msg.sendToServer();
    });
}

void InstantMessagingUpdater::addMessageToModel(InstantMessaging::InstantMessagingModel* model,
                                                NetworkMessageReader* msg)
{
    using IM= InstantMessaging::MessageInterface;
    auto type= static_cast<InstantMessaging::ChatRoom::ChatRoomType>(msg->uint8());
    auto uuid= msg->string8();
    auto messageType= static_cast<IM::MessageType>(msg->uint8());
    auto owner= msg->string16();
    auto writer= msg->string16();
    auto text= msg->string32();
    auto time= msg->dateTime();

    MessageInterface* imMessage= InstantMessaging::MessageFactory::createMessage(owner, writer, time, messageType);

    if(imMessage == nullptr)
        return;
    imMessage->setText(text);

    model->addMessageIntoChatroom(imMessage, type, uuid);
}

void InstantMessaging::InstantMessagingUpdater::sendMessage() {}

void InstantMessaging::InstantMessagingUpdater::closeChat() {}

template <typename T>
void InstantMessagingUpdater::sendOffChatRoomChanges(ChatRoom* chatRoom, const QString& property)
{
    if(nullptr == chatRoom)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::UpdateMediaProperty);
    msg.uint8(static_cast<int>(Core::ContentType::INSTANTMESSAGING));
    msg.string8(chatRoom->uuid());
    msg.string16(property);
    auto val= chatRoom->property(property.toLocal8Bit().data());
    Helper::variantToType<T>(val.value<T>(), msg);
    msg.sendToServer();
}

} // namespace InstantMessaging
