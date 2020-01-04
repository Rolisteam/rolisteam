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
#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QUuid>
#include <cmath>

#include <QGraphicsScene>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "map/map.h"
#include "vmap/controller/visualitemcontroller.h"

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

QColor VisualItem::m_highlightColor= QColor(Qt::red);
int VisualItem::m_highlightWidth= 6;

QStringList VisualItem::s_type2NameList
    = QStringList() << QObject::tr("Path") << QObject::tr("Line") << QObject::tr("Ellipse") << QObject::tr("Character")
                    << QObject::tr("Text") << QObject::tr("Rect") << QObject::tr("Rule") << QObject::tr("Image");

VisualItem::VisualItem(vmap::VisualItemController* ctrl)
    : QGraphicsObject(), m_ctrl(ctrl), m_id(QUuid::createUuid().toString())
{
    connect(m_ctrl, &vmap::VisualItemController::posChanged, this, [this]() { setPos(m_ctrl->pos()); });
    connect(m_ctrl, &vmap::VisualItemController::removeItem, this, [this]() {
        scene()->removeItem(this);
        deleteLater();
    });
    connect(m_ctrl, &vmap::VisualItemController::rotationChanged, this, [this]() { setRotation(m_ctrl->rotation()); });
    connect(m_ctrl, &vmap::VisualItemController::visibleChanged, this, [this]() {
        // test anretnaute ue utanite nauriteuet naut euirenru e
        setVisible(m_ctrl->visible());
        // rnatune uir ntenautien
    });
    init();
}

VisualItem::~VisualItem() {}

void VisualItem::init()
{
    createActions();
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    QActionGroup* group= new QActionGroup(this);
    m_putGroundLayer= new QAction(m_ctrl->getLayerText(Core::Layer::GROUND), this);
    m_putGroundLayer->setData(static_cast<int>(Core::Layer::GROUND));
    m_putObjectLayer= new QAction(m_ctrl->getLayerText(Core::Layer::OBJECT), this);
    m_putObjectLayer->setData(static_cast<int>(Core::Layer::OBJECT));
    m_putCharacterLayer= new QAction(m_ctrl->getLayerText(Core::Layer::CHARACTER_LAYER), this);
    m_putCharacterLayer->setData(static_cast<int>(Core::Layer::CHARACTER_LAYER));

    m_putGroundLayer->setCheckable(true);
    m_putObjectLayer->setCheckable(true);
    m_putCharacterLayer->setCheckable(true);

    m_putGroundLayer->setChecked(true);

    group->addAction(m_putGroundLayer);
    group->addAction(m_putObjectLayer);
    group->addAction(m_putCharacterLayer);

    updateItemFlags();
}
void VisualItem::updateItemFlags()
{
    bool editable= canBeMoved();
    if(editable)
    {
        /// @warning if two connected people have editable item, it will lead to endless loop.
        setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges
                 | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
        setAcceptedMouseButtons(Qt::AllButtons);
        connect(this, &VisualItem::xChanged, this, &VisualItem::posChange);
        connect(this, &VisualItem::yChanged, this, &VisualItem::posChange);
        connect(this, &VisualItem::zChanged, this, &VisualItem::sendZValueMsg);
        connect(this, &VisualItem::heightChanged, this, &VisualItem::rectChange);
        connect(this, &VisualItem::widthChanged, this, &VisualItem::rectChange);
        connect(this, &VisualItem::rotationChanged, this, &VisualItem::rotationChange); // sendRotationMsg
        connect(this, &VisualItem::opacityChanged, this, &VisualItem::sendOpacityMsg);
    }
    else
    {
        setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsSelectable);
        setAcceptedMouseButtons(Qt::NoButton);
        disconnect(this, &VisualItem::xChanged, this, &VisualItem::posChange);
        disconnect(this, &VisualItem::yChanged, this, &VisualItem::posChange);
        disconnect(this, &VisualItem::zChanged, this, &VisualItem::sendZValueMsg);
        disconnect(this, &VisualItem::widthChanged, this, &VisualItem::rectChange);
        disconnect(this, &VisualItem::heightChanged, this, &VisualItem::rectChange);
        disconnect(this, &VisualItem::rotationChanged, this, &VisualItem::rotationChange);
        disconnect(this, &VisualItem::opacityChanged, this, &VisualItem::sendOpacityMsg);
    }

    for(auto& itemChild : m_children)
    {
        itemChild->setEditableItem(editable);
    }
}

QColor VisualItem::getColor()
{
    return m_color;
}

void VisualItem::setPenColor(QColor& penColor)
{
    m_color= penColor;
    update();
}
void VisualItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
void VisualItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mouseMoveEvent(event);
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
    endOfGeometryChange(ChildPointItem::Moving);
}

