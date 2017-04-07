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

#include "network/networkmessagewriter.h"
//#ifndef UNIT_TEST
//#include "mainwindow.h"
//#endif
#include <QDebug>

NetworkMessageWriter::NetworkMessageWriter(NetworkManager* server,NetMsg::Category category, NetMsg::Action action, int size)
    : NetworkMessage(server)
{
    int headerSize = sizeof(NetworkMessageHeader);

    if (size < headerSize)
    {
        size = 128;
    }
    m_sizeBuffer = size;
    m_sizeData = headerSize;
    m_buffer = new char[m_sizeBuffer];
    m_header = (NetworkMessageHeader *) m_buffer;

    m_begin = m_buffer + headerSize;
    m_currentPos = m_begin;
    m_end = m_buffer + m_sizeBuffer;

    m_header->category = category;
    m_header->action = action;
#ifndef UNIT_TEST
  //  m_server = MainWindow::getInstance()->getNetWorkManager();
#endif
}

NetworkMessageWriter::~NetworkMessageWriter()
{
    delete[] m_buffer;
}
int NetworkMessageWriter::getDataSize()
{
    return m_currentPos - m_begin;

}
NetMsg::Category NetworkMessageWriter::category() const
{
    return NetMsg::Category(m_header->category);
}

NetMsg::Action NetworkMessageWriter::action() const
{
    return NetMsg::Action(m_header->action);
}

void NetworkMessageWriter::reset()
{
    m_currentPos = m_begin;
}

NetworkMessageHeader * NetworkMessageWriter::buffer()
{
    m_header->dataSize = m_currentPos - m_begin;
    return m_header;
}

void NetworkMessageWriter::uint8(quint8 data)
{
    int size = sizeof(quint8);
    makeRoom(size);

    *((quint8 *)m_currentPos) = data;
    m_currentPos += size;
}

void NetworkMessageWriter::uint16(quint16 data)
{
    int size = sizeof(quint16);
    makeRoom(size);

    *((quint16 *)m_currentPos) = data;
    m_currentPos += size;
}

void NetworkMessageWriter::uint32(quint32 data)
{
    int size = sizeof(quint32);
    makeRoom(size);

    *((quint32 *)m_currentPos) = data;
    m_currentPos += size;
}
 void NetworkMessageWriter::uint64(quint64 data)
 {
     int size = sizeof(quint64);
     makeRoom(size);

     *((quint64 *)m_currentPos) = data;
     m_currentPos += size;
 }

void NetworkMessageWriter::string8(const QString & data)
{
    int sizeQChar = data.size();
    uint8(sizeQChar);
    string(data, sizeQChar);
}

void NetworkMessageWriter::string16(const QString & data)
{
    int sizeQChar = data.size();
    uint16(sizeQChar);
    string(data, sizeQChar);
}

void NetworkMessageWriter::string32(const QString & data)
{
    int sizeQChar = data.size();
    uint32(sizeQChar);
    string(data, sizeQChar);
}

void NetworkMessageWriter::string(const QString & data, int sizeQChar)
{
    int sizeBytes = sizeQChar * sizeof(QChar);

    makeRoom(sizeBytes);
    memcpy(m_currentPos, data.constData(), sizeBytes);
    m_currentPos += sizeBytes;
}

void NetworkMessageWriter::byteArray32(const QByteArray & data)
{
    int size = data.size();
    uint32(size);
    makeRoom(size);
    memcpy(m_currentPos, data.constData(), size);
    m_currentPos += size;
}

void NetworkMessageWriter::rgb(const QColor & color)
{
    int size = sizeof(QRgb);
    makeRoom(size);

    *((QRgb *)m_currentPos) = color.rgb();
    m_currentPos += size;
}

void NetworkMessageWriter::makeRoom(int size)
{
    while (m_currentPos + size > m_end)
        {
            int newSize = (m_end - m_buffer) * 2;
            char * newBuffer = new char[newSize];
            memcpy(newBuffer, m_buffer, m_currentPos - m_buffer);

            long long int diff = newBuffer - m_buffer;

            m_begin  += diff;
            m_currentPos    += diff;
            m_end = newBuffer + newSize;

            delete[] m_buffer;

            m_buffer = newBuffer;
            m_header = (NetworkMessageHeader *) m_buffer;
        }
}
void NetworkMessageWriter::int8(qint8 data)
{
    int size = sizeof(qint8);
    makeRoom(size);

    *((qint8 *)m_currentPos) = data;
    m_currentPos += size;
}

void NetworkMessageWriter::int16(qint16 data)
{
    int size = sizeof(qint16);
    makeRoom(size);

    *((qint16 *)m_currentPos) = data;
    m_currentPos += size;
}

void NetworkMessageWriter::int32(qint32 data)
{
    int size = sizeof(qint32);
    makeRoom(size);

    *((qint32 *)m_currentPos) = data;
    m_currentPos += size;
}
 void NetworkMessageWriter::int64(qint64 data)
 {
     int size = sizeof(qint64);
     makeRoom(size);

     *((qint64 *)m_currentPos) = data;
     m_currentPos += size;
 }
 void NetworkMessageWriter::real(qreal data)
 {
     int size = sizeof(qreal);
     makeRoom(size);

     *((qreal *)m_currentPos) = data;
     m_currentPos += size;
 }
