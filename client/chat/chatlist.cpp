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
#include <QApplication>

#include "chat/chat.h"
#include "network/networkmessagereader.h"
#include "data/person.h"
#include "data/player.h"
#include "userlist/playersList.h"
#include "network/receiveevent.h"
#include "chatwindow.h"
#include "mainwindow.h"
#include "preferences/preferencesmanager.h"

BlinkingDecorationDelegate::BlinkingDecorationDelegate()
{
    m_timer = new QTimer(this);
    m_timer->start(1000);
    connect(m_timer,SIGNAL(timeout()),this,SIGNAL(refresh()));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(timeOutCount()));
    m_red=true;
}

void BlinkingDecorationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);
    QVariant var = index.data(Qt::DecorationRole);
    QColor color = var.value<QColor>();
    if((color.red()==255)&&(!m_red))
    {
       QStyleOptionViewItemV4 opt = option;
       QStyledItemDelegate::initStyleOption(&opt, index);
       QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

       QRect rect = style->subElementRect( QStyle::SE_ItemViewItemDecoration ,&opt);

       painter->fillRect(rect,QColor(Qt::darkGreen));

    }
}
void BlinkingDecorationDelegate::timeOutCount()
{
    m_red=!m_red;
}
////////////////////////////////
// Code for ChatList class
////////////////////////////////
ChatList::ChatList(MainWindow * mainWindow)
    : QAbstractItemModel(nullptr), m_chatWindowList(), m_chatMenu(),m_mainWindow(mainWindow)
{
    m_chatMenu.setTitle(tr("ChatWindows"));

	// Stay sync with g_playersList
	PlayersList * g_playersList = PlayersList::instance();
	connect(g_playersList, SIGNAL(playerAdded(Player *)), this, SLOT(addPlayerChat(Player *)));
	connect(g_playersList, SIGNAL(playerDeleted(Player *)), this, SLOT(delPlayer(Player *)));

	// Allready there player's chat
	int maxPlayerIndex = g_playersList->getPlayerCount();
    Player * localPlayer = g_playersList->getLocalPlayer();
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
void ChatList::addPublicChat()
{
	// main (public) chat
    auto chat = new ChatWindow(new PublicChat(), m_mainWindow);
    for(auto& diceBookmark : m_pairList)
    {
        chat->appendDiceShortCut(diceBookmark);
    }
    addChatWindow(chat);
}
void ChatList::readSettings(QSettings& settings)
{
    int size = settings.beginReadArray(QStringLiteral("dicebookmarks"));
    m_pairList.clear();
    for(int i = 0; i<size; ++i)
    {
        settings.setArrayIndex(i);
        QString title =  settings.value(QStringLiteral("title")).toString();
        QString cmd = settings.value(QStringLiteral("command")).toString();
        m_pairList.push_back(std::pair<QString,QString>(title,cmd));
    }
    settings.endArray();
}
void ChatList::writeSettings(QSettings& settings)
{
    if(!m_chatWindowList.isEmpty())
    {
        auto chat = m_chatWindowList.first();
        std::vector<std::pair<QString,QString>> pairList = chat->getDiceShortCuts();
        settings.beginWriteArray(QStringLiteral("dicebookmarks"));
        int i = 0;
        for(auto& pair : pairList)
        {
            settings.setArrayIndex(i);
            settings.setValue(QStringLiteral("title"),pair.first);
            settings.setValue(QStringLiteral("command"),pair.second);
            ++i;
        }
        settings.endArray();
    }
}
void ChatList::rollDiceCmd(QString cmd, QString owner)
{
    if(!m_chatWindowList.isEmpty())
    {
        ChatWindow* wid = m_chatWindowList.first();
        if(nullptr!=wid)
        {
            wid->rollDiceCmd(cmd,owner);
        }
    }
}

bool ChatList::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QMdiSubWindow* chatw = getChatSubWindowByIndex(index);
    if (chatw == nullptr)
        return false;

    switch (role)
    {
    default:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::DecorationRole:
        return false;
    case Qt::CheckStateRole:
    {
        bool visible = chatw->isVisible();
        chatw->widget()->setVisible(!visible);
        chatw->setVisible(!visible);

    }
        return true;
    }



}

