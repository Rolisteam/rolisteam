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
#include "controller/playercontroller.h"

#include "controller/gamecontroller.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "model/characterstatemodel.h"
#include "model/playermodel.h"
#include "undoCmd/addlocalcharactercommand.h"
#include "undoCmd/removelocalcharactercommand.h"
#include "updater/controller/playerupdater.h"
#include "worker/characterfinder.h"
#include <QtDebug>

PlayerController::PlayerController(NetworkController* network, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_model(new PlayerModel)
    , m_characterModel(new CharacterModel)
    , m_updater(new PlayerUpdater(network, this))
    , m_localPlayer(new Player)
{

    m_characterModel->setSourceModel(m_model.get());
    CharacterFinder::setPcModel(m_characterModel.get());

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

QColor PlayerController::localColor() const
{
    return (m_localPlayer) ? m_localPlayer->getColor() : QColor(Qt::black);
}

void PlayerController::setGameController(GameController* gameCtrl)
{
    m_characterStateModel= gameCtrl->campaign()->stateModel();
    m_updater->setGameController(gameCtrl);
    emit characterStateModelChanged();
}

Player* PlayerController::localPlayer() const
{
    return m_localPlayer;
}

PlayerModel* PlayerController::model() const
{
    return m_model.get();
}

bool PlayerController::localIsGm() const
{
    return (localPlayerId() == gameMasterId());
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

void PlayerController::addLocalCharacter()
{
    if(!m_characterStateModel || m_characterStateModel->rowCount() > 0)
        return;
    auto idState= m_characterStateModel->index(0, 0).data(CharacterStateModel::ID).toString();
    auto cmd= new AddLocalCharacterCommand(m_model.get(), idState, m_model->index(0, 0));
    emit performCommand(cmd);
}

void PlayerController::removeLocalCharacter(const QModelIndex& index)
{
    auto cmd= new RemoveLocalCharacterCommand(m_model.get(), index);
    emit performCommand(cmd);
}
