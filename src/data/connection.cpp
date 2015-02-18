/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include "connection.h"

Connection::Connection()
{
}
Connection::Connection(QString name,QString address,int port)
    : m_name(name),m_address(address),m_port(port)
{

}


void Connection::setName(QString name)
{
    m_name = name;
}

const QString& Connection::getName() const
{
    return m_name;
}


void Connection::setAddress(QString add)
{
    m_address=add;
}

const QString& Connection::getAddress() const
{
    return m_address;
}


void Connection::setPort(int port)
{
    m_port= port;
}

int Connection::getPort() const
{
    return m_port;
}
