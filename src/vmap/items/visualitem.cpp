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


#include "map/map.h"

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
    m_propertiesHash = NULL;
    m_resizing = false;
    m_rotating = false;
    m_layer = VisualItem::NONE;
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
        connect(this,SIGNAL(xChanged()),this,SLOT(posChange()));
        connect(this,SIGNAL(yChanged()),this,SLOT(posChange()));
        connect(this,SIGNAL(zChanged()),this,SLOT(sendZValueMsg()));
        //connect(this,SIGNAL(heightChanged()),this,SLOT(sendRectGeometryMsg()));
        //connect(this,SIGNAL(widthChanged()),this,SLOT(sendRectGeometryMsg()));
        connect(this,SIGNAL(heightChanged()),this,SLOT(rectChange()));
        connect(this,SIGNAL(widthChanged()),this,SLOT(rectChange()));
        connect(this,SIGNAL(rotationChanged()),this,SLOT(rotationChange()));//sendRotationMsg
        connect(this,SIGNAL(opacityChanged()),this,SLOT(sendOpacityMsg()));
    }
    else
    {
        setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsFocusable);
        disconnect(this,SIGNAL(xChanged()),this,SLOT(posChange()));
        disconnect(this,SIGNAL(yChanged()),this,SLOT(posChange()));
        disconnect(this,SIGNAL(zChanged()),this,SLOT(sendZValueMsg()));
        disconnect(this,SIGNAL(widthChanged()),this,SLOT(rectChange()));
        disconnect(this,SIGNAL(heightChanged()),this,SLOT(rectChange()));
        disconnect(this,SIGNAL(rotationChanged()),this,SLOT(rotationChange()));
        disconnect(this,SIGNAL(opacityChanged()),this,SLOT(sendOpacityMsg()));
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
void VisualItem::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
void VisualItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    update();
    QGraphicsItem::mouseMoveEvent(event);
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
    sendPositionMsg();
}
void VisualItem::keyPressEvent(QKeyEvent* event)
{

    if((event->key ()==Qt::Key_Delete)&&(m_editable)&&(isSelected()))
    {
        emit itemRemoved(m_id);
    }
    else if((event->key() == Qt::Key_C)&&(event->modifiers() == Qt::ControlModifier)&&(m_editable)&&(isSelected()))
    {
        emit duplicateItem(this);
    }
    QGraphicsItem::keyPressEvent(event);
}

void VisualItem::setId(QString id)
{
    m_id = id;
}

void VisualItem::resizeContents(const QRectF& rect, bool keepRatio)
{
    if (!rect.isValid())
        return;
    prepareGeometryChange();
    int width = m_rect.width();
    int height = m_rect.height();
    //sendRectGeometryMsg();
    m_resizing = true;
    //.normalized()
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
    if((m_propertiesHash->contains(VisualItem::LocalIsGM))&&(m_propertiesHash->value(VisualItem::LocalIsGM).toBool() == false))
    {
        return;
    }
    QMenu menu;
    addActionContextMenu(&menu);
    QAction* backOrderAction = menu.addAction(tr("Back"));
    backOrderAction->setIcon(QIcon(":/resources/icons/action-order-back.png"));
    connect(backOrderAction,&QAction::triggered,[&](){
        emit changeStackPosition(this,VisualItem::BACK);
    });


    QAction* frontOrderAction = menu.addAction(tr("Front"));
    frontOrderAction->setIcon(QIcon(":/resources/icons/action-order-front.png"));
    connect(frontOrderAction,&QAction::triggered,[&](){
        emit changeStackPosition(this,VisualItem::FRONT);
    });


    QAction* lowerAction = menu.addAction(tr("Lower"));
    lowerAction->setIcon(QIcon(":/resources/icons/action-order-lower.png"));
    connect(lowerAction,&QAction::triggered,[&](){
        emit changeStackPosition(this,VisualItem::LOWER);
    });

    QAction* raiseAction = menu.addAction(tr("Raise"));
    raiseAction->setIcon(QIcon(":/resources/icons/action-order-raise.png"));
    connect(raiseAction,&QAction::triggered,[&](){
        emit changeStackPosition(this,VisualItem::RAISE);
    });


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
        m_rotating = true;
        endOfGeometryChange();
    }
    else if(selectedAction==rightRotationAct)
    {
        setRotation(90);
        m_rotating = true;
        endOfGeometryChange();
    }
    else if(selectedAction==leftRotationAct)
    {
        setRotation(270);
        m_rotating = true;
        endOfGeometryChange();
    }
    else if((selectedAction==m_putCharacterLayer)||(selectedAction==m_putObjectLayer)||(selectedAction==m_putGroundLayer))
    {
        setLayer((VisualItem::Layer)selectedAction->data().toInt());
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
    if(m_layer!=layer)
    {
        m_layer = layer;
        sendItemLayer();
    }
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
    if((getOption(VisualItem::LocalIsGM).toBool())||((!getOption(VisualItem::LocalIsGM).toBool())&&(getOption(VisualItem::PermissionMode).toInt()==Map::PC_ALL)))
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
    }
    return false;
}
void VisualItem::sendItemLayer()
{
    if(getOption(VisualItem::LocalIsGM).toBool() ||
            getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL)//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::LayerItemChanged);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.uint8(m_layer);
        msg.sendAll();
    }
}

