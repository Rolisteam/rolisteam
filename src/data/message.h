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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QByteArray>

#include  "networkmisc.h"

class QTcpSocket;
/**
  * @brief storage class to store rolisteam protocole messages
  * @brief should implement some features to get important/compulsory fields
  * @todo add operator << and >>
  */
class Message
{
public:
    /**
      * @brief default constructor
      */
    Message();
    /**
      * @brief accessor to the raw data
      * @return Data address
      */
    QByteArray* getDataArray();
    /**
      * @brief allows to know the size of data
      */
    quint32 getSize();
    /**
      * @brief allows to know the type of message
      */
    Network::Category getType();
    /**
      * @brief set the categorie
      */
    void setCategory(Network::Category cat);
    /**
      * @brief write the message into the given socket
      * @param address of TCP socket
      */
    void write(QTcpSocket* );
    /**
      * @brief defines the sender of this message
      */
    void setSender(QTcpSocket*);
    /**
      * @brief accessor to the sender
      * @return tcp socket address
      */
    QTcpSocket* getSender();

    void adduInt32(quint32);
    void addString(QString);
    void addShort(quint16);
    void addByte(quint8);
    void addBoolean(bool);
    void addDouble(qreal);
    void addLongInt(quint64);


    quint32 takeUInt32();
    QString takeString();
    quint16 takeShort();
    quint8 takeByte();
    bool takeBoolean();
    qreal takeDouble();
    quint64 takeLongInt();

    void clear();


private:
    Network::Category m_type;/// type of message
    QByteArray m_internalData;/// internal data
    QString m_tmp;
    QTcpSocket* m_sender;/// address of the sender
};

#endif // MESSAGE_H
