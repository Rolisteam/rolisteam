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

/******************
 * Initialisation *
 ******************/
PlayersList* PlayersList::m_singleton= nullptr;

PlayersList::PlayersList() : QAbstractItemModel(nullptr)
{
    using namespace NetMsg;
    ReceiveEvent::registerReceiver(PlayerCategory, PlayerConnectionAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, DelPlayerAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, ChangePlayerNameAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, ChangePlayerColorAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, AddPlayerCharacterAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, DelPlayerCharacterAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, ChangePlayerCharacterNameAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, ChangePlayerCharacterColorAction, this);
    ReceiveEvent::registerReceiver(CharacterPlayerCategory, ChangePlayerCharacterAvatarAction, this);
    ReceiveEvent::registerReceiver(SetupCategory, AddFeatureAction, this);

    // m_localPlayer = new Player();
    // m_playersList.append(m_localPlayer);

    // connect(QApplication::instance(), SIGNAL(lastWindowClosed()), this, SLOT(sendDelLocalPlayer()));
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

    Person* person;

    int row= index.row();
    if(row < 0)
    {
        return QVariant();
    }

    quint32 parentRow= static_cast<quint32>((index.internalId() & NoParent));
    if(parentRow == NoParent)
    {
        if(row >= m_playersList.size())
            return QVariant();

        Player* player= m_playersList.at(row);
        person= player;
        if(player != nullptr)
        {
            if((role == Qt::BackgroundRole) && (player->getUuid() == m_idCurrentGM) && player->isGM())
            {
                QPalette pal= qApp->palette();
                return QVariant(pal.color(QPalette::Active, QPalette::Dark));
            }
        }
    }
    else
    {
        if(parentRow >= static_cast<quint32>(m_playersList.size()))
            return QVariant();
        Player* player= m_playersList.at(static_cast<int>(parentRow));

        if(row >= player->getChildrenCount())
            return QVariant();
        person= player->getCharacterByIndex(row);
    }

    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return person->name();
    case Qt::ToolTipRole:
        return person->getToolTip();
    case Qt::DecorationRole:
    {
        if((person->isLeaf()) && (!person->getAvatar().isNull()))
        {
            return person->getAvatar().scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        else
            return QVariant(person->getColor());
    }
    case IdentifierRole:
        return QVariant(person->getUuid());
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
    if(column != 0)
        return QModelIndex();

    if(parent.isValid())
    {
        auto parentRow= parent.row();
        if(parentRow >= m_playersList.size())
            return QModelIndex();

        Player* player= m_playersList.at(parentRow);
        if(row < 0 || row >= player->getChildrenCount())
            return QModelIndex();

        return QAbstractItemModel::createIndex(row, 0, static_cast<quint32>(parentRow));
    }
    else
    {
        if(row < 0 && row >= m_playersList.size())
            return QModelIndex();

        return QAbstractItemModel::createIndex(row, 0, NoParent);
    }
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

    quint32 parentRow= static_cast<quint32>(index.internalId() & NoParent);

    if(parentRow == NoParent || parentRow >= static_cast<quint32>(m_playersList.size()))
    {
        return QModelIndex();
    }

    return QAbstractItemModel::createIndex(static_cast<int>(parentRow), 0, NoParent);
}

int PlayersList::rowCount(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return m_playersList.size();
    }

    quint32 parentRow= static_cast<quint32>(index.internalId() & NoParent);
    int row= index.row();
    if(parentRow != NoParent || row < 0 || row >= m_playersList.size())
    {
        return 0;
    }

    return m_playersList.at(row)->getChildrenCount();
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

QModelIndex PlayersList::mapIndexToMe(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    quint32 parentRow= static_cast<quint32>(index.internalId() & NoParent);
    return QAbstractItemModel::createIndex(index.row(), index.column(), parentRow);
}

QModelIndex PlayersList::createIndex(Person* person) const
{
    auto size= static_cast<unsigned int>(m_playersList.size());
    for(unsigned int row= 0; row < size; row++)
    {
        Player* player= m_playersList.at(static_cast<int>(row));
        if(person == player)
        {
            return QAbstractItemModel::createIndex(static_cast<int>(row), 0, NoParent);
        }
        else
        {
            int c_row;
            if(player->searchCharacter(static_cast<Character*>(person), c_row))
                return QAbstractItemModel::createIndex(c_row, 0, row);
        }
    }

    return QModelIndex();
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

    int row= index.row();
    if(row < 0)
        return nullptr;

    quint32 parentRow= static_cast<quint32>(index.internalId() & NoParent);
    if(parentRow == NoParent)
    {
        if(row < m_playersList.size())
            return m_playersList.at(row);
    }
    else if(parentRow < static_cast<quint32>(m_playersList.size()))
    {
        Player* player= m_playersList.at(static_cast<int>(parentRow));

        if(row < player->getChildrenCount())
            return player->getCharacterByIndex(row);
    }

    return nullptr;
}

Player* PlayersList::getPlayer(const QModelIndex& index) const
{
    if(!index.isValid() || index.column() != 0)
        return nullptr;

    int row= index.row();
    quint32 parentRow= static_cast<quint32>(index.internalId() & NoParent);
    if(parentRow == NoParent && row >= 0 && row < m_playersList.size())
        return m_playersList.at(row);

    return nullptr;
}

Character* PlayersList::getCharacter(const QModelIndex& index) const
{
    if(!index.isValid() || index.column() != 0)
        return nullptr;

    int row= index.row();
    quint32 parentRow= static_cast<quint32>(index.internalId() & NoParent);
    if(parentRow == NoParent && row >= 0 && parentRow < static_cast<quint32>(m_playersList.size()))
    {
        Player* player= m_playersList.at(row);
        if(row < player->getChildrenCount())
            return player->getCharacterByIndex(row);
    }

    return nullptr;
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
    return m_localPlayer;
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
    if(player != m_localPlayer)
    {
        if(m_playersList.size() > 0)
        {
            return;
        }
        m_localPlayer= player;
        setLocalFeatures(*player);
        addPlayer(player);
    }
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

void PlayersList::addLocalCharacter(Character* newCharacter)
{
    addCharacter(getLocalPlayer(), newCharacter);

    NetworkMessageWriter message(NetMsg::CharacterPlayerCategory, NetMsg::AddPlayerCharacterAction);
    newCharacter->fill(message);
    message.uint8(1); // add it to the map
    message.sendToServer();
}

void PlayersList::changeLocalPerson(Person* person, const QString& name, const QColor& color, const QImage& icon)
{
    if(!isLocal(person) && !localIsGM())
        return;

    if(p_setLocalPersonName(person, name) || p_setLocalPersonColor(person, color) || setLocalPersonAvatar(person, icon))
        notifyPersonChanged(person);
}

void PlayersList::setLocalPersonName(Person* person, const QString& name)
{
    if(!isLocal(person) && !localIsGM())
        return;

    if(p_setLocalPersonName(person, name))
        notifyPersonChanged(person);
}

bool PlayersList::localIsGM() const
{
    if(!m_localPlayer)
        return false;

    return m_localPlayer->isGM();
}

void PlayersList::setLocalPersonColor(Person* person, const QColor& color)
{
    if(!isLocal(person) && !localIsGM())
        return;

    if(p_setLocalPersonColor(person, color))
        notifyPersonChanged(person);
}

bool PlayersList::setLocalPersonAvatar(Person* person, const QImage& image)
{
    if(person->getAvatar() != image)
    {
        person->setAvatar(image);
        NetworkMessageWriter* message
            = new NetworkMessageWriter(NetMsg::CharacterPlayerCategory, NetMsg::ChangePlayerCharacterAvatarAction);

        message->string8(person->getUuid());

        QByteArray data;
        QDataStream in(&data, QIODevice::WriteOnly);
        in.setVersion(QDataStream::Qt_5_7);
        in << image;
        message->byteArray32(data);
        message->sendToServer();

        return true;
    }
    return false;
}

bool PlayersList::p_setLocalPersonColor(Person* person, const QColor& color)
{
    if(person->setColor(color))
    {
        NetworkMessageWriter* message;

        if(person->parentPerson() == nullptr)
            message= new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::ChangePlayerColorAction);
        else
            message
                = new NetworkMessageWriter(NetMsg::CharacterPlayerCategory, NetMsg::ChangePlayerCharacterColorAction);

        message->string8(person->getUuid());
        message->rgb(person->getColor().rgb());
        message->sendToServer();

        return true;
    }
    return false;
}
bool PlayersList::p_setLocalPersonName(Person* person, const QString& name)
{
    if(person->name() == name)
        return false;
    person->setName(name);
    NetworkMessageWriter* message;

    if(person->parentPerson() == nullptr)
        message= new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::ChangePlayerNameAction);
    else
        message= new NetworkMessageWriter(NetMsg::CharacterPlayerCategory, NetMsg::ChangePlayerCharacterNameAction);

    message->string16(person->name());
    message->string8(person->getUuid());
    message->sendToServer();

    return true;
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
}

