/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "instantmessaging/messageinterface.h"

#include <QDebug>

namespace InstantMessaging
{
MessageInterface::MessageInterface(QObject* parent) : QObject(parent) {}

MessageBase::MessageBase(const QString& owner, const QString& writer, const QDateTime& time,
                         MessageInterface::MessageType type, QObject* parent)
    : MessageInterface(parent), m_ownerId(owner), m_time(time), m_type(type), m_writer(writer)
{
    qDebug() << "messageBase" << owner << writer;
}

QString MessageBase::owner() const
{
    return m_ownerId;
}

QDateTime MessageBase::dateTime() const
{
    return m_time;
}

MessageInterface::MessageType MessageBase::type() const
{
    return m_type;
}

QString MessageBase::writer() const
{
    return m_writer;
}

} // namespace InstantMessaging
