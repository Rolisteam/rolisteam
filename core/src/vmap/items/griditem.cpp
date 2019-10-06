/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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

#include "userlist/playersList.h"

#include "data/character.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "vmap/vmap.h"

#define PI 3.14159265

/////////////////////////////////
/// Code SightItem
/////////////////////////////////

GridItem::GridItem() : m_isGM(false)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption);
    createActions();
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    m_layer= GRIDLAYER;
    setFlags(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
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
    if(nullptr != scene())
    {
        QList<QGraphicsView*> list= scene()->views();
        if(!list.isEmpty())
        {
            QGraphicsView* view= list.at(0);

            QPointF A= view->mapToScene(QPoint(0, 0));
            QPointF B= view->mapToScene(QPoint(view->viewport()->width(), view->viewport()->height()));

            return QRectF(A, B);
        }
        return scene()->sceneRect();
    }
    else
    {
        return QRectF();
    }
}
void GridItem::setNewEnd(QPointF& nend)
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
    msg->real(m_rect.x());
    msg->real(m_rect.y());
    msg->real(m_rect.width());
    msg->real(m_rect.height());
    // pos
    msg->real(pos().x());
    msg->real(pos().y());
    msg->real(zValue());
}
void GridItem::computePattern()
{
    if((getOption(VisualItem::GridPattern).toInt() != VMap::NONE) && getOption(VisualItem::ShowGrid).toBool()
       && getOption(VisualItem::GridAbove).toBool())
    {
        QPolygonF polygon;

        if(getOption(VisualItem::GridPattern).toInt() == VMap::HEXAGON)
        {
            qreal radius= getOption(VisualItem::GridSize).toInt() / 2;
            qreal hlimit= radius * qSin(M_PI / 3);
            qreal offset= radius - hlimit;
            QPointF A(2 * radius, radius - offset);
            QPointF B(radius * 1.5, radius - hlimit - offset);
            QPointF C(radius * 0.5, radius - hlimit - offset);
            QPointF D(0, radius - offset);
            QPointF E(radius * 0.5, radius + hlimit - offset - 1);
            QPointF F(radius * 1.5, radius + hlimit - offset - 1);

            QPointF G(2 * radius + radius, radius - offset);
            polygon << C << D << E << F << A << B << A << G;

            m_computedPattern= QImage(static_cast<int>(getOption(VisualItem::GridSize).toInt() * 1.5),
                                      static_cast<int>(2 * hlimit), QImage::Format_ARGB32);
            m_computedPattern.fill(Qt::transparent);
        }
        else if(getOption(VisualItem::GridPattern).toInt() == VMap::SQUARE)
        {
            m_computedPattern= QImage(getOption(VisualItem::GridSize).toInt(), getOption(VisualItem::GridSize).toInt(),
                                      QImage::Format_ARGB32);
            m_computedPattern.fill(Qt::transparent);
            int sizeP= getOption(VisualItem::GridSize).toInt();
            QPointF A(0, 0);
            QPointF B(0, sizeP - 1);
            QPointF C(sizeP - 1, sizeP - 1);

            polygon << A << B << C; //<< D << A;
        }
        QPainter painter(&m_computedPattern);
        QPen pen;
        pen.setColor(getOption(VisualItem::GridColor).value<QColor>());
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawPolyline(polygon);
        painter.end();
        // setBackgroundBrush(QPixmap::fromImage(m_computedPattern));
    }
}
void GridItem::readItem(NetworkMessageReader* msg)
{
    m_id= msg->string16();
    // rect
    m_rect.setX(msg->real());
    m_rect.setY(msg->real());
    m_rect.setWidth(msg->real());
    m_rect.setHeight(msg->real());

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
    m_child= new QVector<ChildPointItem*>();
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
    if(getOption(VisualItem::GridAbove).toBool() && getOption(VisualItem::ShowGrid).toBool()
       && (getOption(VisualItem::GridPattern).toInt() != VMap::NONE))
    {
        painter->fillRect(boundingRect(), QBrush(m_computedPattern));
    }
}

void GridItem::setVisible(bool visible)
{
    VisualItem::setVisible(visible);
}
void GridItem::createActions() {}
