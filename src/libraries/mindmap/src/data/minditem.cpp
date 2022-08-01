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
#include "mindmap/data/minditem.h"

#include <QDebug>
#include <QRectF>
#include <QUuid>

#include "mindmap/data/linkcontroller.h"

namespace mindmap
{
MindItem::MindItem(Type type, QObject* parent)
    : QObject{parent}, m_type(type), m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}
bool MindItem::isVisible() const
{
    return m_visible;
}
void MindItem::setVisible(bool op)
{
    if(op == m_visible)
        return;
    m_visible= op;
    emit visibleChanged(m_visible);
}

QString MindItem::text() const
{
    return m_text;
}

void MindItem::setText(QString text)
{
    // qDebug() << "text" << text << m_text;
    if(m_text == text)
        return;

    m_text= text;
    emit textChanged(m_text);
}

QString MindItem::id() const
{
    return m_id;
}
void MindItem::setSelected(bool isSelected)
{
    if(m_selected == isSelected)
        return;
    m_selected= isSelected;
    emit selectedChanged();
}

void MindItem::setId(const QString& id)
{
    if(id == m_id)
        return;
    m_id= id;
    emit idChanged();
}
bool MindItem::selected() const
{
    return m_selected;
}

MindItem::Type MindItem::type() const
{
    return m_type;
}
} // namespace mindmap
