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

template <typename T>
bool isNull(const T& value);

template <>
bool isNull(const QPixmap& value)
{
    return value.isNull();
}
template <>
bool isNull(const QString& value)
{
    return value.isEmpty();
}

template <>
bool isNull(const int& value)
{
    return value == 0;
}

template <>
bool isNull(const bool& value)
{
    return !value;
}

template <typename T>
bool isAccepted(FilteredCharacterModel::Definition def, T value)
{
    if(def == FilteredCharacterModel::All)
    {
        return true;
    }
    else if(def == FilteredCharacterModel::With)
    {
        return !isNull(value);
    }
    else
    {
        return isNull(value);
    }
}

FilteredCharacterModel::FilteredCharacterModel()
{
    setDynamicSortFilter(true);

    connect(this, &FilteredCharacterModel::searchChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::initiativeCmdDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::propertiesDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::shapeDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::actionDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::initiativeScoreDefChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::avatarDefinitionChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::advancedChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::excludeChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::tagsChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::excludeTagsChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::hlStateChanged, this, &FilteredCharacterModel::invalidateFilter);
    connect(this, &FilteredCharacterModel::characterStateIdChanged, this, &FilteredCharacterModel::invalidateFilter);
}

FilteredCharacterModel::Definition FilteredCharacterModel::initiativeCmdDef() const
{
    return m_initiativeCmdDef;
}

