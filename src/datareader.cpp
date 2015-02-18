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


#include "datareader.h"

#include "string.h"

DataReader::DataReader(quint32 bufferSize, const char * buffer)
{
    m_buffer = new char[bufferSize];
    memcpy(m_buffer, buffer, bufferSize);

    m_pos = m_buffer;
    m_end = m_buffer + bufferSize + 1;
}

DataReader::~DataReader()
{
    delete[] m_buffer;
}

void
DataReader::reset()
{
    m_pos = m_buffer;
}

int
DataReader::left() const
{
    return (m_end - m_pos);
}


quint8
DataReader::uint8()
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

QString
DataReader::string8()
{
    int sizeQChar = uint8();
    int sizeBytes = sizeQChar * sizeof(QChar);
    if (sizeBytes > 0 && left() >= sizeBytes)
    {
        QString ret((const QChar *) m_pos, sizeQChar);
        m_pos += sizeBytes;
        return ret;
    }
    return QString();
}
