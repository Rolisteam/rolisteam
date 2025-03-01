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
#include "linkcontroller.h"

#include <QDebug>
#include <QRectF>
#include <cmath>

#include "mindnode.h"
namespace mindmap
{

LinkController::LinkController(QObject* parent) : MindItem(MindItem::LinkType, parent)
{
    connect(this, &LinkController::visibleChanged, this,
            [this]()
            {
                if(!m_end || !m_constraint)
                    return;
                m_end->setVisible(isVisible());
            });
    connect(this, &LinkController::geometryChanged, this, &LinkController::computeNormalizedRect);
    connect(this, &LinkController::endChanged, this, &LinkController::computeNormalizedRect);
    connect(this, &LinkController::startChanged, this, &LinkController::computeNormalizedRect);
}

void LinkController::setDirection(const Direction& direction)
{
    if(m_dir == direction)
        return;
    m_dir= direction;
    emit directionChanged();
}

LinkController::Direction LinkController::direction() const
{
    return m_dir;
}

PositionedItem* LinkController::start() const
{
    return m_start;
}

bool LinkController::relatedTo(const QString& id) const
{
    bool res= false;

    if(!m_start.isNull())
        res= (m_start->id() == id);
    else if(!res && !m_end.isNull())
        res= (m_end->id() == id);

    return res;
}

void LinkController::computeNormalizedRect()
{
    if(!m_start)
        return;

    QRectF rect{m_start->centerPoint().x(), m_start->centerPoint().y(), width(), height()};

    auto o= m_orient;
    if(rect.height() < 0 && rect.width() < 0)
        m_orient= LeftTop;
    else if(rect.height() < 0)
        m_orient= RightTop;
    else if(rect.width() < 0)
        m_orient= LeftBottom;
    else
        m_orient= RightBottom;

    if(o != m_orient)
        emit orientationChanged();
    setNormalizedRect(rect.normalized());
}

void LinkController::setStart(PositionedItem* start)
{
    if(start == m_start)
        return;

    if(m_start)
    {
        m_start->removeLink(this);
        disconnect(m_start, 0, this, 0);
    }

    m_start= start;

    if(nullptr != m_start)
    {
        m_start->addLink(this);
        connect(m_start, &MindNode::positionChanged, this, &LinkController::startPointChanged);
        connect(m_start, &MindNode::positionChanged, this, &LinkController::geometryChanged);
        connect(m_start, &MindNode::positionChanged, this, &LinkController::startBoxChanged);
        connect(m_start, &MindNode::widthChanged, this, &LinkController::startBoxChanged);
        connect(m_start, &MindNode::heightChanged, this, &LinkController::startBoxChanged);
        connect(m_start, &MindNode::visibleChanged, this,
                [this](bool isVisible) { setVisible(isVisible && m_start->open()); });
        connect(m_start, &MindNode::textChanged, this,
                [this]()
                {
                    emit startBoxChanged();
                    emit startPointChanged();
                });
    }
    emit startChanged();
}

void LinkController::setEnd(PositionedItem* end)
{
    if(end == m_end)
        return;

    if(m_end)
    {
        disconnect(m_end, 0, this, 0);
    }
    m_end= end;
    if(nullptr != m_end)
    {
        connect(m_end, &MindNode::positionChanged, this, &LinkController::endPointChanged);
        connect(m_end, &MindNode::positionChanged, this, &LinkController::geometryChanged);
        connect(m_end, &MindNode::positionChanged, this, &LinkController::endBoxChanged);
        connect(m_end, &MindNode::widthChanged, this, &LinkController::endBoxChanged);
        connect(m_end, &MindNode::heightChanged, this, &LinkController::endBoxChanged);
        connect(m_end, &MindNode::visibleChanged, this, &LinkController::setVisible);
        connect(m_end, &MindNode::textChanged, this,
                [this]()
                {
                    emit endBoxChanged();
                    emit endPointChanged();
                });
    }

    emit endChanged();
}

qreal LinkController::width() const
{
    if(!end() || !start())
        return 0.0;
    return end()->centerPoint().x() - start()->centerPoint().x();
}

qreal LinkController::height() const
{
    if(!end() || !start())
        return 0.0;
    return end()->centerPoint().y() - start()->centerPoint().y();
}

PositionedItem* LinkController::end() const
{
    return m_end;
}

QPointF LinkController::endPoint() const
{
    if(nullptr == m_end)
        return {};
    return m_end->centerPoint();
}

QPointF LinkController::startPoint() const
{
    if(nullptr == m_start)
        return {};
    return m_start->centerPoint();
}

QPointF LinkController::topLeftCorner() const
{
    return m_normalizedRect.topLeft();
}

qreal LinkController::normalizedWidth() const
{
    return m_normalizedRect.width();
}

qreal LinkController::normalizedHeight() const
{
    return m_normalizedRect.height();
}

LinkController::Orientation LinkController::orientation() const
{
    return m_orient;
}

void LinkController::computePosition()
{
    auto pos1= m_start->position();
    pos1.setY(pos1.y() + 50);
    m_end->setNextPosition(pos1, this);
}

void LinkController::setNormalizedRect(QRectF rect)
{
    if(m_normalizedRect == rect)
        return;
    m_normalizedRect= rect;
    emit normalizedRectChanged();
}

float LinkController::getStiffness() const
{
    return m_stiffness;
}

void LinkController::setStiffness(float stiffness)
{
    m_stiffness= stiffness;
}

void LinkController::cleanUpLink()
{
    m_start->removeLink(this);
}

float LinkController::getLength() const
{
    auto length= 100.;
    auto rect1= m_start->boundingRect();
    auto diag1= std::sqrt(std::pow(rect1.width(), 2) + std::pow(rect1.height(), 2)) / 2;

    auto rect2= m_end->boundingRect();
    auto diag2= std::sqrt(std::pow(rect2.width(), 2) + std::pow(rect2.height(), 2)) / 2;

    auto realDiag= std::max(diag1, diag2);

    auto length1= static_cast<float>(length + 2 * realDiag);
    if(m_end == nullptr || m_start == nullptr)
        return length1;

    auto nodeCount= static_cast<int>(m_start->subLinks().size());

    QSet<LinkController*> a;
    auto endNodeCount= (m_end->subNodeCount(a) + nodeCount) / 3;
    auto length2= static_cast<float>(length * (1 + endNodeCount));

    return std::max(length1, length2);
}

const QRectF LinkController::endBox() const
{
    return m_end ? m_end->boundingRect() : QRectF{};
}

const QRectF LinkController::startBox() const
{
    return m_start ? m_start->boundingRect() : QRectF{};
}

bool LinkController::color() const
{
    return m_color;
}

void LinkController::setColor(const bool& newColor)
{
    if(m_color == newColor)
        return;
    m_color= newColor;
    emit colorChanged();
}

const Qt::PenStyle& LinkController::lineStyle() const
{
    return m_lineStyle;
}

void LinkController::setLineStyle(const Qt::PenStyle& newLineStyle)
{
    if(m_lineStyle == newLineStyle)
        return;
    m_lineStyle= newLineStyle;
    emit lineStyleChanged();
}

bool LinkController::constraint() const
{
    return m_constraint;
}

void LinkController::setConstraint(bool newConstraint)
{
    if(m_constraint == newConstraint)
        return;
    m_constraint= newConstraint;
    emit constraintChanged();
}

QString LinkController::toString(bool) const
{
    return id();
}
} // namespace mindmap
