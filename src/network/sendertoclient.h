/***************************************************************************
    *     Copyright (C) 2009 by Renaud Guezennec                              *
    *   http://www.rolisteam.org                                              *
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

#ifndef SENDERTOCLIENT_H
#define SENDERTOCLIENT_H

#include <QThread>
class Message;
class QTcpSocket;
/**
    * @brief SenderToClient sends messages from the queue to appropriate client
    */
class SenderToClient : public QThread
{
    Q_OBJECT
public:
    /**
    * @brief construct with parameter
    * @param listm : address of list of message (perhaps it should be protected with mutex)
    * @param listsoc : address of client socket's list
    */
    explicit SenderToClient(QList<Message*>* listm,  QList<QTcpSocket*>* listsoc ,QObject *parent = 0);
    /**
    * @brief overriden method, called when the QThread is started
    */
    void run();
signals:
    
    
public slots:
    /**
    * @brief performs the sending
    */
    void messageToSend();
    
private:
    QList<Message*>* m_messageQueue;/// shared queue of messages
    QList<QTcpSocket*>* m_clientsList; /// shared list of client
};

#endif // SENDERTOCLIENT_H
