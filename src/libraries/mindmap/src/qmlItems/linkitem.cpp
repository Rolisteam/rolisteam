/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "mindmap/qmlItems/linkitem.h"

#include "mindmap/geometry/linknode.h"

namespace mindmap
{
LinkItem::LinkItem()
{
    setFlag(QQuickItem::ItemHasContents, true);
    // setFlag(QQuickItem::Ite)
    setAntialiasing(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setWidth(280);
    setHeight(280);
}

PointList LinkItem::points() const
{
    return m_points;
}

void LinkItem::setPoints(const PointList& list)
{
    if(list == m_points)
        return;
    m_points= list;
    emit pointsChanged();
}

QColor LinkItem::color() const
{
    return m_color;
}

void LinkItem::setColor(QColor color)
{
    if(m_color == color)
        return;
    m_color= color;
    emit colorChanged();
    m_colorChanged= true;
    update();
}

void LinkItem::mousePressEvent(QMouseEvent* event)
{
    if(event->button() & Qt::LeftButton)
    {
        emit selected(true);
        event->accept();
    }
}

void LinkItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if(!writable())
        return;

    if(event->button() & Qt::LeftButton)
    {
        setEditing(true);
        event->accept();
    }
}

QSGNode* LinkItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*)
{

    LinkNode* link= static_cast<LinkNode*>(node);
    if(!link)
    {
        link= new LinkNode();
        link->setColor(m_color);
    }
    if(m_colorChanged)
    {
        link->setColor(m_color);
        m_colorChanged= false;
    }

    if(m_controller)
        link->update(QRectF{0, 0, width(), height()}, m_controller->orientation(), m_controller->startBox(),
                     m_controller->endBox());
    return link;
}

qreal LinkItem::horizontalOffset() const
{
    return m_horizontalOffset;
}

qreal LinkItem::verticalOffset() const
{
    return m_verticalOffset;
}
void LinkItem::setHorizontalOffset(qreal r)
{
    if(qFuzzyCompare(r, m_horizontalOffset))
        return;
    m_horizontalOffset= r;
    emit horizontalOffsetChanged();
}
void LinkItem::setVerticalOffset(qreal r)
{
    if(qFuzzyCompare(r, m_verticalOffset))
        return;
    m_verticalOffset= r;
    emit verticalOffsetChanged();
}

LinkController* LinkItem::controller() const
{
    return m_controller;
}

void LinkItem::setController(LinkController* newController)
{
    if(m_controller == newController)
        return;
    m_controller= newController;
    emit controllerChanged();

    auto updateOffset= [this]()
    {
        auto endBox= m_controller->endBox();
        auto startBox= m_controller->startBox();

        QRectF rect{0, 0, width(), height()};
        QPointF p1, p2;
        QRectF rect1= startBox;
        rect1.moveTo(-startBox.width() / 2, -startBox.height() / 2);
        QRectF rect2= endBox;
        rect2.moveTo(-endBox.width() / 2, -endBox.height() / 2);

        switch(m_controller->orientation())
        {
        case LinkController::RightBottom:
        {
            p1= rect.topLeft();
            p2= rect.bottomRight();
            rect2= rect2.translated(p2.x(), p2.y());
        }
        break;
        case LinkController::LeftBottom:
        {
            p1= rect.topRight();
            p2= rect.bottomLeft();
            rect2= rect2.translated(p2.x(), p2.y());
            rect1= rect1.translated(p1.x(), p1.y());
        }
        break;
        case LinkController::RightTop:
        {
            p1= rect.bottomLeft();
            p2= rect.topRight();
            rect2= rect2.translated(p2.x(), p2.y());
            rect1= rect1.translated(p1.x(), p1.y());
        }
        break;
        case LinkController::LeftTop:
        {
            p1= rect.bottomRight();
            p2= rect.topLeft();
            rect1= rect1.translated(p1.x(), p1.y());
        }
        break;
        }

        QLineF line(p1, p2);

        QLineF rect1Bottom(rect1.bottomLeft(), rect1.bottomRight());
        QLineF rect1Top(rect1.topLeft(), rect1.topRight());
        QLineF rect1Left(rect1.topLeft(), rect1.bottomLeft());
        QLineF rect1Right(rect1.topRight(), rect1.bottomRight());

        QVector<QLineF> lines({rect1Bottom, rect1Top, rect1Left, rect1Right});

        QPointF intersection1;
        for(auto const& rectSide : std::as_const(lines))
        {
            QPointF point;
            if(line.intersects(rectSide, &point) == QLineF::BoundedIntersection)
                intersection1= point;
        }

        QLineF rect2Bottom(rect2.bottomLeft(), rect2.bottomRight());
        QLineF rect2Top(rect2.topLeft(), rect2.topRight());
        QLineF rect2Left(rect2.topLeft(), rect2.bottomLeft());
        QLineF rect2Right(rect2.topRight(), rect2.bottomRight());

        QVector<QLineF> lines2({rect2Bottom, rect2Top, rect2Left, rect2Right});

        QPointF intersection2;
        for(auto const& rectSide : std::as_const(lines2))
        {
            QPointF point;
            if(line.intersects(rectSide, &point) == QLineF::BoundedIntersection)
                intersection2= point;
        }

        line= QLineF(intersection1, intersection2);

        setHorizontalOffset(line.center().x());
        setVerticalOffset(line.center().y());
    };
    connect(m_controller, &LinkController::startBoxChanged, this, updateOffset);
    connect(m_controller, &LinkController::endBoxChanged, this, updateOffset);
    connect(m_controller, &LinkController::startChanged, this, updateOffset);
    connect(m_controller, &LinkController::endChanged, this, updateOffset);
}

bool LinkItem::editing() const
{
    return m_editing;
}

void LinkItem::setEditing(bool newEditing)
{
    if(m_editing == newEditing)
        return;
    m_editing= newEditing;
    emit editingChanged();
}

bool LinkItem::writable() const
{
    return m_writable;
}

void LinkItem::setWritable(bool newWritable)
{
    if(m_writable == newWritable)
        return;
    m_writable= newWritable;
    emit writableChanged();
}

} // namespace mindmap
