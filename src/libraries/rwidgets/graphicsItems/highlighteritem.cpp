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
#include "highlighteritem.h"

#include <QDebug>
#include <QPainter>
#include <QPropertyAnimation>

#include "preferences/preferencesmanager.h"
#include <cmath>
#include <math.h>

HighlighterItem::HighlighterItem(PreferencesManager* pref, const QPointF& center, int penSize, const QColor& penColor,
                                 QGraphicsItem* parent, bool autoDestruction)
    : QGraphicsObject(parent)
    , m_preferences(pref)
    , m_center(center)
    , m_color(penColor)
    , m_penSize(static_cast<quint16>(penSize))
{
    setZValue(std::numeric_limits<qreal>::max());
    m_center= center;
    setPos(m_center);
    m_center.setX(0);
    m_center.setY(0);
    m_radius= 0;

    initAnimation(autoDestruction);
}

void HighlighterItem::initAnimation(bool autoDestruction)
{
    if(!autoDestruction)
        return;

    m_animation= new QPropertyAnimation(this, "radius");
    m_animation->setDuration(m_preferences ? m_preferences->value("Map_Highlighter_time", 1000).toInt() : 1000);
    m_animation->setStartValue(0);
    m_animation->setEndValue(m_preferences ? m_preferences->value("Map_Highlighter_radius", 100).toInt() : 100);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setLoopCount(m_preferences ? m_preferences->value("Map_Highlighter_loop", 3).toInt() : 3);

    connect(m_animation, &QPropertyAnimation::finished, this,
            [this]()
            {
                // setVisible(false);
                // emit itemRemoved(m_id, true, false);
                deleteLater();
            });
    m_animation->start();
}
QRectF HighlighterItem::boundingRect() const
{
    return {0, 0, m_radius * 2, m_radius * 2};
}
QPainterPath HighlighterItem::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}
void HighlighterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->save();
    QPen pen= painter->pen();
    pen.setColor(m_color);
    pen.setWidth(m_penSize);
    painter->setPen(pen);

    painter->drawEllipse(m_center, m_radius, m_radius);

    painter->restore();
}
void HighlighterItem::setRadius(qreal radius)
{
    if(qFuzzyCompare(radius, m_radius))
        return;

    m_radius= radius;
    update();
    emit radiusChanged();
}
qreal HighlighterItem::getRadius() const
{
    return m_radius;
}
