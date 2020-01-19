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

#include "anchoritem.h"

#include "vmap/controller/visualitemcontroller.h"
#include <QDebug>

AnchorItem::AnchorItem() : m_pen(QColor(Qt::darkGray))
{
    qDebug() << "point anchor";
}

AnchorItem::~AnchorItem() {}
QRectF AnchorItem::boundingRect() const
{
    return QRectF(m_startPoint, m_endPoint);
}

void AnchorItem::setNewEnd(const QPointF& nend)
{
    if(nend.isNull())
        return;

    m_endPoint+= nend;
}
void AnchorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    m_pen.setWidth(3);
    m_pen.setColor(Qt::darkGray);
    painter->setPen(m_pen);
    QLineF line(m_startPoint, m_endPoint);
    painter->drawLine(line);
    painter->restore();

    QFont f= painter->font();
    f.setPixelSize(15);
    painter->setFont(f);
}

QPointF AnchorItem::getStart() const
{
    return mapToScene(m_startPoint);
}

QPointF AnchorItem::getEnd() const
{
    return mapToScene(m_endPoint);
}
