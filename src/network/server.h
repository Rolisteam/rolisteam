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

#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include  "networkmisc.h"

class SenderToClient;
class Message;
/**
    * @brief rolisteam server, core of the network feature.
    */
class Server : public QTcpServer
{
    Q_OBJECT
    
public:
    /**
    * @brief constructor
    */
    explicit Server(int port=6660,QObject *parent = 0);
    /**
    * @brief start to listen the port. Waiting client connection
    */
    void startConnection();
    
    
public slots:
    void readDataFromClient();
    
    
signals:
    /**
    * @brief is called when something wrong.
    */
    void error(QString);
    
    void messageToAnalyse();
    
    
private slots:
    void incommingTcpConnection();
    
private:
    /**
    * @brief network port which be lockup by the server.
    */
    int m_port;
    
    QList<QTcpSocket*>* m_list;
    QList<Message*>* m_messageQueue;
    
    SenderToClient* m_clientSender;
    
};

#endif // SERVER_H
