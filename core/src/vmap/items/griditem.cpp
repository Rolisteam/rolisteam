/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#include "griditem.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <math.h>

#include "userlist/playermodel.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "media/mediatype.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/visualitemcontroller.h"
#include "vmap/vmap.h"

#define PI 3.14159265

/////////////////////////////////
/// Code SightItem
/////////////////////////////////

GridItem::GridItem(vmap::GridController* ctrl) : VisualItem(ctrl), m_gridCtrl(ctrl)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    createActions();
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    m_ctrl->setLayer(Core::Layer::GRIDLAYER);
    setFlags(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsSelectable, false);

    connect(m_gridCtrl, &vmap::GridController::gridPatternChanged, this, [this]() { update(); });
    connect(m_gridCtrl, &vmap::GridController::rectChanged, this, [this]() { update(); });
}

GridItem::~GridItem() {}

void GridItem::updateItemFlags()
{
    VisualItem::updateItemFlags();
    setAcceptedMouseButtons(Qt::NoButton);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}
QRectF GridItem::boundingRect() const
{
    /*if(nullptr == scene())
        return QRectF();

    auto rect= scene()->sceneRect();

    if(rect.isNull())
        rect= QRectF(0, 0, 1000, 1000);

    return rect;*/

    return m_gridCtrl->rect();

    /*QList<QGraphicsView*> list= scene()->views();
    if(!list.isEmpty())
    {
        QGraphicsView* view= list.at(0);

        QPointF A= view->mapToScene(QPoint(0, 0));
        QPointF B= view->mapToScene(QPoint(view->viewport()->width(), view->viewport()->height()));

        return QRectF(A, B);
    }
    return scene()->sceneRect();*/
}
void GridItem::setNewEnd(const QPointF& nend)
{
    Q_UNUSED(nend)
    return;
}
void GridItem::writeData(QDataStream& out) const
{
    Q_UNUSED(out)
}

void GridItem::readData(QDataStream& in){Q_UNUSED(in)} VisualItem::ItemType GridItem::getType() const
{
    return VisualItem::GRID;
}

void GridItem::fillMessage(NetworkMessageWriter* msg)
{
    msg->string16(m_id);
    // rect
    /*msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());*/
    // pos
    msg->real(pos().x());
    msg->real(pos().y());
    msg->real(zValue());
}

void GridItem::readItem(NetworkMessageReader* msg)
{
    m_id= msg->string16();
    // rect
    /*    m_rect.setX(msg->real());
        m_rect.setY(msg->real());
        m_rect.setWidth(msg->real());
        m_rect.setHeight(msg->real());*/

    // pos
    qreal x= msg->real();
    qreal y= msg->real();
    setPos(x, y);
    qreal z= msg->real();
    setZValue(z);
    update();
}
void GridItem::setGeometryPoint(qreal, QPointF&) {}
void GridItem::initChildPointItem()
{
    connect(scene(), &QGraphicsScene::sceneRectChanged, m_gridCtrl, &vmap::GridController::setRect);
    m_gridCtrl->setRect(scene()->sceneRect());
}
VisualItem* GridItem::getItemCopy()
{
    return nullptr;
}

void GridItem::updateChildPosition() {}

void GridItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(!m_gridCtrl->visible())
        return;

    painter->fillRect(boundingRect(), QBrush(m_gridCtrl->gridPattern()));
}

void GridItem::createActions() {}
