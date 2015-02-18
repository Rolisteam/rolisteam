/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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


#include "receiveevent.h"

#include "datareader.h"

const int ReceiveEvent::Type = QEvent::registerEventType();

ReceiveEvent::ReceiveEvent(quint8 categorie, quint8 action, quint32 bufferSize, const char * buffer)
    : QEvent((QEvent::Type)ReceiveEvent::Type), m_categorie(categorie), m_action(action)
{
    m_data = new DataReader(bufferSize, buffer);
}

ReceiveEvent::~ReceiveEvent()
{
    delete m_data;
}

quint8
ReceiveEvent::categorie()
{
    return m_categorie;
}

quint8
ReceiveEvent::action()
{
    return m_action;
}

DataReader &
ReceiveEvent::data()
{
    return *m_data;
}
