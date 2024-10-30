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
#include <QActionGroup>
#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneHoverEvent>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMenu>
#include <QUuid>
#include <cmath>

#include <QGraphicsScene>

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

QColor VisualItem::m_highlightColor= QColor(Qt::red);
QColor VisualItem::m_selectedColor= QColor(Qt::blue);
int VisualItem::m_highlightWidth= 6;

QStringList VisualItem::s_type2NameList
    = QStringList() << QObject::tr("Path") << QObject::tr("Line") << QObject::tr("Ellipse") << QObject::tr("Character")
                    << QObject::tr("Text") << QObject::tr("Rect") << QObject::tr("Rule") << QObject::tr("Image");

VisualItem::VisualItem(vmap::VisualItemController* ctrl) : QGraphicsObject(), m_ctrl(ctrl)
{
    if(!m_ctrl)
        return;

    connect(m_ctrl, &vmap::VisualItemController::posChanged, this, [this]() { setPos(m_ctrl->pos()); });
    connect(m_ctrl, &vmap::VisualItemController::removeItem, this,
            [this]()
            {
                auto sceneP = scene();
                if(sceneP)
                    sceneP->removeItem(this);
                deleteLater();
            });
    connect(m_ctrl, &vmap::VisualItemController::destroyed, this,
            [this]()
            {
                auto sceneP = scene();
                if(sceneP)
                    sceneP->removeItem(this);
                deleteLater();
            });
    auto func= [this]()
    {
        if(m_ctrl->editable())
            m_ctrl->setPos(pos());
    };
    connect(this, &VisualItem::xChanged, this, func);
    connect(this, &VisualItem::yChanged, this, func);

    // connect(this, &VisualItem::rotationChanged, this, [this]() { m_ctrl->setRotation(rotation()); });

    connect(m_ctrl, &vmap::VisualItemController::colorChanged, this, [this]() { update(); });
    connect(m_ctrl, &vmap::VisualItemController::editableChanged, this, &VisualItem::updateItemFlags);
    connect(m_ctrl, &vmap::VisualItemController::rotationChanged, this, [this]() { setRotation(m_ctrl->rotation()); });
    connect(m_ctrl, &vmap::VisualItemController::selectedChanged, this, [this](bool b) { setSelected(b); });
    connect(m_ctrl, &vmap::VisualItemController::selectableChanged, this, &VisualItem::updateItemFlags);

    connect(m_ctrl, &vmap::VisualItemController::visibleChanged, this, &VisualItem::evaluateVisible);
    connect(m_ctrl, &vmap::VisualItemController::visibilityChanged, this, &VisualItem::evaluateVisible);
    connect(m_ctrl, &vmap::VisualItemController::opacityChanged, this, [this]() { setOpacity(m_ctrl->opacity()); });
    connect(m_ctrl, &vmap::VisualItemController::layerChanged, this,
            [this]() { setOpacity(m_ctrl->layer() == Core::Layer::GAMEMASTER_LAYER ? 0.5 : 1.0); });

    init();

    connect(m_ctrl, &vmap::VisualItemController::zOrderChanged, this, [this](qreal z) { setZValue(z); });

    setPos(m_ctrl->pos());
    evaluateVisible();

    updateItemFlags();
    setOpacity(m_ctrl->layer() == Core::Layer::GAMEMASTER_LAYER ? 0.5 : 1.0);
}

VisualItem::~VisualItem() {}

void VisualItem::evaluateVisible()
{
    // qDebug() << "debug item visibility: Item is visible:" << m_ctrl->visible() << "local is gm:" <<
    // m_ctrl->localIsGM()
    //        << "visiblitiy:" << m_ctrl->visibility() << m_ctrl->tool() << m_ctrl->itemType();
    auto localIsGM= m_ctrl->localIsGM();
    auto visibleFromPermission= localIsGM || m_ctrl->visibility() != Core::HIDDEN;
    auto visibleLayer= localIsGM || m_ctrl->layer() != Core::Layer::GAMEMASTER_LAYER;
    setVisible(m_ctrl->visible() && visibleFromPermission && visibleLayer);
}
void VisualItem::init()
{

    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    /*QActionGroup* group= new QActionGroup(this);
    m_putGroundLayer= new QAction(m_ctrl->getLayerText(Core::Layer::GROUND), this);
    m_putGroundLayer->setData(static_cast<int>(Core::Layer::GROUND));
    m_putObjectLayer= new QAction(m_ctrl->getLayerText(Core::Layer::OBJECT), this);
    m_putObjectLayer->setData(static_cast<int>(Core::Layer::OBJECT));
    m_putCharacterLayer= new QAction(m_ctrl->getLayerText(Core::Layer::CHARACTER_LAYER), this);
    m_putCharacterLayer->setData(static_cast<int>(Core::Layer::CHARACTER_LAYER));

    m_putGameMasterLayer= new QAction(m_ctrl->getLayerText(Core::Layer::GAMEMASTER_LAYER), this);
    m_putGameMasterLayer->setData(static_cast<int>(Core::Layer::GAMEMASTER_LAYER));

    m_putGroundLayer->setCheckable(true);
    m_putObjectLayer->setCheckable(true);
    m_putCharacterLayer->setCheckable(true);
    m_putGameMasterLayer->setCheckable(true);

    m_putGroundLayer->setChecked(true);

    group->addAction(m_putGroundLayer);
    group->addAction(m_putObjectLayer);
    group->addAction(m_putCharacterLayer);
    group->addAction(m_putGameMasterLayer);*/
}
vmap::VisualItemController* VisualItem::controller() const
{
    return m_ctrl;
}

