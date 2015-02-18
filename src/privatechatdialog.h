/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
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


#ifndef PRIVATE_CHAT_DIALOG_H
#define PRIVATE_CHAT_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSet>

#include "playerslistproxy.h"

class Player;
class PrivateChat;

class PrivateChatDialogModel : public PlayersListProxyModel
{
    Q_OBJECT

    public:
        PrivateChatDialogModel(const QSet<Player *> & set, QObject * parent = 0);

        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);

        QSet<Player *> & playersSet();
        void setPlayersSet(const QSet<Player *> & set);

    private:
       QSet<Player *> m_set;
};

class PrivateChatDialog : public QDialog
{
    Q_OBJECT

    public:
        PrivateChatDialog(QWidget * parent = NULL);

        QSize sizeHint() const;

    public slots:
        int edit(PrivateChat * chat = NULL);

    private:
        QLineEdit * m_name_w;
        PrivateChatDialogModel m_model;
};

#endif
