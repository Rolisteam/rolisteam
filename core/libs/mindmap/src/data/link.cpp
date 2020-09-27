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
#include "link.h"

#include "mindnode.h"
#include <QRectF>
#include <cmath>

Link::Link(QObject* parent) : QObject(parent)
{
    setText(tr("is linked"));
}

void Link::setDirection(const Direction& direction)
{
    m_dir= direction;
}

Link::Direction Link::direction() const
{
    return m_dir;
}

MindNode* Link::start() const
{
    return m_start;
}

void Link::setStart(MindNode* start)
{
    m_start= start;
    connect(m_start, &MindNode::positionChanged, this, &Link::linkChanged);
}

MindNode* Link::end() const
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
    m_end= end;
    connect(m_end, &MindNode::positionChanged, this, &Link::linkChanged);
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
    auto length= 100.;
    auto rect1= m_start->boundingRect();
    auto diag1= std::sqrt(std::pow(rect1.width(), 2) + std::pow(rect1.height(), 2)) / 2;

    auto rect2= m_end->boundingRect();
    auto diag2= std::sqrt(std::pow(rect2.width(), 2) + std::pow(rect2.height(), 2)) / 2;

    auto realDiag= std::max(diag1, diag2);

    auto length1= static_cast<float>(length + 2 * realDiag);
    if(m_end == nullptr || m_start == nullptr)
        return length1;

    auto nodeCount= static_cast<int>(m_start->getSubLinks().size());

    auto endNodeCount= (m_end->subNodeCount() + nodeCount) / 3;
    auto length2= static_cast<float>(length * (1 + endNodeCount));

    return std::max(length1, length2);
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
