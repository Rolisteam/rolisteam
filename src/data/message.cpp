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
#include <QDataStream>
#include <QDebug>
Message::Message()
{
    //m_internalData << (quint32)0 << (quint32)0;
}
quint32  Message::getSize()
{
    return m_internalData.size();
}

Network::Category Message::getType()
{
    return m_type;
}

void Message::setCategory(Network::Category type)
{
    m_type = type;
}

void Message::write(QTcpSocket* tmp)
{
    
    
    /* QByteArray msg;*/
    QDataStream cout(&m_internalData,QIODevice::WriteOnly);
    cout.setVersion(QDataStream::Qt_4_4);
    cout.device()->seek(0);
    //cout << (quint32)m_type<<((quint32)m_internalData.size() - sizeof(quint32) - sizeof(quint32));
    //quint32 size= m_internalData.size();
    //cou << (quint32)m_type<<(quint32)size << m_internalData;
    //qDebug() <<(quint32)m_type<<(quint32)size;*/


    qDebug() << tmp->write(m_internalData) << "Message::write(QTcpSocket* tmp)";
}
void Message::clear()
{
    m_internalData.clear();
}

QByteArray* Message::getDataArray()
{

    return &m_internalData;
}
void Message::setSender(QTcpSocket* sender)
{
    m_sender = sender;
}

QTcpSocket* Message::getSender()
{
    return m_sender;

}
void Message::adduInt32(quint32 value)
{
    char* tmp= new char[sizeof(quint32)];
    tmp=(char*)&value;
    m_internalData.append(tmp,sizeof(quint32));
    //m_internalData.append(m_tmp.setNum(value));
}

void Message::addString(QString value)
{
    adduInt32(value.size());
    m_internalData.append(value);
    // m_internalData << value.size() << value;
}

void Message::addShort(quint16 value)
{
    qDebug()<< "avant"<< m_internalData.size() << sizeof(value);
    char* tmp= new char[sizeof(quint16)];
    tmp=(char*)&value;
    m_internalData.append(tmp,sizeof(quint16));
    //delete tmp;
    qDebug()<< "apres" << m_internalData.size();
}

void Message::addByte(quint8 value)
{
    char* tmp= new char[sizeof(quint8)];
    tmp=(char*)&value;
    m_internalData.append(tmp,sizeof(quint8));
    //m_internalData.append(m_tmp.setNum(value));
}

void Message::addBoolean(bool value)
{
    addByte((quint8)value);
}

void Message::addDouble(qreal value)
{
    char* tmp= new char[sizeof(qreal)];
    tmp=(char*)&value;
    m_internalData.append(tmp,sizeof(qreal));
    // m_internalData.append(m_tmp.setNum(value));
}

void Message::addLongInt(quint64 value)
{
    m_internalData.append(m_tmp.setNum(value));
}


quint32 Message::takeUInt32()
{
    char* tmp= new char[sizeof(quint32)];
    for(int i=0;((i<sizeof(quint32)) && (m_internalData.size()>i));i++)
    {
        tmp[i]=m_internalData[i];
    }

    quint32* value=(quint32*)tmp;
    quint32 copy = *value;

    delete tmp;
    m_internalData.remove(0,sizeof(quint32));
    return copy;
}

QString Message::takeString()
{

    quint32 size = takeUInt32();
    char* tmp= new char[size];
    for(int i=0;((i<size) && (m_internalData.size()>i));i++)
    {
        tmp[i]=m_internalData[i];
    }

    QString value = QString::fromLatin1(tmp,size);
    //QString copy = value;

    delete tmp;
    m_internalData.remove(0,size);
    return value;

}

quint16 Message::takeShort()
{
    char* tmp= new char[sizeof(quint16)];
    for(int i=0;((i<sizeof(quint16)) && (m_internalData.size()>i));i++)
    {
        tmp[i]=m_internalData[i];
    }

    quint16* value=(quint16*)tmp;
    quint16 copy = *value;

    delete tmp;
    m_internalData.remove(0,sizeof(quint16));
    return copy;
}

quint8 Message::takeByte()
{
    char* tmp= new char[sizeof(quint8)];
    for(int i=0;((i<sizeof(quint8)) && (m_internalData.size()>i));i++)
    {
        tmp[i]=m_internalData[i];
    }

    quint8* value=(quint8*)tmp;
    quint8 copy = *value;

    delete tmp;
    m_internalData.remove(0,sizeof(quint8));
    return copy;
}

bool Message::takeBoolean()
{
    return (bool)takeByte();
}

qreal Message::takeDouble()
{
    char* tmp= new char[sizeof(qreal)];
    for(int i=0;((i<sizeof(qreal)) && (m_internalData.size()>i));i++)
    {
        tmp[i]=m_internalData[i];
    }

    qreal* value=(qreal*)tmp;
    qreal copy = *value;

    delete tmp;
    m_internalData.remove(0,sizeof(qreal));
    return copy;
}

quint64 Message::takeLongInt()
{
    char* tmp= new char[sizeof(quint64)];
    for(int i=0;((i<sizeof(quint64)) && (m_internalData.size()>i));i++)
    {
        tmp[i]=m_internalData[i];
    }

    quint64* value=(quint64*)tmp;
    quint64 copy = *value;

    delete tmp;
    m_internalData.remove(0,sizeof(quint64));
    return copy;
}

