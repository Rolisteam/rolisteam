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

    if(NULL!=m_vmap)
    {
        connect(m_vmap,SIGNAL(mapChanged()),this,SLOT(sendOffMapChange()));
    }
    setAcceptDrops(true);
    m_preferences = PreferencesManager::getInstance();
//    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setViewport(new QOpenGLWidget());
    fitInView(sceneRect(),Qt::KeepAspectRatio);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

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
       /* else if(event->modifiers() & Qt::ShiftModifier)
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
        }*/
		else
		{
			setDragMode(QGraphicsView::RubberBandDrag);
		}
	}
	QGraphicsView::mousePressEvent (event);
}
void RGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    m_lastPoint = QPoint();
    QGraphicsView::mouseReleaseEvent(event);
    if(dragMode()==QGraphicsView::ScrollHandDrag)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}
void RGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if((VToolsBar::HANDLER == m_currentTool)&&
       (event->modifiers() & Qt::ShiftModifier)&&
       (event->buttons() & Qt::LeftButton))
    {
        if(!m_lastPoint.isNull())
        {
            QRectF rect = sceneRect();
            int dx = event->x() - m_lastPoint.x();
            int dy = event->y() - m_lastPoint.y();
            rect.translate(-dx,-dy);
            m_lastPoint = event->pos();
            setSceneRect(rect);
        }
        m_lastPoint = event->pos();
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
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
        setResizeAnchor(QGraphicsView::AnchorUnderMouse);
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

    }
    else
        QGraphicsView::wheelEvent(event);
}
void RGraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
    if(m_vmap->isIdle())
    {
        QList<QGraphicsItem*> list = scene()->selectedItems();

        //QList<QGraphicsItem*> list = items(event->pos());
        QMenu menu;
        if((list.isEmpty())||((list.size()==1)&&(list.contains(m_vmap->getFogItem()))))
        {
            menu.setTitle(tr("Change the map"));

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
            menu.addAction(m_importImage);
            menu.addSeparator();
            menu.addAction(m_properties);
            menu.exec(event->globalPos());
        }
        else if (list.size() > 1)
        {
            menu.setTitle(tr("Change selected Items"));

            QAction* removeAction = menu.addAction(tr("Remove"));

            QAction* backOrderAction = menu.addAction(tr("Back"));
            backOrderAction->setIcon(QIcon(":/resources/icons/action-order-back.png"));
            backOrderAction->setData(VisualItem::BACK);

            QAction* frontOrderAction = menu.addAction(tr("Front"));
            frontOrderAction->setIcon(QIcon(":/resources/icons/action-order-front.png"));
            frontOrderAction->setData(VisualItem::FRONT);

            QAction* lowerAction = menu.addAction(tr("Lower"));
            lowerAction->setIcon(QIcon(":/resources/icons/action-order-lower.png"));
            lowerAction->setData(VisualItem::LOWER);

            QAction* raiseAction = menu.addAction(tr("Raise"));
            raiseAction->setIcon(QIcon(":/resources/icons/action-order-raise.png"));
            raiseAction->setData(VisualItem::RAISE);

            QMenu* rotationMenu = menu.addMenu(tr("Rotate"));
            QAction* resetRotationAct = rotationMenu->addAction(tr("To 360"));
            QAction* rightRotationAct = rotationMenu->addAction(tr("Right"));
            QAction* leftRotationAct =  rotationMenu->addAction(tr("Left"));
            QAction* angleRotationAct = rotationMenu->addAction(tr("Set Angle…"));

            QMenu* setLayerMenu = menu.addMenu(tr("Set Layer"));
            setLayerMenu->addAction(m_putGroundLayer);
            setLayerMenu->addAction(m_putObjectLayer);
            setLayerMenu->addAction(m_putCharacterLayer);

            QAction* selectedAction = menu.exec(event->globalPos());

            if(removeAction==selectedAction)
            {
                deleteItem(list);
            }
            else if(resetRotationAct==selectedAction)
            {
                setRotation(list,0);
            }
            else if(selectedAction==rightRotationAct)
            {
                setRotation(list,90);
            }
            else if(selectedAction==leftRotationAct)
            {
                setRotation(list,270);
            }
            else if(selectedAction==angleRotationAct)
            {
                int angle = QInputDialog::getInt(this,tr("Rotation Value ?"),tr("Please, set the rotation angle you want [0-360]"),0,0,360);
                setRotation(list,angle);
            }
            else if((backOrderAction == selectedAction)||(frontOrderAction == selectedAction)||(lowerAction == selectedAction)||(raiseAction == selectedAction))
            {
                changeZValue(list,(VisualItem::StackOrder)selectedAction->data().toInt());
            }
            else if((selectedAction==m_putCharacterLayer)||(selectedAction==m_putObjectLayer)||(selectedAction==m_putGroundLayer))
            {
                setItemLayer(list,(VisualItem::Layer)selectedAction->data().toInt());
            }
        }
        else
        {
            QGraphicsView::contextMenuEvent(event);
        }
    }
}
void RGraphicsView::setRotation(QList<QGraphicsItem*> list, int value)
{
    foreach(QGraphicsItem* item, list)
    {
        item->setRotation(value);
    }
}
void RGraphicsView::setItemLayer(QList<QGraphicsItem*> list,VisualItem::Layer layer)
{
    foreach(QGraphicsItem* item, list)
    {
        VisualItem* vItem = dynamic_cast<VisualItem*>(item);
        if((vItem != NULL))
        {
            vItem->setLayer(layer);
        }
    }
}
void RGraphicsView::deleteItem(QList<QGraphicsItem*> list)
{
    foreach(QGraphicsItem* item, list)
    {
        if(NULL!=m_vmap)
        {
            m_vmap->removeItem(item);
        }
    }
}
void RGraphicsView::changeZValue(QList<QGraphicsItem*> list,VisualItem::StackOrder order)
{
    foreach(QGraphicsItem* item, list)
    {
        VisualItem* vItem = dynamic_cast<VisualItem*>(item);
        if((NULL!=m_vmap)&&(vItem != NULL))
        {
            m_vmap->changeStackOrder(vItem,order);
        }
    }
}

