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
#include "charactersheetitem.h"
#include <QDebug>

//////////////////////////////
//Item
/////////////////////////////
CharacterSheetItem::CharacterSheetItem()
    : m_parent(NULL),m_page(0),m_readOnly(false)
{
}

bool CharacterSheetItem::hasChildren()
{
    return false;
}

int CharacterSheetItem::getChildrenCount() const
{
    return 0;
}

QVariant CharacterSheetItem::getValueFrom(CharacterSheetItem::ColumnId i) const
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
    emit readOnlyChanged();
}

int CharacterSheetItem::getPage() const
{
    return m_page;
}

void CharacterSheetItem::setPage(int page)
{
    m_page = page;
    emit pageChanged();
}

QString CharacterSheetItem::getFormula() const
{
    if(m_formula.isEmpty())
    {
        return m_value;
    }
    return m_formula;
}

void CharacterSheetItem::setFormula(const QString &formula)
{
    m_formula = formula;
}

QString CharacterSheetItem::value() const
{
    return m_value;
}

void CharacterSheetItem::setValue(const QString &value)
{
    if(m_value!=value)
    {
        m_value = value;
        emit valueChanged();
    }
}

QString CharacterSheetItem::getId() const
{
    return m_id;
}

QString CharacterSheetItem::getLabel() const
{
    return m_label;
}

void CharacterSheetItem::setLabel(const QString &label)
{
    m_label = label;
}
void CharacterSheetItem::setId(const QString &id)
{
    m_id = id;
}
int CharacterSheetItem::rowInParent()
{
    return m_parent->indexOfChild(this);
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
        if(!path.isEmpty())
        {
            path.append('.');
        }
    }
    return path.append(m_id);
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

