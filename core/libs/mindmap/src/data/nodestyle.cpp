/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "nodestyle.h"

NodeStyle::NodeStyle(QObject* parent) : QObject(parent) {}

QColor NodeStyle::colorOne() const
{
    return m_colorOne;
}

QColor NodeStyle::colorTwo() const
{
    return m_colorTwo;
}

QColor NodeStyle::textColor() const
{
    return m_textColor;
}

void NodeStyle::setColorOne(const QColor& color)
{
    if(color == m_colorOne)
        return;
    m_colorOne= color;
    emit colorOneChanged();
}
void NodeStyle::setColorTwo(const QColor& color)
{
    if(color == m_colorTwo)
        return;
    m_colorTwo= color;
    emit colorTwoChanged();
}
void NodeStyle::setTextColor(const QColor& color)
{
    if(color == m_textColor)
        return;
    m_textColor= color;
    emit textColorChanged();
}