void VisualItem::sendOpacityMsg()
{
    if(getOption(VisualItem::LocalIsGM).toBool() ||
            getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL)//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::OpacityItemChanged);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(opacity());
        msg.sendAll();
    }
}
void VisualItem::readOpacityMsg(NetworkMessageReader* msg)
{
    qreal opa = msg->real();
    blockSignals(true);
    setOpacity(opa);
    blockSignals(false);
}
void VisualItem::readLayerMsg(NetworkMessageReader* msg)
{
    quint8 lay = msg->uint8();
    blockSignals(true);
    setLayer((VisualItem::Layer)lay);
    blockSignals(false);
}
bool VisualItem::isLocal()
{
    return false;
}
void VisualItem::posChange()
{
    m_pointList.append(pos());
}
void VisualItem::rectChange()
{
    m_resizing = true;//not used
}
void VisualItem::rotationChange()
{
    m_rotating = true;
}

void VisualItem::sendPositionMsg()
{
    if((getOption(VisualItem::LocalIsGM).toBool()) ||
            (getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL) ||
            ((getOption(VisualItem::PermissionMode).toInt() == Map::PC_MOVE)&&
             (getType() == VisualItem::CHARACTER)&&
             (isLocal())))//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::MoveItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.uint64(m_pointList.size());
        for(auto point : m_pointList)
        {
            msg.real(point.x());
            msg.real(point.y());
        }
        msg.sendAll();
    }
}
void VisualItem::readPositionMsg(NetworkMessageReader* msg)
{
    quint64 size = msg->uint64();
    for(quint64 i = 0; i < size;++i)
    {
        qreal x = msg->real();
        qreal y = msg->real();
        blockSignals(true);
        setPos(x,y);
        blockSignals(false);
    }
    update();
}
void VisualItem::sendZValueMsg()
{
    if((getOption(VisualItem::LocalIsGM).toBool()) ||
            (getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL) ||
            ((getOption(VisualItem::PermissionMode).toInt() == Map::PC_MOVE)&&
             (getType() == VisualItem::CHARACTER)&&
             (isLocal())))//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::ZValueItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(zValue());
        msg.sendAll();
    }
}
void VisualItem::readZValueMsg(NetworkMessageReader* msg)
{
    qreal z = msg->real();
    blockSignals(true);
    setZValue(z);
    blockSignals(false);
    update();
}
void VisualItem::sendRotationMsg()
{
    if((getOption(VisualItem::LocalIsGM).toBool()) ||
            (getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL) ||
            ((getOption(VisualItem::PermissionMode).toInt() == Map::PC_MOVE)&&
             (getType() == VisualItem::CHARACTER)&&
             (isLocal())))//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::RotationItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(rotation());
        msg.sendAll();
    }
}
void VisualItem::readRotationMsg(NetworkMessageReader* msg)
{
    qreal rot = msg->real();
    blockSignals(true);
    setRotation(rot);
    blockSignals(false);
    update();
}
void VisualItem::sendRectGeometryMsg()
{
    if((getOption(VisualItem::LocalIsGM).toBool()) ||
            (getOption(VisualItem::PermissionMode).toInt() == Map::PC_ALL) ||
            ((getOption(VisualItem::PermissionMode).toInt() == Map::PC_MOVE)&&
             (getType() == VisualItem::CHARACTER)&&
             (isLocal())))//getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::RectGeometryItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(m_rect.x());
        msg.real(m_rect.y());
        msg.real(m_rect.width());
        msg.real(m_rect.height());
        msg.sendAll();
    }
}
void VisualItem::readRectGeometryMsg(NetworkMessageReader* msg)
{
    qreal xR = msg->real();
    qreal yR = msg->real();
    qreal w = msg->real();
    qreal h = msg->real();
    blockSignals(true);
    setRectSize(xR,yR,w,h);
    blockSignals(false);
    update();
}

void VisualItem::setRectSize(qreal x,qreal y,qreal w,qreal h)
{
    m_rect.setX(x);
    m_rect.setY(y);
    m_rect.setWidth(w);
    m_rect.setHeight(h);
}

void VisualItem::setPropertiesHash(QHash<VisualItem::Properties,QVariant>* hash)
{
    m_propertiesHash = hash;
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
    //emit itemGeometryChanged(this);
    if(!m_pointList.isEmpty())
    {
        sendPositionMsg();
        m_pointList.clear();
    }

    if(m_resizing)
    {
        sendRectGeometryMsg();
        m_resizing=false;
    }

    if(m_rotating)
    {
        sendRotationMsg();
        m_rotating = false;
    }
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
    if((!b)||(canBeMoved()))
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
}

bool VisualItem::canBeMoved() const
{
    if(!isEditable())
    {
        return false;
    }
    else if(getOption(VisualItem::LocalIsGM).toBool())
    {
        return true;
    }
    else if(getOption(VisualItem::PermissionMode).toInt()==Map::PC_ALL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool VisualItem::getHoldSize() const
{
    return m_holdSize;
}

void VisualItem::setHoldSize(bool holdSize)
{
    m_holdSize = holdSize;
}

bool VisualItem::isEditable() const
{
    return m_editable;
}

QString VisualItem::getLayerToText(VisualItem::Layer id)
{
    if(s_layerName.size()>(int)id)
    {
        return s_layerName.at(id);
    }
    return QString();
}

QVariant VisualItem::getOption(VisualItem::Properties pop) const
{
    if(NULL!=m_propertiesHash)
    {
        return m_propertiesHash->value(pop);
    }
    return QVariant();
}

void VisualItem::setSize(QSizeF size)
{
    //qDebug() << "Size:"<< size;
    m_rect.setSize(size);
    updateChildPosition();
    update();
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
