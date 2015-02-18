/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CONNECTION_H
#define CONNECTION_H


#include <QString>
/**
* @brief class to store all informations about remote server
*/
class Connection
{
public:
    /**
      * @brief default constructor
      */
    Connection();
    /**
      * @brief destructor
      */
    ~Connection();
    /**
      * @brief copy constructor
      */
    Connection(const Connection& con);
    /**
      * @brief constructor with all member as param
      * @param name any relevant text for better understanding
      * @param address either ip address or hostname
      * @param port TCP/IP port
      */
    Connection(QString name,QString address,int port);


    /**
      * @brief Accessor to set the name
      * @param name
      */
    void setName(QString name);
    /**
      * @brief Accessor to get the name
      * @return name
      */
    const QString& getName() const;

    /**
      * @brief Accessor to set the address
      * @param address
      */
    void setAddress(QString add);
    /**
      * @brief Accessor to get the address
      * @return address
      */
    const QString& getAddress() const;

    /**
      * @brief Accessor to set the port number
      * @param port number
      */
    void setPort(int port);
    /**
      * @brief Accessor to get the port number
      * @return port number
      */
    int getPort() const;


private:
    /**
    *   Store the name of this connection. It's relevant for the user.
    */
    QString m_name;

    /**
      * Store either ip or hostname address.
      */
    QString m_address;
    /**
      *
      */
    int m_port;


    friend QDataStream& operator<<(QDataStream& os,const Connection&);
    friend QDataStream& operator>>(QDataStream& is,Connection&);

};
typedef QList<Connection> ConnectionList;
#include <QVariant>
Q_DECLARE_METATYPE ( Connection )
Q_DECLARE_METATYPE ( Connection* )
Q_DECLARE_METATYPE ( ConnectionList )


#endif // CONNECTION_H
