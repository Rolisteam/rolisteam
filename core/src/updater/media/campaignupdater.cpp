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
#include "campaignupdater.h"

#include "data/campaign.h"
#include "diceparser.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

namespace campaign
{
CampaignUpdater::CampaignUpdater(DiceParser* dice, Campaign* manager, QObject* parent)
    : QObject(parent), m_manager(manager), m_dice(dice)
{
    Q_ASSERT(m_manager);
    Q_ASSERT(m_dice);
    auto model= m_manager->diceAliases();
    connect(model, &DiceAliasModel::aliasAdded, this,
            [](const DiceAlias* alias, int i) { MessageHelper::sendOffOneDiceAlias(alias, i); });
    connect(model, &DiceAliasModel::aliasRemoved, this, [](int i) { MessageHelper::sendOffDiceAliasRemoved(i); });
    connect(model, &DiceAliasModel::aliasMoved, this, [](int i, int j) { MessageHelper::sendOffDiceAliasMoved(i, j); });
    connect(model, &DiceAliasModel::modelReset, this, [model]() { MessageHelper::sendOffAllDiceAlias(model); });

    auto states= m_manager->stateModel();
    connect(states, &CharacterStateModel::characterStateAdded, this,
            [](const CharacterState* state, int i) { MessageHelper::sendOffOneCharacterState(state, i); });
    connect(states, &CharacterStateModel::stateRemoved, this,
            [](const QString& id) { MessageHelper::sendOffCharacterStateRemoved(id); });
    connect(states, &CharacterStateModel::stateMoved, this,
            [](int i, int j) { MessageHelper::sendOffCharacterStateMoved(i, j); });
    connect(states, &CharacterStateModel::modelReset, this,
            [states]() { MessageHelper::sendOffAllCharacterState(states); });

    // GM player
    auto updateAlias= [this]() {
        auto aliases= m_dice->aliases();
        qDeleteAll(*aliases);
        aliases->clear();
        const auto& newAliases= m_manager->diceAliases()->aliases();
        std::for_each(std::begin(newAliases), std::end(newAliases), [aliases](const std::unique_ptr<DiceAlias>& alias) {
            auto p= alias.get();
            aliases->append(new DiceAlias(*p));
        });
        FileSerializer::writeDiceAliasIntoCampaign(m_manager->rootDirectory(),
                                                   FileSerializer::dicesToArray(newAliases));
    };

    connect(model, &DiceAliasModel::aliasAdded, this, updateAlias);
    connect(model, &DiceAliasModel::aliasRemoved, this, updateAlias);
    connect(model, &DiceAliasModel::aliasMoved, this, updateAlias);
    connect(model, &DiceAliasModel::modelReset, this, updateAlias);
    connect(model, &DiceAliasModel::dataChanged, this, updateAlias);
    connect(model, &DiceAliasModel::aliasChanged, this, updateAlias);

    // GM player
    auto updateState= [this]() {
        auto const& states= m_manager->stateModel()->statesList();
        FileSerializer::writeStatesIntoCampaign(m_manager->rootDirectory(),
                                                FileSerializer::statesToArray(states, m_manager->rootDirectory()));
    };

    connect(states, &CharacterStateModel::characterStateAdded, this, updateState);
    connect(states, &CharacterStateModel::stateRemoved, this, updateState);
    connect(states, &CharacterStateModel::stateMoved, this, updateState);
    connect(states, &CharacterStateModel::modelReset, this, updateState);
    connect(states, &CharacterStateModel::dataChanged, this, updateState);
    connect(states, &CharacterStateModel::stateChanged, this, updateState);

    ReceiveEvent::registerNetworkReceiver(NetMsg::CampaignCategory, this);
}

NetWorkReceiver::SendType CampaignUpdater::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::CharactereStateModel && msg->category() == NetMsg::CampaignCategory)
    {
        MessageHelper::fetchCharacterStatesFromNetwork(msg, m_manager->stateModel());
    }
    else if(msg->action() == NetMsg::DiceAliasModel && msg->category() == NetMsg::CampaignCategory)
    {
        MessageHelper::fetchDiceAliasFromNetwork(msg, m_dice->aliases());
    }
    return NetWorkReceiver::NONE;
}

void CampaignUpdater::updateDiceModel()
{
    MessageHelper::sendOffAllDiceAlias(m_manager->diceAliases());
}

void CampaignUpdater::updateStateModel()
{
    MessageHelper::sendOffAllCharacterState(m_manager->stateModel());
}
} // namespace campaign
