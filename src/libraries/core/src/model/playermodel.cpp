/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#include <QDebug>
#include <QLoggingCategory>
#include <QPalette>

#include "model/playermodel.h"

#include "data/character.h"
#include "data/person.h"
#include "data/player.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"
#include "utils/iohelper.h"
#include "worker/iohelper.h"

QLoggingCategory rUser("rolisteam.user");

template <typename T>
void convertVariantToType(const T& val, NetworkMessageWriter& msg)
{
    msg.string32(val);
}

template <>
void convertVariantToType<QColor>(const QColor& val, NetworkMessageWriter& msg)
{
    msg.rgb(val.rgb());
}

template <>
void convertVariantToType<QImage>(const QImage& val, NetworkMessageWriter& msg)
{
    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_7);
    in << val;
    msg.byteArray32(data);
}

template <>
void convertVariantToType<CharacterState*>(CharacterState* const& val, NetworkMessageWriter& msg)
{
    if(val)
        msg.string32(val->label());
    else
    {
        msg.string32(QStringLiteral(""));
    }
}

Player* findPerson(const std::vector<std::unique_ptr<Player>>& data, Person* person)
{
    auto it= std::find_if(data.begin(), data.end(),
                          [person](const std::unique_ptr<Player>& player)
                          {
                              bool val= ((player.get() == person) || (person->uuid() == player->uuid()));

                              if(!val)
                              {
                                  const auto& children= player->children();
                                  auto subIt= std::find_if(children.begin(), children.end(),
                                                           [person](const std::unique_ptr<Character>& character) {
                                                               return (character.get() == person)
                                                                      || (person->uuid() == character->uuid());
                                                           });
                                  val= (subIt != children.end());
                              }
                              return val;
                          });
    return it->get();
}

bool contains(const std::vector<std::unique_ptr<Player>>& data, Person* person)
{
    auto it= std::find_if(data.begin(), data.end(),
                          [person](const std::unique_ptr<Player>& player)
                          {
                              bool val= ((player.get() == person) || (person->uuid() == player->uuid()));

                              if(!val)
                              {
                                  const auto& children= player->children();
                                  auto subIt= std::find_if(children.begin(), children.end(),
                                                           [person](const std::unique_ptr<Character>& character) {
                                                               return (character.get() == person)
                                                                      || (person->uuid() == character->uuid());
                                                           });
                                  val= (subIt != children.end());
                              }
                              return val;
                          });
    return it == data.end();
}

bool isLocal(Person* person, const QString& uuid)
{
    if(!person)
        return false;

    if(person->isLeaf())
        person= person->parentPerson();

    if(!person)
        return false;

    return (uuid == person->uuid());
}

PlayerModel::PlayerModel(QObject* parent) : QAbstractItemModel(parent) {}

PlayerModel::~PlayerModel()= default;

/*************
 * ItemModel *
 *************/

QVariant PlayerModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || index.column() != 0)
        return QVariant();

    Person* person= static_cast<Person*>(index.internalPointer());

    if(!person)
        return {};

    Character* character= dynamic_cast<Character*>(person);
    Player* player= dynamic_cast<Player*>(person);
    bool isGM= false;
    bool isCharacter= (character != nullptr);
    bool isNPC= false;

    if(isCharacter)
        isNPC= character->isNpc();

    if((player != nullptr))
    {
        isGM= player->isGM();
    }
    if(isGM && (role == Qt::BackgroundRole))
    {
        QPalette pal;
        return QVariant(pal.color(QPalette::Active, QPalette::Dark));
    }
    QVariant var;
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        auto name= person->name();
        if((nullptr != character) && (character->hasInitScore()))
        {
            name= QStringLiteral("%1 (%2)").arg(name).arg(character->getInitiativeScore());
        }
        var= name;
        break;
    }
    case Qt::ToolTipRole:
    {
        if(nullptr != character)
        {
            QString stateName(tr("Not defined"));
            stateName= character->stateId();

            var= tr("%1:\n"
                    "HP: %2/%3\n"
                    "State: %4\n"
                    "Initiative Score: %5\n"
                    "Distance Per Turn: %6\n"
                    "type: %7\n")
                     .arg(character->name())
                     .arg(character->getHealthPointsCurrent())
                     .arg(character->getHealthPointsMax())
                     .arg(stateName)
                     .arg(character->getInitiativeScore())
                     .arg(character->getDistancePerTurn())
                     .arg(character->isNpc() ? tr("NPC") : tr("PC"));
        }
        break;
    }
    case PlayerModel::ColorRole:
    case Qt::DecorationRole:
    {
        if(!person->avatar().isNull()) // (person->isLeaf()) &&
        {
            var= utils::IOHelper::dataToImage(person->avatar())
                     .scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else
            var= QVariant(person->getColor());
        break;
    }
    case PlayerModel::IdentifierRole:
        var= person->uuid();
        break;
    case PlayerModel::PersonPtrRole:
        var= QVariant::fromValue(person);
        break;
    case PlayerModel::CharacterRole:
        var= person->isLeaf();
        break;
    case PlayerModel::NpcRole:
        var= isNPC;
        break;
    case PlayerModel::NameRole:
        var= person->name();
        break;
    case PlayerModel::CharacterStateIdRole:
    {
        if(nullptr != character)
            var= character->stateId();
    }
    break;
    case PlayerModel::LocalRole:
        var= isLocal(person, m_localPlayerId);
        break;
    case PlayerModel::GmRole:
        var= isGM;
        break;
    case PlayerModel::AvatarRole:
        var= utils::IOHelper::dataToImage(person->avatar());
        break;
    }

    return var;
}

