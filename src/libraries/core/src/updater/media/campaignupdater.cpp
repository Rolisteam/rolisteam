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
#include "updater/media/campaignupdater.h"

#include "data/campaign.h"
#include "data/media.h"
#include "diceparser/dicealias.h"
#include "diceparser_qobject/diceroller.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "utils/logcategories.h"
#include "worker/fileserializer.h"
#include "worker/messagehelper.h"
#include <QDir>

namespace campaign
{
CampaignUpdater::CampaignUpdater(DiceRoller* dice, Campaign* manager, QObject* parent)
    : QObject(parent), m_campaign(manager), m_dice(dice)
{
    setCampaign(manager);

    ReceiveEvent::registerNetworkReceiver(NetMsg::CampaignCategory, this);
}

void CampaignUpdater::setCampaign(Campaign* campaign)
{
    if(campaign == m_campaign)
        return;
    m_campaign= campaign;

    // GM player
    auto model= m_campaign->diceAliases();
    auto updateAlias= [this]()
    {
        if(!canForward())
            return;
        auto aliases= m_dice->aliases();
        if(!aliases)
            return;
        qDeleteAll(*aliases);
        aliases->clear();
        const auto& newAliases= m_campaign->diceAliases()->aliases();
        std::for_each(std::begin(newAliases), std::end(newAliases),
                      [aliases](const std::unique_ptr<DiceAlias>& alias)
                      {
                          auto p= alias.get();
                          aliases->append(new DiceAlias(*p));
                      });

        FileSerializer::writeDiceAliasIntoCampaign(m_campaign->rootDirectory(),
                                                   FileSerializer::dicesToArray(newAliases));
        updateDiceModel();
    };

    connect(model, &DiceAliasModel::aliasAdded, this, updateAlias);
    connect(model, &DiceAliasModel::aliasRemoved, this, updateAlias);
    connect(model, &DiceAliasModel::aliasMoved, this, updateAlias);
    connect(model, &DiceAliasModel::modelReset, this, updateAlias);
    connect(model, &DiceAliasModel::dataChanged, this, updateAlias);
    connect(model, &DiceAliasModel::aliasChanged, this, updateAlias);

    // GM player
    auto states= m_campaign->stateModel();

    auto updateState= [this]()
    {
        if(!canForward())
            return;
        auto const& states= m_campaign->stateModel()->statesList();
        FileSerializer::writeStatesIntoCampaign(m_campaign->rootDirectory(),
                                                FileSerializer::statesToArray(states, m_campaign->rootDirectory()));
        updateStateModel();
        auto list= new QList<CharacterState*>();
        std::transform(std::begin(states), std::end(states), std::back_inserter(*list),
                       [](const std::unique_ptr<CharacterState>& item) { return item.get(); });
        Character::setListOfCharacterState(list);
    };

    connect(states, &CharacterStateModel::characterStateAdded, this, updateState);
    connect(states, &CharacterStateModel::stateRemoved, this, updateState);
    connect(states, &CharacterStateModel::stateMoved, this, updateState);
    connect(states, &CharacterStateModel::modelReset, this, updateState);
    connect(states, &CharacterStateModel::dataChanged, this, updateState);
    connect(states, &CharacterStateModel::stateChanged, this, updateState);

    // updateNPCModel
    auto npcModel= m_campaign->npcModel();
    auto updateNpcModel= [this]()
    {
        auto const& npcs= m_campaign->npcModel()->npcList();
        if(!npcs.empty())
            FileSerializer::writeNpcIntoCampaign(m_campaign->rootDirectory(),
                                                 FileSerializer::npcToArray(npcs, m_campaign->rootDirectory()));
    };
    connect(npcModel, &NonPlayableCharacterModel::characterAdded, this, updateNpcModel);
    connect(npcModel, &NonPlayableCharacterModel::characterRemoved, this, updateNpcModel);
    connect(npcModel, &NonPlayableCharacterModel::dataChanged, this, updateNpcModel);
    connect(npcModel, &NonPlayableCharacterModel::modelReset, this, updateNpcModel);

    auto updateCampaign= [this]()
    { FileSerializer::writeCampaignInfo(m_campaign->rootDirectory(), FileSerializer::campaignToObject(m_campaign)); };

    connect(m_campaign, &Campaign::nameChanged, this, updateCampaign);
    connect(m_campaign, &Campaign::currentChapterChanged, this, updateCampaign);
    connect(m_campaign, &Campaign::mediaAdded, this, updateCampaign);
    connect(m_campaign, &Campaign::mediaNameChanged, this, updateCampaign);
    connect(m_campaign, &Campaign::mediaRemoved, this, updateCampaign);
}

NetWorkReceiver::SendType CampaignUpdater::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::CharactereStateModel && msg->category() == NetMsg::CampaignCategory && !m_localIsGm)
    {
        setUpdating(true);
        MessageHelper::fetchCharacterStatesFromNetwork(msg, m_campaign->stateModel());
        setUpdating(false);
    }
    else if(msg->action() == NetMsg::DiceAliasModel && msg->category() == NetMsg::CampaignCategory && !m_localIsGm)
    {
        MessageHelper::fetchDiceAliasFromNetwork(msg, m_dice->aliases());
    }
    return NetWorkReceiver::NONE;
}