QVariant ChatList::data(const QModelIndex &index, int role) const
{
    QMdiSubWindow * chatw2 = getChatSubWindowByIndex(index);
    ChatWindow * chatw = getChatWindowByIndex(index);

    if (chatw == nullptr)
        return QVariant();

    if (chatw->chat() == nullptr)
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return QVariant(chatw->chat()->name());
    case Qt::DecorationRole:
        return QVariant(QColor(chatw->hasUnseenMessage() ? "red" : "green"));
    case Qt::CheckStateRole:
        return QVariant(chatw2->isVisible());
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

QMenu * ChatList::chatMenu()
{
    return &m_chatMenu;
}

AbstractChat * ChatList::chat(const QModelIndex & index)
{
    ChatWindow * chatw = getChatWindowByIndex(index);
    if (chatw == nullptr)
        return nullptr;
    return chatw->chat();
}

bool ChatList::addLocalChat(PrivateChat * chat)
{
    if (!chat->belongsToLocalPlayer())
        return false;

    if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
        m_privateChatMap.insert(chat->identifier(), chat);
    addChatWindow(new ChatWindow(chat,m_mainWindow));
    return true;
}

bool ChatList::delLocalChat(const QModelIndex & index)
{
    ChatWindow * chatw =  getChatWindowByIndex(index);
    PrivateChat * chat = qobject_cast<PrivateChat *>(chatw->chat());
    if (chat == nullptr || !chat->belongsToLocalPlayer())
        return false;
    
    chat->sendDel();
    delChatWindow(chatw);
    if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        m_privateChatMap.remove(chat->identifier());
        delete chat;
    }
    return true;
}

void ChatList::addChatWindow(ChatWindow* chatw)
{
    connect(m_mainWindow, SIGNAL(closing()), chatw, SLOT(save()));
    int listSize = m_chatWindowList.size();
    beginInsertRows(QModelIndex(), listSize, listSize);


    m_chatMenu.addAction(chatw->toggleViewAction());
    connect(chatw, SIGNAL(ChatWindowHasChanged(ChatWindow *)), this, SLOT(changeChatWindow(ChatWindow *)));
    connect(m_mainWindow, SIGNAL(closing()), chatw, SLOT(save()));

    QMdiSubWindow* subWindowChat = static_cast<QMdiSubWindow*>(m_mainWindow->registerSubWindow(chatw,chatw->toggleViewAction()));

    connect(chatw->chat(), SIGNAL(changedName(QString)), subWindowChat, SLOT(setWindowTitle(QString)));
    m_chatWindowList.append(chatw);
    m_chatSubWindowList.append(subWindowChat);

    if((subWindowChat->height()<451)||(subWindowChat->width()<264))
    {
        subWindowChat->resize(264,451);
    }

    if(nullptr!=subWindowChat)
    {
        chatw->setSubWindow(subWindowChat);
        subWindowChat->setWindowTitle(tr("%1 (Chat)").arg(chatw->getTitleFromChat()));
        subWindowChat->setWindowIcon(QIcon(":/chat.png"));
        chatw->setLocalPlayer(PlayersList::instance()->getLocalPlayer());
        subWindowChat->setAttribute(Qt::WA_DeleteOnClose, false);
        chatw->setAttribute(Qt::WA_DeleteOnClose, false);
        subWindowChat->setVisible(chatw->toggleViewAction()->isChecked());
    }
    endInsertRows();
}

void ChatList::delChatWindow(ChatWindow* chatw)
{
    int pos = m_chatWindowList.indexOf(chatw);
    if (pos < 0)
        return;

    beginRemoveRows(QModelIndex(), pos, pos);

    m_chatMenu.removeAction(chatw->toggleViewAction());
    m_chatWindowList.removeOne(chatw);

    QMdiSubWindow* subWindow = m_chatSubWindowList.takeAt(pos);
    subWindow->setVisible(false);

    delete chatw;
    delete subWindow;

    endRemoveRows();
}
void ChatList::cleanChat()
{
    beginResetModel();
    m_chatMenu.clear();
    m_chatWindowList.clear();
    for(auto chatw : m_chatSubWindowList)
    {
        chatw->setVisible(false);

    }
    qDeleteAll(m_chatSubWindowList.begin(),m_chatSubWindowList.end());
    m_chatSubWindowList.clear();
    qDeleteAll(m_chatWindowList.begin(),m_chatWindowList.end());
    m_chatWindowList.clear();


    endResetModel();
}

