/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
#include "csitem.h"

int CSItem::m_count=0;
CSItem::CSItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
    ++m_count;
}

void CSItem::setNewEnd(QPointF nend)
{
    m_rect.setBottomRight(nend);
}

QColor CSItem::bgColor() const
{
    return m_bgColor;
}

void CSItem::setBgColor(const QColor &bgColor)
{
    m_bgColor = bgColor;
}

QColor CSItem::textColor() const
{
    return m_textColor;
}

void CSItem::setTextColor(const QColor &textColor)
{
    m_textColor = textColor;
}

qreal CSItem::getX() const
{

}

qreal CSItem::getY() const
{

}

qreal CSItem::getWidth() const
{

}

qreal CSItem::getHeight() const
{

}

void CSItem::setX(qreal x)
{

}

void CSItem::setY(qreal y)
{

}

void CSItem::setWidth(qreal width)
{

}

void CSItem::setHeight(qreal height)
{

}
