/*************************************************************************
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *   http://www.rolisteam.org/                                           *
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

#include "chatlist.h"

#include <QAbstractItemModel>

#include "datareader.h"
#include "persons.h"
#include "playersList.h"
#include "receiveevent.h"
#include "Tchat.h"

#include "types.h"

ChatList::ChatList(MainWindow * mainWindow)
    : QAbstractItemModel(NULL), m_chatList(), m_chatMenu()
{
    m_chatMenu.setTitle(tr("Tchats"));

    // main (public) chat
    addChat(new Tchat(QString(), tr("Tchat commun"), mainWindow));

    // Stay sync with g_playersList
    PlayersList & g_playersList = PlayersList::instance();
    connect(&g_playersList, SIGNAL(playerAdded(Player *)), this, SLOT(addPlayerChat(Player *)));
    connect(&g_playersList, SIGNAL(playerDeleted(Player *)), this, SLOT(delPlayer(Player *)));

    // Allready there player's chat
    int maxPlayerIndex = g_playersList.numPlayers();
    Player * localPlayer = g_playersList.localPlayer();
    for (int i = 0 ; i < maxPlayerIndex ; i++)
    {
        Player * player = g_playersList.getPlayer(i);
        if (player != localPlayer)
        {
            addPlayerChat(player, mainWindow);
        }
    }

    // Receive events
    ReceiveEvent::registerReceiver(discussion, TCHAT_MESSAGE, this);
    ReceiveEvent::registerReceiver(discussion, DICE_MESSAGE, this);
    ReceiveEvent::registerReceiver(discussion, EMOTE_MESSAGE, this);
}

ChatList::~ChatList()
{
    m_chatMenu.clear();

    int maxChatIndex = m_chatList.size();
    for (int i = 0 ; i < maxChatIndex ; i++)
    {
        delete m_chatList.at(i);
    }
}


QVariant ChatList::data(const QModelIndex &index, int role) const
{
    Tchat * chat = getChat(index);

    if (chat == NULL)
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return QVariant(chat->name());
        case Qt::DecorationRole:
            return QVariant(QColor(chat->hasUnseenMessage() ? "red" : "green"));
        case Qt::CheckStateRole:
            return QVariant(chat->isVisible());
    }

    return QVariant();
}

Qt::ItemFlags ChatList::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QVariant ChatList::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

QModelIndex ChatList::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || column != 0 || row < 0 || row >= m_chatList.size())
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex ChatList::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ChatList::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_chatList.size();
}

int ChatList::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 1;
}

bool ChatList::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);

    Tchat * chat = getChat(index);

    if (chat == NULL)
        return false;

    switch (role)
    {
        case Qt::CheckStateRole:
            chat->toggleViewAction()->toggle();
        default:
            return false;
    }

    // this signal won't probably be emited for every changes
    emit dataChanged(index, index);
    return true;
}

QMenu * ChatList::chatMenu()
{
    return &m_chatMenu;
}

bool ChatList::event(QEvent * event)
{
    if (event->type() == ReceiveEvent::Type)
    {
        ReceiveEvent * netEvent = static_cast<ReceiveEvent *>(event);
        actionDiscussion action = (actionDiscussion) netEvent->action();

        if ( netEvent->categorie() == discussion &&
             (action == TCHAT_MESSAGE || action == DICE_MESSAGE  || action == EMOTE_MESSAGE)
           )
        {
            dispatchMessage(netEvent);
            return true;
        }
    }

    return QObject::event(event);
}

void ChatList::addChat(Tchat * chat)
{
    int listSize = m_chatList.size();
    beginInsertRows(QModelIndex(), listSize, listSize);

    m_chatList.append(chat);
    m_chatMenu.addAction(chat->toggleViewAction());
    connect(chat, SIGNAL(changed(Tchat *)), this, SLOT(chatChanged(Tchat *)));

    endInsertRows();
}

void ChatList::delChat(Tchat * chat)
{
    int pos = m_chatList.indexOf(chat);
    if (pos < 0)
        return;

    beginRemoveRows(QModelIndex(), pos, pos);

    m_chatMenu.removeAction(chat->toggleViewAction());
    m_chatList.removeOne(chat);
    delete chat;

    endRemoveRows();
}


Tchat * ChatList::getChat(const QString & uuid) const
{
    int maxChatIndex = m_chatList.size();
    for (int i = 0 ; i < maxChatIndex ; i++)
    {
        Tchat * chat = m_chatList.at(i);
        if (uuid == chat->identifiant())
            return chat;
    }
    return NULL;
}

Tchat * ChatList::getChat(const QModelIndex & index) const
{
    if (!index.isValid() || index.parent().isValid() || index.column() != 0)
        return NULL;

    int row = index.row();
    if (row < 0 || row >= m_chatList.size())
        return NULL;

    return m_chatList.at(row);
}

void ChatList::dispatchMessage(ReceiveEvent * event)
{
    DataReader & data = event->data();
    QString from = data.string8();
    QString to   = data.string8();
    QString msg  = data.string32();

    PlayersList & g_playersList = PlayersList::instance();

    QString chatUuid = to;
    if (to == g_playersList.localPlayer()->uuid())
        chatUuid = from;

    Tchat * chat = getChat(chatUuid);
    if (chat == NULL)
    {
        qWarning("Message for unknown chat %s", qPrintable(chatUuid));
        return;
    }
    
    Person * person = g_playersList.getPerson(from);
    if (person == NULL)
    {
        qWarning("Message from unknown person %s", qPrintable(from));
        return;
    }

    chat->afficherMessage(person->name(), person->color(), msg, (actionDiscussion)event->action());
}

void ChatList::addPlayerChat(Player * player, MainWindow * mainWindow)
{
    Tchat * chat = getChat(player->uuid());
    if (chat == NULL)
    {
        addChat(new Tchat(player, mainWindow));
    }
}

void ChatList::delPlayer(Player * player)
{
    Tchat * chat = getChat(player->uuid());
    if (chat != NULL)
    {
        delChat(chat);
    }
}

void ChatList::chatChanged(Tchat * chat)
{
    QModelIndex modelIndex = createIndex(m_chatList.indexOf(chat), 0);
    emit dataChanged(modelIndex, modelIndex);
}
