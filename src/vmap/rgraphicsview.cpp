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
#include "data/character.h"
#include "rgraphicsview.h"
#include "userlist/rolisteammimedata.h"

#include "network/networkmessagewriter.h"

RGraphicsView::RGraphicsView(VMap *vmap)
    : QGraphicsView(vmap),m_vmap(vmap)
{
    m_counterZoom = 0;

    setAcceptDrops(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setViewport(new QOpenGLWidget());
    fitInView(sceneRect(),Qt::KeepAspectRatio);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);

    createAction();
}
void RGraphicsView::keyPressEvent ( QKeyEvent * event)
{

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
void RGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() & Qt::ControlModifier)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Scale the view / do the zoom
        double scaleFactor = 1.1;

        if((event->delta() > 0)&&(m_counterZoom<20))
        {
            scale(scaleFactor, scaleFactor);
            ++m_counterZoom;
        }
        else if(m_counterZoom>-20)
        {
            --m_counterZoom;
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        ;
    }
    else
        QGraphicsView::wheelEvent(event);
}
void RGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QList<QGraphicsItem*> list = items(event->pos());
    if(list.isEmpty())
    {
        QMenu menu;
        menu.addAction(m_zoomInMax);
        menu.addAction(m_zoomNormal);
        menu.addAction(m_zoomOutMax);
        menu.addSeparator();
        menu.addAction(m_properties);
        menu.exec(event->globalPos());
    }
    else
    {
        QGraphicsView::contextMenuEvent(event);
    }
}
void RGraphicsView::createAction()
{
    //ZOOM MANAGEMENT
    m_zoomNormal= new QAction(tr("Zoom to Normal"),this);
    m_zoomInMax= new QAction(tr("Zoom In Max"),this);
    m_zoomOutMax = new QAction(tr("Zoom Out Max"),this);

    connect(m_zoomNormal,SIGNAL(triggered()),this,SLOT(setZoomFactor()));
    connect(m_zoomInMax,SIGNAL(triggered()),this,SLOT(setZoomFactor()));
    connect(m_zoomOutMax,SIGNAL(triggered()),this,SLOT(setZoomFactor()));

    addAction(m_zoomNormal);
    addAction(m_zoomInMax);
    addAction(m_zoomOutMax);

    //PROPERTIES
    m_properties = new QAction(tr("Properties"),this);
    connect(m_properties,SIGNAL(triggered()),this,SLOT(showMapProperties()));

}
void RGraphicsView::showMapProperties()
{
    MapWizzardDialog m_propertiesDialog;

    m_propertiesDialog.updateDataFrom(m_vmap);
    if(m_propertiesDialog.exec())
    {
        m_propertiesDialog.setAllMap(m_vmap);

        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::vmapChanges);
        m_vmap->fill(msg);
        msg.sendAll();
    }

}

/*void RGraphicsView::dragEnterEvent ( QDragEnterEvent * event )
{
    const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(data)
    {
        if (data->hasFormat("rolisteam/userlist-item"))
        {
            event->acceptProposedAction();
        }
    }
    
}*/

void RGraphicsView::setZoomFactor()
{
    QAction* senderAct = qobject_cast<QAction*>(sender());
    int destination = 0;
    int step = 0;
    if(senderAct == m_zoomInMax)
    {
        destination = 20;
        step = 1;
    }
    else if(senderAct == m_zoomNormal)
    {
        destination = 0;
        if(m_counterZoom > 0)
        {
            step = -1;
        }
        else
        {
            step = 1;
        }
    }
    else if(senderAct == m_zoomOutMax)
    {
        destination = -20;
        step = -1;
    }
    double scaleFactor = 1.1;
    double realFactor = 1.0;
    while( destination != m_counterZoom)
    {
        if(step>0)
        {
            realFactor = realFactor*scaleFactor;
        }
        else
        {
            realFactor = realFactor * (1.0 / scaleFactor);
        }
        m_counterZoom += step;
    }
    scale(realFactor,realFactor);
}

/*void RGraphicsView::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void RGraphicsView::dropEvent ( QDropEvent * event )
{
    const RolisteamMimeData* data= qobject_cast<const RolisteamMimeData*>(event->mimeData());
    if(data)
    {
        if (data->hasFormat("rolisteam/userlist-item"))
        {
            Person* item = data->getData();
            Character* character = dynamic_cast<Character*>(item);
            if(character)
            {
                m_vmap->addCharacter(character,mapToScene(event->pos()));
            }
        }
    }
    
}*/