Qt::ItemFlags PlayerModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    if(index.row() == 0 || gameMasterId() == localPlayerId())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PlayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant(tr("Players List"));
    return QVariant();
}

QModelIndex PlayerModel::index(int row, int column, const QModelIndex& parent) const
{
    if(column != 0 || row < 0)
        return QModelIndex();
    Person* childItem= nullptr;
    auto row_t= static_cast<std::size_t>(row);
    if(parent.isValid())
    {
        auto parentPerson= static_cast<Person*>(parent.internalPointer());
        auto parentPlayer= dynamic_cast<Player*>(parentPerson);
        if(nullptr == parentPlayer)
            return {};
        childItem= parentPlayer->getCharacterByIndex(row);
    }
    else if(m_players.size() > row_t)
    {
        childItem= m_players[row_t].get();
    }

    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex PlayerModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto childItem= static_cast<Person*>(index.internalPointer());
    auto parentPerson= childItem->parentPerson();

    if(nullptr == parentPerson)
    {
        return QModelIndex();
    }
    auto it
        = std::find_if(m_players.begin(), m_players.end(),
                       [parentPerson](const std::unique_ptr<Player>& person) { return parentPerson == person.get(); });

    return createIndex(static_cast<int>(std::distance(m_players.begin(), it)), 0, parentPerson);
}

int PlayerModel::rowCount(const QModelIndex& parent) const
{
    int result= 0;
    if(!parent.isValid())
    {
        result= static_cast<int>(m_players.size());
    }
    else
    {
        auto parentItem= static_cast<Person*>(parent.internalPointer());
        auto player= dynamic_cast<Player*>(parentItem);
        if(player)
        {
            result= player->characterCount();
        }
    }
    return result;
}

int PlayerModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool PlayerModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool val= false;
    auto childItem= static_cast<Person*>(index.internalPointer());
    if(nullptr == childItem)
        return val;

    QVector<int> roles;
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case NameRole:
        childItem->setName(value.toString());
        roles << role << Qt::DisplayRole << Qt::EditRole << NameRole;
        val= true;
        break;
    case GmRole:
        break;
    case CharacterRole:
        break;
    case CharacterStateIdRole:
        break;
    case NpcRole:
        break;
    case AvatarRole:
        break;
    case Qt::DecorationRole:
    case PlayerModel::ColorRole:
        childItem->setColor(value.value<QColor>());
        val= true;
        roles << PlayerModel::ColorRole << Qt::DecorationRole;
        break;
    case LocalRole:
    case IdentifierRole:
    case PersonPtrRole:
    default:
        break;
    }
    if(val)
        emit dataChanged(index, index, roles);
    return val;
}

QModelIndex PlayerModel::personToIndex(Person* person) const
{
    QModelIndex parent;
    int row= -1;
    if(person->isLeaf())
    {
        auto player= dynamic_cast<Player*>(person->parentPerson());
        auto posIt= std::find_if(m_players.begin(), m_players.end(),
                                 [player](const std::unique_ptr<Player>& person) { return player == person.get(); });
        if(posIt != m_players.end())
        {
            parent= index(static_cast<int>(std::distance(m_players.begin(), posIt)), 0, QModelIndex());
            row= player->indexOf(dynamic_cast<Character*>(person));
        }
    }
    else
    {
        auto player= dynamic_cast<Player*>(person);
        auto posIt= std::find_if(m_players.begin(), m_players.end(),
                                 [player](const std::unique_ptr<Player>& person) { return player == person.get(); });
        if(posIt != m_players.end())
            row= static_cast<int>(std::distance(m_players.begin(), posIt));
    }
    return index(row, 0, parent);
}

