/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "playercontroller.h"

#include "controller/gamecontroller.h"
#include "controller/preferencescontroller.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "model/characterstatemodel.h"
#include "model/playermodel.h"
#include "network/receiveevent.h"
#include "undoCmd/addlocalcharactercommand.h"
#include "undoCmd/removelocalcharactercommand.h"
#include "worker/messagehelper.h"
#include "worker/playermessagehelper.h"

#include <QtDebug>

void addPlayerToModel(PlayerModel* model, NetworkMessageReader* msg)
{
    Player* player= new Player();
    PlayerMessageHelper::readPlayer(*msg, player);
    model->addPlayer(player);
}

PlayerController::PlayerController(QObject* parent)
    : AbstractControllerInterface(parent)
    , m_model(new PlayerModel)
    , m_characterModel(new CharacterModel)
    , m_localPlayer(new Player)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::PlayerCategory, this);

    m_characterModel->setSourceModel(m_model.get());
    connect(m_model.get(), &PlayerModel::gameMasterIdChanged, this, &PlayerController::gameMasterIdChanged);
    connect(m_localPlayer, &Player::uuidChanged, this, &PlayerController::localPlayerIdChanged);
    connect(m_localPlayer, &Player::uuidChanged, m_model.get(), &PlayerModel::setLocalPlayerId);

    m_model->setLocalPlayerId(m_localPlayer->uuid());
}

PlayerController::~PlayerController()= default;

void PlayerController::clear()
{
    m_model->clear();
}

QString PlayerController::gameMasterId() const
{
    return m_model->gameMasterId();
}

QString PlayerController::localPlayerId() const
{
    if(nullptr == m_localPlayer)
        return {};
    return m_localPlayer->uuid();
}

NetWorkReceiver::SendType PlayerController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::AllExceptSender;
    switch(msg->action())
    {
    case NetMsg::PlayerConnectionAction:
        addPlayerToModel(m_model.get(), msg);
        break;
    case NetMsg::DelPlayerAction:
        removePlayerById(MessageHelper::readPlayerId(*msg));
        break;
    case NetMsg::ChangePlayerProperty:
        MessageHelper::updatePerson(*msg, m_model.get());
        break;
    default:
        break;
    }
    return type;
}

void PlayerController::setGameController(GameController* gameCtrl)
{
    // auto prefsCtrl= gameCtrl->preferencesController();

    connect(gameCtrl, &GameController::connectedChanged, this, [this](bool b) {
        if(b)
            PlayerMessageHelper::sendOffPlayerInformations(localPlayer());
        else
            m_model->clear();
    });

    // m_characterStateModel= prefsCtrl->characterStateModel();
    // emit characterStateModelChanged();
}

Player* PlayerController::localPlayer() const
{
    return m_localPlayer;
}

PlayerModel* PlayerController::model() const
{
    return m_model.get();
}

QAbstractItemModel* PlayerController::characterStateModel() const
{
    return m_characterStateModel;
}

CharacterModel* PlayerController::characterModel() const
{
    return m_characterModel.get();
}

void PlayerController::addPlayer(Player* player)
{
    m_model->addPlayer(player);
}

void PlayerController::removePlayer(Player* player)
{
    m_model->removePlayer(player);
}

void PlayerController::removePlayerById(const QString& id)
{
    auto player= m_model->playerById(id);
    if(player == nullptr)
        return;

    removePlayer(player);
}

void PlayerController::addLocalCharacter()
{
    auto idState= m_characterStateModel->index(0, 0).data(CharacterStateModel::ID).toString();
    auto cmd= new AddLocalCharacterCommand(m_model.get(), idState, m_model->index(0, 0));
    emit performCommand(cmd);
}

void PlayerController::removeLocalCharacter(const QModelIndex& index)
{
    auto cmd= new RemoveLocalCharacterCommand(m_model.get(), index);
    emit performCommand(cmd);
}
