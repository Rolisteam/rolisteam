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
#include <QDebug>
#include <QTimer>

#include "chat/chat.h"
#include "chatwindow.h"
#include "data/person.h"
#include "data/player.h"
#include "mainwindow.h"
#include "network/networkmessagereader.h"
#include "network/receiveevent.h"
#include "preferences/preferencesmanager.h"
#include "userlist/playersList.h"

BlinkingDecorationDelegate::BlinkingDecorationDelegate()
{
    m_timer= new QTimer(this);
    m_timer->start(1000);
    connect(m_timer, SIGNAL(timeout()), this, SIGNAL(refresh()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeOutCount()));
    m_red= true;
}

void BlinkingDecorationDelegate::paint(
    QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    QVariant var= index.data(Qt::DecorationRole);
    QColor color= var.value<QColor>();
    if((color.red() == 255) && (!m_red))
    {
        QStyleOptionViewItemV4 opt= option;
        QStyledItemDelegate::initStyleOption(&opt, index);
        QStyle* style= opt.widget ? opt.widget->style() : QApplication::style();

        QRect rect= style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt);

        painter->fillRect(rect, QColor(Qt::darkGreen));
    }
}
void BlinkingDecorationDelegate::timeOutCount()
{
    m_red= !m_red;
}
////////////////////////////////
// Code for ChatList class
////////////////////////////////
ChatList::ChatList(MainWindow* mainWindow) : QAbstractItemModel(nullptr), m_chatMenu(), m_mainWindow(mainWindow)
{
    m_chatMenu.setTitle(tr("ChatWindows"));

    // Stay sync with g_playersList
    PlayersList* playersList= PlayersList::instance();
    connect(playersList, &PlayersList::playerAdded, this, &ChatList::addPlayerChat);
    connect(playersList, &PlayersList::playerDeleted, this, &ChatList::deletePlayerChat);

    // Allready there player's chat
    int maxPlayerIndex = playersList->getPlayerCount();
    Player* localPlayer= playersList->getLocalPlayer();
    for(int i= 0; i < maxPlayerIndex; i++)
    {
        Player* player= playersList->getPlayer(i);
        if(player != localPlayer)
        {
            addPlayerChat(player); // m_mainWindow
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
    std::for_each(m_data.begin(), m_data.end(),
        [this](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) { pair.first->deleteLater(); });
    m_data.clear();
}
void ChatList::addPublicChat()
{
    // main (public) chat
    auto chat= new ChatWindow(new PublicChat(), m_mainWindow);
    for(auto& diceBookmark : m_pairList)
    {
        chat->appendDiceShortCut(diceBookmark);
    }
    addChatWindow(chat);
}
void ChatList::readSettings(QSettings& settings)
{
    int size= settings.beginReadArray(QStringLiteral("dicebookmarks"));
    m_pairList.clear();
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        DiceShortCut cut;
        cut.setText(settings.value(QStringLiteral("title")).toString());
        cut.setCommand(settings.value(QStringLiteral("command")).toString());
        cut.setAlias(settings.value(QStringLiteral("alias")).toBool());
        m_pairList.push_back(cut);
    }
    settings.endArray();
}
void ChatList::writeSettings(QSettings& settings)
{
    if(m_data.empty())
        return;

    auto chat= m_data.begin()->second;

    std::vector<DiceShortCut> pairList= chat->getDiceShortCuts();
    settings.beginWriteArray(QStringLiteral("dicebookmarks"));
    int i= 0;
    for(auto& pair : pairList)
    {
        settings.setArrayIndex(i);
        settings.setValue(QStringLiteral("title"), pair.text());
        settings.setValue(QStringLiteral("command"), pair.command());
        settings.setValue(QStringLiteral("alias"), pair.alias());
        ++i;
    }
    settings.endArray();
}
void ChatList::rollDiceCmd(QString cmd, QString owner, bool alias)
{
    if(m_data.empty())
        return;

    ChatWindow* wid= m_data.begin()->second;
    if(nullptr != wid)
    {
        wid->rollDiceCmd(cmd, owner, alias);
    }
}

void ChatList::rollDiceCmdForCharacter(QString cmd, QString uuid, bool alias)
{
    if(m_data.empty())
        return;

    ChatWindow* wid= m_data.begin()->second;
    if(nullptr != wid)
    {
        wid->rollDiceCmdForCharacter(cmd, uuid, alias);
    }
}

bool ChatList::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Q_UNUSED(value)

    QMdiSubWindow* chatw= getChatSubWindowByIndex(index);
    if(chatw == nullptr)
        return false;

    switch(role)
    {
    default:
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::DecorationRole:
        return false;
    case Qt::CheckStateRole:
    {
        bool visible= chatw->isVisible();
        chatw->widget()->setVisible(!visible);
        chatw->setVisible(!visible);
    }
        return true;
    }
}

