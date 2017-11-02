/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#include "resourcesnode.h"

ResourcesNode::ResourcesNode()
    : m_parent(nullptr)
{

}

QString ResourcesNode::name() const
{
    return m_name;
}

void ResourcesNode::setName(const QString &name)
{
    m_name = name;
}

QString ResourcesNode::getValue() const
{
    return m_value;
}

void ResourcesNode::setValue(QString value)
{
    m_value = value;
}

ResourcesNode *ResourcesNode::getChildAt(int) const
{
    return nullptr;
}

QIcon ResourcesNode::getIcon()
{
    return QIcon();
}

ResourcesNode *ResourcesNode::getParentNode() const
{
    return m_parent;
}

void ResourcesNode::setParentNode(ResourcesNode *parent)
{
    m_parent = parent;
}

int ResourcesNode::rowInParent()
{
    if(nullptr!=m_parent)
    {
        return m_parent->indexOf(this);
    }
    return 0;
}

int ResourcesNode::indexOf(ResourcesNode* )
{
    return 0;
}

int ResourcesNode::getChildrenCount() const
{
    return 0;
}

bool ResourcesNode::mayHaveChildren() const
{
    return false;
}

bool ResourcesNode::hasChildren() const
{
    return false;
}
bool ResourcesNode::contains(ResourcesNode*)
{
    return false;
}
bool ResourcesNode::removeChild(ResourcesNode*)
{
    return false;
}

void ResourcesNode::insertChildAt(int row, ResourcesNode *)
{

}
