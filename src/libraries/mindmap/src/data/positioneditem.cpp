/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "positioneditem.h"

#include <QDebug>

#include "mindmap/data/linkcontroller.h"

namespace mindmap
{
constexpr int velocityMaxLimit{20};
PositionedItem::PositionedItem(Type type, QObject* parent) : MindItem(type, parent)
{
    if(type == MindItem::NodeType)
    {
        setWidth(100);
        setHeight(20);
    }
    else if(type == MindItem::PackageType)
    {
        setWidth(100);
        setHeight(100);
    }
    // comment
    connect(this, &PositionedItem::visibleChanged, this, &PositionedItem::setLinkVisibility);
}

QPointF PositionedItem::position() const
{
    return m_position;
}

QPointF PositionedItem::centerPoint() const
{
    return boundingRect().center();
}

qreal PositionedItem::width() const
{
    return m_w;
}

qreal PositionedItem::height() const
{
    return m_h;
}

void PositionedItem::setWidth(qreal w)
{
    if(qFuzzyCompare(w, m_w))
        return;
    m_w= w;
    emit widthChanged();
}

void PositionedItem::setHeight(qreal h)
{
    if(qFuzzyCompare(h, m_h))
        return;
    m_h= h;
    emit heightChanged();
}

void PositionedItem::setDragged(bool isdragged)
{
    if(m_isDragged == isdragged)
        return;
    m_isDragged= isdragged;
    emit isDraggedChanged();
}
void PositionedItem::setOpen(bool op)
{
    if(op == m_open)
        return;
    m_open= op;
    emit openChanged();
    setLinkVisibility();
}
void PositionedItem::setPosition(const QPointF& p)
{
    if(m_position == p || p.x() < 0 || p.y() < 0)
        return;
    auto motion= m_position - p;
    m_position= p;
    emit positionChanged(m_position);
    if(isDragged())
        emit itemDragged(motion);
}

bool PositionedItem::isDragged() const
{
    return m_isDragged;
}

int PositionedItem::mass() const
{
    return m_mass;
}
void PositionedItem::setMass(int m)
{
    if(m_mass == m)
        return;
    m_mass= m;
    emit massChanged();
}

QRectF PositionedItem::boundingRect() const
{
    return QRectF(m_position, QSizeF(m_w, m_h));
}

QVector2D PositionedItem::getVelocity() const
{
    return m_velocity;
}

void PositionedItem::setVelocity(const QVector2D& velocity)
{
    if(qIsNaN(velocity.x()) && qIsNaN(velocity.y()))
        return;

    if(velocity.x() > velocityMaxLimit)
        m_velocity.setX(velocityMaxLimit);

    if(velocity.y() > velocityMaxLimit)
        m_velocity.setY(velocityMaxLimit);

    if(velocity.y() < velocityMaxLimit && velocity.x() < velocityMaxLimit)
        m_velocity= velocity;
}

QVector2D PositionedItem::getAcceleration() const
{
    return m_acceleration;
}

void PositionedItem::setAcceleration(const QVector2D& acceleration)
{
    m_acceleration= acceleration;
}

void PositionedItem::applyForce(const QVector2D& force)
{
    m_acceleration+= force / m_mass;
}

void PositionedItem::setNextPosition(const QPointF& pos, LinkController* emiter)
{
    m_nextPositions.erase(emiter);
    m_nextPositions.insert(std::pair<LinkController*, QPointF>(emiter, pos));
    updatePosition();
}
void PositionedItem::addLink(LinkController* link)
{
    auto h= hasLink();
    m_subNodelinks.push_back(link);
    if(h != hasLink())
        emit hasLinkChanged();
}
void PositionedItem::updatePosition()
{
    if(m_nextPositions.size() == 0)
        return;

    if(m_nextPositions.size() == 1)
        setPosition(m_nextPositions.begin()->second);

    qreal x= 0;
    qreal y= 0;
    for(auto it= m_nextPositions.begin(); it != m_nextPositions.end(); ++it)
    {
        x+= it->second.x();
        y+= it->second.y();
    }
    x/= m_nextPositions.size();
    y/= m_nextPositions.size();

    setPosition(QPointF(x, y));
}

void PositionedItem::setParentNode(PositionedItem* parent)
{
    m_parent= parent;
}

PositionedItem* PositionedItem::parentNode() const
{
    return m_parent;
}

bool PositionedItem::hasLink() const
{
    return !m_subNodelinks.empty();
}

bool PositionedItem::open() const
{
    return m_open;
}

QString PositionedItem::parentId() const
{
    return parentNode() ? parentNode()->id() : QString();
}

const std::vector<QPointer<LinkController>>& PositionedItem::subLinks() const
{
    return m_subNodelinks;
}

int PositionedItem::subNodeCount() const
{
    int sum= std::accumulate(m_subNodelinks.begin(), m_subNodelinks.end(), 0, [](int& a, LinkController* link) {
        if(nullptr == link)
            return 0;
        auto end= link->end();
        if(nullptr == end)
            return 0;

        return a + 1 + end->subNodeCount();
    });
    return sum;
}

void PositionedItem::removeLink(LinkController* link)
{
    auto it= std::find(m_subNodelinks.begin(), m_subNodelinks.end(), link);

    if(it == m_subNodelinks.end())
        return;

    m_subNodelinks.erase(it);
}

void PositionedItem::setLinkVisibility()
{
    bool visiblility= isVisible() & m_open;
    std::for_each(m_subNodelinks.begin(), m_subNodelinks.end(), [visiblility](LinkController* link) {
        if(nullptr == link)
            return;
        link->setVisible(visiblility);
    });
}

void PositionedItem::translate(const QPointF& motion)
{
    setPosition(m_position + motion);
}

bool PositionedItem::isLocked() const
{
    return m_locked;
}

void PositionedItem::setLocked(bool newLocked)
{
    if(m_locked == newLocked)
        return;
    m_locked= newLocked;
    emit lockedChanged();
}
} // namespace mindmap
