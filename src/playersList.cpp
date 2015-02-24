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


#include "playersList.h"

#include "networkmessagereader.h"
#include "networkmessagewriter.h"
#include "Features.h"
#include "persons.h"
#include "receiveevent.h"

#include "types.h"

// we don't want to include variablesGlobales.h just for some bool

extern bool G_joueur;


/******************
 * Initialisation *
 ******************/
PlayersList* PlayersList::m_singleton=NULL;

PlayersList::PlayersList()
    : QAbstractItemModel(NULL), m_gmCount(0),m_localPlayer(NULL)
{
    using namespace NetMsg;
    ReceiveEvent::registerReceiver(PlayerCategory, PlayerConnectionAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, AddPlayerAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, DelPlayerAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, ChangePlayerNameAction, this);
    ReceiveEvent::registerReceiver(PlayerCategory, ChangePlayerColorAction, this);
    ReceiveEvent::registerReceiver(CharacterCategory, AddCharacterAction, this);
    ReceiveEvent::registerReceiver(CharacterCategory, DelCharacterAction, this);
    ReceiveEvent::registerReceiver(CharacterCategory, ChangeCharacterNameAction, this);
    ReceiveEvent::registerReceiver(CharacterCategory, ChangeCharacterColorAction, this);
    ReceiveEvent::registerReceiver(SetupCategory, AddFeatureAction, this);
    
    connect(QApplication::instance(), SIGNAL(lastWindowClosed()), this, SLOT(sendDelLocalPlayer()));
}


PlayersList::~PlayersList()
{
    for (int i = 0; i < m_playersList.size(); i++)
    {
        delete m_playersList.at(i);
    }
}

PlayersList* PlayersList::instance()
{
    if(NULL==m_singleton)
    {
        m_singleton = new PlayersList();
    }
    return m_singleton;
}


/*************
 * ItemModel *
 *************/

QVariant PlayersList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() != 0)
            return QVariant();

    Person * person;

    int row = index.row();
    if (row < 0)
        return QVariant();

    quint32 parentRow = (quint32)(index.internalId() & NoParent);
    if (parentRow == NoParent)
    {
        if (row >= m_playersList.size())
            return QVariant();

        Player * player = m_playersList.at(row);
        person = player;
        if (role == Qt::BackgroundRole && player->isGM())
            return QVariant(QColor(200, 255, 200));
    }
    else
    {
        if (parentRow >= (quint32)m_playersList.size())
            return QVariant();
        Player * player = m_playersList.at(parentRow);

        if (row >= player->getCharactersCount())
            return QVariant();
        person = player->getCharacterByIndex(row);
    }

    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return QVariant(person->name());
        case Qt::DecorationRole:
            return QVariant(person->color());
        case IdentifierRole:
            return QVariant(person->uuid());
    }

    return QVariant();
}

Qt::ItemFlags PlayersList::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PlayersList::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return QVariant("Liste des Joueurs");
    return QVariant();
}

QModelIndex PlayersList::index(int row, int column, const QModelIndex &parent) const
{
    if (column != 0)
        return QModelIndex();

    if (parent.isValid())
    {
        quint32 parentRow = parent.row();
        if (parentRow >= (quint32)m_playersList.size())
            return QModelIndex();

        Player * player = m_playersList.at(parentRow);
        if (row < 0 || row >= player->getCharactersCount())
            return QModelIndex();

        return QAbstractItemModel::createIndex(row, 0, parentRow);
    }
    else
    {
        if (row < 0 && row >= m_playersList.size())
            return QModelIndex();

        return QAbstractItemModel::createIndex(row, 0, NoParent);
    }
}

QModelIndex PlayersList::parent(const QModelIndex & index) const
{
    if (!index.isValid())
        return QModelIndex();

    quint32 parentRow = (quint32)(index.internalId() & NoParent);

    if (parentRow == NoParent || parentRow >= (quint32)m_playersList.size())
    {
        return QModelIndex();
    }

    return QAbstractItemModel::createIndex(parentRow, 0, NoParent);
}