QVariant VisualItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if(change == ItemPositionChange)
    {
        QPointF newPos= computeClosePoint(value.toPointF());
        if(newPos != value.toPointF())
        {
            return newPos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

QPointF VisualItem::computeClosePoint(QPointF pos)
{
    if(Qt::AltModifier & QGuiApplication::keyboardModifiers())
    {
        int size= m_ctrl->gridSize();
        pos.setX(std::round(pos.x() / size) * size);
        pos.setY(std::round(pos.y() / size) * size);
    }
    return pos;
}
void VisualItem::keyPressEvent(QKeyEvent* event)
{
    if((event->key() == Qt::Key_Delete) && (isSelected()) && canBeMoved())
    {
        emit itemRemoved(m_id, true, true);
    }
    else if((event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier) && (isSelected()))
    {
        emit duplicateItem(this);
    }
    QGraphicsItem::keyPressEvent(event);
}

void VisualItem::setId(QString id)
{
    m_id= id;
}

void VisualItem::resizeContents(const QRectF& rect, int pointId, TransformType transformType)
{
    /*    if(!rect.isValid() || isHoldSize())
        {
            return;
        }
        prepareGeometryChange();
        auto width= m_ctrl->rect().width();
        auto height= m_ctrl->rect().height();
        // sendRectGeometryMsg();
        m_resizing= true;
        m_rect= rect;
        if(transformType == VisualItem::KeepRatio)
        {
            auto hfw= height * rect.width() / width;
            if(hfw > 1)
            {
                m_rect.setTop(-hfw / 2);
                m_rect.setHeight(hfw);
            }
        }

        updateChildPosition();*/
}
void VisualItem::updateChildPosition() {}

void VisualItem::addPromoteItemMenu(QMenu* menu)
{
    for(ItemType& type : m_promoteTypeList)
    {
        QAction* action= menu->addAction(s_type2NameList[type]);
        action->setData(type);
        connect(action, SIGNAL(triggered()), this, SLOT(promoteItem()));
    }
}
void VisualItem::promoteItem()
{
    QAction* act= qobject_cast<QAction*>(sender());
    if(nullptr != act)
    {
        VisualItem::ItemType type= static_cast<VisualItem::ItemType>(act->data().toInt());
        emit promoteItemTo(this, type);
    }
}

void VisualItem::createActions()
{
    m_duplicateAct= new QAction(tr("Duplicate Item"), this);
    m_duplicateAct->setShortcut(QKeySequence("Ctrl+C"));
    connect(m_duplicateAct, SIGNAL(triggered()), this, SLOT(manageAction()), Qt::QueuedConnection);
}
void VisualItem::manageAction()
{
    QAction* tmp= qobject_cast<QAction*>(sender());
    if(m_duplicateAct == tmp)
    {
        emit duplicateItem(this);
    }
}

void VisualItem::addActionContextMenu(QMenu&)
{
    /// @brief must be implemented in child classes.
}

QString VisualItem::getId()
{
    return m_id;
}

bool VisualItem::hasFocusOrChild()
{
    if(!m_ctrl->editable())
        return false;

    auto result= isSelected();

    for(auto child : m_children)
    {
        if(nullptr == child)
            continue;

        if(child->isSelected())
        {
            result= true;
        }
    }

    return result;
}
void VisualItem::sendItemLayer()
{
    if(m_ctrl->editable()) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::LayerItemChanged);
        msg.string8(m_mapId);
        msg.string16(m_id);
        // msg.uint8(static_cast<quint8>(m_layer));
        msg.sendToServer();
    }
}

void VisualItem::sendOpacityMsg()
{
    if(m_ctrl->editable()) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::OpacityItemChanged);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(opacity());
        msg.sendToServer();
    }
}
void VisualItem::readOpacityMsg(NetworkMessageReader* msg)
{
    qreal opa= msg->real();
    blockSignals(true);
    setOpacity(opa);
    blockSignals(false);
}
void VisualItem::readLayerMsg(NetworkMessageReader* msg)
{
    quint8 lay= msg->uint8();
    blockSignals(true);
    // setLayer(static_cast<Core::Layer>(lay));
    blockSignals(false);
}
bool VisualItem::isLocal() const
{
    return false;
}
void VisualItem::posChange()
{
    m_pointList.append(pos());
}
void VisualItem::rectChange()
{
    m_resizing= true; // not used
}
void VisualItem::rotationChange()
{
    m_rotating= true;
}

