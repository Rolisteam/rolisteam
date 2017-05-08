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

//#include "mainwindow.h"
#include "network/networkmessagereader.h"

NetworkMessageReader::NetworkMessageReader()
    : NetworkMessage(nullptr)
{

}

NetworkMessageReader::NetworkMessageReader(const NetworkMessageHeader & header, const char * buffer)
    : NetworkMessage(nullptr)
{
    size_t headerSize = sizeof(NetworkMessageHeader);
    m_buffer = new char[header.dataSize + headerSize];
    memcpy(m_buffer, &header, headerSize);
    m_header = (NetworkMessageHeader *)m_buffer;

    m_buffer += headerSize;
    memcpy(m_buffer, buffer, m_header->dataSize);

    m_pos = m_buffer;
    m_end = m_buffer + m_header->dataSize;

    readRecipient();
}

NetworkMessageReader::NetworkMessageReader(const NetworkMessageReader & other)
: NetworkMessage(nullptr)
{
    size_t size = other.m_end - ((char *)other.m_header);
    char * copy = new char[size];
    memcpy(copy, other.m_header, size);

    m_header = (NetworkMessageHeader *)copy;
    m_buffer = copy + sizeof(NetworkMessageHeader);
    m_pos = m_buffer;
    m_end = copy + size;

    readRecipient();
}

NetworkMessageReader::~NetworkMessageReader()
{
    delete[] ((char *)m_header);
}
void NetworkMessageReader::setData(QByteArray& bytes)
{
    int size = bytes.size();
    const char* data = bytes.data();

    size_t headerSize = sizeof(NetworkMessageHeader);
    m_buffer = new char[size + headerSize];

    memcpy(m_buffer,data,size+headerSize);

    m_header = (NetworkMessageHeader *)m_buffer;

    m_pos = m_buffer + headerSize;
    m_end = m_buffer + headerSize + m_header->dataSize;

    readRecipient();
}

NetMsg::Category NetworkMessageReader::category() const
{
    return NetMsg::Category(m_header->category);
}

NetMsg::Action NetworkMessageReader::action() const
{
    return NetMsg::Action(m_header->action);
}

NetworkMessageHeader* NetworkMessageReader::buffer()
{
    return m_header;
}

QStringList NetworkMessageReader::recipientList() const
{
    return m_recipientList;
}

NetworkMessage::RecipientMode NetworkMessageReader::mode() const
{
    return m_mode;
}


NetworkMessageHeader *NetworkMessageReader::header() const
{
    return m_header;
}

void NetworkMessageReader::setHeader(NetworkMessageHeader *header)
{
    m_header = header;
}

void NetworkMessageReader::reset()
{
    m_pos = m_buffer;
}
void NetworkMessageReader::resetToData()
{
    m_pos = m_buffer+sizeof(NetworkMessageHeader);
    readRecipient();
}

void NetworkMessageReader::readRecipient()
{
    m_mode = static_cast<NetworkMessage::RecipientMode>(uint8());
    if(m_mode == NetworkMessage::OneOrMany)
    {
        int size = uint8();
        m_recipientList.clear();
        for(int i = 0; i<size ; ++i)
        {
            m_recipientList << string8();
        }
    }
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
        quint64 ret;
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
qint8 NetworkMessageReader::int8()
{
    size_t size = sizeof(qint8);
    if (left() >= size)
    {
        qint8 ret = (qint8) *m_pos;
        m_pos += size;
        return ret;
    }
    return 0;
}

qint16 NetworkMessageReader::int16()
{
    size_t size = sizeof(qint16);
    if (left() >= size)
    {
        qint16 ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}

qint32 NetworkMessageReader::int32()
{
    size_t size = sizeof(qint32);
    if (left() >= size)
    {
        qint32 ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}
qint64 NetworkMessageReader::int64()
{
    size_t size = sizeof(qint64);
    if (left() >= size)
    {
        qint64 ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}
qreal  NetworkMessageReader::real()
{
    size_t size = sizeof(qreal);
    if (left() >= size)
    {
        qreal ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}
