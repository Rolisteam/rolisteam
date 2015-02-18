/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QThread>
class Server;
/**
  * @brief is dedicated to start the server into another thread, none blocking.
  */
class ServerManager : public QThread
{
    Q_OBJECT
public:
    /**
      * @brief constructor, allocates/initializes few parameters.
      */
    explicit ServerManager(int port,QObject *parent = 0);


public slots:
    /**
      * @brief is called when error occurs
      */
    void errorOccur(QString);

protected:
    /**
      * @brief create another thread and execute the server start connection into it.
      */
    void run();



private:
    /**
      * @brief  port for network lockup
      */
    int m_port;
    /**
      * @brief Server
      */
    Server* m_server;
};

#endif // SERVERMANAGER_H
