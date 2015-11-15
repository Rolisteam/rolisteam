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
#include "network/networkmessagereader.h"

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

    vmap->initScene();
}
void RGraphicsView::keyPressEvent ( QKeyEvent * event)
{
    QGraphicsView::keyPressEvent(event);
}
void RGraphicsView::mousePressEvent ( QMouseEvent * event)
{
	if(m_currentTool == VToolsBar::HANDLER)
	{
        if(!items(event->pos()).isEmpty())
		{
			setDragMode(QGraphicsView::NoDrag);
		}
        else if(event->modifiers() & Qt::ShiftModifier)
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
        }
		else
		{
			setDragMode(QGraphicsView::RubberBandDrag);
		}
	}
	QGraphicsView::mousePressEvent (event);
}
void RGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if(dragMode()==QGraphicsView::ScrollHandDrag)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void RGraphicsView::focusInEvent ( QFocusEvent * event )
{
    QGraphicsView::focusInEvent (event);
}
void RGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() & Qt::ShiftModifier)
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
void RGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
    if(m_vmap->isIdle())
    {
        QList<QGraphicsItem*> list = items(event->pos());
        if((list.isEmpty())||((list.size()==1)&&(list.contains(m_vmap->getFogItem()))))
        {
            QMenu menu;

            switch (m_vmap->getCurrentLayer())
            {
            case VisualItem::OBJECT:
                m_editObjectLayer->setChecked(true);
                break;
            case VisualItem::GROUND:
                m_editGroundLayer->setChecked(true);
                break;
            case VisualItem::CHARACTER_LAYER:
                m_editCharacterLayer->setChecked(true);
                break;
            }
            QMenu* editLayer = menu.addMenu(tr("Edit Layer"));
            editLayer->addAction(m_editGroundLayer);
            editLayer->addAction(m_editObjectLayer);
            editLayer->addAction(m_editCharacterLayer);

            QMenu* changeVibility = menu.addMenu(tr("Change Visibility"));
            changeVibility->addAction(m_hiddenVisibility);
            changeVibility->addAction(m_characterVisibility);
            changeVibility->addAction(m_allVisibility);


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



    //Layers
    QActionGroup* group = new QActionGroup(this);
    m_editGroundLayer= new QAction(tr("Ground"),this);
    m_editGroundLayer->setData(VisualItem::GROUND);
    m_editGroundLayer->setCheckable(true);
    m_editObjectLayer= new QAction(tr("Object"),this);
    m_editObjectLayer->setData(VisualItem::OBJECT);
    m_editObjectLayer->setCheckable(true);
    m_editCharacterLayer = new QAction(tr("Character"),this);
    m_editCharacterLayer->setData(VisualItem::CHARACTER_LAYER);
    m_editCharacterLayer->setCheckable(true);

    group->addAction(m_editGroundLayer);
    group->addAction(m_editObjectLayer);
    group->addAction(m_editCharacterLayer);



    connect(m_editGroundLayer,SIGNAL(triggered()),this,SLOT(changeLayer()));
    connect(m_editObjectLayer,SIGNAL(triggered()),this,SLOT(changeLayer()));
    connect(m_editCharacterLayer,SIGNAL(triggered()),this,SLOT(changeLayer()));


    QActionGroup* group2 = new QActionGroup(this);
    m_allVisibility = new QAction(tr("All"),this);
    m_allVisibility->setData(VMap::ALL);
    m_hiddenVisibility= new QAction(tr("Hidden"),this);
    m_hiddenVisibility->setData(VMap::HIDDEN);
    m_characterVisibility= new QAction(tr("Character"),this);
    m_characterVisibility->setData(VMap::CHARACTER);

    group2->addAction(m_allVisibility);
    group2->addAction(m_hiddenVisibility);
    group2->addAction(m_characterVisibility);

    connect(m_allVisibility,SIGNAL(triggered()),this,SLOT(changeVisibility()));
    connect(m_hiddenVisibility,SIGNAL(triggered()),this,SLOT(changeVisibility()));
    connect(m_characterVisibility,SIGNAL(triggered()),this,SLOT(changeVisibility()));

}
void RGraphicsView::showMapProperties()
{
    MapWizzardDialog m_propertiesDialog;

    m_propertiesDialog.updateDataFrom(m_vmap);
    if(m_propertiesDialog.exec())
    {
        m_propertiesDialog.setAllMap(m_vmap);

        sendOffMapChange();
    }
}
void RGraphicsView::sendOffMapChange()
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::vmapChanges);
    msg.string8(m_vmap->getId());
    m_vmap->fill(msg);
    msg.sendAll();
}

void RGraphicsView::changeLayer()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(m_vmap->editLayer((VisualItem::Layer)act->data().toInt()))
    {
        sendOffMapChange();
    }
}
void RGraphicsView::changeVisibility()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(m_vmap->setVisibilityMode((VMap::VisibilityMode)act->data().toInt()))
    {
        sendOffMapChange();
    }
}
void RGraphicsView::rubberBandGeometry(QRect viewportRect, QPointF fromScenePoint, QPointF toScenePoint)
{
	qDebug() << viewportRect << fromScenePoint << toScenePoint;
}

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
void RGraphicsView::currentToolChanged(VToolsBar::SelectableTool selectedtool)
{
	m_currentTool = selectedtool;
}
void RGraphicsView::resizeEvent(QResizeEvent* event)
{
    //GM is the references
   if((NULL!=scene())&&(m_vmap->isGM()))
   {
        scene()->setSceneRect(geometry());

       NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::GeometryViewChanged);
       msg.string8(m_vmap->getId());
       QRectF r = sceneRect();

       msg.real(r.x());
       msg.real(r.y());
       msg.real(r.width());
       msg.real(r.height());

       msg.sendAll();
   }
   QGraphicsView::resizeEvent(event);
}
void RGraphicsView::readMessage(NetworkMessageReader* msg)
{
    qreal x  = msg->real();
    qreal y  = msg->real();
    qreal width  = msg->real();
    qreal height  = msg->real();

    if(NULL!=scene())
    {
        setSceneRect(x,y,width,height);
    }
}
