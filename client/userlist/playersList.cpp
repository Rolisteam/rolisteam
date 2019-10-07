/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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

#include <QApplication>
#include <QDebug>
#include <QPalette>

#include "userlist/playersList.h"

#include "Features.h"
#include "data/character.h"
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

/******************
 * Initialisation *
 ******************/
PlayersList* PlayersList::m_singleton= nullptr;

PlayersList::PlayersList() : QAbstractItemModel(nullptr)
{
    using namespace NetMsg;
    ReceiveEvent::registerReceiver(PlayerCategory, PlayerConnectionAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, DelPlayerAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, ChangePlayerProperty, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, AddPlayerCharacterAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, DelPlayerCharacterAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, ChangePlayerCharacterProperty, this);
    ReceiveEvent::registerReceiver(SetupCategory, AddFeatureAction, this);
}

PlayersList::~PlayersList()
{
    for(int i= 0; i < m_playersList.size(); i++)
    {
        delete m_playersList.at(i);
    }
}

PlayersList* PlayersList::instance()
{
    if(nullptr == m_singleton)
    {
        m_singleton= new PlayersList();
    }
    return m_singleton;
}

/*************
 * ItemModel *
 *************/

QVariant PlayersList::data(const QModelIndex& index, int role) const
{
    if(!index.isValid() || index.column() != 0)
        return QVariant();

    Person* person= static_cast<Person*>(index.internalPointer());

    Character* character= dynamic_cast<Character*>(person);
    if(!person->isLeaf() && (role == Qt::BackgroundRole))
    {
        Player* player= dynamic_cast<Player*>(person);
        if(player != nullptr)
        {
            if((person->getUuid() == m_idCurrentGM) && player->isGM())
            {
                QPalette pal= qApp->palette();
                return QVariant(pal.color(QPalette::Active, QPalette::Dark));
            }
        }
    }

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
        return name;
    }
    case Qt::ToolTipRole:
        return person->getToolTip();
    case Qt::DecorationRole:
    {
        if(!person->getAvatar().isNull()) // (person->isLeaf()) &&
        {
            return person->getAvatar().scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else
            return QVariant(person->getColor());
    }
    case IdentifierRole:
        return person->getUuid();
    case PersonPtr:
        return QVariant::fromValue(person);
    case IsLocal:
        return isLocal(person);
    case IsGM:
    {
        auto pl= dynamic_cast<Player*>(person);
        if(pl)
            return pl->isGM();
        else
        {
            return false;
        }
    }
    case LocalIsGM:
        return localIsGM();
    }

    return QVariant();
}

Qt::ItemFlags PlayersList::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PlayersList::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant(tr("Players List"));
    return QVariant();
}

QModelIndex PlayersList::index(int row, int column, const QModelIndex& parent) const
{
    if(column != 0 || row < 0)
        return QModelIndex();
    Person* childItem= nullptr;
    if(parent.isValid())
    {
        auto parentPerson= static_cast<Person*>(parent.internalPointer());
        auto parentPlayer= dynamic_cast<Player*>(parentPerson);
        if(nullptr == parentPlayer)
            return {};
        childItem= parentPlayer->getCharacterByIndex(row);
    }
    else if(m_playersList.size() > row)
    {
        childItem= m_playersList[row];
    }

    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
QList<Character*> PlayersList::getCharacterList()
{
    QList<Character*> list;

    for(auto& player : m_playersList)
    {
        list << player->getChildrenCharacter();
    }
    return list;
}

QModelIndex PlayersList::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto childItem= static_cast<Person*>(index.internalPointer());
    auto parentPerson= childItem->parentPerson();

    if(nullptr == parentPerson)
    {
        return QModelIndex();
    }
    return createIndex(parentPerson->indexOf(childItem), 0, parentPerson);
}

int PlayersList::rowCount(const QModelIndex& parent) const
{
    int result= 0;
    if(!parent.isValid())
    {
        result= m_playersList.size();
    }
    else
    {
        auto parentItem= static_cast<Person*>(parent.internalPointer());
        auto player= dynamic_cast<Player*>(parentItem);
        if(player)
        {
            result= player->getChildrenCount();
        }
    }
    return result;
}

int PlayersList::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool PlayersList::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}

QModelIndex PlayersList::personToIndex(Person* person) const
{
    QModelIndex parent;
    int row= -1;
    if(person->isLeaf())
    {
        auto player= dynamic_cast<Player*>(person->parentPerson());
        if(player)
        {
            parent= index(m_playersList.indexOf(player), 0, QModelIndex());
            row= player->indexOf(person);
        }
    }
    else
    {
        auto player= dynamic_cast<Player*>(person);
        row= m_playersList.indexOf(player);
    }
    return index(row, 0, parent);
}

/***********
 * Getters *
 ***********/
bool PlayersList::isLocal(Person* person) const
{
    if(person == nullptr)
        return false;

    Player* local= getLocalPlayer();
    if(nullptr == local)
        return false;

    return (person == local || person->parentPerson() == local);
}

