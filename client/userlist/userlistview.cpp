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

#include <QHeaderView>
#include <QMouseEvent>
#include <QDebug>
#include <QColorDialog>
#include <QMenu>
#include <QFileDialog>
#include <QPainter>

#include <QDrag>

#include "userlistdelegate.h"
#include "userlistview.h"
#include "playersListWidget.h"
#include "playerslistproxy.h"

#include "data/character.h"
#include "rolisteammimedata.h"
#include "playersList.h"

UserListView::UserListView(QWidget *parent) :
    QTreeView(parent)
{
    setHeaderHidden(true);
    //m_delegate = new UserListDelegate(this);
    setContextMenuPolicy (Qt::CustomContextMenu);
    //setItemDelegate(m_delegate);
    connect(this,SIGNAL(editCurrentItemColor()),this,SLOT(onEditCurrentItemColor()));
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuEvent(QPoint)));
    
    
    m_addAvatarAct = new QAction(tr("Set Avatar..."),this);
    m_removeAvatarAct = new QAction(tr("Remove Avatar..."),this);
    connect(m_addAvatarAct,SIGNAL(triggered()),this,SLOT(addAvatar()));
    connect(m_removeAvatarAct,SIGNAL(triggered()),this,SLOT(deleteAvatar()));
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
    popMenu.addAction(m_addAvatarAct);
    popMenu.addAction(m_removeAvatarAct);
    QModelIndex index = indexAt(e);
    if(index.isValid())
    {
        QString uuid = index.data(PlayersList::IdentifierRole).toString();

        Person* tmpperso = PlayersList::instance()->getPerson(uuid);
        if(nullptr!=tmpperso)
        {
            if(PlayersList::instance()->isLocal(tmpperso))
            {
                m_addAvatarAct->setEnabled(true);
                m_removeAvatarAct->setEnabled(true);
            }
            else
            {
                m_addAvatarAct->setEnabled(false);
                m_removeAvatarAct->setEnabled(false);
            }
            popMenu.exec(mapToGlobal(e));
        }
    }
}
#include "preferences/preferencesmanager.h"
void UserListView::addAvatar()
{
    /// @TODO: Here! options manager is required to get access to the photo directory
    PreferencesManager* m_preferencesManager = PreferencesManager::getInstance();
    QString directory(".");
    if(nullptr != m_preferencesManager)
    {
        directory = m_preferencesManager->value("imageDirectory",QDir::homePath()).toString();
    }
    QString path = QFileDialog::getOpenFileName(this, tr("Avatar"),directory,tr("Supported Image formats (*.jpg *.jpeg *.png *.bmp *.svg)"));
    QModelIndex index= currentIndex();
    if(path.isEmpty())
        return;
    if(index.isValid())
    {
        QString uuid = index.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso = PlayersList::instance()->getPerson(uuid);
        QImage im(path);
        PlayersList::instance()->setLocalPersonAvatar(tmpperso,im);
        emit m_model->dataChanged(index,index);
    }    
}

void UserListView::deleteAvatar()
{
    QModelIndex index= currentIndex();
    if(index.isValid())
    {
        QString uuid = index.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso = PlayersList::instance()->getPerson(uuid);
        QImage im;
        PlayersList::instance()->setLocalPersonAvatar(tmpperso,im);
    }
}
void UserListView::onEditCurrentItemColor()
{
    QModelIndex index= currentIndex();

    if(!index.isValid())
        return;

    //QString uuid = index.data(PlayersList::IdentifierRole).toString();
    //Person* tmpperso = PlayersList::instance()->getPerson(uuid);

    QVariant valueVar = index.data(Qt::DecorationRole);

    QColor color= QColorDialog::getColor(valueVar.value<QColor>(),this);

    if(color.isValid())
    {
        model()->setData(index, QVariant(color), Qt::DecorationRole);
        //tmpperso->setColor(color);
    }

}
void UserListView::setPlayersListModel(PlayersListWidgetModel* model)
{
    QTreeView::setModel(model);
    m_model = model;
}
void UserListView::mousePressEvent ( QMouseEvent * event)
{
    //QModelIndex tmp = indexAt(event->pos());
    
    QTreeView::mousePressEvent(event);
    
    /*if ((event->button() == Qt::LeftButton) && (tmp.isValid()))
    {
        QString uuid = tmp.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso = PlayersList::instance()->getCharacter(uuid);
        if(nullptr!= tmpperso)
        {
            QDrag *drag = new QDrag(this);
            RolisteamMimeData *mimeData = new RolisteamMimeData();

            mimeData->setPerson(tmpperso);
            drag->setMimeData(mimeData);
            drag->setPixmap(generateAvatar(tmpperso));

            Qt::DropAction dropAction = drag->exec();
        }
    }*/
}
void UserListView::mouseMoveEvent ( QMouseEvent * event)
{
    QModelIndex tmp = indexAt(event->pos());

    QTreeView::mousePressEvent(event);

    if ((event->buttons() == Qt::LeftButton) && (tmp.isValid()))
    {
        QString uuid = tmp.data(PlayersList::IdentifierRole).toString();
        Person* tmpperso = PlayersList::instance()->getCharacter(uuid);
        if(nullptr!= tmpperso)
        {
            QDrag *drag = new QDrag(this);
            RolisteamMimeData *mimeData = new RolisteamMimeData();

            mimeData->setPerson(tmpperso);
            drag->setMimeData(mimeData);
            drag->setPixmap(generateAvatar(tmpperso));

            //Qt::DropAction dropAction =
            drag->exec();
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
