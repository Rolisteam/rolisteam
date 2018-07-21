/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "data/person.h"

#include <QUuid>

#include "network/networkmessagewriter.h"
#include "resourcesnode.h"
#include "cleveruri.h"

/**********
 * Person *
 **********/
Person::Person()
    : m_uuid(QUuid::createUuid().toString()),m_parent(nullptr)
{
}

Person::Person(const QString & name, const QColor & color)
    : m_uuid(QUuid::createUuid().toString()),m_color(color),m_parent(nullptr)
{
    m_name = name;
}

Person::Person(const QString & uuid, const QString & name, const QColor & color)
    : m_uuid(uuid),m_color(color),m_parent(nullptr)
{
    m_name = name;
}

Person::~Person()
{
}
const QString Person::getUuid() const
{
    return m_uuid;
}

QColor Person::getColor() const
{
    return m_color;
}

Person *Person::getParent() const
{
    return m_parent;
}

void Person::setParentPerson(Person *parent)
{
    m_parent = parent;
}

bool Person::setColor(const QColor & color)
{
    if (color == m_color)
        return false;

    m_color = color;
    return true;
}

const QImage& Person::getAvatar() const
{
    return m_avatar;
}

bool Person::hasAvatar() const
{
    return !m_avatar.isNull();
}

bool Person::setAvatar(const QImage& p)
{
    if(m_avatar == p)
    {
        return false;
    }

    m_avatar = p;
    return true;
}
Qt::CheckState Person::checkedState()
{
    return m_checkState;
}

bool Person::isLeaf() const
{
    return true;
}
void Person::setState(Qt::CheckState c)
{
    m_checkState = c;
}
ResourcesNode::TypeResource Person::getResourcesType() const
{
    return ResourcesNode::Person;
}
QVariant Person::getData(ResourcesNode::DataValue data)
{
    switch(data)
    {
    case NAME:
        return m_name;
    case MODE:
        return CleverURI::Internal;
    case DISPLAYED:
        return true;
    case URI:
        return {};
    }
    return {};
}

QString Person::getToolTip() const
{
    return m_name;
}
void Person::write(QDataStream&, bool, bool) const
{
    //default implement does nothing [virtual]
}
void Person::read(QDataStream&)
{
    //default implement does nothing [virtual]
}

bool Person::seekNode(QList<ResourcesNode *>&, ResourcesNode*)
{
    //default implement does nothing [virtual]
    return false;
}
QIcon Person::getIcon()
{
    return QIcon(QPixmap::fromImage(m_avatar));
}