QVariant ChatList::data(const QModelIndex& index, int role) const
{
    QMutexLocker locker(&m_mutex);
    auto pair            = getPairByIndex(index);
    QMdiSubWindow* chatw2= pair.first;
    ChatWindow* chatw    = pair.second;

    if(chatw == nullptr)
        return QVariant();

    if(chatw->chat() == nullptr)
        return QVariant();

    switch(role)
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

Qt::ItemFlags ChatList::flags(const QModelIndex& index) const
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

QModelIndex ChatList::index(int row, int column, const QModelIndex& parent) const
{
    if(parent.isValid() || column != 0 || row < 0 || row >= static_cast<int>(m_data.size()))
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex ChatList::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ChatList::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_data.size());
}

int ChatList::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return 1;
}

QMenu* ChatList::chatMenu()
{
    return &m_chatMenu;
}

AbstractChat* ChatList::chat(const QModelIndex& index)
{
    ChatWindow* chatw= getChatWindowByIndex(index);
    if(chatw == nullptr)
        return nullptr;
    return chatw->chat();
}

bool ChatList::addLocalChat(PrivateChat* chat)
{
    if(!chat->belongsToLocalPlayer())
        return false;

    addChatWindow(new ChatWindow(chat, m_mainWindow));
    return true;
}

bool ChatList::delLocalChat(const QModelIndex& index)
{
    ChatWindow* chatw= getChatWindowByIndex(index);
    PrivateChat* chat= qobject_cast<PrivateChat*>(chatw->chat());
    if(chat == nullptr || !chat->belongsToLocalPlayer())
        return false;

    chat->sendDel();
    delChatWindow(chatw);
    /// @warning dead code
    /*if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
        m_privateChatMap.remove(chat->identifier());
        delete chat;
    }*/
    return true;
}

void ChatList::addChatWindow(ChatWindow* chatw)
{
    if(!chatw)
        return;

    // TODO remove that
    auto window= qobject_cast<QMdiSubWindow*>(m_mainWindow->registerSubWindow(chatw, chatw->toggleViewAction()));

    if(!window)
        return;
    window->setObjectName("MdiChatWindow");

    int insertPos= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), insertPos, insertPos);

    m_chatMenu.addAction(chatw->toggleViewAction());

    connect(chatw, SIGNAL(ChatWindowHasChanged(ChatWindow*)), this, SLOT(changeChatWindow(ChatWindow*)));
    connect(chatw->chat(), SIGNAL(changedName(QString)), window, SLOT(setWindowTitle(QString)));

    m_data.push_back({window, chatw});
    endInsertRows();

    if((window->height() < 451) || (window->width() < 264))
    {
        window->resize(264, 451);
    }

    /*connect(window, &QMdiSubWindow::destroyed, this, [this]() {
        qDebug() << "mdi subwindow destroyed";
        auto window= qobject_cast<QMdiSubWindow*>(sender());
        auto pairIt= std::find_if(m_data.begin(), m_data.end(),
            [window](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) { return pair.first == window; });
        auto idx   = static_cast<int>(std::distance(m_data.begin(), pairIt));
        beginRemoveRows(QModelIndex(), idx, idx);
        m_data.erase(pairIt);
        endRemoveRows();
    });*/

    /*connect(chatw, &ChatWindow::destroyed, this, [this]() {
        qDebug() << "chatwindow destroyed";
        auto chat  = qobject_cast<ChatWindow*>(sender());
        auto pairIt= std::find_if(m_data.begin(), m_data.end(),
            [chat](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) { return pair.second == chat; });
        auto idx   = static_cast<int>(std::distance(m_data.begin(), pairIt));
        beginRemoveRows(QModelIndex(), idx, idx);
        m_data.erase(pairIt);
        endRemoveRows();
    });*/

    if(nullptr != window)
    {
        // TODO remove that
        chatw->setSubWindow(window);
        window->setWindowTitle(QStringLiteral("%1 (%2)").arg(chatw->getTitleFromChat(), tr("Chat", "chat room")));
        window->setWindowIcon(QIcon(":/chat.png"));
        chatw->setLocalPlayer(PlayersList::instance()->getLocalPlayer());
        window->setAttribute(Qt::WA_DeleteOnClose, false);
        chatw->setAttribute(Qt::WA_DeleteOnClose, false);
        window->setVisible(chatw->toggleViewAction()->isChecked());
    }
}

