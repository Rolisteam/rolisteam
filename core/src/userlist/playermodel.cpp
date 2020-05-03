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
#include <QPalette>

#include "userlist/playermodel.h"

#include "data/character.h"
#include "data/features.h"
#include "data/person.h"
#include "data/player.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"

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
        msg.string32(val->getLabel());
    else
    {
        msg.string32(QStringLiteral(""));
    }
}

Player* findPerson(const std::vector<std::unique_ptr<Player>>& data, Person* person)
{
    auto it= std::find_if(data.begin(), data.end(), [person](const std::unique_ptr<Player>& player) {
        bool val= ((player.get() == person) || (person->uuid() == player->uuid()));

        if(!val)
        {
            const auto& children= player->children();
            auto subIt
                = std::find_if(children.begin(), children.end(), [person](const std::unique_ptr<Character>& character) {
                      return (character.get() == person) || (person->uuid() == character->uuid());
                  });
            val= (subIt != children.end());
        }
        return val;
    });
    return it->get();
}

bool contains(const std::vector<std::unique_ptr<Player>>& data, Person* person)
{
    auto it= std::find_if(data.begin(), data.end(), [person](const std::unique_ptr<Player>& player) {
        bool val= ((player.get() == person) || (person->uuid() == player->uuid()));

        if(!val)
        {
            const auto& children= player->children();
            auto subIt
                = std::find_if(children.begin(), children.end(), [person](const std::unique_ptr<Character>& character) {
                      return (character.get() == person) || (person->uuid() == character->uuid());
                  });
            val= (subIt != children.end());
        }
        return val;
    });
    return it == data.end();
}

/******************
 * Initialisation *
 ******************/

PlayerModel::PlayerModel(QObject* parent) : QAbstractItemModel(parent)
{
    /*    using namespace NetMsg;
        ReceiveEvent::registerReceiver(PlayerCategory, PlayerConnectionAction, this);
        ReceiveEvent::registerReceiver(PlayerCategory, DelPlayerAction, this);
        ReceiveEvent::registerReceiver(PlayerCategory, ChangePlayerProperty, this);
        ReceiveEvent::registerReceiver(CharacterPlayerCategory, AddPlayerCharacterAction, this);
        ReceiveEvent::registerReceiver(CharacterPlayerCategory, DelPlayerCharacterAction, this);
        ReceiveEvent::registerReceiver(CharacterPlayerCategory, ChangePlayerCharacterProperty, this);
        ReceiveEvent::registerReceiver(SetupCategory, AddFeatureAction, this);*/
}

PlayerModel::~PlayerModel()= default;

/*************
 * ItemModel *
 *************/

QVariant PlayerModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || index.column() != 0)
        return QVariant();

    Person* person= static_cast<Person*>(index.internalPointer());

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
    case Qt::DecorationRole:
    {
        if(!person->getAvatar().isNull()) // (person->isLeaf()) &&
        {
            var= person->getAvatar().scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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
        var= true; // isLocal(person);
        break;
    case PlayerModel::GmRole:
        var= isGM;
        break;
    case PlayerModel::AvatarRole:
        var= person->getAvatar();
        break;
    }

    return var;
}

Qt::ItemFlags PlayerModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

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
/*QList<Character*> PlayerModel::getCharacterList()
{
    QList<Character*> list;

    for(auto& player : m_playersList)
    {
        list << player->getChildrenCharacter();
    }
    return list;
}*/

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
    auto it= std::find_if(m_players.begin(), m_players.end(), [parentPerson](const std::unique_ptr<Player>& person) {
        return parentPerson == person.get();
    });

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
            result= player->childrenCount();
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
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return true;
    }
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
            row= player->indexOf(person);
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

    const auto& it= std::find_if(m_players.begin(), m_players.end(), [id](const std::unique_ptr<Player>& player) {
        return (nullptr != player->characterById(id));
    });

    if(it != m_players.end())
        return it->get()->characterById(id);
    else
        return nullptr;
}

/***********
 * Getters *
 ***********/
