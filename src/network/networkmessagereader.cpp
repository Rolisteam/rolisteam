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

#include "mainwindow.h"
#include "network/networkmessagereader.h"

NetworkMessageReader::NetworkMessageReader(const NetworkMessageHeader & header, const char * buffer)
    : NetworkMessage()
{
    size_t headerSize = sizeof(NetworkMessageHeader);
    m_buffer = new char[header.dataSize + headerSize];
    memcpy(m_buffer, &header, headerSize);
    m_header = (NetworkMessageHeader *)m_buffer;

    m_buffer += headerSize;
    memcpy(m_buffer, buffer, m_header->dataSize);

    m_pos = m_buffer;
    m_end = m_buffer + m_header->dataSize;
    m_server = MainWindow::getInstance()->getNetWorkManager();
}

NetworkMessageReader::NetworkMessageReader(const NetworkMessageReader & other)
: NetworkMessage()
{
    size_t size = other.m_end - ((char *)other.m_header);
    char * copy = new char[size];
    memcpy(copy, other.m_header, size);

    m_header = (NetworkMessageHeader *)copy;
    m_buffer = copy + sizeof(NetworkMessageHeader);
    m_pos = m_buffer;
    m_end = copy + size;
}

NetworkMessageReader::~NetworkMessageReader()
{
    delete[] ((char *)m_header);
}

NetMsg::Category NetworkMessageReader::category() const
{
    return NetMsg::Category(m_header->category);
}

NetMsg::Action NetworkMessageReader::action() const
{
    return NetMsg::Action(m_header->action);
}

NetworkMessageHeader * NetworkMessageReader::buffer()
{
    return m_header;
}

void NetworkMessageReader::reset()
{
    m_pos = m_buffer;
}

size_t NetworkMessageReader::left() const
{
    return (m_end - m_pos);
}

quint8 NetworkMessageReader::uint8()
{
    size_t size = sizeof(quint8);
    if (left() >= size)
    {
        quint8 ret = (quint8) *m_pos;
        m_pos += size;
        return ret;
    }
    return 0;
}

quint16 NetworkMessageReader::uint16()
{
    size_t size = sizeof(quint16);
    if (left() >= size)
    {
        quint16 ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}

quint32 NetworkMessageReader::uint32()
{
    size_t size = sizeof(quint32);
    if (left() >= size)
    {
        quint32 ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}
quint64 NetworkMessageReader::uint64()
{
    size_t size = sizeof(quint64);
    if (left() >= size)
    {
        quint32 ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}
QString NetworkMessageReader::string8()
{
    return string(uint8());
}

QString NetworkMessageReader::string16()
{
    return string(uint16());
}

QString NetworkMessageReader::string32()
{
    return string(uint32());
}

QString NetworkMessageReader::string(int sizeQChar)
{
    size_t sizeBytes = sizeQChar * sizeof(QChar);
    if (sizeBytes > 0 && left() >= sizeBytes)
    {
        QString ret((const QChar *) m_pos, sizeQChar);
        m_pos += sizeBytes;
        return ret;
    }
    return QString();
}

QRgb NetworkMessageReader::rgb()
{
    size_t size = sizeof(QRgb);
    if (left() >= size)
    {
        QRgb ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return qRgb(255, 255, 255);
}
QByteArray NetworkMessageReader::byteArray32()
{
	int size = uint32();
	QByteArray result(m_pos,size);
	m_pos += size;
	return result;

}
