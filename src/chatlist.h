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
#include <QMenu>

class MainWindow;
class Player;
class ReceiveEvent;
class Tchat;

class ChatList
 : public QAbstractItemModel
{
    Q_OBJECT

    public:
        ChatList(MainWindow * mainWindow);
        ~ChatList();

        // implements QAbstractItemModel
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation,
                             int role = Qt::DisplayRole) const;
        virtual QModelIndex index(int row, int column,
                           const QModelIndex &parent = QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex &index) const;
        virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

        QMenu * chatMenu();

        // Event handler
        virtual bool event(QEvent * event);

    private:
        QList<Tchat *> m_chatList;
        QMenu          m_chatMenu;

        void addChat(Tchat * chat);
        void delChat(Tchat * chat);

        Tchat * getChat(const QString & uuid) const;
        Tchat * getChat(const QModelIndex & index) const;

        void dispatchMessage(ReceiveEvent * event);

    private slots:
        void addPlayerChat(Player * player, MainWindow * mainWindow = NULL);
        void delPlayer(Player * player);
        void chatChanged(Tchat * chat);
};

#endif
