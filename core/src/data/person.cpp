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

#include "cleveruri.h"
#include "network/networkmessagewriter.h"
#include "resourcesnode.h"

/**********
 * Person *
 **********/
Person::Person() : QObject(), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces)) {}

Person::Person(const QString& name, const QColor& color, const QString& uuid)
    : QObject(), m_name(name), m_uuid(uuid), m_color(color)
{
    if(m_uuid.isNull())
        m_uuid= QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QString Person::name() const
{
    return m_name;
}

Person::~Person()= default;

QColor Person::getColor() const
{
    return m_color;
}
QString Person::uuid() const
{
    return m_uuid;
}
Person* Person::parentPerson() const
{
    return m_parentPerson;
}

void Person::setParentPerson(Person* parent)
{
    m_parentPerson= parent;
}

bool Person::setColor(const QColor& color)
{
    if(color == m_color)
        return false;

    m_color= color;
    emit colorChanged();
    return true;
}

const QImage& Person::getAvatar() const
{
    return m_avatar;
}
void Person::setAvatar(const QImage& p)
{
    if(p == m_avatar)
        return;

    m_avatar= p;
    emit avatarChanged();
}
bool Person::hasAvatar() const
{
    return !m_avatar.isNull();
}

bool Person::isLeaf() const
{
    return true;
}

QIcon Person::icon() const
{
    return QIcon(QPixmap::fromImage(m_avatar));
}
void Person::setAvatarPath(const QString& avatarPath)
{
    if(m_avatarPath == avatarPath)
        return;

    m_avatarPath= avatarPath;
    m_avatar= QImage(avatarPath);
    emit avatarPathChanged();
    emit avatarChanged();
}

void Person::setName(const QString& name)
{
    if(m_name == name)
        return;
    m_name= name;
    emit nameChanged(m_name);
}

void Person::setUuid(const QString& uuid)
{
    if(m_uuid == uuid)
        return;
    m_uuid= uuid;
    emit uuidChanged(m_uuid);
}
QString Person::avatarPath() const
{
    return m_avatarPath;
}
