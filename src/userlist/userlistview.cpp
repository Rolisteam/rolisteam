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
#include "userlistview.h"
#include <QHeaderView>
#include "userlistmodel.h"
#include <userlistdelegate.h>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>

UserListView::UserListView(QWidget *parent) :
    QTreeView(parent)
{
    setHeaderHidden(true);
    m_delegate = new UserListDelegate(this);
    setItemDelegate(m_delegate);
    connect(this,SIGNAL(editCurrentItemColor()),this,SLOT(onEditCurrentItemColor()));
}
void UserListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    emit currentItemChanged(current);
}
void  UserListView::mouseDoubleClickEvent ( QMouseEvent * event)
{
    QModelIndex tmp = indexAt(event->pos());
    int indentationValue = indentation();
    if(tmp.isValid())
    {
        event->pos();
        int depth = 1;
        while(tmp.parent().isValid())
        {
            depth++;
            tmp=tmp.parent();
        }
        //if the click is on the color icon.
        if((depth*indentationValue+indentationValue<event->pos().x())&&((++depth)*indentationValue+indentationValue>event->pos().x()))
        {
            emit editCurrentItemColor();
        }
        else
            QTreeView::mouseDoubleClickEvent(event);

    }
    else
        QTreeView::mouseDoubleClickEvent(event);
}
void UserListView::onEditCurrentItemColor()
{
    QModelIndex index= currentIndex();
    PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());



    QColor color= QColorDialog::getColor(childItem->getPerson()->getColor(),this);

    if(color.isValid())
        childItem->getPerson()->setColor(color);

}
