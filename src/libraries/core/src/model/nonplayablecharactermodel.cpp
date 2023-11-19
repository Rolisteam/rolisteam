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
#include "model/nonplayablecharactermodel.h"
#include "worker/iohelper.h"
namespace campaign
{
NonPlayableCharacter::NonPlayableCharacter(QObject* parent) : Character(parent)
{
    setNpc(true);
}

QStringList NonPlayableCharacter::tags() const
{
    return m_tags;
}

QString NonPlayableCharacter::avatarPath() const
{
    return m_avatarPath;
}

int NonPlayableCharacter::size() const
{
    return m_size;
}

QString NonPlayableCharacter::description() const
{
    return m_description;
}

void NonPlayableCharacter::setTags(const QStringList& list)
{
    if(list == m_tags)
        return;
    m_tags= list;
    emit tagsChanged();
}

void NonPlayableCharacter::setAvatarPath(const QString& path)
{
    if(path == m_avatarPath)
        return;
    m_avatarPath= path;
    emit avatarPathChanged();
}

void NonPlayableCharacter::setSize(int size)
{
    if(size == m_size)
        return;
    m_size= size;
    emit sizeChanged();
}

void NonPlayableCharacter::setDescription(const QString& desc)
{
    if(desc == m_description)
        return;
    m_description= desc;
    emit descriptionChanged();
}

NonPlayableCharacterModel::NonPlayableCharacterModel(CharacterStateModel* states, QObject* parent)
    : QAbstractListModel(parent), m_states(states)
{
    m_header << tr("Avatar") << tr("Name") << tr("Description") << tr("GM Details") << tr("Tags") << tr("Color")
             << tr("Min HP") << tr("Current HP") << tr("Max HP") << tr("Initiative") << tr("Distance Per turn")
             << tr("State") << tr("Life Color") << tr("Init Command");
}

QVariant NonPlayableCharacterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return {};

    if(Qt::DisplayRole == role)
    {
        return m_header[section];
    }

    return {};
}

int NonPlayableCharacterModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.size();
}

int NonPlayableCharacterModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}

QVariant NonPlayableCharacterModel::data(const QModelIndex& index, int role) const
{
    static QSet<int> acceptedRole({Qt::DecorationRole,
                                   Qt::DisplayRole,
                                   Qt::TextAlignmentRole,
                                   Qt::BackgroundRole,
                                   Qt::EditRole,
                                   RoleUuid,
                                   RoleAvatar,
                                   RoleName,
                                   RoleDescription,
                                   RoleGmDetails,
                                   RoleTags,
                                   RoleColor,
                                   RoleMinHp,
                                   RoleCurrentHp,
                                   RoleMaxHp,
                                   RoleInitiative,
                                   RoleDistancePerTurn,
                                   RoleState,
                                   RoleLifeColor,
                                   RoleInitCommand,
                                   RoleUuid,
                                   RoleAvatarPath,
                                   RoleHasInitiative,
                                   RoleActionCount,
                                   RoleShapeCount,
                                   RolePropertiesCount});
    if(!index.isValid() || !acceptedRole.contains(role))
        return QVariant();

    if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    auto customRole= role > Qt::UserRole ? role : Qt::UserRole + 1 + index.column();

    if(role == Qt::BackgroundRole && (customRole != RoleColor && customRole != RoleLifeColor))
        return {};

    if(role == Qt::DecorationRole && (customRole != RoleAvatar))
        return {};

    auto const& character= m_data[index.row()];
    QVariant res;
    switch(customRole)
    {
    case RoleAvatar:
        if(role == Qt::EditRole)
            res= QString{};
        else
            res= QVariant::fromValue(IOHelper::dataToPixmap(character->avatar()));
        break;
    case RoleUuid:
        res= character->uuid();
        break;
    case RoleName:
        res= character->name();
        break;
    case RoleDescription:
        res= character->description();
        break;
    case RoleColor:
        res= character->getColor();
        break;
    case RoleMinHp:
        res= character->getHealthPointsMin();
        break;
    case RoleCurrentHp:
        res= character->getHealthPointsCurrent();
        break;
    case RoleMaxHp:
        res= character->getHealthPointsMax();
        break;
    case RoleInitiative:
        res= character->getInitiativeScore();
        break;
    case RoleDistancePerTurn:
        res= character->getDistancePerTurn();
        break;
    case RoleState:
        if(role == Qt::DisplayRole)
            res= stateName(character->stateId());
        else if(role == Qt::EditRole || role == RoleState)
            res= character->stateId();
        break;
    case RoleLifeColor:
        res= character->getLifeColor();
        break;
    case RoleInitCommand:
        res= character->initCommand();
        break;
    case RoleTags:
        if(role == Qt::DisplayRole || role == RoleTags)
            res= character->tags();
        else if(role == Qt::EditRole)
            res= character->tags().join(";");
        break;
    case RoleAvatarPath:
        res= character->avatarPath();
        break;
    case RoleHasInitiative:
        res= character->hasInitScore();
        break;
    case RoleActionCount:
        res= character->actionList().size();
        break;
    case RoleShapeCount:
        res= character->shapeList().size();
        break;
    case RolePropertiesCount:
        res= character->propertiesList().size();
        break;
    case RoleGmDetails:
        res= character->gameMasterDesc();
        break;
    }

    return res;
}