void VisualItem::sendPositionMsg()
{
    if(m_pointList.isEmpty())
        return;

    if(m_ctrl->editable()) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::MoveItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.uint64(m_pointList.size());
        for(auto& point : m_pointList)
        {
            msg.real(point.x());
            msg.real(point.y());
        }
        msg.sendToServer();
    }
}
void VisualItem::readPositionMsg(NetworkMessageReader* msg)
{
    quint64 size= msg->uint64();
    for(quint64 i= 0; i < size; ++i)
    {
        qreal x= msg->real();
        qreal y= msg->real();
        blockSignals(true);
        setPos(x, y);
        blockSignals(false);
    }
    update();
}
void VisualItem::sendZValueMsg()
{
    if(m_ctrl->editable() && !m_receivingZValue) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::ZValueItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(zValue());
        msg.sendToServer();
    }
}
void VisualItem::readZValueMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        qreal z= msg->real();
        m_receivingZValue= true;
        setZValue(z);
        m_receivingZValue= false;
        // blockSignals(false);
        update();
    }
}
void VisualItem::sendRotationMsg()
{
    if(m_ctrl->editable()) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::RotationItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        msg.real(rotation());
        msg.sendToServer();
    }
}
void VisualItem::readRotationMsg(NetworkMessageReader* msg)
{
    qreal rot= msg->real();
    blockSignals(true);
    setRotation(rot);
    blockSignals(false);
    update();
}
void VisualItem::sendRectGeometryMsg()
{
    if(m_ctrl->editable()) // getOption PermissionMode
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::RectGeometryItem);
        msg.string8(m_mapId);
        msg.string16(m_id);
        /*msg.real(m_rect.x());
        msg.real(m_rect.y());
        msg.real(m_rect.width());
        msg.real(m_rect.height());*/
        msg.sendToServer();
    }
}
void VisualItem::readRectGeometryMsg(NetworkMessageReader* msg)
{
    /* qreal xR= msg->real();
     qreal yR= msg->real();
     qreal w= msg->real();
     qreal h= msg->real();
     blockSignals(true);
     setRectSize(xR, yR, w, h);
     blockSignals(false);
     update();*/
}
void VisualItem::readMovePointMsg(NetworkMessageReader* msg)
{
    // Do nothing - only used for now on by pathItem.
    Q_UNUSED(msg)
}

void VisualItem::setRectSize(qreal x, qreal y, qreal w, qreal h)
{
    /*   m_rect.setX(x);
       m_rect.setY(y);
       m_rect.setWidth(w);
       m_rect.setHeight(h);*/
}

void VisualItem::setMapId(QString id)
{
    m_mapId= id;
}

QString VisualItem::getMapId()
{
    return m_mapId;
}
void VisualItem::endOfGeometryChange(ChildPointItem::Change change)
{
    m_ctrl->endGeometryChange();
    if(!m_pointList.isEmpty())
    {
        sendPositionMsg();
        m_pointList.clear();
    }

    if(m_resizing)
    {
        sendRectGeometryMsg();
        m_resizing= false;
    }

    if(m_rotating)
    {
        sendRotationMsg();
        m_rotating= false;
    }
}

void VisualItem::setModifiers(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    /// @brief must be implemented in child classes.
    return;
}
VisualItem* VisualItem::promoteTo(VisualItem::ItemType type)
{
    Q_UNUSED(type)
    /// @brief must be implemented in child classes.
    return nullptr;
}

void VisualItem::setChildrenVisible(bool b)
{
    if((!b) || (canBeMoved()))
    {

        for(auto& item : m_children)
        {
            bool isVisionAndFog= true; // (m_ctrl->visibility() == Core::FOGOFWAR) & m_ctrl->characterVision();
            if((!item->isVisionHandler()) || (isVisionAndFog) || (!b))
            {
                item->setVisible(b);
            }
            else
            {
                item->setVisible(!b);
            }
        }
    }
}

bool VisualItem::canBeMoved() const
{
    return m_ctrl->editable() && hasPermissionToMove();
}
bool VisualItem::hasPermissionToMove(bool allowCharacter) const
{

    return m_ctrl->editable();
    /*bool movable= false;
    if(m_ctrl->localGM())
    {
        movable= true;
    }
    else if((m_ctrl->permission() == Core::PC_MOVE) && (getType() == VisualItem::CHARACTER) && (isLocal())
            && (allowCharacter))
    {
        movable= true;
    }
    else if(m_ctrl->permission() == Core::PC_ALL)
    {
        movable= true;
    }
    return movable;*/
}

QColor VisualItem::getHighlightColor()
{
    return m_highlightColor;
}

void VisualItem::setHighlightColor(const QColor& highlightColor)
{
    m_highlightColor= highlightColor;
}

int VisualItem::getHighlightWidth()
{
    return m_highlightWidth;
}

void VisualItem::setHighlightWidth(int highlightWidth)
{
    m_highlightWidth= highlightWidth;
}

bool VisualItem::isHoldSize() const
{
    return m_holdSize;
}

void VisualItem::setHoldSize(bool holdSize)
{
    m_holdSize= holdSize;
}

void VisualItem::setSize(QSizeF size)
{
    // m_rect.setSize(size);
    updateChildPosition();
    update();
}
// friend functions
QDataStream& operator<<(QDataStream& os, const VisualItem& c)
{
    c.writeData(os);
    return os;
}
QDataStream& operator>>(QDataStream& is, VisualItem& c)
{
    c.readData(is);
    return is;
}