void FilteredCharacterModel::setInitiativeCmdDef(FilteredCharacterModel::Definition newInitiativeCmdDef)
{
    if(m_initiativeCmdDef == newInitiativeCmdDef)
        return;
    m_initiativeCmdDef= newInitiativeCmdDef;
    emit initiativeCmdDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::propertiesDef() const
{
    return m_propertiesDef;
}

void FilteredCharacterModel::setPropertiesDef(Definition newPropertiesDef)
{
    if(m_propertiesDef == newPropertiesDef)
        return;
    m_propertiesDef= newPropertiesDef;
    emit propertiesDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::shapeDef() const
{
    return m_shapeDef;
}

void FilteredCharacterModel::setShapeDef(FilteredCharacterModel::Definition newShapeDef)
{
    if(m_shapeDef == newShapeDef)
        return;
    m_shapeDef= newShapeDef;
    emit shapeDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::actionDef() const
{
    return m_actionDef;
}

void FilteredCharacterModel::setActionDef(FilteredCharacterModel::Definition newActionDef)
{
    if(m_actionDef == newActionDef)
        return;
    m_actionDef= newActionDef;
    emit actionDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::initiativeScoreDef() const
{
    return m_initiativeScoreDef;
}

void FilteredCharacterModel::setInitiativeScoreDef(FilteredCharacterModel::Definition newInitiativeScoreDef)
{
    if(m_initiativeScoreDef == newInitiativeScoreDef)
        return;
    m_initiativeScoreDef= newInitiativeScoreDef;
    emit initiativeScoreDefChanged();
}

FilteredCharacterModel::Definition FilteredCharacterModel::avatarDefinition() const
{
    return m_avatarDefinition;
}

void FilteredCharacterModel::setAvatarDefinition(FilteredCharacterModel::Definition newAvatarDefinition)
{
    if(m_avatarDefinition == newAvatarDefinition)
        return;
    m_avatarDefinition= newAvatarDefinition;
    emit avatarDefinitionChanged();
}

bool FilteredCharacterModel::advanced() const
{
    return m_advanced;
}

void FilteredCharacterModel::setAdvanced(bool newAdvanced)
{
    if(m_advanced == newAdvanced)
        return;
    m_advanced= newAdvanced;
    emit advancedChanged();
}

const QString& FilteredCharacterModel::exclude() const
{
    return m_exclude;
}

void FilteredCharacterModel::setExclude(const QString& newExclude)
{
    if(m_exclude == newExclude)
        return;
    m_exclude= newExclude;
    emit excludeChanged();
}

const QString& FilteredCharacterModel::tags() const
{
    return m_tags;
}

void FilteredCharacterModel::setTags(const QString& newTags)
{
    if(m_tags == newTags)
        return;
    m_tags= newTags;
    emit tagsChanged();
}

const QString& FilteredCharacterModel::excludeTags() const
{
    return m_excludeTags;
}

void FilteredCharacterModel::setExcludeTags(const QString& newExcludeTags)
{
    if(m_excludeTags == newExcludeTags)
        return;
    m_excludeTags= newExcludeTags;
    emit excludeTagsChanged();
}

QString FilteredCharacterModel::search() const
{
    return m_search;
}

void FilteredCharacterModel::setSearch(const QString& search)
{
    if(search == m_search)
        return;
    m_search= search;
    emit searchChanged();
}

bool FilteredCharacterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    using nm= NonPlayableCharacterModel;
    auto nameIndex= sourceModel()->index(source_row, 0, source_parent);
    auto name= sourceModel()->data(nameIndex, nm::RoleName).toString();
    auto tagList= sourceModel()->data(nameIndex, nm::RoleTags).toStringList();

    bool accepted= true;
    if(advanced())
    {
        auto gmdetails= sourceModel()->data(nameIndex, nm::RoleGmDetails).toString();
        // clang-format off
        auto avatar= isAccepted<QPixmap>(m_avatarDefinition, sourceModel()->data(nameIndex, nm::RoleAvatar).value<QPixmap>());
        auto initScore= isAccepted<bool>(m_initiativeScoreDef, sourceModel()->data(nameIndex, nm::RoleHasInitiative).toBool());
        auto initCmd= isAccepted<QString>(m_initiativeCmdDef, sourceModel()->data(nameIndex, nm::RoleInitCommand).toString());
        auto actions= isAccepted<int>(m_actionDef, sourceModel()->data(nameIndex, nm::RoleActionCount).toInt());
        auto properties= isAccepted<int>(m_propertiesDef, sourceModel()->data(nameIndex, nm::RolePropertiesCount).toInt());
        auto shapes= isAccepted<int>(m_shapeDef, sourceModel()->data(nameIndex, nm::RoleShapeCount).toInt());
        auto details= isAccepted<QString>(m_gmdetailsDef, gmdetails);
        auto characterState= m_characterStateId.isEmpty() ? true : (m_characterStateId == sourceModel()->data(nameIndex, nm::RoleState).toString());

        auto current = sourceModel()->data(nameIndex, nm::RoleCurrentHp).toInt();
        auto min = sourceModel()->data(nameIndex, nm::RoleMinHp).toInt();
        auto max = sourceModel()->data(nameIndex, nm::RoleMaxHp).toInt();
        auto health = m_hlState == FilteredCharacterModel::HS_All ? true : m_hlState == FilteredCharacterModel::HS_Full ? current == max : m_hlState == FilteredCharacterModel::HS_Dead ? min == current :  (current > min && current < max);
        // clang-format on

        auto nameval= m_search.isEmpty() ? true : name.contains(m_search, Qt::CaseInsensitive);
        auto gmDetails= m_gmdetails.isEmpty() ? true : gmdetails.contains(m_gmdetails, Qt::CaseInsensitive);
        auto exclude= !(m_exclude.isEmpty() ? false : name.contains(m_exclude, Qt::CaseInsensitive));
        auto tags= m_tags.isEmpty() ? true : tagList.join(QString()).contains(m_tags, Qt::CaseInsensitive);
        auto excludeTags
            = !(m_excludeTags.isEmpty() ? false : tagList.join(QString()).contains(m_excludeTags, Qt::CaseInsensitive));

        accepted= avatar & initScore & actions & properties & shapes & details & nameval & exclude & tags & excludeTags
                  & initCmd & health & gmDetails & characterState;
    }
    else
    {
        auto nameVal= m_search.isEmpty() ? true : name.contains(m_search, Qt::CaseInsensitive);
        auto tags= m_search.isEmpty() ? true : tagList.join(QString()).contains(m_search, Qt::CaseInsensitive);
        accepted= (nameVal || tags);
    }

    return accepted;
}

QString FilteredCharacterModel::characterStateId() const
{
    return m_characterStateId;
}

void FilteredCharacterModel::setCharacterStateId(const QString& newCharacterStateId)
{
    if(m_characterStateId == newCharacterStateId)
        return;
    m_characterStateId= newCharacterStateId;
    emit characterStateIdChanged();
}

bool FilteredCharacterModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    auto data1= source_left.data().toString();
    auto data2= source_right.data().toString();

    return data1 < data2;
}

AntagonistBoardController::AntagonistBoardController(campaign::CampaignEditor* editor, QObject* parent)
    : QObject(parent)
    , m_editor(editor)
    , m_models{{new GenericModel({QStringLiteral("Name"), QStringLiteral("Command")}, {}, this),
                new GenericModel({QStringLiteral("Name"), QStringLiteral("Uri")}, {1}, this),
                new GenericModel({QStringLiteral("Name"), QStringLiteral("Value")}, {}, this)}}
    , m_filteredModel(new FilteredCharacterModel)
    , m_colorModel(new ColorModel)
{
    if(m_editor)
    {
        auto campaign= m_editor->campaign();
        m_filteredModel->setSourceModel(campaign->npcModel());
        m_colorModel->setSourceModel(campaign->npcModel());
        connect(
            campaign->npcModel(), &campaign::NonPlayableCharacterModel::modelReset, m_colorModel.get(),
            [this]()
            {
                auto campaign= m_editor->campaign();
                if(!campaign)
                    return;

                m_colorModel->setSourceModel(campaign->npcModel());
            },
            Qt::QueuedConnection);

        connect(m_filteredModel.get(), &FilteredCharacterModel::searchChanged, this,
                &AntagonistBoardController::searchTextChanged);
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

CharacterStateModel* AntagonistBoardController::stateModel() const
{
    if(!m_editor)
        return nullptr;

    auto camp= m_editor->campaign();
    if(!camp)
        return nullptr;

    return camp->stateModel();
}

} // namespace campaign