void RGraphicsView::createAction()
{
    //ZOOM MANAGEMENT
    m_zoomNormal= new QAction(tr("Zoom to Normal"),this);
    m_zoomInMax= new QAction(tr("Zoom In Max"),this);
    m_zoomOutMax = new QAction(tr("Zoom Out Max"),this);

    m_importImage = new QAction(tr("Import Image"),this);

    connect(m_zoomNormal,SIGNAL(triggered()),this,SLOT(setZoomFactor()));
    connect(m_zoomInMax,SIGNAL(triggered()),this,SLOT(setZoomFactor()));
    connect(m_zoomOutMax,SIGNAL(triggered()),this,SLOT(setZoomFactor()));
    connect(m_importImage,SIGNAL(triggered()),this,SLOT(addImageToMap()));

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

    m_putGroundLayer = new QAction(tr("Ground"),this);
    m_putGroundLayer->setData(VisualItem::GROUND);
    m_putObjectLayer = new QAction(tr("Object"),this);
    m_putObjectLayer->setData(VisualItem::OBJECT);
    m_putCharacterLayer= new QAction(tr("Character"),this);
    m_putCharacterLayer->setData(VisualItem::CHARACTER_LAYER);

    connect(m_editGroundLayer,SIGNAL(triggered()),this,SLOT(changeLayer()));
    connect(m_editObjectLayer,SIGNAL(triggered()),this,SLOT(changeLayer()));
    connect(m_editCharacterLayer,SIGNAL(triggered()),this,SLOT(changeLayer()));

    QActionGroup* group2 = new QActionGroup(this);
    m_allVisibility = new QAction(tr("All"),this);
    m_allVisibility->setCheckable(true);
    m_allVisibility->setData(VMap::ALL);
    m_hiddenVisibility= new QAction(tr("Hidden"),this);
    m_hiddenVisibility->setCheckable(true);
    m_hiddenVisibility->setChecked(true);
    m_hiddenVisibility->setData(VMap::HIDDEN);
    m_characterVisibility= new QAction(tr("Fog Of War"),this);
    m_characterVisibility->setData(VMap::CHARACTER);
    m_characterVisibility->setCheckable(true);

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
    //qDebug() << viewportRect << fromScenePoint << toScenePoint;
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
   if((NULL!=scene())&&(m_vmap->getOption(VisualItem::LocalIsGM).toBool()))
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
void RGraphicsView::addImageToMap()
{
    QString imageToLoad = QFileDialog::getOpenFileName(this,tr("Open image file"),
                                 m_preferences->value("ImageDirectory",QDir::homePath()).toString(),
                                 m_preferences->value("ImageFileFilter","*.jpg *.jpeg *.png *.bmp *.svg").toString());

    if(NULL!=m_vmap)
    {
        m_vmap->addImageItem(imageToLoad);
    }
}
