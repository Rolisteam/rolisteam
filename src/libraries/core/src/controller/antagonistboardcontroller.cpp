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
#include "controller/antagonistboardcontroller.h"

#include <data/campaign.h>
#include <data/campaigneditor.h>
#include <data/character.h>
#include <worker/iohelper.h>

namespace campaign
{

template <typename T>
QList<CharacterField*> convertToGeneric(QList<T> list)
{
    QList<CharacterField*> res;

    std::transform(std::begin(list), std::end(list), std::back_inserter(res),
                   [](T type)
                   {
                       CharacterField* tmp= type;
                       return tmp;
                   });

    return res;
}

template <typename T>
QList<T> convertToSpecific(QList<CharacterField*> list)
{
    QList<T> res;

    std::transform(std::begin(list), std::end(list), std::back_inserter(res),
                   [](CharacterField* type) { return dynamic_cast<T>(type); });

    return res;
}

AntagonistBoardController::AntagonistBoardController(campaign::CampaignEditor* editor, QObject* parent)
    : QObject(parent)
    , m_editor(editor)
    , m_models{{new GenericModel({QStringLiteral("Name"), QStringLiteral("Command")}, {}, this),
                new GenericModel({QStringLiteral("Name"), QStringLiteral("Uri")}, {1}, this),
                new GenericModel({QStringLiteral("Name"), QStringLiteral("Value")}, {}, this)}}
    , m_filteredModel(new FilteredCharacterModel)
    , m_colorModel(new ColorModel)
    , m_stateModel(new StateModel)
{
    if(m_editor)
    {
        auto campaign= m_editor->campaign();
        m_filteredModel->setSourceModel(campaign->npcModel());
        m_colorModel->setSourceModel(campaign->npcModel());
        m_stateModel->setSourceModel(campaign->npcModel());
        connect(
            campaign->npcModel(), &campaign::NonPlayableCharacterModel::modelReset, m_colorModel.get(),
            [this]()
            {
                auto campaign= m_editor->campaign();
                if(!campaign)
                    return;

                m_colorModel->setSourceModel(campaign->npcModel());
                m_stateModel->setSourceModel(campaign->npcModel());
            },
            Qt::QueuedConnection);

        connect(m_filteredModel.get(), &FilteredCharacterModel::searchChanged, this,
                &AntagonistBoardController::searchTextChanged);
        connect(campaign, &campaign::Campaign::stateModelChanged, this,
                &AntagonistBoardController::characterStateModelChanged);
        connect(campaign, &campaign::Campaign::stateModelChanged, this, &AntagonistBoardController::stateModelChanged);
    }
}

QString AntagonistBoardController::searchText() const
{
    return m_filteredModel->search();
}

FilteredCharacterModel* AntagonistBoardController::filteredModel() const
{
    return m_filteredModel.get();
}

GenericModel* AntagonistBoardController::actionModel() const
{
    return m_models[static_cast<int>(ModelType::Model_Action)];
}

GenericModel* AntagonistBoardController::shapeModel() const
{
    return m_models[static_cast<int>(ModelType::Model_Shape)];
}

GenericModel* AntagonistBoardController::propertyModel() const
{
    return m_models[static_cast<int>(ModelType::Model_Properties)];
}

bool AntagonistBoardController::editingCharacter() const
{
    return !m_character.isNull();
}
NonPlayableCharacter* AntagonistBoardController::character() const
{
    return m_character;
}

void AntagonistBoardController::removeData(const QModelIndex& index, campaign::ModelType type)
{
    if(!index.isValid())
        return;

    m_models[static_cast<int>(type)]->removeData(index);
}

void AntagonistBoardController::appendData(ModelType type)
{
    switch(type)
    {
    case ModelType::Model_Action:
        m_models[static_cast<int>(type)]->addData(new CharacterAction);
        break;
    case ModelType::Model_Properties:
        m_models[static_cast<int>(type)]->addData(new CharacterProperty);
        break;
    case ModelType::Model_Shape:
        m_models[static_cast<int>(type)]->addData(new CharacterShape);
        break;
    }
}

void AntagonistBoardController::editCharacter(const QString& id)
{
    auto campaign= m_editor->campaign();
    auto const& list= campaign->npcModel()->npcList();
    auto it= std::find_if(std::begin(list), std::end(list),
                          [id](const std::unique_ptr<NonPlayableCharacter>& npc) { return npc->uuid() == id; });

    if(it == std::end(list))
        return;

    setCharacter((*it).get());
}

void AntagonistBoardController::setCharacter(NonPlayableCharacter* character)
{
    if(character == m_character)
        return;
    m_character= character;

    if(m_character)
    {
        auto actMo= actionModel();
        actMo->resetData(convertToGeneric<CharacterAction*>(character->actionList()));

        auto pros= propertyModel();
        pros->resetData(convertToGeneric<CharacterProperty*>(character->propertiesList()));

        auto shapes= shapeModel();
        shapes->resetData(convertToGeneric<CharacterShape*>(character->shapeList()));
    }

    emit characterChanged();
}

void AntagonistBoardController::saveToken()
{
    if(!m_character)
        return;

    m_character->defineActionList(convertToSpecific<CharacterAction*>(actionModel()->fields()));
    m_character->definePropertiesList(convertToSpecific<CharacterProperty*>(propertyModel()->fields()));
    m_character->defineShapeList(convertToSpecific<CharacterShape*>(shapeModel()->fields()));

    auto model= m_editor->campaign()->npcModel();
    model->refresh(m_character->uuid());

    setCharacter(nullptr);
}

void AntagonistBoardController::addCharacter()
{
    m_editor->campaign()->addCharacter();
}

void AntagonistBoardController::changeImage(const QString& id, const QByteArray& data)
{
    auto campaign= m_editor->campaign();
    auto const& model= campaign->npcModel();
    auto index= model->indexFromUuid(id);
    auto path= m_editor->saveAvatar(id, data);
    model->setData(index, data, NonPlayableCharacterModel::RoleAvatar);
    model->setData(index, path, NonPlayableCharacterModel::RoleAvatarPath);
}

QString AntagonistBoardController::imageFolder() const
{
    return m_editor->campaign()->directory(campaign::Campaign::Place::MEDIA_ROOT);
}

void AntagonistBoardController::removeCharacter(const QString& id)
{
    m_editor->campaign()->removeCharacter(id);
}

void AntagonistBoardController::setSearchText(const QString& text)
{
    m_filteredModel->setSearch(text);
}

FilteredCharacterModel::Definition FilteredCharacterModel::gmdetailsDef() const
{
    return m_gmdetailsDef;
}

void FilteredCharacterModel::setGmdetailsDef(FilteredCharacterModel::Definition newGmdetailsDef)
{
    if(m_gmdetailsDef == newGmdetailsDef)
        return;
    m_gmdetailsDef= newGmdetailsDef;
    emit gmdetailsDefChanged();
}

const QString& FilteredCharacterModel::gmdetails() const
{
    return m_gmdetails;
}

void FilteredCharacterModel::setGmdetails(const QString& newGmdetails)
{
    if(m_gmdetails == newGmdetails)
        return;
    m_gmdetails= newGmdetails;
    emit gmdetailsChanged();
}

FilteredCharacterModel::HealthState FilteredCharacterModel::hlState() const
{
    return m_hlState;
}

void FilteredCharacterModel::setHlState(FilteredCharacterModel::HealthState newHlState)
{
    if(m_hlState == newHlState)
        return;
    m_hlState= newHlState;
    emit hlStateChanged();
}

ColorModel* AntagonistBoardController::colorModel() const
{
    return m_colorModel.get();
}

StateModel* AntagonistBoardController::stateModel() const
{
    return m_stateModel.get();
}

CharacterStateModel* AntagonistBoardController::characterStateModel() const
{
    if(!m_editor)
        return nullptr;

    auto camp= m_editor->campaign();
    if(!camp)
        return nullptr;

    return camp->stateModel();
}

} // namespace campaign
