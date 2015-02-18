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

#include "chat.h"
#include "networkmessagereader.h"
#include "persons.h"
#include "playersList.h"
#include "receiveevent.h"
#include "chatwindow.h"

extern bool G_client;

ChatList::ChatList(MainWindow * mainWindow)
    : QAbstractItemModel(NULL), m_chatWindowList(), m_chatMenu(),m_mainWindow(mainWindow)
{
    m_chatMenu.setTitle(tr("ChatWindows"));

    // main (public) chat
    addChatWindow(new ChatWindow(new PublicChat(), m_mainWindow));

    // Stay sync with g_playersList
    PlayersList * g_playersList = PlayersList::instance();
    connect(g_playersList, SIGNAL(playerAdded(Player *)), this, SLOT(addPlayerChat(Player *)));
    connect(g_playersList, SIGNAL(playerDeleted(Player *)), this, SLOT(delPlayer(Player *)));

    // Allready there player's chat
    int maxPlayerIndex = g_playersList->numPlayers();
    Player * localPlayer = g_playersList->localPlayer();
    for (int i = 0 ; i < maxPlayerIndex ; i++)
    {
        Player * player = g_playersList->getPlayer(i);
        if (player != localPlayer)
        {
            addPlayerChat(player);//m_mainWindow
        }
    }

    // Receive events
    ReceiveEvent::registerReceiver(NetMsg::ChatCategory, NetMsg::ChatMessageAction, this);
    ReceiveEvent::registerReceiver(NetMsg::ChatCategory, NetMsg::DiceMessageAction, this);
    ReceiveEvent::registerReceiver(NetMsg::ChatCategory, NetMsg::EmoteMessageAction, this);
    ReceiveEvent::registerReceiver(NetMsg::ChatCategory, NetMsg::UpdateChatAction, this);
    ReceiveEvent::registerReceiver(NetMsg::ChatCategory, NetMsg::DelChatAction, this);
}

ChatList::~ChatList()
{
    m_chatMenu.clear();

    int maxChatIndex = m_chatWindowList.size();
    for (int i = 0 ; i < maxChatIndex ; i++)
    {
        delete m_chatWindowList.at(i);
    }
}


