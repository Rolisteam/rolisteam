/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * https://rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "charactersheet/charactersheetitem.h"
#include <QDebug>

//////////////////////////////
// Item
/////////////////////////////
TreeSheetItem::TreeSheetItem(TreeSheetItem::TreeItemType type, QObject* parent)
    : QObject(parent), m_itemType(type), m_parent(nullptr)
{
    connect(this, &TreeSheetItem::idChanged, this, [this] { emit characterSheetItemChanged(this); });
}

bool TreeSheetItem::hasChildren()
{
    return false;
}

int TreeSheetItem::childrenCount() const
{
    return 0;
}

TreeSheetItem* TreeSheetItem::childFromId(const QString& id) const
{
    return nullptr;
}

void TreeSheetItem::changeKeyChild(const QString& oldkey, const QString& newKey, TreeSheetItem* child)
{
    Q_UNUSED(oldkey);
    Q_UNUSED(newKey);
    Q_UNUSED(child);
}

TreeSheetItem::TreeItemType TreeSheetItem::itemType() const
{
    return m_itemType;
}

bool TreeSheetItem::removeChild(TreeSheetItem*)
{
    return false;
}

bool TreeSheetItem::deleteChild(TreeSheetItem*)
{
    return false;
}
int TreeSheetItem::rowInParent()
{
    if(nullptr == m_parent)
        return -1;
    return m_parent->indexOfChild(this);
}
bool TreeSheetItem::mayHaveChildren() const
{
    return false;
}
TreeSheetItem* TreeSheetItem::childAt(int) const
{
    return nullptr;
}

QString TreeSheetItem::id() const
{
    return m_id;
}
QString TreeSheetItem::path() const
{
    QString path;
    if(nullptr != m_parent)
    {
        path= m_parent->path();
        if(!path.isEmpty())
        {
            path.append('.');
        }
    }
    return path.append(m_id);
}
void TreeSheetItem::appendChild(TreeSheetItem*)
{
    // nothing
}
TreeSheetItem* TreeSheetItem::parentTreeItem() const
{
    return m_parent;
}

void TreeSheetItem::setParent(TreeSheetItem* parent)
{
    m_parent= parent;
}
int TreeSheetItem::indexOfChild(TreeSheetItem* itm)
{
    Q_UNUSED(itm);
    return 0;
}

void TreeSheetItem::setId(const QString& newPath)
{
    if(m_id == newPath)
        return;
    auto old= m_id;
    m_id= newPath;
    emit idChanged(old, m_id);
}
