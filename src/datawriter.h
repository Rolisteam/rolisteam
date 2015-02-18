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


#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <QString>

#include "types.h"

class Liaison;

class DataWriter
{
    public:
        DataWriter(quint8 categorie, quint8 action, int size = 128);
        ~DataWriter();

        void reset();

        void sendTo(int linkIndex);
        void sendAll(Liaison * butLink);

        void uint8(quint8 data);
        void uint16(quint16 data);
        void uint32(quint32 data);

        void string8(QString data);
        void string16(QString data);
        void string32(QString data);

    private:
        enteteMessage * m_header;
        char * m_buffer;
        char * m_begin;
        char * m_pos;
        char * m_end;

        void string(QString data, int sizeQChar);
        void makeRoom(int size);
};

#endif
