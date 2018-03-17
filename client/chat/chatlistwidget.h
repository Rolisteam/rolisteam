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


#ifndef CHATLISTWIDGET_H
#define CHATLISTWIDGET_H

#include <QDockWidget>
#include <QItemSelectionModel>
#include <QMenu>
#include <QPushButton>
#include <QListView>
#include <QSettings>

#include "network/networkreceiver.h"

class ChatList;
class MainWindow;
class PrivateChatDialog;
class DiceAlias;
/**
 * @brief The ChatListWidget class manages all chat windows and dice alias.
 */
class ChatListWidget : public QDockWidget, public NetWorkReceiver
{
    Q_OBJECT

public:
    ChatListWidget(MainWindow * parent=0);
    ~ChatListWidget();

    QMenu * chatMenu() const;

    // for connect, QObject is enought
    QObject * chatList() const;

    bool eventFilter(QObject * object, QEvent * event);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);

    void cleanChatList();

    void readSettings(QSettings &setting);
    void writeSettings(QSettings& setting);
public slots:
    void createPrivateChat();
	void addPublicChat();
    /**
     * @brief rollDiceCmd into the global  chat
     * @param cmd
     * @param owner
     */
    void rollDiceCmd(QString cmd,QString owner, bool alias);
private slots:
    void selectAnotherChat(const QModelIndex & index);
    void editChat(const QModelIndex & index);
    void deleteSelectedChat();
    void updateAllUnreadChat();
    void processAddDiceAlias(NetworkMessageReader* msg);
    void processRemoveDiceALias(NetworkMessageReader* msg);
    void processMoveDiceALias(NetworkMessageReader* msg);



private:
	ChatList* m_chatList;
    PrivateChatDialog * m_privateChatDialog;
    QItemSelectionModel * m_selectionModel;
    QPushButton * m_deleteButton;
    QListView* m_listView;
    QList<DiceAlias*>* m_diceAliasMapFromGM;
};

#endif