vmap::VisualItemController::ItemType VisualItem::getType() const
{
    return m_ctrl->itemType();
}

void VisualItem::updateItemFlags()
{
    GraphicsItemFlags flags= QGraphicsItem::ItemIsFocusable;
    Qt::MouseButtons buttons= Qt::NoButton;

    if(m_ctrl->editable())
    {
        flags
            |= QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges | QGraphicsItem::ItemIsMovable;
        buttons= Qt::AllButtons;
    }

    setAcceptedMouseButtons(buttons);
    setFlags(flags);

    for(auto& itemChild : m_children)
    {
        itemChild->setEditableItem(m_ctrl->editable());
    }
}

QColor VisualItem::color() const
{
    return m_ctrl->color();
}

void VisualItem::setColor(QColor color)
{
    m_ctrl->setColor(color);
}

void VisualItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}
void VisualItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);
    update();
}
void VisualItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
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

QString VisualItem::uuid() const
{
    return m_ctrl ? m_ctrl->uuid() : QString();
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
        // m_ctrl->rem
    }
    else if((event->key() == Qt::Key_C) && (event->modifiers() == Qt::ControlModifier) && (isSelected()))
    {
        // emit duplicateItem(this);
        event->accept();
    }
    QGraphicsItem::keyPressEvent(event);
}

void VisualItem::resizeContents(const QRectF& rect, int pointId, Core::TransformType transformType)
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
    for(auto& type : m_promoteTypeList)
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
        auto type= static_cast<vmap::VisualItemController::ItemType>(act->data().toInt());
        emit promoteItemTo(this, type);
    }
}

void VisualItem::addActionContextMenu(QMenu& menu) {}

bool VisualItem::hasFocusOrChild()
{
    if(!m_ctrl)
        return false;

    if(!m_ctrl->editable())
        return false;

    auto result= isSelected();
    for(auto const& child : std::as_const(m_children))
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

bool VisualItem::isLocal() const
{
    return !m_ctrl->remote();
}

void VisualItem::endOfGeometryChange(ChildPointItem::Change change)
{
    if(change == ChildPointItem::Resizing)
    {
        auto oldScenePos= scenePos();
        setTransformOriginPoint(m_ctrl->rect().center());
        auto newScenePos= scenePos();
        auto oldPos= pos();
        m_ctrl->setPos(QPointF(oldPos.x() + (oldScenePos.x() - newScenePos.x()),
                               oldPos.y() + (oldScenePos.y() - newScenePos.y())));
    }
    m_ctrl->endGeometryChange();
}

void VisualItem::setModifiers(Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)
    /// @brief must be implemented in child classes.
    return;
}
VisualItem* VisualItem::promoteTo(vmap::VisualItemController::ItemType type)
{
    Q_UNUSED(type)
    /// @brief must be implemented in child classes.
    return nullptr;
}

void VisualItem::setChildrenVisible(bool b)
{
    if(!b)
    {
        std::for_each(std::begin(m_children), std::end(m_children),
                      [](ChildPointItem* item) { item->setVisible(false); });
        return;
    }

    if(!canBeMoved())
        return;

    for(auto& item : m_children)
    {
        bool isVision= item->control() == ChildPointItem::Control::Vision;
        bool hasFog= m_ctrl->visibility() == Core::FOGOFWAR;
        bool isGeometry= item->control() == ChildPointItem::Control::Geometry;

        item->setVisible(isGeometry || (isVision && hasFog));
    }
}

bool VisualItem::canBeMoved() const
{
    return m_ctrl->editable();
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

void VisualItem::setSize(QSizeF size)
{
    updateChildPosition();
    update();
}