DiceRoller* CampaignUpdater::diceParser() const
{
    return m_dice;
}

bool CampaignUpdater::localIsGM()
{
    return m_localIsGm;
}

void CampaignUpdater::updateDiceModel()
{
    MessageHelper::sendOffAllDiceAlias(m_campaign->diceAliases());
}

void CampaignUpdater::updateStateModel()
{
    MessageHelper::sendOffAllCharacterState(m_campaign->stateModel());
}

void CampaignUpdater::saveCampaignTo(const QString& dir)
{
    createCampaignTemplate(dir);
    saveDataInto(dir);
}

void CampaignUpdater::save()
{
    saveDataInto(m_campaign->rootDirectory());
}

void CampaignUpdater::saveDataInto(const QString& path)
{
    // Dice Aliases
    const auto& newAliases= m_campaign->diceAliases()->aliases();
    FileSerializer::writeDiceAliasIntoCampaign(path, FileSerializer::dicesToArray(newAliases));

    // States
    auto const& states= m_campaign->stateModel()->statesList();
    if(!states.empty())
        FileSerializer::writeStatesIntoCampaign(path, FileSerializer::statesToArray(states, path));

    // NPC

    auto const& npcs= m_campaign->npcModel()->npcList();
    if(!npcs.empty())
        FileSerializer::writeNpcIntoCampaign(path, FileSerializer::npcToArray(npcs, m_campaign->rootDirectory()));

    // Campaign Info
    FileSerializer::writeCampaignInfo(path, FileSerializer::campaignToObject(m_campaign));
}

void CampaignUpdater::setLocalIsGM(bool b)
{
    if(b == m_localIsGm)
        return;
    m_localIsGm= b;
    emit localIsGMChanged();
}

bool CampaignUpdater::createCampaignTemplate(const QString& dirPath)
{
    auto dir= QDir(dirPath);

    if(dir.exists() && !dir.isEmpty())
    {
        qCInfo(logCategory::campaign) << tr("'%1' is not empty").arg(dirPath);
        return false;
    }
    else if(!dir.exists())
    {
        auto parentDir= dir;
        parentDir.cdUp();
        if(!parentDir.mkdir(dirPath))
        {
            qCInfo(logCategory::campaign) << tr("Could not create '%1'").arg(dirPath);
            return false;
        }
        FileSerializer::createCampaignDirectory(dirPath);
    }
    else
    {
        FileSerializer::createCampaignDirectory(dirPath);
    }
    return true;
}

void CampaignUpdater::setUpdating(bool b)
{
    m_updatingModel= b;
}

bool CampaignUpdater::canForward()
{
    return !m_updatingModel && m_localIsGm;
}
} // namespace campaign