bool NonPlayableCharacterModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    auto customRole= Qt::UserRole + 1 + index.column();
    if(role > Qt::UserRole)
        customRole= role;

    if(data(index, role) != value)
    {

        bool res= true;
        auto const& character= m_data[index.row()];
        switch(customRole)
        {
        case RoleName:
            character->setName(value.toString());
            break;
        case RoleDescription:
            character->setDescription(value.toString());
            break;
        case RoleColor:
            character->setColor(value.value<QColor>());
            break;
        case RoleMinHp:
            character->setHealthPointsMin(value.toInt());
            break;
        case RoleCurrentHp:
            character->setHealthPointsCurrent(value.toInt());
            break;
        case RoleMaxHp:
            character->setHealthPointsMax(value.toInt());
            break;
        case RoleInitiative:
            character->setInitiativeScore(value.toInt());
            break;
        case RoleDistancePerTurn:
            character->setDistancePerTurn(value.toInt());
            break;
        case RoleState:
            character->setStateId(value.toString());
            break;
        case RoleLifeColor:
            character->setLifeColor(value.value<QColor>());
            break;
        case RoleInitCommand:
            character->setInitCommand(value.toString());
            break;
        case RoleTags:
            character->setTags(value.toString().split(';'));
            break;
        case RoleAvatar:
            if(value.typeId() == qMetaTypeId<QByteArray>())
            {
                character->setAvatar(value.toByteArray());
            }
            else if(value.typeId() == qMetaTypeId<QString>())
            {
                auto str= value.toString();
                if(str.isEmpty())
                    return false;
                auto path= IOHelper::copyImageFileIntoCampaign(str, m_npcRoot);
                character->setAvatarPath(path);
                character->setAvatar(IOHelper::imageToData(IOHelper::readImageFromURL(QUrl::fromUserInput(path))));
            }
            else if(value.typeId() == qMetaTypeId<QUrl>())
            {
                character->setAvatar(IOHelper::imageToData(IOHelper::readImageFromURL(value.toUrl())));
            }
            break;
        case RoleAvatarPath:
            character->setAvatarPath(value.toString());
            break;
        default:
            res= false;
            break;
        }
        if(res)
            emit dataChanged(index, index, QVector<int>() << role);
        return res;
    }
    return false;
}

QString NonPlayableCharacterModel::stateName(const QString& id) const
{
    if(!m_states)
        return id;

    return m_states->stateLabelFromId(id);
}

Qt::ItemFlags NonPlayableCharacterModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    if(index.column() == NonPlayableCharacterModel::ColAvatar)
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsDragEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void NonPlayableCharacterModel::append()
{
    addCharacter(new NonPlayableCharacter);
}

void NonPlayableCharacterModel::addCharacter(NonPlayableCharacter* character)
{
    auto size= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), size, size);
    std::unique_ptr<NonPlayableCharacter> uni{character};
    m_data.push_back(std::move(uni));
    endInsertRows();
    emit characterAdded();
}

void NonPlayableCharacterModel::setModelData(const std::vector<NonPlayableCharacter*>& data)
{
    beginResetModel();
    m_data.clear();
    std::for_each(std::begin(data), std::end(data),
                  [this](NonPlayableCharacter* npc)
                  {
                      std::unique_ptr<NonPlayableCharacter> unique(npc);
                      m_data.push_back(std::move(unique));
                  });
    endResetModel();
}

void NonPlayableCharacterModel::removeNpc(const QString& uuid)
{
    auto index= indexFromUuid(uuid);
    if(!index.isValid())
        return;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    auto const& it= m_data.begin() + index.row();
    m_data.erase(it);
    endRemoveRows();

    emit characterRemoved(uuid);
}

void NonPlayableCharacterModel::refresh(const QString& uuid)
{
    auto index= indexFromUuid(uuid);
    if(!index.isValid())
        return;

    emit dataChanged(index, index);
}

QModelIndex NonPlayableCharacterModel::indexFromUuid(const QString& id)
{
    auto it= std::find_if(std::begin(m_data), std::end(m_data),
                          [id](const std::unique_ptr<NonPlayableCharacter>& character)
                          { return character->uuid() == id; });

    if(it == std::end(m_data))
        return {};
    else
        return index(std::distance(std::begin(m_data), it), 0);
}

NonPlayableCharacter* NonPlayableCharacterModel::characterFromUuid(const QString& id)
{
    auto it= std::find_if(std::begin(m_data), std::end(m_data),
                          [id](const std::unique_ptr<NonPlayableCharacter>& character)
                          { return character->uuid() == id; });

    if(it != std::end(m_data))
        return it->get();

    return nullptr;
}

const std::vector<std::unique_ptr<NonPlayableCharacter>>& NonPlayableCharacterModel::npcList() const
{
    return m_data;
}

QStringList NonPlayableCharacterModel::headers()
{
    return m_header;
}

const QString& NonPlayableCharacterModel::npcRoot() const
{
    return m_npcRoot;
}

void NonPlayableCharacterModel::setNpcRoot(const QString& newNpcRoot)
{
    if(m_npcRoot == newNpcRoot)
        return;
    m_npcRoot= newNpcRoot;
    emit npcRootChanged();
}

const QString& NonPlayableCharacter::gameMasterDesc() const
{
    return m_gameMasterDesc;
}

void NonPlayableCharacter::setGameMasterDesc(const QString& newGameMasterDesc)
{
    if(m_gameMasterDesc == newGameMasterDesc)
        return;
    m_gameMasterDesc= newGameMasterDesc;
    emit gameMasterDescChanged();
}

} // namespace campaign
