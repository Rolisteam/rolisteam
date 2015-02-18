/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
class UserListModel;
class UserListDelegate;
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
    explicit UserListView(QWidget *parent = 0);


signals:
    /**
      * @brief emited when user selects another item
      */
    void currentItemChanged(const QModelIndex &);
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
    void customContextMenuEvent ( QPoint);

protected slots:
    /**
      * @brief defines new behaviours for mouseDoubleClickEvent
      */
    virtual void mouseDoubleClickEvent ( QMouseEvent * );
    /**
      * @brief called when current item changed
      */
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    /**
      * @brief pointer to the delegate
      */
    UserListDelegate* m_delegate;
};

#endif // USERLISTVIEW_H
