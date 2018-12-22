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
    m_header = reinterpret_cast<NetworkMessageHeader*>(m_buffer);

    m_buffer += headerSize;
    memcpy(m_buffer, buffer, m_header->dataSize);

    m_pos = m_buffer;
    m_end = m_buffer + m_header->dataSize;

    readRecipient();
}

NetworkMessageReader::NetworkMessageReader(const NetworkMessageReader & other)
: NetworkMessage(nullptr)
{
    size_t size = other.m_end - reinterpret_cast<char*>(other.m_header);
    char * copy = new char[size];
    memcpy(copy, other.m_header, size);

    m_header = reinterpret_cast<NetworkMessageHeader*>(copy);
    m_buffer = copy + sizeof(NetworkMessageHeader);
    m_pos = m_buffer;
    m_end = copy + size;

    readRecipient();
}

NetworkMessageReader::~NetworkMessageReader()
{
    delete[] ((char *)m_header);
}

bool NetworkMessageReader::isValid()
{
    return !m_outMemory;
}
void NetworkMessageReader::setData(const QByteArray& bytes)
{
    int size = bytes.size();
    const char* data = bytes.data();

    size_t headerSize = sizeof(NetworkMessageHeader);
    m_buffer = new char[size + headerSize];

    memcpy(m_buffer,data,size+headerSize);

    m_header = reinterpret_cast<NetworkMessageHeader*>(m_buffer);

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

bool NetworkMessageReader::isSizeReadable(size_t size)
{
  m_outMemory =! (left() >= size);
  return !m_outMemory;
}

QStringList NetworkMessageReader::getRecipientList() const
{
    return m_recipientList;
}

NetworkMessage::RecipientMode NetworkMessageReader::getRecipientMode() const
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
    return static_cast<size_t>(m_end - m_pos);
}

quint8 NetworkMessageReader::uint8()
{
    size_t size = sizeof(quint8);
    if (isSizeReadable(size))
    {
        quint8 ret = static_cast<quint8>(*m_pos);
        m_pos += size;
        return ret;
    }
    return 0;
}

quint16 NetworkMessageReader::uint16()
{
    size_t size = sizeof(quint16);
    if (isSizeReadable(size))
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
    if (isSizeReadable(size))
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
    if (isSizeReadable(size))
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

QString NetworkMessageReader::string(quint64 sizeQChar)
{
    size_t sizeBytes = sizeQChar * sizeof(QChar);
    if (sizeBytes > 0 && isSizeReadable(sizeBytes))
    {
        QString ret(reinterpret_cast<const QChar*>(m_pos),
                    static_cast<int>(sizeQChar));
        m_pos += sizeBytes;
        return ret;
    }
    return QString();
}

unsigned int NetworkMessageReader::rgb()
{
    size_t size = sizeof(unsigned int);
    if (isSizeReadable(size))
    {
        unsigned int ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0x00ffffff;
}

QByteArray NetworkMessageReader::byteArray32()
{
    size_t size = static_cast<size_t>(uint32());
    if(isSizeReadable(size))
    {
      QByteArray result(m_pos, static_cast<int>(size));
      m_pos += size;
      return result;
    }
    return {};
}
qint8 NetworkMessageReader::int8()
{
    size_t size = sizeof(qint8);
    if (isSizeReadable(size))
    {
        qint8 ret = static_cast<qint8>( *m_pos);
        m_pos += size;
        return ret;
    }
    return 0;
}

qint16 NetworkMessageReader::int16()
{
    size_t size = sizeof(qint16);
    if (isSizeReadable(size))
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
    if (isSizeReadable(size))
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
    if (isSizeReadable(size))
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
    if (isSizeReadable(size))
    {
        qreal ret;
        memcpy(&ret, m_pos, size);
        m_pos += size;
        return ret;
    }
    return 0;
}
