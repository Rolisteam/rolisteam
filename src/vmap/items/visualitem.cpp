/***************************************************************************
    *      Copyright (C) 2010 by Renaud Guezennec                             *
    *                                                                         *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "visualitem.h"

#include <QGraphicsSceneHoverEvent>
#include <math.h>
#include <QCursor>
#include <QDebug>
#include <QMenu>
#include <QUuid>
#include <QKeyEvent>

#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

QStringList VisualItem::type2NameList =  QStringList() << tr("Path")<< tr("Line")<< tr("Ellipse")<< tr("Character")<< tr("Text")<< tr("Rect")<< tr("Rule")<< tr("Image");
QStringList VisualItem::s_layerName = QStringList() << tr("Ground")<< tr("Object")<< tr("Character");

VisualItem::VisualItem()
    : QGraphicsObject(),m_editable(false),m_child(NULL)
{
    m_id = QUuid::createUuid().toString();
    init();
}

VisualItem::VisualItem(QColor& penColor,bool b,QGraphicsItem * parent )
    : QGraphicsObject(parent),m_color(penColor),m_editable(b),m_child(NULL)
{
    m_id = QUuid::createUuid().toString();
    init();
}
VisualItem::~VisualItem()
{

}

void VisualItem::init()
{
    createActions();
    m_layer = VisualItem::GROUND;
    QActionGroup* group = new QActionGroup(this);
	m_putGroundLayer = new QAction(s_layerName[0],this);
    m_putGroundLayer->setData(VisualItem::GROUND);
	m_putObjectLayer = new QAction(s_layerName[1],this);
    m_putObjectLayer->setData(VisualItem::OBJECT);
	m_putCharacterLayer= new QAction(s_layerName[2],this);
    m_putCharacterLayer->setData(VisualItem::CHARACTER_LAYER);

    m_putGroundLayer->setCheckable(true);
    m_putObjectLayer->setCheckable(true);
    m_putCharacterLayer->setCheckable(true);

    m_putGroundLayer->setChecked(true);

    group->addAction(m_putGroundLayer);
    group->addAction(m_putObjectLayer);
    group->addAction(m_putCharacterLayer);

    setEditableItem(m_editable);
}
void VisualItem::setEditableItem(bool b)
{
    m_editable=b;
    if(m_editable)
    {
        /// @warning if two connected people have editable item, it will lead to endless loop.
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsGeometryChanges|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
        connect(this,SIGNAL(xChanged()),this,SLOT(sendPositionMsg()));
        connect(this,SIGNAL(yChanged()),this,SLOT(sendPositionMsg()));
        connect(this,SIGNAL(zChanged()),this,SLOT(sendPositionMsg()));
        //connect(this,SIGNAL(rotationChanged()),this,SLOT(sendPositionMsg()));
    }
    else
    {
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable);
    }
    if(NULL!=m_child)
    {
        foreach (ChildPointItem* itemChild, *m_child)
        {
            itemChild->setEditableItem(m_editable);
        }
    }
}

void VisualItem::setPenColor(QColor& penColor)
{
    m_color = penColor;
}
void VisualItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mouseMoveEvent(event);
}
void VisualItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
void VisualItem::keyPressEvent(QKeyEvent* event)
{
    if(event->key ()==Qt::Key_Delete)
    {
        emit itemRemoved(m_id);
    }
    QGraphicsItem::keyPressEvent(event);
}

void VisualItem::setId(QString id)
{
    m_id = id;
}
void VisualItem::resizeContents(const QRect& rect, bool keepRatio)
{
    if (!rect.isValid())
        return;
    prepareGeometryChange();
    int width = m_rect.width();
    int height = m_rect.height();
    m_rect = rect;
    if (keepRatio)
    {
        int hfw = height * rect.width() / width;
        if (hfw > 1)
        {
            m_rect.setTop(-hfw / 2);
            m_rect.setHeight(hfw);
        }
    }

    updateChildPosition();
}
void VisualItem::updateChildPosition()
{

}
void VisualItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    addActionContextMenu(&menu);
    QAction* backOrderAction = menu.addAction(tr("Back"));
    backOrderAction->setIcon(QIcon(":/resources/icons/action-order-back.png"));
    QAction* frontOrderAction = menu.addAction(tr("Front"));
    frontOrderAction->setIcon(QIcon(":/resources/icons/action-order-front.png"));
    QAction* lowerAction = menu.addAction(tr("Lower"));
    lowerAction->setIcon(QIcon(":/resources/icons/action-order-lower.png"));
    QAction* raiseAction = menu.addAction(tr("Raise"));
    raiseAction->setIcon(QIcon(":/resources/icons/action-order-raise.png"));
    menu.addSeparator();
    QAction* removeAction = menu.addAction(tr("Remove"));
    menu.addAction(m_duplicateAct);

    QMenu* rotationMenu = menu.addMenu(tr("Rotate"));
    QAction* resetRotationAct = rotationMenu->addAction(tr("To 360"));
    QAction* rightRotationAct =rotationMenu->addAction(tr("Right"));
    QAction* leftRotationAct =rotationMenu->addAction(tr("Left"));
    QAction* angleRotationAct =rotationMenu->addAction(tr("Set Angle…"));
    event->accept();

    if(!m_promoteTypeList.isEmpty())
    {
        QMenu* promoteMenu = menu.addMenu(tr("Promote to"));
        addPromoteItemMenu(promoteMenu);
    }


    QMenu* setLayerMenu = menu.addMenu(tr("Set Layer"));
    setLayerMenu->addAction(m_putGroundLayer);
    setLayerMenu->addAction(m_putObjectLayer);
    setLayerMenu->addAction(m_putCharacterLayer);

    switch (m_layer)
    {
    case OBJECT:
        m_putObjectLayer->setChecked(true);
        break;
    case GROUND:
        m_putGroundLayer->setChecked(true);
        break;
    case CHARACTER_LAYER:
        m_putCharacterLayer->setChecked(true);
        break;
    }

    QAction* selectedAction = menu.exec(event->screenPos());
    if(removeAction==selectedAction)
    {
        emit itemRemoved(m_id);
    }
    else if(resetRotationAct==selectedAction)
    {
        setRotation(0);
        endOfGeometryChange();
    }
    else if(selectedAction==rightRotationAct)
    {
        setRotation(90);
        endOfGeometryChange();
    }
    else if(selectedAction==leftRotationAct)
    {
        setRotation(270);
        endOfGeometryChange();
    }
    else if((selectedAction==m_putCharacterLayer)||(selectedAction==m_putObjectLayer)||(selectedAction==m_putCharacterLayer))
    {
        m_layer = (VisualItem::Layer)selectedAction->data().toInt();
        itemLayerChanged(this);
    }
}
void VisualItem::addPromoteItemMenu(QMenu* menu)
{
    foreach(ItemType type, m_promoteTypeList)
    {
        QAction* action = menu->addAction(type2NameList[type]);
        action->setData(type);
        connect(action,SIGNAL(triggered()),this,SLOT(promoteItem()));
    }
}
void VisualItem::promoteItem()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(NULL!=act)
    {
        VisualItem::ItemType type = (VisualItem::ItemType)act->data().toInt();
        emit promoteItemTo(this,type);
    }
}

void VisualItem::createActions()
{
    m_duplicateAct = new QAction(tr("Duplicate Item"),this);
    m_duplicateAct->setShortcut(QKeySequence("Ctrl+C"));
    connect(m_duplicateAct,SIGNAL(triggered()),this,SLOT(manageAction()),Qt::QueuedConnection);
}
void VisualItem::manageAction()
{
    QAction* tmp = qobject_cast<QAction*>(sender());
    if(m_duplicateAct == tmp)
    {
        emit duplicateItem(this);
    }
}
VisualItem::Layer VisualItem::getLayer()
{
    return m_layer;
}

void VisualItem::setLayer(VisualItem::Layer layer)
{
    m_layer = layer;
}

void VisualItem::addActionContextMenu(QMenu*)
{
    /// @brief must be implemented in child classes.
}

QString VisualItem::getId()
{
    return m_id;
}
bool VisualItem::hasFocusOrChild()
{
    if(isSelected())
    {
        return true;
    }
    else
    {
        if(m_child==NULL)
        {
            return false;
        }
        for(int i = 0; i< m_child->size();++i)
        {
            if((m_child->at(i)!=NULL)&&(m_child->at(i)->isSelected()))
            {
                return true;
            }
        }
    }
    return false;
}
void VisualItem::setId(QString id)
{
    m_id = id;
}

QString VisualItem::getId()
{
    return m_id;
}

void VisualItem::sendPositionMsg()
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::MoveItem);
    msg.string8(m_mapId);
    msg.string16(m_id);
    msg.real(pos().x());
    msg.real(pos().y());
    msg.real(zValue());
    msg.sendAll();
}
void VisualItem::readPositionMsg(NetworkMessageReader* msg)
{
    qreal x = msg->real();
    qreal y = msg->real();
    qreal z = msg->real();

    setPos(x,y);
    setZValue(z);
}

void VisualItem::setMapId(QString id)
{
    m_mapId = id;
}

QString VisualItem::getMapId()
{
    return m_mapId;
}









void VisualItem::sendPositionMsg()
{
    NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::MoveItem);
    msg.string8(m_mapId);
    msg.string16(m_id);
    msg.real(pos().x());
    msg.real(pos().y());
    msg.real(rotation());

    msg.real(zValue());
    msg.sendAll();
}
void VisualItem::readPositionMsg(NetworkMessageReader* msg)
{
    qreal x = msg->real();
    qreal y = msg->real();
    qreal z = msg->real();
    qreal rot = msg->real();

    setPos(x,y);
    setZValue(z);
    setRotation(rot);
}

void VisualItem::setMapId(QString id)
{
    m_mapId = id;
}

QString VisualItem::getMapId()
{
    return m_mapId;
}
void VisualItem::endOfGeometryChange()
{
    emit itemGeometryChanged(this);
}

void VisualItem::setModifiers(Qt::KeyboardModifiers modifiers)
{
    /// @brief must be implemented in child classes.
    return;
}
VisualItem* VisualItem::promoteTo(VisualItem::ItemType type)
{
    /// @brief must be implemented in child classes.
    return NULL;
}

void VisualItem::setChildrenVisible(bool b)
{
    if(NULL!=m_child)
    {
        foreach(ChildPointItem* item, *m_child)
        {
            if(!item->isVisionHandler())
            {
                item->setVisible(b);
            }
        }
    }
}
//friend functions
QDataStream& operator<<(QDataStream& os,const VisualItem& c)
{
    c.writeData(os);
    return os;
}
QDataStream& operator>>(QDataStream& is,VisualItem& c)
{
    c.readData(is);
    return is;
}
QString VisualItem::getLayerToText(VisualItem::Layer id)
{
	if(s_layerName.isEmpty())
	{
		return QString();
	}
	if(s_layerName.size()<(int)id)
	{
		return s_layerName.at(id);
	}
	return QString();
}
