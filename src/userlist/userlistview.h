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
class UserListView : public QTreeView
{
    Q_OBJECT
public:
    explicit UserListView(QWidget *parent = 0);


signals:
    void currentItemChanged(const QModelIndex &);
    void editCurrentItemColor();

public slots:
    void onEditCurrentItemColor();

protected slots:
    virtual void mouseDoubleClickEvent ( QMouseEvent * );
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    UserListDelegate* m_delegate;
};

#endif // USERLISTVIEW_H
