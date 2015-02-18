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

#include "datawriter.h"

#include "ClientServeur.h"
#include "Liaison.h"

#include "types.h"
#include "variablesGlobales.h"

DataWriter::DataWriter(quint8 categorie, quint8 action, int size)
{
    int headerSize = sizeof(enteteMessage);
    if (size < headerSize)
        size = 128;

    m_buffer = new char[size];
    m_header = (enteteMessage *) m_buffer;

    m_begin = m_buffer + headerSize;
    m_pos = m_begin;
    m_end = m_buffer + size;

    m_header->categorie = categorie;
    m_header->action = action;
}


DataWriter::~DataWriter()
{
    delete[] m_buffer;
}

void
DataWriter::reset()
{
    m_pos = m_begin;
}

void
DataWriter::sendTo(Liaison * link)
{
    m_header->tailleDonnees = m_pos - m_begin;
    link->emissionDonnees(m_buffer, m_header->tailleDonnees + sizeof(enteteMessage));
}

void
DataWriter::sendTo(int linkIndex)
{
    m_header->tailleDonnees = m_pos - m_begin;
    G_clientServeur->emettreDonnees(m_buffer, m_header->tailleDonnees + sizeof(enteteMessage), linkIndex);
}

void
DataWriter::sendAll(Liaison * butLink)
{
    m_header->tailleDonnees = m_pos - m_begin;
    G_clientServeur->emettreDonnees(m_buffer, m_header->tailleDonnees + sizeof(enteteMessage), butLink);
}

void
DataWriter::uint8(quint8 data)
{
    int size = sizeof(quint8);
    makeRoom(size);

    *((quint8 *)m_pos) = data;
    m_pos += size;
}

void
DataWriter::string8(QString data)
{
    int sizeQChar = data.size();
    int sizeBytes = sizeQChar * sizeof(QChar);

    uint8(sizeQChar);
    makeRoom(sizeBytes);
    memcpy(m_pos, data.constData(), sizeBytes);
    m_pos += sizeBytes;
}

void
DataWriter::makeRoom(int size)
{
    while (m_pos + size > m_end)
    {
        char * newBuffer = new char[(m_end - m_buffer) * 2];
        memcpy(newBuffer, m_buffer, m_pos - m_buffer);

        int diff = newBuffer - m_buffer;
        m_begin  += diff;
        m_pos    += diff;
        m_end    += diff;

        delete[] m_buffer;

        m_buffer = newBuffer;
        m_header = (enteteMessage *) m_buffer;
    }
}
