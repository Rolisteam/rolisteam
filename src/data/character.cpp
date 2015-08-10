/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include <QColor>
#include <QUuid>
#include <QDebug>
#include <QPixmap>
#include <QBuffer>

#include "character.h"
#include "data/player.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

Character::Character()
{

}

Character::Character(const QString & nom, const QColor & color,bool npc,int number)
    : Person(nom, color), m_parent(NULL),m_isNpc(npc),m_number(number),m_health(Healthy)
{
}

Character::Character(const QString & uuid, const QString & nom, const QColor & color,bool npc,int number)
    : Person(uuid, nom, color), m_parent(NULL),m_isNpc(npc),m_number(number),m_health(Healthy)
{
}

Character::Character(NetworkMessageReader & data)
    : Person(), m_parent(NULL),m_health(Healthy)
{
    m_uuid = data.string8();
    m_name = data.string16();
    m_health = (Character::HeathState)data.int8();
    m_isNpc = (bool)data.uint8();
    m_number = data.int32();
    m_color = QColor(data.rgb());

    bool hasAvatar = (bool) data.uint8();

    if(hasAvatar)
    {
        m_avatar = QImage::fromData(data.byteArray32());
    }


}

void Character::fill(NetworkMessageWriter & message)
{
    message.string8(m_parent->uuid());
    message.string8(m_uuid);
    message.string16(m_name);
    message.int8((int)m_health);
    message.uint8((int)m_isNpc);
    message.int32(m_number);
    message.rgb(m_color);
    message.uint8((bool)!m_avatar.isNull());
    if(!m_avatar.isNull())
    {
        QByteArray baImage;
        QBuffer bufImage(&baImage);
        if (m_avatar.save(&bufImage, "PNG", 70))
        {
            qDebug() << "png size:" << bufImage.size();
        }
        message.byteArray32(baImage);
    }

}

Person* Character::parent() const
{
    return m_parent;
}

void Character::setParent(Person * parent)
{
    m_parent = parent;
}
int Character::number() const
{
    return m_number;
}

bool Character::isNpc() const
{
    return m_isNpc;
}
void  Character::setHeathState(Character::HeathState h)
{
  m_health = h;
}
Character::HeathState  Character::getHeathState() const
{
    return m_health;
}
void Character::writeData(QDataStream& out) const
{
    out << m_uuid;
    out << m_name;
    out << (int)m_health;
    out << m_isNpc;
    out << m_number;
    out << m_color;
    out << m_avatar;
}
void Character::readData(QDataStream& in)
{
    in >> m_uuid;
    in >> m_name;
    int value;
    in >> value;
    m_health=(Character::HeathState) value;
    in >> m_isNpc;
    in >> m_number;
    in >> m_color;
    in >> m_avatar;
}