QVariant ChatList::data(const QModelIndex &index, int role) const
{
    ChatWindow * chatw = chatWindow(index);

    if (chatw == NULL)
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
            return QVariant(chatw->chat()->name());
        case Qt::DecorationRole:
            return QVariant(QColor(chatw->hasUnseenMessage() ? "red" : "green"));
        case Qt::CheckStateRole:
            return QVariant(chatw->isVisible());
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
    if (parent.isValid() || column != 0 || row < 0 || row >= m_chatWindowList.size())
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

    return m_chatWindowList.size();
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

    ChatWindow * chatw = chatWindow(index);

    if (chatw == NULL)
        return false;

    switch (role)
    {
        case Qt::CheckStateRole:
            chatw->toggleViewAction()->toggle();
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

AbstractChat * ChatList::chat(const QModelIndex & index)
{
    ChatWindow * chatw = chatWindow(index);
    if (chatw == NULL)
        return NULL;
    return chatw->chat();
}

bool ChatList::addLocalChat(PrivateChat * chat)
{
    if (!chat->belongsToLocalPlayer())
        return false;

    if (!G_client)
        m_privateChatMap.insert(chat->identifier(), chat);
    addChatWindow(new ChatWindow(chat,m_mainWindow));
    return true;
}

bool ChatList::delLocalChat(const QModelIndex & index)
{
    ChatWindow * chatw =  chatWindow(index);
    PrivateChat * chat = qobject_cast<PrivateChat *>(chatw->chat());
    if (chat == NULL || !chat->belongsToLocalPlayer())
        return false;
    
    chat->sendDel();
    delChatWindow(chatw);
    if (!G_client)
    {
        m_privateChatMap.remove(chat->identifier());
        delete chat;
    }
    return true;
}

void ChatList::addChatWindow(ChatWindow * chatw)
{
    int listSize = m_chatWindowList.size();
    beginInsertRows(QModelIndex(), listSize, listSize);

    m_chatWindowList.append(chatw);
    m_chatMenu.addAction(chatw->toggleViewAction());
    connect(chatw, SIGNAL(changed(ChatWindow *)), this, SLOT(changeChatWindow(ChatWindow *)));

    endInsertRows();
}

void ChatList::delChatWindow(ChatWindow * chatw)
{
    int pos = m_chatWindowList.indexOf(chatw);
    if (pos < 0)
        return;

    beginRemoveRows(QModelIndex(), pos, pos);

    m_chatMenu.removeAction(chatw->toggleViewAction());
    m_chatWindowList.removeOne(chatw);
    delete chatw;

    endRemoveRows();
}


ChatWindow * ChatList::chatWindow(const QString & uuid) const
{
    int maxChatIndex = m_chatWindowList.size();
    for (int i = 0 ; i < maxChatIndex ; i++)
    {
        ChatWindow * chatw = m_chatWindowList.at(i);
        if (uuid == chatw->chat()->identifier())
            return chatw;
    }
    return NULL;
}

ChatWindow * ChatList::chatWindow(const QModelIndex & index) const
{
    if (!index.isValid() || index.parent().isValid() || index.column() != 0)
        return NULL;

    int row = index.row();
    if (row < 0 || row >= m_chatWindowList.size())
        return NULL;

    return m_chatWindowList.at(row);
}

void ChatList::addPlayerChat(Player * player)
{
    ChatWindow * chatw = chatWindow(player->uuid());
    if (chatw == NULL)
    {
        addChatWindow(new ChatWindow(new PlayerChat(player), m_mainWindow));
    }
}

void ChatList::delPlayer(Player * player)
{
    bool shouldReset = false;
    QMutableListIterator<ChatWindow *> i(m_chatWindowList);
    while (i.hasNext())
    {
        ChatWindow * chatw = i.next();
        if (chatw->chat()->belongsTo(player))
        {
            i.remove();
            chatw->deleteLater();
            shouldReset = true;
        }
    }
    if (shouldReset)
        reset();

    if (!G_client)
    {
        QMutableMapIterator<QString, PrivateChat *> i(m_privateChatMap);
        while (i.hasNext())
        {
            PrivateChat * chat = i.next().value();
            if (chat->belongsTo(player))
            {
                i.remove();
                chat->deleteLater();
            }
            else
            {
                chat->removePlayer(player);
            }
        }
    }
}

void ChatList::changeChatWindow(ChatWindow * chat)
{
    QModelIndex modelIndex = createIndex(m_chatWindowList.indexOf(chat), 0);
    emit dataChanged(modelIndex, modelIndex);
}

bool ChatList::event(QEvent * event)
{
    if (event->type() == ReceiveEvent::Type)
    {
        ReceiveEvent * netEvent = static_cast<ReceiveEvent *>(event);
        NetworkMessageReader & data = netEvent->data();

        if ( data.category() == NetMsg::ChatCategory)
        {
            NetMsg::Action action = data.action();
            switch (action)
            {
                case NetMsg::ChatMessageAction:
                case NetMsg::DiceMessageAction:
                case NetMsg::EmoteMessageAction:
                    dispatchMessage(netEvent);
                    return true;
                case NetMsg::UpdateChatAction:
                    updatePrivateChat(netEvent);
                    return true;
                case NetMsg::DelChatAction:
                    deletePrivateChat(netEvent);
                    return true;
                default:
                    qWarning("Unknown chat action %d", action);
            }
        }
    }

    return QObject::event(event);
}

void ChatList::dispatchMessage(ReceiveEvent * event)
{
    NetworkMessageReader & data = event->data();
    QString from = data.string8();
    QString to   = data.string8();
    QString msg  = data.string32();

    PlayersList* g_playersList = PlayersList::instance();

    Person * sender = g_playersList->getPerson(from);
    if (sender == NULL)
    {
        qWarning("Message from unknown person %s", qPrintable(from));
        return;
    }

    if (to == g_playersList->localPlayer()->uuid())
    {
        Player * owner = g_playersList->getParent(from);
        chatWindow(owner->uuid())->afficherMessage(sender->name(), sender->color(), msg, data.action());
        return;
    }

    else
    {
        Player * addressee = g_playersList->getPlayer(to);
        if (addressee != NULL)
        {
            if (!G_client)
                data.sendTo(addressee->link());
            return;
        }
    }

    ChatWindow * chatw = chatWindow(to);
    if (chatw != NULL)
        chatw->afficherMessage(sender->name(), sender->color(), msg, data.action());

    if (!G_client)
    {
        if (m_privateChatMap.contains(to))
            m_privateChatMap.value(to)->sendThem(data, event->link());
        else if (to.isEmpty())
            data.sendAll(event->link());
    }
}

void ChatList::updatePrivateChat(ReceiveEvent * event)
{
    PrivateChat * newChat = new PrivateChat(*event);
    if (newChat->identifier().isNull())
    {
        qWarning("Bad PrivateChat, removed");
        delete newChat;
        return;
    }

    if (!G_client)
    {
        if (m_privateChatMap.contains(newChat->identifier()))
        {
            PrivateChat * oldChat = m_privateChatMap[newChat->identifier()];
            oldChat->set(*newChat);
            delete newChat;
            newChat = oldChat;
        }
        else
        {
            m_privateChatMap.insert(newChat->identifier(), newChat);
            newChat->sendUpdate();
        }
    }

    ChatWindow * chatw = chatWindow(newChat->identifier());
    if (chatw != NULL)
    {
        if (newChat->includeLocalPlayer())
        {
            if (G_client)
            {
                PrivateChat * oldChat = qobject_cast<PrivateChat *>(chatw->chat());
                if (oldChat == NULL)
                {
                    qWarning("%s is not a private chat", qPrintable(newChat->identifier()));
                    return;
                }
                oldChat->set(*newChat, false);
                delete newChat;
            }
        }
        else
            delChatWindow(chatw);
    }

    else if (newChat->includeLocalPlayer())
    {
        addChatWindow(new ChatWindow(newChat,m_mainWindow));
    }


    else if (G_client)
        delete newChat;
}

void ChatList::deletePrivateChat(ReceiveEvent * event)
{
    QString uuid = event->data().string8();

    if (!G_client)
    {
        if (!m_privateChatMap.contains(uuid))
        {
            qWarning("Can't delete a unknown chat %s", qPrintable(uuid));
            return;
        }
        
        PrivateChat * chat = m_privateChatMap.value(uuid);
        if (!chat->sameLink(event->link()))
        {
            qWarning("Not alloawed to delete chat %s", qPrintable(uuid));
            return;
        }

        chat->sendDel();
        chat->deleteLater();
        m_privateChatMap.remove(uuid);
    }

    ChatWindow * chatw = chatWindow(uuid);
    if (chatw != NULL)
        delChatWindow(chatw);
}
