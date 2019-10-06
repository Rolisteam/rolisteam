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
#include <QSortFilterProxyModel>

class Player;
class PrivateChat;
/**
 * @brief The PrivateChatDialogModel class is the model which stored all opened chat room.
 */
class PrivateChatDialogModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    /**
     * @brief PrivateChatDialogModel
     * @param parent
     */
    PrivateChatDialogModel(QObject* parent= nullptr);
    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index, int role) const override;
    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    /**
     * @brief playersSet
     * @return
     */
    QSet<Player*>& playersSet();
    /**
     * @brief setPlayersSet
     * @param set
     */
    void setPlayersSet(const QSet<Player*>& set);
    /**
     * @brief setEditable
     * @param isEditable
     */
    void setEditable(bool isEditable);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QSet<Player*> m_set;
    bool m_isEditable;
};
/**
 * @brief The PrivateChatDialog class is the dialog box displayed when the user wants to create new chat room.
 */
class PrivateChatDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief PrivateChatDialog
     * @param parent
     */
    PrivateChatDialog(QWidget* parent= nullptr);
    /**
     * @brief sizeHint
     * @return
     */
    QSize sizeHint() const;

public slots:
    /**
     * @brief edit
     * @param chat
     * @return
     */
    int edit(PrivateChat* chat= nullptr);

private:
    QLineEdit* m_name_w;
    QLineEdit* m_owner_w;
    PrivateChatDialogModel m_model;
    QDialogButtonBox* m_buttonBox;
};

#endif