int PlayersList::getPlayerCount() const
{
    return m_playersList.size();
}

Player* PlayersList::getPlayer(int index) const
{
    if(index < 0 && index > m_playersList.size())
        return nullptr;
    return m_playersList.at(index);
}

Person* PlayersList::getPerson(const QString& uuid) const
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

Player* PlayersList::getPlayer(const QString& uuid) const
{
    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr || person->parentPerson() != nullptr) // No person or if person has parent return Player
        return nullptr;
    return static_cast<Player*>(person);
}

Character* PlayersList::getCharacter(const QString& uuid) const
{
    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr || person->parentPerson() == nullptr)
        return nullptr;
    return static_cast<Character*>(person);
}

Player* PlayersList::getParent(const QString& uuid) const
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

Person* PlayersList::getPerson(const QModelIndex& index) const
{
    if(!index.isValid() || index.column() != 0)
        return nullptr;

    auto person= static_cast<Person*>(index.internalPointer());

    return person;
}

Player* PlayersList::getPlayer(const QModelIndex& index) const
{
    auto person= getPerson(index);
    return dynamic_cast<Player*>(person);
}

Character* PlayersList::getCharacter(const QModelIndex& index) const
{
    auto person= getPerson(index);
    return dynamic_cast<Character*>(person);
}

QString PlayersList::getUuidFromName(QString owner)
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

bool PlayersList::everyPlayerHasFeature(const QString& name, quint8 version) const
{
    int playersCount= m_playersList.size();
    for(int i= 0; i < playersCount; i++)
    {
        if(!m_playersList.at(i)->hasFeature(name, version))
            return false;
    }
    return true;
}
Player* PlayersList::getLocalPlayer() const
{
    return m_localPlayer.data();
}

QString PlayersList::getLocalPlayerId() const
{
    if(!m_localPlayer)
        return {};

    return m_localPlayer->getUuid();
}

void PlayersList::sendOffLocalPlayerInformations()
{
    if(nullptr == m_localPlayer)
        return;

    NetworkMessageWriter message(NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
    setLocalFeatures(*m_localPlayer);
    m_localPlayer->fill(message);
    message.sendToServer();
}
void PlayersList::sendOffFeatures(Player* player)
{
    setLocalFeatures(*player);
    SendFeaturesIterator i(*player);
    while(i.hasNext())
    {
        i.next();
        i.message().sendToServer();
    }
}

/***********
 * Setters *
 ***********/

void PlayersList::setLocalPlayer(Player* player)
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

void PlayersList::cleanListButLocal()
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
}

template <typename T>
void PlayersList::sendOffPersonChanges(const QString& property)
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
}

void PlayersList::monitorPlayer(Player* player)
{
    connect(player, &Player::avatarChanged, this, [this]() { sendOffPersonChanges<QImage>(QStringLiteral("avatar")); });
    connect(player, &Player::nameChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("name")); });
    connect(player, &Player::colorChanged, this, [this]() { sendOffPersonChanges<QColor>(QStringLiteral("color")); });
}

void PlayersList::monitorCharacter(Character* charac)
{
    // clang-format off
    connect(charac, &Character::currentHealthPointsChanged, this,[this]() { sendOffPersonChanges<QString>(QStringLiteral("healthPoints")); });
    connect(charac, &Character::avatarChanged, this, [this]() { sendOffPersonChanges<QImage>(QStringLiteral("avatar")); });
    connect(charac, &Character::nameChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("name")); });
    connect(charac, &Character::colorChanged, this, [this]() { sendOffPersonChanges<QColor>(QStringLiteral("color")); });
    connect(charac, &Character::initCommandChanged, this,[this]() { sendOffPersonChanges<QString>(QStringLiteral("initCommand")); });
    connect(charac, &Character::initiativeChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("initiative")); });
    connect(charac, &Character::hasInitScoreChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("hasInitiative")); });
    connect(charac, &Character::stateChanged, this, [this]() { sendOffPersonChanges<CharacterState*>(QStringLiteral("state")); });
    connect(charac, &Character::maxHPChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("maxHP")); });
    connect(charac, &Character::minHPChanged, this, [this]() { sendOffPersonChanges<QString>(QStringLiteral("minHP")); });
    connect(charac, &Character::distancePerTurnChanged, this,[this]() { sendOffPersonChanges<QString>(QStringLiteral("distancePerTurn")); });
    connect(charac, &Character::lifeColorChanged, this,[this]() { sendOffPersonChanges<QColor>(QStringLiteral("lifeColor")); });
    //clang-format on

    if(localIsGM())
    {
        connect(charac, &Character::initiativeChanged, this, [this,charac]() {
            emit eventOccurs(tr("%1's initiative has changed: %2").arg(charac->name()).arg(charac->getInitiativeScore()));
        });
    }

    charac->initCharacter();
}

void PlayersList::addLocalCharacter(Character* newCharacter)
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