int PlayersList::rowCount(const QModelIndex & index) const
{
    if (!index.isValid())
    {
        return m_playersList.size();
    }

    quint32 parentRow = (quint32)(index.internalId() & NoParent);
    int row = index.row();
    if (parentRow != NoParent || row < 0 || row >= m_playersList.size())
    {
        return 0;
    }

    return m_playersList.at(row)->getCharactersCount();
}

int PlayersList::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool PlayersList::setData(const QModelIndex & index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}

QModelIndex PlayersList::mapIndexToMe(const QModelIndex & index) const
{
    if (!index.isValid())
        return QModelIndex();

    quint32 parentRow = (quint32)(index.internalId() & NoParent);
    return QAbstractItemModel::createIndex(index.row(), index.column(), parentRow);
}

QModelIndex PlayersList::createIndex(Person * person) const
{
    int size = m_playersList.size();
    for (int row = 0; row < size; row++)
    {
        Player * player = m_playersList.at(row);
        if (person == player)
        {
            return QAbstractItemModel::createIndex(row, 0, NoParent);
        }
        else
        {
            int c_row;
            if (player->searchCharacter((Character *)(person), c_row))
                return QAbstractItemModel::createIndex(c_row, 0, row);
        }
    }

    return QModelIndex();
}


/***********
 * Getters *
 ***********/

Player* PlayersList::localPlayer() const
{
    if(m_playersList.isEmpty())
    {
        return NULL;
    }
    return m_playersList.first();
}

bool PlayersList::isLocal(Person * person) const
{
    if (person == NULL)
        return false;

    Person * local = localPlayer();
    return (person == local || person->parent() == local);
}

int PlayersList::numPlayers() const
{
    return m_playersList.size();
}

Player * PlayersList::getPlayer(int index) const
{
    if (index < 0 && index > m_playersList.size())
        return NULL;
    return m_playersList.at(index);
}

Person * PlayersList::getPerson(const QString & uuid) const
{
    //qDebug()<< "uuid person:" << uuid ;
    return m_uuidMap.value(uuid);
}

Player * PlayersList::getPlayer(const QString & uuid) const
{
    Person * person = m_uuidMap.value(uuid);
    if (person == NULL || person->parent() != NULL)
        return NULL;
    return static_cast<Player *>(person);
}

Character * PlayersList::getCharacter(const QString & uuid) const
{
    Person * person = m_uuidMap.value(uuid);
    if (person == NULL || person->parent() == NULL)
        return NULL;
    return static_cast<Character *>(person);
}

Player * PlayersList::getParent(const QString & uuid) const
{
    Person * person = m_uuidMap.value(uuid);
    if (person == NULL)
        return NULL;
    
    Player * parent = person->parent();
    if (parent == NULL)
        return static_cast<Player *>(person);
    return parent;
}

Person * PlayersList::getPerson(const QModelIndex & index) const
{
    if (!index.isValid() || index.column() != 0)
        return NULL;

    int row = index.row();
    if (row < 0)
        return NULL;

    quint32 parentRow = (quint32)(index.internalId() & NoParent);
    if (parentRow == NoParent)
    {
        if (row < m_playersList.size())
            return m_playersList.at(row);
    }
    else if (parentRow < (quint32)m_playersList.size())
    {
        Player * player = m_playersList.at(parentRow);

        if (row < player->getCharactersCount())
            return player->getCharacterByIndex(row);
    }

    return NULL;
}
    
Player * PlayersList::getPlayer(const QModelIndex & index) const
{
    if (!index.isValid() || index.column() != 0)
        return NULL;

    int row = index.row();
    quint32 parentRow = (quint32)(index.internalId() & NoParent);
    if (parentRow == NoParent && row >= 0 && row < m_playersList.size())
        return m_playersList.at(row);

    return NULL;
}

Character * PlayersList::getCharacter(const QModelIndex & index) const
{
    if (!index.isValid() || index.column() != 0)
        return NULL;

    int row = index.row();
    quint32 parentRow = (quint32)(index.internalId() & NoParent);
    if (parentRow == NoParent && row >= 0 && parentRow < (quint32)m_playersList.size())
    {
        Player * player = m_playersList.at(row);
        if (row < player->getCharactersCount())
            return player->getCharacterByIndex(row);
    }

    return NULL;
}