void ChatList::delChatWindow(ChatWindow* chatw)
{
    QMutexLocker locker(&m_mutex);
    auto it= std::find_if(m_data.begin(), m_data.end(),
        [chatw](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) { return pair.second == chatw; });

    if(it == m_data.end())
        return;

    m_chatMenu.removeAction(chatw->toggleViewAction());
    auto index = static_cast<int>(std::distance(m_data.begin(), it));
    auto window= it->first;
    beginRemoveRows(QModelIndex(), index, index);
    m_data.erase(it);
    endRemoveRows();

    window->setVisible(false);
    window->deleteLater();
}
void ChatList::cleanChat()
{
    QMutexLocker locker(&m_mutex);
    beginResetModel();
    m_chatMenu.clear();
    for(auto& pair : m_data)
    {
        pair.first->deleteLater();
    }
    m_data.clear();
    endResetModel();
}

ChatWindow* ChatList::getChatWindowByUuid(const QString& uuid) const
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [uuid](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) {
        auto window= pair.second;
        if(!window)
            return false;
        auto chat= window->chat();
        if(!chat)
            return false;
        return uuid == chat->identifier();
    });

    if(it == m_data.end())
        return nullptr;

    return it->second;
}

const std::pair<QMdiSubWindow*, ChatWindow*> ChatList::getPairByIndex(const QModelIndex& index) const
{
    if(!index.isValid() || index.parent().isValid() || index.column() != 0)
        return std::make_pair<QMdiSubWindow*, ChatWindow*>(nullptr, nullptr);

    size_t row= static_cast<size_t>(index.row());
    if(row >= m_data.size())
        return std::make_pair<QMdiSubWindow*, ChatWindow*>(nullptr, nullptr);

    return m_data[row];
}

ChatWindow* ChatList::getChatWindowByIndex(const QModelIndex& index) const
{
    auto pair= getPairByIndex(index);
    return pair.second;
}
QMdiSubWindow* ChatList::getChatSubWindowByIndex(const QModelIndex& index) const
{
    auto pair= getPairByIndex(index);
    return pair.first;
}
void ChatList::addPlayerChat(Player* player)
{
    if(player != PlayersList::instance()->getLocalPlayer())
    {
        ChatWindow* chatw= getChatWindowByUuid(player->getUuid());
        if(chatw == nullptr)
        {
            addChatWindow(new ChatWindow(new PlayerChat(player), m_mainWindow));
        }
    }
}

void ChatList::deletePlayerChat(Player* player)
{
    QMutexLocker locker(&m_mutex);
    std::vector<std::pair<QMdiSubWindow*, ChatWindow*>> toRemove;
    std::copy_if(m_data.begin(), m_data.end(), std::back_inserter(toRemove),
        [player](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) {
            auto widget= pair.second;
            if(!widget)
                return false;

            auto chat= widget->chat();
            if(!chat)
                return false;

            return chat->belongsTo(player);
        });

    Q_ASSERT(toRemove.size() < 2);

    std::for_each(toRemove.begin(), toRemove.end(), [this](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) {
        auto it= std::find_if(
            m_data.begin(), m_data.end(), [pair](const std::pair<QMdiSubWindow*, ChatWindow*>& dataPair) {
                return (pair.first == dataPair.first && pair.second == dataPair.second);
            });
        if(it == m_data.end())
            return;
        auto index= static_cast<int>(std::distance(m_data.begin(), it));
        beginRemoveRows(QModelIndex(), index, index);
        auto size= m_data.size();
        m_data.erase(it);
        Q_ASSERT(size > m_data.size());
        endRemoveRows();
        pair.first->deleteLater();
    });
}