void PlayersList::changeLocalPerson(Person* person, const QString& name, const QColor& color, const QImage& icon)
{
    if(nullptr == person || (!isLocal(person) && !localIsGM()))
        return;

    person->setName(name);
    person->setColor(color);
    person->setAvatar(icon);
}

bool PlayersList::localIsGM() const
{
    if(!m_localPlayer)
        return false;

    return m_localPlayer->isGM();
}

void PlayersList::delLocalCharacter(int index)
{
    Player* parent= getLocalPlayer();
    if(index < 0 || index >= parent->getChildrenCount())
        return;

    NetworkMessageWriter message(NetMsg::CharacterPlayerCategory, NetMsg::DelPlayerCharacterAction);
    message.string8(parent->getCharacterByIndex(index)->getUuid());
    message.sendToServer();

    delCharacter(parent, index);

    emit localPlayerChanged();
}

void PlayersList::addPlayer(Player* player)
{
    int size= m_playersList.size();
    QString uuid= player->getUuid();

    if(m_uuidMap.contains(uuid))
        return;

    if(player == getLocalPlayer() || localIsGM())
        monitorPlayer(player);

    beginInsertRows(QModelIndex(), size, size);

    m_playersList << player;
    m_uuidMap.insert(uuid, player);

    emit playerAdded(player);

    endInsertRows();

    for(int i= 0; i < player->getChildrenCount(); ++i)
    {
        Character* character= player->getCharacterByIndex(i);
        addCharacter(player, character);
    }
}

void PlayersList::addCharacter(Player* player, Character* character)
{
    if(player == nullptr || character == nullptr)
        return;

    int size= player->getChildrenCount();
    QString uuid= character->getUuid();

    if(m_uuidMap.contains(uuid))
        return;

    auto index= personToIndex(player);

    if(player == getLocalPlayer() || localIsGM())
        monitorCharacter(character);

    beginInsertRows(index, size, size);

    if(character->getParentPlayer() != player)
    {
        player->addCharacter(character);
    }
    m_uuidMap.insert(uuid, character);

    emit characterAdded(character);

    endInsertRows();
}

void PlayersList::delPlayer(Player* player)
{
    int index= m_playersList.indexOf(player);
    if(index < 0)
        return;

    int charactersCount= player->getChildrenCount();
    for(int i= 0; i < charactersCount; i++)
    {
        delCharacter(player, 0);
    }

    beginRemoveRows(QModelIndex(), index, index);

    m_uuidMap.remove(player->getUuid());
    m_playersList.removeAt(index);

    emit playerDeleted(player);
    delete player;

    endRemoveRows();
}

void PlayersList::delCharacter(Player* parent, int index)
{
    Character* character= parent->getCharacterByIndex(index);
    QModelIndex parentItem= personToIndex(parent);
    beginRemoveRows(parentItem, index, index);

    emit characterDeleted(character);

    m_uuidMap.remove(character->getUuid());
    parent->removeChild(character);

    endRemoveRows();
}

/***********
 * Network *
 ***********/

bool PlayersList::event(QEvent* event)
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
}

void PlayersList::receivePlayer(NetworkMessageReader& data)
{
    Player* newPlayer= new Player(data);
    if(m_uuidMap.contains(newPlayer->getUuid()))
    {
        qWarning("A Player and a Character have the same UUID %s : %s - %s", qPrintable(newPlayer->getUuid()),
            qPrintable(newPlayer->name()), qPrintable(m_uuidMap.value(newPlayer->getUuid())->name()));
        delete newPlayer;
        return;
    }
    addPlayer(newPlayer);
}

void PlayersList::delPlayer(NetworkMessageReader& data)
{
    /// @todo: If the player is the GM, call AudioPlayer::pselectNewFile("").
    QString uuid= data.string8();
    Player* player= getPlayer(uuid);
    if(player != nullptr)
    {
        delPlayer(player);
    }
}

void PlayersList::updatePerson(NetworkMessageReader& data)
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

void PlayersList::addCharacter(NetworkMessageReader& data)
{
    Character* character= new Character();
    QString parentId= character->read(data);

    Player* player= getPlayer(parentId);
    if(player == nullptr)
        return;
    addCharacter(player, character);
}

void PlayersList::delCharacter(NetworkMessageReader& data)
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

void PlayersList::setCurrentGM(QString idGm)
{
    m_idCurrentGM= idGm;
}
void PlayersList::completeListClean()
{
    beginResetModel();
    m_playersList.clear();
    m_uuidMap.clear();
    endResetModel();

    Player* player= getLocalPlayer();
    if(nullptr != player)
    {
        player->clearCharacterList();
    }
    m_localPlayer= nullptr;
}

/*********
 * Other *
 *********/

Player* PlayersList::getGM()
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

QString PlayersList::getGmId()
{
    auto gm= getGM();
    if(nullptr == gm)
        return {};
    else
        return gm->getUuid();
}
bool PlayersList::hasPlayer(Player* player)
{
    return m_playersList.contains(player);
}

void PlayersList::addNpc(Character* character)
{
    if(character == nullptr)
        return;

    m_npcList.append(character);
}