bool PlayersList::everyPlayerHasFeature(const QString & name, quint8 version) const
{
    int playersCount = m_playersList.size();
    for (int i = 0; i < playersCount; i++)
    {
        if (!m_playersList.at(i)->hasFeature(name, version))
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
    NetworkMessageWriter message (NetMsg::PlayerCategory, NetMsg::PlayerConnectionAction);
    m_localPlayer->fill(message);
    message.sendAll();
}
void PlayersList::sendOffFeatures(Player* player)
{
    setLocalFeatures(*player);
    SendFeaturesIterator i(*player);
    while (i.hasNext())
    {
        i.next();
        i.message().sendAll();
    }
}

/***********
 * Setters *
 ***********/

void PlayersList::setLocalPlayer(Player * player)
{

    if(player!=m_localPlayer)
    {
        if (m_playersList.size() > 0)
        {
            qFatal("We have too many local players.");
        }
        m_localPlayer=player;
        addPlayer(player);
    }
}

void PlayersList::cleanListButLocal()
{
    beginResetModel();
    foreach(Player* tmp,m_playersList )
    {
        if(tmp != m_localPlayer)
        {
            delPlayer(tmp);
        }
    }
    endResetModel();

}

void PlayersList::addLocalCharacter(Character * newCharacter)
{
    addCharacter(localPlayer(), newCharacter);

    NetworkMessageWriter message (NetMsg::CharacterCategory, NetMsg::AddCharacterAction);
    newCharacter->fill(message);
    message.uint8(1); // add it to the map
    message.sendAll();
}

void PlayersList::changeLocalPerson(Person * person, const QString & name, const QColor & color)
{
    if (!isLocal(person))
        return;

    if (p_setLocalPersonName(person, name) | p_setLocalPersonColor(person, color))
        notifyPersonChanged(person);
}

void PlayersList::setLocalPersonName(Person * person, const QString & name)
{
    if (!isLocal(person))
        return;

    if (p_setLocalPersonName(person, name))
        notifyPersonChanged(person);
}

void PlayersList::setLocalPersonColor(Person * person, const QColor & color)
{
    if (!isLocal(person))
        return;

    if (p_setLocalPersonColor(person, color))
        notifyPersonChanged(person);
}

bool PlayersList::p_setLocalPersonName(Person * person, const QString & name)
{
    if (person->setName(name))
    {
        NetworkMessageWriter * message;

        if (person->parent() == NULL)
            message = new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::ChangePlayerNameAction);
        else
            message = new NetworkMessageWriter(NetMsg::CharacterCategory, NetMsg::ChangeCharacterNameAction);

        message->string16(person->name());
        message->string8(person->uuid());
        message->sendAll();

        return true;
    }
    return false;
}

bool PlayersList::p_setLocalPersonColor(Person * person, const QColor & color)
{
    if (person->setColor(color))
    {
        NetworkMessageWriter * message;

        if (person->parent() == NULL)
            message = new NetworkMessageWriter(NetMsg::PlayerCategory, NetMsg::ChangePlayerColorAction);
        else
            message = new NetworkMessageWriter(NetMsg::CharacterCategory, NetMsg::ChangeCharacterColorAction);

        message->string8(person->uuid());
        message->rgb(person->color());
        message->sendAll();

        return true;
    }
    return false;
}

void PlayersList::delLocalCharacter(int index)
{
    Player * parent = localPlayer();
    if (index < 0 || index >= parent->getCharactersCount())
        return;

    NetworkMessageWriter message (NetMsg::CharacterCategory, NetMsg::DelCharacterAction);
    message.string8(parent->getCharacterByIndex(index)->uuid());
    message.sendAll();

    delCharacter(parent, index);
}

