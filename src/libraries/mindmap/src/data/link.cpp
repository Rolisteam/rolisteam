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
#include "mindmap/data/link.h"

#include "mindnode.h"

#include <QLineF>
#include <QRectF>
#include <QUuid>
#include <cmath>

namespace mindmap
{
float Link::m_minimunLenght= 150.f;
Link::Link(QObject* parent) : QObject(parent), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    setText(tr("is linked"));
}

void Link::setDirection(const mindmap::ArrowDirection& direction)
{
    if(direction == m_dir)
        return;
    m_dir= direction;
    emit directionChanged();
}

mindmap::ArrowDirection Link::direction() const
{
    return m_dir;
}

MindNode* Link::start() const
{
    return m_start;
}

QString Link::p1Id() const
{
    return m_start ? m_start->id() : QString();
}

QString Link::p2Id() const
{
    return m_end ? m_end->id() : QString();
}

void Link::setStart(MindNode* start)
{
    if(start == m_start)
        return;
    if(m_start)
        disconnect(m_start, 0, this, 0);
    m_start= start;
    emit startPointChanged();

    connect(m_start, &MindNode::positionChanged, this, [this]() { emit startPositionChanged(); });
}

MindNode* Link::endNode() const
{
    return m_end;
}

QPointF Link::endPoint() const
{
    if(nullptr == m_end)
        return {};
    return m_end->centerPoint();
}

QPointF Link::startPoint() const
{
    if(nullptr == m_start)
        return {};
    return m_start->centerPoint();
}

void Link::setEnd(MindNode* end)
{
    if(end == m_end)
        return;
    if(m_end)
        disconnect(m_end, 0, this, 0);
    m_end= end;
    emit endPointChanged();

    connect(m_end, &MindNode::positionChanged, this, [this]() { emit endPositionChanged(); });
}

QPointF Link::computePoint(bool p1) const
{
    QLineF line(startPoint(), endPoint());

    auto node= p1 ? m_start : m_end;

    if(!node)
        return {};

    auto top= QLineF(node->boundingRect().topLeft(), node->boundingRect().topRight());
    auto bottom= QLineF(node->boundingRect().bottomLeft(), node->boundingRect().bottomRight());
    auto left= QLineF(node->boundingRect().topLeft(), node->boundingRect().bottomLeft());
    auto right= QLineF(node->boundingRect().topRight(), node->boundingRect().bottomRight());

    QPointF res;
    auto intersect= line.intersects(top, &res);
    if(intersect != QLineF::BoundedIntersection)
        intersect= line.intersects(bottom, &res);

    if(intersect != QLineF::BoundedIntersection)
        intersect= line.intersects(left, &res);

    if(intersect != QLineF::BoundedIntersection)
        line.intersects(right, &res);

    return res;
}

QPointF Link::p1() const
{
    return computePoint(true);
}

QPointF Link::p2() const
{
    return computePoint(false);
}

QString Link::id() const
{
    return m_uuid;
}

void Link::computePosition()
{
    auto pos1= m_start->position();
    pos1.setY(pos1.y() + 50);
    m_end->setNextPosition(pos1, this);
}

void Link::setText(const QString& text)
{
    if(m_text == text)
        return;
    m_text= text;
    emit textChanged();
}

void Link::setId(const QString& text)
{
    if(m_uuid == text)
        return;
    m_uuid= text;
    emit idChanged();
}

float Link::getStiffness() const
{
    return m_stiffness;
}

void Link::setStiffness(float stiffness)
{
    m_stiffness= stiffness;
}

void Link::cleanUpLink()
{
    m_start->removeLink(this);
}

float Link::getLength() const
{
    QLineF line(p1(), p2());
    auto r1= m_start->boundingRect();
    auto r2= m_end->boundingRect();

    auto diagonal= std::sqrt(r1.width() / 2 * r1.width() / 2 + r1.height() / 2 * r1.height() / 2)
                   + std::sqrt(r2.width() / 2 * r2.width() / 2 + r2.height() / 2 * r2.height() / 2) + m_minimunLenght;
    auto length= std::max(static_cast<float>(diagonal), std::max(static_cast<float>(line.length()), m_minimunLenght));

    if(m_end == nullptr || m_start == nullptr)
        return length;

    auto nodeCount= static_cast<int>(m_start->subLinks().size());

    auto endNodeCount= (m_end->subNodeCount() + nodeCount) / 3;
    auto length2= static_cast<float>(length * (1 + endNodeCount));

    return std::max(length, length2);
}

void Link::setVisible(bool vi)
{
    if(m_visible == vi)
        return;
    m_visible= vi;
    emit visibleChanged();

    if(nullptr != m_end)
        m_end->setVisible(vi);
}

bool Link::isVisible() const
{
    return m_visible;
}

QString Link::text() const
{
    return m_text;
}

void Link::setMinimumLenght(float v)
{
    m_minimunLenght= v;
}

QString Link::toString(bool withLabel)
{
    return withLabel ? QStringLiteral("%1 [label=\"Link text:%2\"]").arg(m_uuid, m_text) : m_uuid;
}
} // namespace mindmap
