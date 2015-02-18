/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "message.h"
#include <QTcpSocket>

#include <QDebug>
Message::Message()
{
}
quint32  Message::getSize()
{
    return m_internalData.size();
}

quint32 Message::getType()
{
    return m_type;
}

void Message::setType(int type)
{
    m_type = type;
}

void Message::write(QTcpSocket* tpm)
{
    qDebug() << tpm->errorString () << tpm->isOpen() << tpm->write((char*)&m_type);
    qDebug() << tpm->errorString ();
    qDebug() << m_internalData.size();
    quint32 size= m_internalData.size();
    qDebug() << size;
    qDebug() << tpm->write((char*)&size);
    qDebug() << tpm->write(m_internalData);
}

QByteArray& Message::getDataArray()
{
    return m_internalData;
}
