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
#include "charactersheet/csitem.h"
#include <QDebug>

int CSItem::m_count= 0;
CSItem::CSItem(CharacterSheetItemType type, QObject* parent, bool addCount)
    : CharacterSheetItem(type, parent), m_rect(0, 0)
{
    Q_UNUSED(parent);
    if(addCount)
    {
        ++m_count;
    }
}

QColor CSItem::bgColor() const
{
    return m_bgColor;
}

void CSItem::setBgColor(const QColor& bgColor)
{
    m_bgColor= bgColor;
}

QColor CSItem::textColor() const
{
    return m_textColor;
}

void CSItem::setTextColor(const QColor& textColor)
{
    m_textColor= textColor;
}

qreal CSItem::x() const
{
    return m_pos.x();
}

qreal CSItem::y() const
{
    return m_pos.y();
}

qreal CSItem::width() const
{
    return m_rect.width();
}

qreal CSItem::height() const
{
    return m_rect.height();
}

void CSItem::setX(qreal x)
{
    if(qFuzzyCompare(m_pos.x(), x))
        return;
    m_pos.setX(x);
    emit xChanged();
}

void CSItem::setY(qreal y)
{
    if(qFuzzyCompare(m_pos.y(), y))
        return;
    m_pos.setY(y);
    emit yChanged();
}

void CSItem::setWidth(qreal width)
{
    if(qFuzzyCompare(width, m_rect.width()))
        return;
    m_rect.setWidth(width);
    emit widthChanged();
}

void CSItem::setHeight(qreal height)
{
    if(qFuzzyCompare(height, m_rect.height()))
        return;
    m_rect.setHeight(height);
    emit heightChanged();
}

CharacterSheetItem* CSItem::getChildFromId(const QString&) const
{
    return nullptr;
}

CSItem::BorderLine CSItem::border() const
{
    return m_border;
}

void CSItem::setBorder(const CSItem::BorderLine& border)
{
    m_border= border;

    emit askUpdate();
}
void CSItem::resetCount()
{
    m_count= 0;
}
void CSItem::setCount(int i)
{
    m_count= i;
}