QString PlayerModel::gameMasterId() const
{
    return m_gameMasterId;
}

QString PlayerModel::localPlayerId() const
{
    return m_localPlayerId;
}

QHash<int, QByteArray> PlayerModel::roleNames() const
{
    static QHash<int, QByteArray> role({{IdentifierRole, "uuid"},
                                        {PersonPtrRole, "person"},
                                        {NameRole, "name"},
                                        {LocalRole, "local"},
                                        {GmRole, "gm"},
                                        {CharacterRole, "character"},
                                        {CharacterStateIdRole, "characterState"},
                                        {NpcRole, "npc"},
                                        {AvatarRole, "avatar"}});

    return role;
}

Player* PlayerModel::playerById(const QString& id) const
{
    auto it= std::find_if(m_players.begin(), m_players.end(),
                          [id](const std::unique_ptr<Player>& player) { return id == player->uuid(); });
    if(it == m_players.end())
        return nullptr;
    return (*it).get();
}

Person* PlayerModel::personById(const QString& id) const
{
    auto player= playerById(id);

    if(nullptr != player)
        return player;

    const auto& it
        = std::find_if(m_players.begin(), m_players.end(),
                       [id](const std::unique_ptr<Player>& player) { return (nullptr != player->characterById(id)); });

    if(it != m_players.end())
        return it->get()->characterById(id);
    else
        return nullptr;
}

void PlayerModel::addPlayer(Player* player)
{
    if(nullptr == player)
        return;

    auto it= std::find_if(m_players.begin(), m_players.end(),
                          [player](const std::unique_ptr<Player>& t)
                          { return (t.get() == player || t->uuid() == player->uuid()); });

    if(it != m_players.end())
    {
        qWarning() << tr("Duplicated player or uuid") << player->name();
        return;
    }

    if(player->isGM())
    {
        setGameMasterId(player->uuid());
    }

    int size= static_cast<int>(m_players.size());

    beginInsertRows(QModelIndex(), size, size);
    m_players.push_back(std::unique_ptr<Player>(player));
    endInsertRows();

    qCInfo(rUser) << QString("Player %1 just arrived").arg(player->name());
    emit playerJoin(player);
}

void PlayerModel::addCharacter(const QModelIndex& parent, Character* character, int pos)
{
    if(!parent.isValid() || nullptr == character)
        return;

    int size= pos;
    if(size < 0)
        size= rowCount(parent.parent());

    auto person= static_cast<Person*>(parent.internalPointer());
    auto player= dynamic_cast<Player*>(person);

    character->setNpc(player->isGM());

    /* beginInsertRows(parent, size, size);
     player->addCharacter(character);
     endInsertRows();*/
}

void PlayerModel::removeCharacter(Character* character)
{
    if(nullptr == character)
        return;

    auto player= character->getParentPlayer();

    if(nullptr == player)
        return;

    auto parent= personToIndex(player);
    auto idx= player->indexOf(character);

    beginRemoveRows(parent, idx, idx);
    player->removeChild(character);
    endRemoveRows();
}

void PlayerModel::setLocalPlayerId(const QString& uuid)
{
    if(uuid == m_localPlayerId)
        return;
    m_localPlayerId= uuid;
    emit localPlayerIdChanged(m_localPlayerId);
}

void PlayerModel::removePlayer(Player* player)
{
    auto itPlayer= std::find_if(m_players.begin(), m_players.end(),
                                [player](const std::unique_ptr<Player>& person) { return person.get() == player; });
    if(itPlayer == m_players.end())
        return;

    //    int charactersCount= player->get->childrenCount();
    /*    for(int i= 0; i < charactersCount; i++)
        {
            delCharacter(player, 0);
        }*/

    auto index= static_cast<int>(std::distance(m_players.begin(), itPlayer));
    qCInfo(rUser) << QString("Player %s left").arg(player->name());

    emit playerLeft(player);

    if(player->isGM())
    {
        setGameMasterId("");
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_players.erase(itPlayer);
    endRemoveRows();
}

void PlayerModel::setGameMasterId(const QString& id)
{
    if(id == m_gameMasterId)
        return;
    m_gameMasterId= id;
    emit gameMasterIdChanged(m_gameMasterId);
}

void PlayerModel::clear()
{
    beginResetModel();
    m_players.clear();
    endResetModel();
    setGameMasterId("");
}
