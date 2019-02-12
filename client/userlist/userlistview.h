/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef USERLISTVIEW_H
#define USERLISTVIEW_H

#include <QTreeView>

class PlayersListWidgetModel;
class UserListDelegate;
class Person;

/**
 * @brief custom view to display tree person
 */
class UserListView : public QTreeView
{
    Q_OBJECT
public:
    /**
     * @brief default constructor
     */
    explicit UserListView(QWidget* parent= nullptr);

    virtual void setPlayersListModel(PlayersListWidgetModel* model);
signals:
    /**
     * @brief emited when user selects another item
     */
    void currentItemChanged(const QModelIndex&);
    /**
     * @brief emited when user doubleclicks on the color rectangle
     */
    void editCurrentItemColor();

public slots:
    /**
     * @brief called to change the current color
     */
    void onEditCurrentItemColor();
    /**
     * @brief requires context menu
     */
    void customContextMenuEvent(QPoint);

protected slots:
    /**
     * @brief defines new behaviours for mouseDoubleClickEvent
     */
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    /**
     * @brief mousePressEvent
     */
    virtual void mousePressEvent(QMouseEvent*);
    /**
     * @brief mouseMoveEvent
     * @param event
     */
    virtual void mouseMoveEvent(QMouseEvent* event);
    /**
     * @brief called when current item changed
     */
    void currentChanged(const QModelIndex& current, const QModelIndex& previous);

    void addAvatar();
    void deleteAvatar();

    QPixmap generateAvatar(Person* p);

private:
    /**
     * @brief pointer to the delegate
     */
    UserListDelegate* m_delegate;

    PlayersListWidgetModel* m_model;

    QAction* m_addAvatarAct;
    QAction* m_removeAvatarAct;
};

#endif // USERLISTVIEW_H
