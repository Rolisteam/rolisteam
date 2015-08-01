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

#include <QDebug>

#include <QOpenGLWidget>

#include "data/person.h"
#include "rgraphicsview.h"
//#include "rolisteammimedata.h"

RGraphicsView::RGraphicsView(VMap *vmap)
    : QGraphicsView(vmap),m_vmap(vmap)
{
    setAcceptDrops(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setViewport(new QOpenGLWidget());
    fitInView(sceneRect(),Qt::KeepAspectRatio);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
    setRubberBandSelectionMode(Qt::IntersectsItemShape);
}
void RGraphicsView::keyPressEvent ( QKeyEvent * event)
{
 /*   if(event->key ()==Qt::Key_Delete)
    {
        QList<QGraphicsItem*> list= scene()->selectedItems();
        if(list.size()>0)
        {
            foreach(QGraphicsItem* tmp, list)
            {
                scene()->removeItem(tmp);
            }
        }

    }*/
    QGraphicsView::keyPressEvent(event);
}
void RGraphicsView::mousePressEvent ( QMouseEvent * event)
{
    QGraphicsView::mousePressEvent (event);
}
void RGraphicsView::focusInEvent ( QFocusEvent * event )
{
    QGraphicsView::focusInEvent (event);
}
void RGraphicsView::dragEnterEvent ( QDragEnterEvent * event )
{
  /*  const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(data)
    {
        if (data->hasFormat("rolisteam/userlist-item"))
        {
            event->acceptProposedAction();
        }
    }*/
    
}
void RGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void RGraphicsView::dropEvent ( QDropEvent * event )
{
  /*  const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(data)
    {
        if (data->hasFormat("rolisteam/userlist-item"))
        {
            const Person* item = data->getData();
            const Character* character = dynamic_cast<const Character*>(item);
            if(character)
            {
                m_map->addCharacter(character,mapToScene(event->pos()));
            }
        }
    }*/
    
}