void ChatList::changeChatWindow(ChatWindow* chat)
{
    auto it= std::find_if(m_data.begin(), m_data.end(),
        [chat](const std::pair<QMdiSubWindow*, ChatWindow*>& pair) { return (chat == pair.second); });
    if(it == m_data.end())
        return;

    auto index= static_cast<int>(std::distance(m_data.begin(), it));

    auto modelIndex= createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
}

bool ChatList::event(QEvent* event)
{
    if(event->type() == ReceiveEvent::Type)
    {
        ReceiveEvent* netEvent    = static_cast<ReceiveEvent*>(event);
        NetworkMessageReader& data= netEvent->data();

        if(data.category() == NetMsg::ChatCategory)
        {
            NetMsg::Action action= data.action();
            switch(action)
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

void ChatList::dispatchMessage(ReceiveEvent* event)
{
    NetworkMessageReader& data= event->data();
    QString from              = data.string8();
    QString to                = data.string8();
    QString msg               = data.string32();
    QString comment;
    if(data.action() == NetMsg::DiceMessageAction)
    {
        comment= data.string32();
    }

    PlayersList* playersList= PlayersList::instance();

    Person* sender= playersList->getPerson(from);
    if(sender == nullptr)
    {
        qWarning("Message from unknown person %s", qPrintable(from));
        return;
    }

    if(to == playersList->getLocalPlayer()->getUuid())
    { // to one person
        ChatWindow* win= getChatWindowByUuid(from);
        if((nullptr == win) && (nullptr != sender->parentPerson()))
        {
            win= getChatWindowByUuid(sender->parentPerson()->getUuid());
        }
        if(nullptr != win)
        {
            win->showMessage(sender->name(), sender->getColor(), msg, comment, data.action());
        }
        return;
    }

    ChatWindow* chatw= getChatWindowByUuid(to);
    if(nullptr != chatw)
    {
        chatw->showMessage(sender->name(), sender->getColor(), msg, comment, data.action());
    }
}

void ChatList::updatePrivateChat(ReceiveEvent* event)
{
    PrivateChat* newChat= new PrivateChat(*event);
    if(newChat->identifier().isNull())
    {
        qWarning("Bad PrivateChat, removed");
        delete newChat;
        return;
    }
    /// @warning dead code
    /*  if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
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
    }*/

    ChatWindow* chatw= getChatWindowByUuid(newChat->identifier());
    if(chatw != nullptr)
    {
        if(newChat->includeLocalPlayer())
        {
            PrivateChat* oldChat= qobject_cast<PrivateChat*>(chatw->chat());
            if(oldChat == nullptr)
            {
                qWarning("%s is not a private chat", qPrintable(newChat->identifier()));
                return;
            }
            oldChat->set(*newChat, false);
            delete newChat;
        }
        else
            delChatWindow(chatw);
    }

    else if(newChat->includeLocalPlayer())
    {
        addChatWindow(new ChatWindow(newChat, m_mainWindow));
    }
    else
    {
        delete newChat;
    }
}

void ChatList::deletePrivateChat(ReceiveEvent* event)
{
    QString uuid= event->data().string8();
    /// @warning dead code
    /*if (!PreferencesManager::getInstance()->value("isClient",true).toBool())
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
    }*/

    ChatWindow* chatw= getChatWindowByUuid(uuid);
    if(chatw != nullptr)
        delChatWindow(chatw);
}
void ChatList::updateDiceAliases(QList<DiceAlias*>* map)
{
    for(auto& tmp : m_data)
    {
        if(!tmp.second)
            continue;
        tmp.second->updateDiceAliases(map);
    }
}