/*bool PlayerModel::isLocal(Person* person) const
{
    if(person == nullptr)
        return false;

    Player* local= getLocalPlayer();
    if(nullptr == local)
        return false;

    return (person == local || person->parentPerson() == local);
}

int PlayerModel::getPlayerCount() const
{
    return m_playersList.size();
}

Player* PlayerModel::getPlayer(int index) const
{
    if(index < 0 && index > m_playersList.size())
        return nullptr;
    return m_playersList.at(index);
}

Person* PlayerModel::getPerson(const QString& uuid) const
{
    auto person= m_uuidMap.value(uuid);
    if(nullptr == person)
    {
        auto it= std::find_if(m_npcList.begin(), m_npcList.end(),
                              [uuid](Character* character) { return (character->getUuid() == uuid); });
        if(it != std::end(m_npcList))
        {
            person= *it;
        }
    }
    return person;
}

Player* PlayerModel::getPlayer(const QString& uuid) const
{
    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr || person->parentPerson() != nullptr) // No person or if person has parent return Player
        return nullptr;
    return static_cast<Player*>(person);
}

Character* PlayerModel::getCharacter(const QString& uuid) const
{
    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr || person->parentPerson() == nullptr)
        return nullptr;
    return static_cast<Character*>(person);
}

Player* PlayerModel::getParent(const QString& uuid) const
{
    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr)
        return nullptr;

    Person* parent= person->parentPerson();
    if(parent == nullptr)
    {
        return nullptr;
    }
    else
    {
        Player* player= dynamic_cast<Player*>(parent);
        if(nullptr != player)
        {
            return player;
        }
    }

    return nullptr;
}

Person* PlayerModel::getPerson(const QModelIndex& index) const
{
    if(!index.isValid() || index.column() != 0)
        return nullptr;

    auto person= static_cast<Person*>(index.internalPointer());

    return person;
}

Player* PlayerModel::getPlayer(const QModelIndex& index) const
{
    auto person= getPerson(index);
    return dynamic_cast<Player*>(person);
}

Character* PlayerModel::getCharacter(const QModelIndex& index) const
{
    auto person= getPerson(index);
    return dynamic_cast<Character*>(person);
}

QString PlayerModel::getUuidFromName(QString owner)
{
    Person* ownerPerson= m_localPlayer;
    QList<Character*> list= getCharacterList();
    bool unfound= true;
    for(int i= 0; i < list.size() && unfound; ++i)
    {
        Character* carac= list.at(i);
        if(carac->name() == owner)
        {
            unfound= false;
            ownerPerson= carac;
        }
    }
    return ownerPerson->getUuid();
}

bool PlayerModel::everyPlayerHasFeature(const QString& name, quint8 version) const
{
    int playersCount= m_playersList.size();
    for(int i= 0; i < playersCount; i++)
    {
        if(!m_playersList.at(i)->hasFeature(name, version))
            return false;
    }
    return true;
}
Player* PlayerModel::getLocalPlayer() const
{
    return m_localPlayer.data();
}

QString PlayerModel::getLocalPlayerId() const
{
    if(!m_localPlayer)
        return {};

    return m_localPlayer->getUuid();
}

void PlayerModel::sendOffLocalPlayerInformations()
{
    if(nullptr == m_localPlayer)
        return;

    NetworkMessageWriter message(NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
    setLocalFeatures(*m_localPlayer);
    m_localPlayer->fill(message);
    message.sendToServer();
}
void PlayerModel::sendOffFeatures(Player* player)
{
    setLocalFeatures(*player);
    SendFeaturesIterator i(*player);
    while(i.hasNext())
    {
        i.next();
        i.message().sendToServer();
    }
}*/

/***********
 * Setters *
 ***********/

/*void PlayerModel::setLocalPlayer(Player* player)
{
    if(player == m_localPlayer)
        return;

    if(m_playersList.size() > 0)
    {
        return;
    }
    m_localPlayer= player;
    setLocalFeatures(*player);
    addPlayer(player);
    auto characterList= m_localPlayer->getChildrenCharacter();
    std::for_each(characterList.begin(), characterList.end(), [this](Character* charac) { monitorCharacter(charac); });

    emit localPlayerChanged();
}

void PlayerModel::cleanListButLocal()
{
    beginResetModel();
    for(auto& tmp : m_playersList)
    {
        if(tmp != m_localPlayer)
        {
            delPlayer(tmp);
        }
    }
    endResetModel();
}*/

/*template <typename T>
void PlayerModel::sendOffPersonChanges(const QString& property)
{
    if(m_receivingData)
        return;

    auto person= qobject_cast<Person*>(sender());
    if(nullptr == person)
        return;

    auto idx= personToIndex(person);
    emit dataChanged(idx, idx);

    auto cat= NetMsg::PlayerCategory;
    auto action= NetMsg::ChangePlayerProperty;
    if(person->isLeaf())
    {
        cat= NetMsg::CharacterPlayerCategory;
        action= NetMsg::ChangePlayerCharacterProperty;
    }

    NetworkMessageWriter message(cat, action);
    message.string8(person->getUuid());
    message.string8(property);
    auto val= person->property(property.toLocal8Bit().data());
    convertVariantToType<T>(val.value<T>(), message);
    message.sendToServer();
}*/

