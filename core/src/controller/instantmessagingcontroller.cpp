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
#include "instantmessagingcontroller.h"

#include <QQmlEngine>

#include "data/localpersonmodel.h"
#include "data/player.h"
#include "network/receiveevent.h"
#include "qmlchat/chatroom.h"
#include "qmlchat/filterinstantmessagingmodel.h"
#include "qmlchat/instantmessagingmodel.h"
#include "qmlchat/messagemodel.h"
#include "qmlchat/textwritercontroller.h"
#include "updater/instantmessagingupdater.h"
#include "userlist/playermodel.h"

void registerType()
{
    qRegisterMetaType<InstantMessaging::FilterInstantMessagingModel*>("FilterInstantMessagingModel*");
    qRegisterMetaType<InstantMessaging::MessageModel*>("MessageModel*");
    qRegisterMetaType<LocalPersonModel*>("LocalPersonModel*");
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<InstantMessaging::ChatRoom*>("ChatRoom*");

    qmlRegisterAnonymousType<InstantMessaging::FilterInstantMessagingModel>("FilterInstantMessagingModel", 1);
    qmlRegisterAnonymousType<InstantMessaging::MessageModel>("MessageModel", 1);
    qmlRegisterAnonymousType<LocalPersonModel>("LocalPersonModel", 1);
    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);
    qmlRegisterUncreatableType<InstantMessaging::ChatRoom>("InstantMessaging", 1, 0, "ChatRoom",
                                                           "ChatRoom can't be created.");
    qmlRegisterType<InstantMessaging::TextWriterController>("InstantMessaging", 1, 0, "TextWriterController");
}

InstantMessagingController::InstantMessagingController(PlayerModel* model, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_model(new InstantMessaging::InstantMessagingModel)
    , m_updater(new InstantMessaging::InstantMessagingUpdater)
    , m_players(model)
    , m_localPersonModel(new LocalPersonModel)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::InstantMessageCategory, this);
    registerType();
    addFilterModel();

    m_localPersonModel->setSourceModel(m_players);

    connect(m_model.get(), &InstantMessaging::InstantMessagingModel::chatRoomCreated, this,
            [this](InstantMessaging::ChatRoom* room) { m_updater->addChatRoom(room); });
    connect(m_model.get(), &InstantMessaging::InstantMessagingModel::localIdChanged, this,
            &InstantMessagingController::localIdChanged);
    connect(m_players, &PlayerModel::playerJoin, this, [this](Player* player) {
        if(nullptr == player)
            return;
        if(player->uuid() == localId())
            return;

        m_model->insertIndividualChatroom(player->uuid(), player->name());
    });

    connect(m_players, &PlayerModel::playerLeft, this, [this](Player* player) {
        if(nullptr == player)
            return;

        m_model->removePlayer(player->uuid());
    });

    m_model->insertGlobalChatroom(tr("Global"), QStringLiteral("global"));
}

InstantMessagingController::~InstantMessagingController()= default;

InstantMessaging::FilterInstantMessagingModel* InstantMessagingController::mainModel() const
{
    Q_ASSERT(m_filterModels.size() > 0);
    return m_filterModels[0].get();
}

InstantMessaging::ChatRoom* InstantMessagingController::globalChatroom() const
{
    return m_model->globalChatRoom();
}

PlayerModel* InstantMessagingController::playerModel() const
{
    return m_players;
}

LocalPersonModel* InstantMessagingController::localPersonModel() const
{
    return m_localPersonModel.get();
}

QString InstantMessagingController::localId() const
{
    return m_model->localId();
}

void InstantMessagingController::setGameController(GameController*) {}

NetWorkReceiver::SendType InstantMessagingController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;
    switch(msg->action())
    {
    case NetMsg::InstantMessageAction:
        m_updater->addMessageToModel(m_model.get(), msg);
        break;
    default:
        break;
    }
    return type;
}

void InstantMessagingController::detach(const QString& id) {}

void InstantMessagingController::reattach(const QString& id) {}

void InstantMessagingController::splitScreen() {}

void InstantMessagingController::setLocalId(const QString& id)
{
    m_model->setLocalId(id);
}

void InstantMessagingController::addFilterModel()
{
    std::unique_ptr<InstantMessaging::FilterInstantMessagingModel> model(
        new InstantMessaging::FilterInstantMessagingModel);
    model->setSourceModel(m_model.get());
    m_filterModels.push_back(std::move(model));
}
