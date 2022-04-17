/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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

#include "data/resourcesnode.h"

#include <QUuid>

ResourcesNode::ResourcesNode(TypeResource type, const QString& uuid) : m_uuid(uuid), m_type(type)
{
    if(m_uuid.isEmpty())
        m_uuid= QUuid::createUuid().toString(QUuid::WithoutBraces);
}

ResourcesNode::~ResourcesNode() {}

QString ResourcesNode::uuid() const
{
    return m_uuid;
}

void ResourcesNode::setUuid(const QString& uuid)
{
    if(uuid == m_uuid)
        return;

    m_uuid= uuid;
    emit uuidChanged(m_uuid);
}
QString ResourcesNode::name() const
{
    return m_name;
}

void ResourcesNode::setName(const QString& name)
{
    if(name == m_name)
        return;
    m_name= name;
    emit nameChanged();
}

QString ResourcesNode::value() const
{
    return m_value;
}

void ResourcesNode::setValue(QString value)
{
    if(value == m_value)
        return;
    m_value= value;
    emit valueChanged();
}

ResourcesNode* ResourcesNode::getChildAt(int) const
{
    return nullptr;
}

QIcon ResourcesNode::icon() const
{
    return QIcon();
}

ResourcesNode::TypeResource ResourcesNode::type() const
{
    return m_type;
}

ResourcesNode* ResourcesNode::parentNode() const
{
    return m_parent;
}

void ResourcesNode::setParentNode(ResourcesNode* parent)
{
    if(parent == m_parent)
        return;
    m_parent= parent;
    emit parentNodeChanged();
}

int ResourcesNode::rowInParent()
{
    if(nullptr != m_parent)
    {
        return m_parent->indexOf(this);
    }
    return 0;
}

int ResourcesNode::indexOf(ResourcesNode*) const
{
    return 0;
}

int ResourcesNode::childrenCount() const
{
    return 0;
}

bool ResourcesNode::isLeaf() const
{
    return true;
}

bool ResourcesNode::hasChildren() const
{
    return false;
}
bool ResourcesNode::contains(const QString& id)
{
    Q_UNUSED(id)
    return false;
}
bool ResourcesNode::removeChild(ResourcesNode*)
{
    return false;
}

void ResourcesNode::insertChildAt(int, ResourcesNode*) {}

ResourcesNode* ResourcesNode::findNode(const QString& id)
{
    Q_UNUSED(id)
    return nullptr;
}