/*void PlayerModel::monitorPlayer(Player* player)
{
    connect(player, &Player::avatarChanged, this, [this]() { sendOffPersonChanges<QImage>(QStringLiteral("avatar")); });
    connect(player, &Player::nameChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("name")); });
    connect(player, &Player::colorChanged, this, [this]() { sendOffPersonChanges<QColor>(QStringLiteral("color")); });
}

void PlayerModel::monitorCharacter(Character* charac)
{
    // clang-format off
    connect(charac, &Character::currentHealthPointsChanged, this,[this]() {
sendOffPersonChanges<QString>(QStringLiteral("healthPoints")); }); connect(charac, &Character::avatarChanged, this,
[this]() { sendOffPersonChanges<QImage>(QStringLiteral("avatar")); }); connect(charac, &Character::nameChanged, this,
[this]() { sendOffPersonChanges<QString>(QStringLiteral("name")); }); connect(charac, &Character::colorChanged, this,
[this]() { sendOffPersonChanges<QColor>(QStringLiteral("color")); }); connect(charac, &Character::initCommandChanged,
this,[this]() { sendOffPersonChanges<QString>(QStringLiteral("initCommand")); }); connect(charac,
&Character::initiativeChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("initiative")); });
    connect(charac, &Character::hasInitScoreChanged, this, [this]() {
sendOffPersonChanges<QString>(QStringLiteral("hasInitiative")); }); connect(charac, &Character::stateChanged, this,
[this]() { sendOffPersonChanges<CharacterState*>(QStringLiteral("state")); }); connect(charac, &Character::maxHPChanged,
this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("maxHP")); }); connect(charac, &Character::minHPChanged,
this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("minHP")); }); connect(charac,
&Character::distancePerTurnChanged, this,[this]() { sendOffPersonChanges<QString>(QStringLiteral("distancePerTurn"));
}); connect(charac, &Character::lifeColorChanged, this,[this]() {
sendOffPersonChanges<QColor>(QStringLiteral("lifeColor")); });
    //clang-format on

    if(localIsGM())
    {
        connect(charac, &Character::initiativeChanged, this, [this,charac]() {
            emit eventOccurs(tr("%1's initiative has changed:
%2").arg(charac->name()).arg(charac->getInitiativeScore()));
        });
    }

    charac->initCharacter();
}

void PlayerModel::addLocalCharacter(Character* newCharacter)
{
    if(nullptr == newCharacter)
        return;
    addCharacter(getLocalPlayer(), newCharacter);

    emit localPlayerChanged();

    NetworkMessageWriter message(NetMsg::CharacterPlayerCategory, NetMsg::AddPlayerCharacterAction);
    newCharacter->fill(message);
    message.uint8(1); // add it to the map
    message.sendToServer();
}

void PlayerModel::changeLocalPerson(Person* person, const QString& name, const QColor& color, const QImage& icon)
{
    if(nullptr == person || (!isLocal(person) && !localIsGM()))
        return;

    person->setName(name);
    person->setColor(color);
    person->setAvatar(icon);
}

bool PlayerModel::localIsGM() const
{
    if(!m_localPlayer)
        return false;

    return m_localPlayer->isGM();
}

void PlayerModel::delLocalCharacter(int index)
{
    Player* parent= getLocalPlayer();
    if(index < 0 || index >= parent->getChildrenCount())
        return;

    NetworkMessageWriter message(NetMsg::CharacterPlayerCategory, NetMsg::DelPlayerCharacterAction);
    message.string8(parent->getCharacterByIndex(index)->getUuid());
    message.sendToServer();

    delCharacter(parent, index);

    emit localPlayerChanged();
}*/