ChatWindow* ChatList::getChatWindowByUuid(const QString & uuid) const
{
    int maxChatIndex = m_chatWindowList.size();
    for (int i = 0 ; i < maxChatIndex ; i++)
    {
        ChatWindow * chatw = m_chatWindowList.at(i);
        if (uuid == chatw->chat()->identifier())
            return chatw;
    }
    return nullptr;
}

ChatWindow * ChatList::getChatWindowByIndex(const QModelIndex & index) const
{
    if (!index.isValid() || index.parent().isValid() || index.column() != 0)
        return nullptr;

    int row = index.row();
    if (row < 0 || row >= m_chatWindowList.size())
        return nullptr;

    return m_chatWindowList.at(row);
}
QMdiSubWindow * ChatList::getChatSubWindowByIndex(const QModelIndex & index) const
{
    if (!index.isValid() || index.parent().isValid() || index.column() != 0)
        return nullptr;

    int row = index.row();
    if (row < 0 || row >= m_chatSubWindowList.size())
        return nullptr;

    return m_chatSubWindowList.at(row);
}
void ChatList::addPlayerChat(Player * player)
{
    if(player != PlayersList::instance()->getLocalPlayer())
    {
        ChatWindow * chatw = getChatWindowByUuid(player->getUuid());
        if (chatw == nullptr)
        {
            addChatWindow(new ChatWindow(new PlayerChat(player), m_mainWindow));
        }
    }
}


void ChatList::delPlayer(Player * player)
{
    foreach (QMdiSubWindow* tmp, m_chatSubWindowList)
    {
        ChatWindow* chatw = static_cast<ChatWindow*>(tmp->widget());
        if((nullptr!=chatw) && (chatw->chat()->belongsTo(player)))
        {
            int pos = m_chatWindowList.indexOf(chatw);
            if (pos >= 0)
            {
                beginRemoveRows(QModelIndex(), pos, pos);
                m_chatSubWindowList.removeOne(tmp);
                m_chatWindowList.removeOne(chatw);
                chatw->deleteLater();
                endRemoveRows();
                delete tmp;
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
    QString comment  = data.string32();

    PlayersList* playersList = PlayersList::instance();

    Person* sender = playersList->getPerson(from);
    if (sender == nullptr)
    {
        qWarning("Message from unknown person %s", qPrintable(from));
        return;
    }

    if (to == playersList->getLocalPlayer()->getUuid())
    {//to one person
       ChatWindow* win = getChatWindowByUuid(from);
       if((nullptr==win) && (nullptr!=sender->getParent()))
       {
            win = getChatWindowByUuid(sender->getParent()->getUuid());
       }
       if(nullptr!=win)
       {
            win->showMessage(sender->getName(), sender->getColor(), msg,comment, data.action());
       }
       return;
    }
    else//Global
    {
        Player * addressee = playersList->getPlayer(to);
        if (addressee != nullptr)
        {
            if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
                data.sendTo(addressee->link());
            return;
        }
    }

    ChatWindow * chatw = getChatWindowByUuid(to);
    if (nullptr != chatw)
    {
        chatw->showMessage(sender->getName(), sender->getColor(), msg,comment, data.action());
    }

    if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
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

    if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
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

    ChatWindow * chatw = getChatWindowByUuid(newChat->identifier());
    if (chatw != nullptr)
    {
        if (newChat->includeLocalPlayer())
        {
            if (PreferencesManager::getInstance()->value("isClient",true).toBool())
            {
                PrivateChat * oldChat = qobject_cast<PrivateChat *>(chatw->chat());
                if (oldChat == nullptr)
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
    else if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        delete newChat;
    }
}

void ChatList::deletePrivateChat(ReceiveEvent * event)
{
    QString uuid = event->data().string8();

    if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
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

    ChatWindow * chatw = getChatWindowByUuid(uuid);
    if (chatw != nullptr)
        delChatWindow(chatw);
}
void ChatList::updateDiceAliases(QList<DiceAlias*>* map)
{
    foreach(ChatWindow* tmp, m_chatWindowList)
    {
        tmp->updateDiceAliases(map);
    }

}
