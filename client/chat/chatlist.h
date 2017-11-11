/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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


#ifndef CHATLIST_H
#define CHATLIST_H

#include <QAbstractItemModel>
#include <QMdiSubWindow>
#include <QMap>
#include <QMenu>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QSettings>

class AbstractChat;
class ChatWindow;
class MainWindow;
class Player;
class PrivateChat;
class ReceiveEvent;
class DiceAlias;
/**
 * @brief The BlinkingDecorationDelegate class is dedicated to show to user when chat has unread message.
 */
class BlinkingDecorationDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    BlinkingDecorationDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
public slots:
    void timeOutCount();

signals:
    void refresh();
private:
    QTimer* m_timer;
    bool m_red;
    QList<const QModelIndex> m_list;
};


/**
 * @brief The ChatList class
 */
class ChatList : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief ChatList
     * @param mainWindow
     */
    ChatList(MainWindow * mainWindow=0);

    ~ChatList();

    // implements QAbstractItemModel
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

    QMenu * chatMenu();

    AbstractChat * chat(const QModelIndex & index);
    /**
     * @brief addLocalChat
     * @param chat
     * @return
     */
    bool addLocalChat(PrivateChat * chat);
    /**
     * @brief delLocalChat
     * @param index
     * @return
     */
    bool delLocalChat(const QModelIndex & index);

    // Event handler
    virtual bool event(QEvent * event);

    void updateDiceAliases(QList<DiceAlias*>* map);

    void cleanChat();
    void readSettings(QSettings& settings);
    void writeSettings(QSettings& settings);
public slots:
	void addPublicChat();
    void rollDiceCmd(QString, QString);

private:
    /**
     * @brief dispatchMessage
     * @param event
     */
    void dispatchMessage(ReceiveEvent * event);
    /**
     * @brief updatePrivateChat
     * @param event
     */
    void updatePrivateChat(ReceiveEvent * event);
    /**
     * @brief deletePrivateChat
     * @param event
     */
    void deletePrivateChat(ReceiveEvent * event);

    void addChatWindow(ChatWindow * chat);
    void delChatWindow(ChatWindow * chat);

    ChatWindow * getChatWindowByUuid(const QString & uuid) const;
    ChatWindow * getChatWindowByIndex(const QModelIndex & index) const;
    QMdiSubWindow * getChatSubWindowByIndex(const QModelIndex & index) const;


private slots:
    /**
     * @brief addPlayerChat
     * @param player
     */
    void addPlayerChat(Player * player);
    /**
     * @brief delPlayer
     * @param player
     */
    void delPlayer(Player * player);
    /**
     * @brief changeChatWindow
     * @param chat
     */
    void changeChatWindow(ChatWindow * chat);

private:
    QList<QMdiSubWindow*> m_chatSubWindowList;
    QList<ChatWindow *> m_chatWindowList;
    QMap<QString, PrivateChat *> m_privateChatMap;
    QMenu m_chatMenu;
    MainWindow * m_mainWindow;
    std::vector<std::pair<QString,QString>> m_pairList;
};

#endif