void PlayerModel::addPlayer(Player* player)
{
    if(nullptr == player)
        return;

    auto it= std::find_if(m_players.begin(), m_players.end(), [player](const std::unique_ptr<Player>& t) {
        return (t.get() == player || t->uuid() == player->uuid());
    });

    if(it != m_players.end())
    {
        qWarning() << tr("Dupplicated player or uuid");
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

void PlayerModel::removePlayer(Player* player)
{
    auto itPlayer= std::find_if(m_players.begin(), m_players.end(),
                                [player](const std::unique_ptr<Player>& person) { return person.get() == player; });
    if(itPlayer == m_players.end())
        return;

    //    int charactersCount= player->getChildrenCount();
    /*    for(int i= 0; i < charactersCount; i++)
        {
            delCharacter(player, 0);
        }*/

    auto index= static_cast<int>(std::distance(m_players.begin(), itPlayer));

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

/*void PlayerModel::delCharacter(Player* parent, int index)
{
    Character* character= parent->getCharacterByIndex(index);
    QModelIndex parentItem= personToIndex(parent);
    beginRemoveRows(parentItem, index, index);

    emit characterDeleted(character);

    parent->removeChild(character);

    endRemoveRows();
}*/

/***********
 * Network *
 ***********/

/*bool PlayerModel::event(QEvent* event)
{
    if(event->type() == ReceiveEvent::Type)
    {
        using namespace NetMsg;
        ReceiveEvent* rEvent= static_cast<ReceiveEvent*>(event);
        NetworkMessageReader& data= rEvent->data();
        switch(data.category())
        {
        case PlayerCategory:
            switch(data.action())
            {
            case PlayerConnectionAction:
                receivePlayer(data);
                return true;
            case DelPlayerAction:
                delPlayer(data);
                return true;
            case ChangePlayerProperty:
                updatePerson(data);
                return true;
            default:
                qWarning() << tr("PlayersList [PlayerCategory]: unknown action (%d)").arg(data.action());
            }
            break;
        case CharacterPlayerCategory:
            switch(data.action())
            {
            case AddPlayerCharacterAction:
                addCharacter(data);
                return true;
            case DelPlayerCharacterAction:
                delCharacter(data);
                return true;
            case ChangePlayerCharacterProperty:
                updatePerson(data);
                return true;
            default:
                qWarning() << tr("PlayersList [CharacterPlayerCategory]: unknown action (%d)").arg(data.action());
            }
            break;
        case SetupCategory:
            switch(data.action())
            {
            case AddFeatureAction:
                addFeature(*rEvent);
                return true;
            default:
                qWarning("PlayersList : message of categorie \"parametres\" with unknown action (%d)", data.action());
            }
            break;
        default:
            qWarning("PlayersList : message of unknown categorie (%d)", data.category());
        }
    }
    return QObject::event(event);
}*/

/*void PlayerModel::receivePlayer(NetworkMessageReader& data)
{
    Player* newPlayer= new Player(data);
    if(contains(m_players, newPlayer))
    {
        qWarning("A Player and a Character have the same UUID %s : %s", qPrintable(newPlayer->getUuid()),
                 qPrintable(newPlayer->name()));
        delete newPlayer;
        return;
    }
    addPlayer(newPlayer);
}*/

/*void PlayerModel::delPlayer(NetworkMessageReader& data)
{
    /// @todo: If the player is the GM, call AudioPlayer::pselectNewFile("").
    QString uuid= data.string8();
    Player* player= getPlayer(uuid);
    if(player != nullptr)
    {
        delPlayer(player);
    }
}*/

/*void PlayerModel::updatePerson(NetworkMessageReader& data)
{
    QString uuid= data.string8();
    Person* person= m_uuidMap.value(uuid);
    if(nullptr == person)
        return;

    auto property= data.string8();
    QVariant var;
    if(property.contains("color"))
    {
        var=QVariant::fromValue(QColor(data.rgb()));
    }
    else if(property.contains("avatar"))
    {
        auto array = data.byteArray32();
        QDataStream out(&array, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_7);
        QImage img;
        out >> img;
        var= QVariant::fromValue(img);
    }
    else if(property.contains(QStringLiteral("state")))
    {
        auto label = data.string32();
        auto state = Character::getStateFromLabel(label);
        var= QVariant::fromValue(state);
    }
    else {
        auto val= data.string32();
        var= QVariant::fromValue(val);
    }

    m_receivingData= true;
    person->setProperty(property.toLocal8Bit().data(), var);
    m_receivingData= false;

    auto idx= personToIndex(person);
    emit dataChanged(idx, idx);
}

void PlayerModel::addCharacter(NetworkMessageReader& data)
{
    Character* character= new Character();
    QString parentId= character->read(data);

    Player* player= getPlayer(parentId);
    if(player == nullptr)
        return;
    addCharacter(player, character);
}

void PlayerModel::delCharacter(NetworkMessageReader& data)
{
    QString uuid= data.string8();
    Character* character= getCharacter(uuid);
    if(character == nullptr)
        return;

    Player* parent= character->getParentPlayer();
    if(nullptr != parent)
    {
        delCharacter(parent, parent->indexOf(character));
    }
}

void PlayerModel::setCurrentGM(QString idGm)
{
    m_idCurrentGM= idGm;
}*/
void PlayerModel::clear()
{
    beginResetModel();
    m_players.clear();
    endResetModel();
    setGameMasterId("");
}

/*********
 * Other *
 *********/

/*Player* PlayerModel::getGM()
{
    for(auto& player : m_playersList)
    {
        if(player->isGM())
        {
            return player;
        }
    }
    return nullptr;
}

QString PlayerModel::getGmId()
{
    auto gm= getGM();
    if(nullptr == gm)
        return {};
    else
        return gm->getUuid();
}
bool PlayerModel::hasPlayer(Player* player)
{
    return m_playersList.contains(player);
}

void PlayerModel::addNpc(Character* character)
{
    if(character == nullptr)
        return;

    m_npcList.append(character);
}*/