void PlayersList::addPlayer(Player * player)
{
    int size = m_playersList.size();
    QString uuid = player->uuid();

    if (m_uuidMap.contains(uuid))
        qFatal("Uuid %s For this player already in use.", qPrintable(uuid));

    beginInsertRows(QModelIndex(), size, size);

    m_playersList << player;
    m_uuidMap.insert(uuid, player);
    if (player->isGM())
        m_gmCount += 1;

    emit playerAdded(player);

    endInsertRows();
}

void PlayersList::addCharacter(Player * player, Character * character)
{
    int size = player->getCharactersCount();
    QString uuid = character->uuid();

    if (m_uuidMap.contains(uuid))
        qFatal("Uuid %s for this character already in use.", qPrintable(uuid));

    beginInsertRows(createIndex(player), size, size);

    player->addCharacter(character);
    m_uuidMap.insert(uuid, character);

    emit characterAdded(character);

    endInsertRows();
}

void PlayersList::delPlayer(Player * player)
{
    int index = m_playersList.indexOf(player);
    if (index < 0)
        return;

    int charactersCount = player->getCharactersCount();
    for (int i = 0; i < charactersCount ; i++)
        delCharacter(player, 0);

    beginRemoveRows(QModelIndex(), index, index);

    m_uuidMap.remove(player->uuid());
    m_playersList.removeAt(index);
    if (player->isGM())
        m_gmCount -= 1;

    emit playerDeleted(player);
    delete player;

    endRemoveRows();
}

void PlayersList::delCharacter(Player * parent, int index)
{
    Character * character = parent->getCharacterByIndex(index);

    beginRemoveRows(createIndex(parent), index, index);

    emit characterDeleted(character);

    m_uuidMap.remove(character->uuid());
    parent->delCharacter(index);

    endRemoveRows();
}

void PlayersList::notifyPersonChanged(Person * person)
{
    QModelIndex index = createIndex(person);
    
    if (index.internalId() != NoParent)
        emit characterChanged(static_cast<Character *>(person));
    else
        emit playerChanged(static_cast<Player *>(person));

    emit dataChanged(index, index);
}


/***********
 * Network *
 ***********/