void PlayersList::addPlayer(Player* player)
{
    int size= m_playersList.size();
    QString uuid= player->getUuid();

    if(m_uuidMap.contains(uuid))
        return;

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

    auto index= createIndex(player);

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
    QModelIndex parentItem= createIndex(parent);
    beginRemoveRows(parentItem, index, index);

    emit characterDeleted(character);

    m_uuidMap.remove(character->getUuid());
    parent->removeChild(character);

    endRemoveRows();
}

void PlayersList::notifyPersonChanged(Person* person)
{
    QModelIndex index= createIndex(person);

    if(index.internalId() != NoParent)
        emit characterChanged(static_cast<Character*>(person));
    else
        emit playerChanged(static_cast<Player*>(person));

    emit dataChanged(index, index);
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
            case ChangePlayerNameAction:
                setPersonName(data);
                return true;
            case ChangePlayerColorAction:
                setPersonColor(data);
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
            case ChangePlayerCharacterNameAction:
                setPersonName(data);
                return true;
            case ChangePlayerCharacterColorAction:
                setPersonColor(data);
                return true;
            case ChangePlayerCharacterAvatarAction:
                setPersonAvatar(data);
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

void PlayersList::setPersonName(NetworkMessageReader& data)
{
    QString name= data.string16();
    QString uuid= data.string8();

    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr)
        return;

    if(person->name() != name)
    {
        person->setName(name);
        notifyPersonChanged(person);
    }
}
void PlayersList::setPersonAvatar(NetworkMessageReader& data)
{
    QString uuid= data.string8();
    QByteArray imageBuffer= data.byteArray32();
    Person* person= m_uuidMap.value(uuid);
    QDataStream out(&imageBuffer, QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_7);
    QImage img;
    out >> img;

    if(person == nullptr)
        return;

    person->setAvatar(img);
}

void PlayersList::setPersonColor(NetworkMessageReader& data)
{
    QString uuid= data.string8();
    QColor color= QColor(data.rgb());

    Person* person= m_uuidMap.value(uuid);
    if(person == nullptr)
        return;

    if(person->setColor(color))
        notifyPersonChanged(person);
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
        delCharacter(parent, parent->getIndexOfCharacter(character));
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

/*void PlayersList::delPlayerWithLink(NetworkLink * link)
{
    int playersCount = m_playersList.size();
    for (int i = 0; i < playersCount ; i++)
    {
        Player * player = m_playersList.at(i);
        if (player->link() == link)
        {
            qWarning("Something wrong happens to %s", qPrintable(player->getName()));
            NetworkMessageWriter message (NetMsg::PlayerCategory, NetMsg::DelPlayerAction);
            message.string8(player->getUuid());
            message.sendAll(link);

            delPlayer(player);

            return;
        }
    }
}*/
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
