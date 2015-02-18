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


#ifndef RECEIVE_EVENT_H
#define RECEIVE_EVENT_H

#include <QEvent>
#include <QString>

class DataReader;

class ReceiveEvent
 : public QEvent
{
    public:
        ReceiveEvent(quint8 categorie, quint8 action, quint32 bufferSize, const char * buffer);
        ~ReceiveEvent();

        static const int Type;

        quint8 categorie();
        quint8 action();
        DataReader & data();

    private:
        quint8 m_categorie;
        quint8 m_action;
        DataReader * m_data;
};

#endif
