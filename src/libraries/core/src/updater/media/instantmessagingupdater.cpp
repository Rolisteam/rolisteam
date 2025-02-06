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
#include "updater/media/instantmessagingupdater.h"

#include "data/chatroom.h"
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/messagefactory.h"
#include "instantmessaging/messageinterface.h"
#include "media/mediatype.h"
#include "model/filteredplayermodel.h"
#include "model/instantmessagingmodel.h"
#include "model/messagemodel.h"

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"
#include "utils/iohelper.h"
#include "worker/convertionhelper.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"

namespace InstantMessaging
{
InstantMessagingUpdater::InstantMessagingUpdater(InstantMessagingController* ctrl, QObject* parent)
    : QObject(parent), m_imCtrl(ctrl)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::InstantMessageCategory, this);

    connect(m_imCtrl, &InstantMessagingController::chatRoomCreated, this,
            [this](InstantMessaging::ChatRoom* room, bool remote) { addChatRoom(room, remote); });

    connect(m_imCtrl, &InstantMessagingController::chatRoomRemoved, this,
            [this](InstantMessaging::ChatRoom* room, bool remote) { addChatRoom(room, remote); });

    auto model= ctrl->model();
    if(!model)
        return;

    auto const& rooms= model->rooms();
    for(auto const& t : rooms)
    {
        addChatRoom(t.get(), false);
    }
}

void InstantMessagingUpdater::openChat(InstantMessaging::ChatRoom* chat)
{
    if(!chat)
        return;

    NetworkMessageWriter msg(NetMsg::InstantMessageCategory, NetMsg::AddChatroomAction);
    auto ids= chat->recipiants()->recipiantIds();
    if(chat->type() == ChatRoom::EXTRA)
    {
        msg.setRecipientList(ids, NetworkMessage::OneOrMany);
    }
    msg.uint8(chat->type());
    msg.string8(chat->uuid());
    msg.string32(chat->title());

    if(chat->type() == ChatRoom::EXTRA)
    {
        msg.uint16(ids.size());
        std::for_each(ids.begin(), ids.end(), [&msg](const QString& id) { msg.string16(id); });
    }

    msg.sendToServer();
}

void InstantMessagingUpdater::readChatroomToModel(InstantMessaging::InstantMessagingModel* model,
                                                  NetworkMessageReader* msg)
{
    if(!model || !msg)
        return;

    auto type= static_cast<ChatRoom::ChatRoomType>(msg->uint8());
    auto uuid= msg->string8();
    auto title= msg->string32();
    auto idCount= msg->uint16();
    QStringList ids;
    for(int i= 0; i < idCount; ++i)
    {
        ids << msg->string16();
    }
    if(type == ChatRoom::EXTRA)
        model->insertExtraChatroom(title, ids, true, uuid);
    else
        model->insertGlobalChatroom(title, uuid);
}

void InstantMessagingUpdater::removeChatRoom(const QString& id, bool remote)
{
    if(remote)
        return;
    NetworkMessageWriter msg(NetMsg::InstantMessageCategory, NetMsg::RemoveChatroomAction);
    msg.string32(id);
    msg.sendToServer();
}

void InstantMessagingUpdater::addChatRoom(InstantMessaging::ChatRoom* room, bool remote)
{
    if(nullptr == room)
        return;

    if(!remote
       && (room->type() == InstantMessaging::ChatRoom::EXTRA
           || (room->type() == InstantMessaging::ChatRoom::GLOBAL && room->uuid() != QStringLiteral("global"))))
        openChat(room);

    connect(room, &InstantMessaging::ChatRoom::titleChanged, this,
            [this, room]() { sendOffChatRoomChanges<QString>(room, QStringLiteral("title")); });

    auto model= room->messageModel();
    connect(model, &InstantMessaging::MessageModel::messageAdded, this,
            [room](MessageInterface* message)
            {
                if(!message)
                    return;
                if(message->type() == InstantMessaging::MessageInterface::Error)
                    return;
                auto type= room->type();
                NetworkMessageWriter msg(NetMsg::InstantMessageCategory, NetMsg::InstantMessageAction);
                if(type != ChatRoom::GLOBAL && room->uuid() != QStringLiteral("global"))
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
                msg.string32(message->imageLink().toString());

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
    auto url= QUrl::fromUserInput(msg->string32());

    MessageInterface* imMessage
        = InstantMessaging::MessageFactory::createMessage(owner, writer, time, messageType, text, url);

    if(imMessage == nullptr)
        return;

    model->addMessageIntoChatroom(imMessage, type, uuid);
}

void InstantMessagingUpdater::sendMessage() {}

void InstantMessagingUpdater::closeChat() {}

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

NetWorkReceiver::SendType InstantMessagingUpdater::processMessage(NetworkMessageReader* msg)
{
    if(!m_imCtrl)
        return NetWorkReceiver::NONE;

    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;
    switch(msg->action())
    {
    case NetMsg::InstantMessageAction:
        addMessageToModel(m_imCtrl->model(), msg);
        break;
    case NetMsg::AddChatroomAction:
        readChatroomToModel(m_imCtrl->model(), msg);
        break;
    case NetMsg::RemoveChatroomAction:
        m_imCtrl->closeChatroom(msg->string32(), true);
        break;
    default:
        break;
    }
    return type;
}

namespace keys
{
constexpr auto modelData{"rooms"};
constexpr auto font{"font"};
constexpr auto nightMode{"nightMode"};
constexpr auto sound{"sound"};
} // namespace keys

void InstantMessagingUpdater::save(const QString& path)
{
    if(!m_imCtrl)
        return;

    QJsonObject rootObj;

    auto model= m_imCtrl->model();

    QJsonObject obj;
    if(m_saveChatrooms)
        obj= ModelHelper::saveInstantMessageModel(model);

    rootObj[keys::modelData]= obj;
    rootObj[keys::nightMode]= m_imCtrl->nightMode();
    rootObj[keys::sound]= m_imCtrl->sound();
    rootObj[keys::font]= m_imCtrl->font().toString();

    QJsonDocument doc;
    doc.setObject(rootObj);

    utils::IOHelper::writeFile(path, doc.toJson(), true);
}

void InstantMessagingUpdater::load(const QString& path)
{
    if(!m_imCtrl)
        return;
    auto model= m_imCtrl->model();

    bool ok;
    auto data= IOHelper::loadJsonFileIntoObject(path, ok);

    auto modelData= data[keys::modelData].toObject();
    m_imCtrl->setSound(data[keys::sound].toBool());
    QFont f;
    f.fromString(data[keys::font].toString());
    m_imCtrl->setFont(f);
    m_imCtrl->setNightMode(data[keys::nightMode].toInt());
    if(ok && !data.isEmpty() && m_saveChatrooms)
        ModelHelper::fetchInstantMessageModel(modelData, model);
}

bool InstantMessagingUpdater::saveChatrooms() const
{
    return m_saveChatrooms;
}

void InstantMessagingUpdater::setSaveChatrooms(bool newSaveChatrooms)
{
    if(m_saveChatrooms == newSaveChatrooms)
        return;
    m_saveChatrooms= newSaveChatrooms;
    emit saveChatroomsChanged();
}

} // namespace InstantMessaging
