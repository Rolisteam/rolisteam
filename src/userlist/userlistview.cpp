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

#include <QHeaderView>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>
#include <QMenu>
#include <QFileDialog>
#include <QPainter>



#include <userlistdelegate.h>
#include "userlistview.h"
#include "userlistmodel.h"
#include "rolisteammimedata.h"

UserListView::UserListView(QWidget *parent) :
    QTreeView(parent)
{
    setHeaderHidden(true);
    m_delegate = new UserListDelegate(this);
    setContextMenuPolicy (Qt::CustomContextMenu);
    setItemDelegate(m_delegate);
    connect(this,SIGNAL(editCurrentItemColor()),this,SLOT(onEditCurrentItemColor()));
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuEvent(QPoint)));
    m_model = NULL;


    m_avatar = new QAction(tr("Set Avatar..."),this);
    connect(m_avatar,SIGNAL(triggered()),this,SLOT(onAvatar()));
    setIconSize(QSize(64,64));
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
    int icon = 0;
    if(iconSize().isValid())
        icon = iconSize().width();
    else
        icon = indentationValue;

    if(tmp.isValid())
    {
        event->pos();
        int depth = 1;
        while(tmp.parent().isValid())
        {
            depth++;
            tmp=tmp.parent();
        }
        if((depth*indentationValue<event->pos().x())&&((depth)*indentationValue+icon>=event->pos().x()))
        {
            emit editCurrentItemColor();
        }
        else
            QTreeView::mouseDoubleClickEvent(event);
    }
    else
        QTreeView::mouseDoubleClickEvent(event);
}
void UserListView::customContextMenuEvent ( QPoint e )
{
    QMenu popMenu(this);
    popMenu.addAction(m_avatar);
    /// @todo check if the position is a valid person (and belongs to the user)
    QModelIndex index = indexAt(e);
    if(m_model->isLocalPlayer(index))
    {
        m_avatar->setEnabled(true);
    }
    else
    {
        m_avatar->setEnabled(false);
    }
    popMenu.exec(mapToGlobal(e));

}
void UserListView::onAvatar()
{
    /// @TODO: Here! options manager is required to get access to the photo directory
    QString path = QFileDialog::getOpenFileName(this, tr("Avatar"),".",tr("Supported Image formats (*.jpg *.jpeg *.png *.bmp *.svg)"));
    QModelIndex index= currentIndex();
    PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
    QImage im(path);
    childItem->getPerson()->setAvatar(im);

}
void UserListView::onEditCurrentItemColor()
{
    QModelIndex index= currentIndex();
    PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());

    QColor color= QColorDialog::getColor(childItem->getPerson()->getColor(),this);

    if(color.isValid())
        childItem->getPerson()->setColor(color);
}
void UserListView::setModel(UserListModel *model)
{
    QTreeView::setModel(model);
    m_model = model;
}
void UserListView::mousePressEvent ( QMouseEvent * event)
{
    QModelIndex tmp = indexAt(event->pos());

    QTreeView::mousePressEvent(event);

    if ((event->button() == Qt::LeftButton) && (tmp.isValid()))
    {
            Person* tmpperso = m_model->getPersonAt(tmp);
            if(tmpperso->isLeaf())
            {
                QDrag *drag = new QDrag(this);
                RolisteamMimeData *mimeData = new RolisteamMimeData();

                mimeData->setPerson(tmpperso);
                drag->setMimeData(mimeData);
                drag->setPixmap(generateAvatar(tmpperso));

                Qt::DropAction dropAction = drag->exec();
            }
     }
}
QPixmap UserListView::generateAvatar(Person* p)
{
    int diameter = 80;
    QPixmap img(diameter,diameter);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QBrush brush;
    if(p->getAvatar().isNull())
    {
        painter.setPen(p->getColor());
        brush.setColor(p->getColor());
        brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        QImage img =p->getAvatar();
        brush.setTextureImage(img.scaled(diameter,diameter));
    }

    painter.setBrush(brush);
    painter.drawRoundedRect(0,0,diameter,diameter,diameter/10,diameter/10);

    return img;
}
