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


#ifndef NETWORK_MESSAGE_WRITER_H
#define NETWORK_MESSAGE_WRITER_H

#include <QColor>
#include <QString>

#include "network/networkmessage.h"

/**
 * @brief The NetworkMessageWriter class
 */
class NetworkMessageWriter : public NetworkMessage
{
    public:
        NetworkMessageWriter(NetMsg::Category categorie, NetMsg::Action action, int size = 128);
        ~NetworkMessageWriter();

        NetMsg::Category category() const;
        NetMsg::Action action() const;

        void reset();

        void uint8(quint8 data);
        void uint16(quint16 data);
        void uint32(quint32 data);
        void uint64(quint64 data);

        void string8(const QString & data);
        void string16(const QString & data);
        void string32(const QString & data);

        void byteArray32(const QByteArray & data);

        void rgb(const QColor & color);

        int getDataSize();

    protected:
        NetworkMessageHeader * buffer();

    private:
        NetworkMessageHeader * m_header;
        char * m_buffer;
        char * m_begin;
        char * m_currentPos;
        char * m_end;

        void string(const QString & data, int sizeQChar);
        void makeRoom(int size);
        int m_sizeBuffer      ;
        int m_sizeData;
};

#endif
