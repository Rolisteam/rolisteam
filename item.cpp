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
Item::Item()
{

}

bool Item::hasChildren()
{
    return false;
}

int Item::getChildrenCount()
{
    return 0;
}

Item *Item::getChildAt(int)
{
    return NULL;
}

QVariant Item::getValue(Item::ColumnId i) const
{
    return QVariant();
}


bool Item::mayHaveChildren()
{
    return false;
}

void Item::appendChild(Item *)
{

}
Item *Item::getParent() const
{
    return m_parent;
}

void Item::setParent(Item *parent)
{
    m_parent = parent;
}
int Item::indexOfChild(Item* itm)
{
    return 0;
}

