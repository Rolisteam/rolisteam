/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     Copyright (C) 2014 by Renaud Guezennec                            *
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
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QSet>

#include "userlist/playerslistproxy.h"

class Player;
class PrivateChat;
/**
 * @brief The PrivateChatDialogModel class
 */
class PrivateChatDialogModel : public PlayersListProxyModel
{
    Q_OBJECT

    public:
        PrivateChatDialogModel(QObject * parent = 0);

        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);

        QSet<Player *> & playersSet();
        void setPlayersSet(const QSet<Player *> & set);
        void setEditable(bool isEditable);

    private:
       QSet<Player *> m_set;
       bool m_isEditable;
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
        QLineEdit * m_owner_w;
        PrivateChatDialogModel m_model;
        QDialogButtonBox * m_buttonBox;
};

#endif