bool PlayersList::event(QEvent * event)
{
    if (event->type() == ReceiveEvent::Type)
    {
        using namespace NetMsg;
        ReceiveEvent * rEvent = static_cast<ReceiveEvent *>(event);
        NetworkMessageReader & data = rEvent->data();
        switch (data.category())
        {
            case PlayerCategory:
                switch (data.action())
                {
                    case PlayerConnectionAction:
                        addPlayerAsServer(rEvent);
                        return true;
                    case AddPlayerAction:
                        addPlayer(data);
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
                        qWarning("PlayersList : message of categorie \"joueur\" with unknown action (%d)", data.action());
                }
                break;
            case CharacterCategory:
                switch (data.action())
                {
                    case AddCharacterAction:
                        addCharacter(data);
                        return true;
                    case DelCharacterAction:
                        delCharacter(data);
                        return true;
                    case ChangeCharacterNameAction:
                        setPersonName(data);
                        return true;
                    case ChangeCharacterColorAction:
                        setPersonColor(data);
                        return true;
                    default:
                        qWarning("PlayersList : message of categorie \"persoJoueur\" with unknown action (%d)", data.action());
                }
                break;
            case SetupCategory:
                switch (data.action())
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

void PlayersList::addPlayer(NetworkMessageReader & data)
{
    Player * newPlayer = new Player(data);

    Person * actualPerson = m_uuidMap.value(newPlayer->uuid());
    if (actualPerson != NULL)
    {
        if (actualPerson->parent() == NULL)
        {

            Player * actualPlayer = static_cast<Player *>(actualPerson);
            bool isGM = newPlayer->isGM();
            if (m_localPlayer->isGM() != isGM)
            {
                m_localPlayer->setGM(false);
                G_joueur = !isGM;
                notifyPersonChanged(actualPlayer);
                emit localGMRefused();
            }
        }
        else
            qWarning("A Player and a Character have the same UUID %s", qPrintable(newPlayer->uuid()));
    }

    else
        addPlayer(newPlayer);
}

void PlayersList::addPlayerAsServer(ReceiveEvent * event)
{
    Liaison * link = event->link();
    Player * player = new Player(event->data(), link);
    if (player->isGM() && m_gmCount > 0)
        player->setGM(false);

    addPlayer(player);

    NetworkMessageWriter msgPlayer (NetMsg::PlayerCategory, NetMsg::AddPlayerAction);
    player->fill(msgPlayer);
    // display it in the log
    msgPlayer.uint8(1);
    msgPlayer.sendAll();

    NetworkMessageWriter msgCharacter (NetMsg::CharacterCategory, NetMsg::AddCharacterAction);
    SendFeaturesIterator featuresIterator;

    int playersListSize = m_playersList.size();
    for (int i = 0 ; i < playersListSize ; i++)
    {
        Player * curPlayer = m_playersList.at(i);
        msgPlayer.reset();
        curPlayer->fill(msgPlayer);
        // don't display it in the log
        msgPlayer.uint8(0);
        if (curPlayer != player)
        {
            msgPlayer.sendTo(link);
            int charactersListSize = curPlayer->getCharactersCount();
            for (int j = 0 ; j < charactersListSize ; j++)
            {
                Character * character = curPlayer->getCharacterByIndex(j);
                msgCharacter.reset();
                character->fill(msgCharacter);
                // add it to the maps
                msgCharacter.uint8(1);
                msgCharacter.sendTo(link);
            }
        }
        featuresIterator = curPlayer;
        while (featuresIterator.hasNext())
        {
            featuresIterator.next();
            featuresIterator.message().sendTo(link);
        }
    }
}

void PlayersList::delPlayer(NetworkMessageReader & data)
{
    /// @todo: If the player is the GM, call AudioPlayer::pselectNewFile("").


    QString uuid = data.string8();
    Player * player = getPlayer(uuid);
    if (player != NULL)
    {
        delPlayer(player);
    }
}

void PlayersList::setPersonName(NetworkMessageReader & data)
{
    QString name = data.string16();
    QString uuid = data.string8();

    Person * person = m_uuidMap.value(uuid);
    if (person == NULL)
        return;

    if (person->setName(name))
        notifyPersonChanged(person);
}

void PlayersList::setPersonColor(NetworkMessageReader & data)
{
    QString uuid = data.string8();
    QColor color = QColor(data.rgb());

    Person * person = m_uuidMap.value(uuid);
    if (person == NULL)
        return;

    if (person->setColor(color))
        notifyPersonChanged(person);
}

void PlayersList::addCharacter(NetworkMessageReader & data)
{
    QString uuid = data.string8();
    Player * player = getPlayer(uuid);
    if (player == NULL)
        return;

    Character * character = new Character(data);
    addCharacter(player, character);
}

void PlayersList::delCharacter(NetworkMessageReader & data)
{
    QString uuid = data.string8();
    Character * character = getCharacter(uuid);
    if (character == NULL)
        return;

    Player * parent = character->parent();
    delCharacter(parent, parent->getIndexOfCharacter(character)); 
}

void PlayersList::sendDelLocalPlayer()
{
    NetworkMessageWriter message (NetMsg::PlayerCategory, NetMsg::DelPlayerAction);
    message.string8(localPlayer()->uuid());
    message.sendAll();
}
void PlayersList::completeListClean()
{
    beginResetModel();
    m_playersList.clear();
    m_uuidMap.clear();
    //reset();
    m_localPlayer=NULL;
    endResetModel();
}

/*********
 * Other *
 *********/

void PlayersList::delPlayerWithLink(Liaison * link)
{
    int playersCount = m_playersList.size();
    for (int i = 0; i < playersCount ; i++)
    {
        Player * player = m_playersList.at(i);
        if (player->link() == link)
        {
            qWarning("Something wrong happens to %s", qPrintable(player->name()));
            NetworkMessageWriter message (NetMsg::PlayerCategory, NetMsg::DelPlayerAction);
            message.string8(player->uuid());
            message.sendAll(link);

            delPlayer(player);

            return;
        }
    }
}
