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

#include <QByteArray>
/**
  * @brief storage class to store rolisteam protocole messages
  * @brief should implement some features to get important/compulsory fields
  * @todo add operator << and >>
  */
class QTcpSocket;
class Message
{
public:
    Message();
    QByteArray& getDataArray();
    quint32 getSize();
    quint32 getType();
    void setType(int type);

    void write(QTcpSocket* );

private:
    int m_type;
    QByteArray m_internalData;
};

#endif // MESSAGE_H
