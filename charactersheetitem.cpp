/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
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
#include "item.h"

//////////////////////////////
//Item
/////////////////////////////
CharacterSheetItem::CharacterSheetItem()
{

}

bool CharacterSheetItem::hasChildren()
{
    return false;
}

int CharacterSheetItem::getChildrenCount()
{
    return 0;
}

CharacterSheetItem *CharacterSheetItem::getChildAt(int)
{
    return NULL;
}

QVariant CharacterSheetItem::getValue(CharacterSheetItem::ColumnId i) const
{
    return QVariant();
}
bool CharacterSheetItem::isReadOnly() const
{
    return m_readOnly;
}

void CharacterSheetItem::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
}
int CharacterSheetItem::rowInParent()
{
    return m_parent->indexOf(this);
}
bool CharacterSheetItem::mayHaveChildren()
{
    return false;
}
CharacterSheetItem*  CharacterSheetItem::getChildAt(int) const
{
    return NULL;
}
QString CharacterSheetItem::getPath()
{
    QString path;
    if(NULL!=m_parent)
    {
        path=m_parent->getPath();
        path.append('.');
    }
    return path.append(m_name);
}
void CharacterSheetItem::appendChild(CharacterSheetItem *)
{

}
CharacterSheetItem *CharacterSheetItem::getParent() const
{
    return m_parent;
}

void CharacterSheetItem::setParent(CharacterSheetItem *parent)
{
    m_parent = parent;
}
int CharacterSheetItem::indexOfChild(CharacterSheetItem* itm)
{
    return 0;
}

