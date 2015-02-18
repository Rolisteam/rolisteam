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

#ifndef RCLIENT_H
#define RCLIENT_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMutex>
#include "message.h"
#include "connection.h"
class ReadingThread;
class WritingThread;
/**
  * @brief Tcp client for establishing the connection with the server. It also manages data transfer from/to the server.
  *
  */
class RClient : public QObject
{
    Q_OBJECT
public:
    /**
      * @brief enum describing each state of the connection.
      */
    enum State{DISCONNECTED,CONNECTED,ERROR};
    /**
      * @brief Constructor
      */
    explicit RClient(QObject *parent = 0);
    /**
      * @brief destructor
      */
    ~RClient();
    /**
      * @brief initializes the connection.
      * @param Connection which is gathering all data to reach the server
      */
    void startConnection(Connection& connection);

    //int getId(void*);
    void addMessageToSendQueue(Message m);

protected slots:
    /**
      * @brief calls when an error occurs during the connection.
      */
    void errorOccurs();
    /**
      * @brief calls when the connection is well established.
      */
    void isConnected();
    /**
      * @brief calls when data must be read.
      */
    void readData();

    void sendMessage();
signals:
    void messageInQueue();

private:
    /**
      * @brief gathering connection parameter: hostname, port...
      */
    Connection m_connection;
    /**
      * @brief the socket class, used for reading or writing from/to the server.
      */
    QTcpSocket* m_client;
    /**
      * @brief stores the current connection state.
      */
    State m_currentState;
    ReadingThread* m_reading;

    //QList<void*> m_registedSender;
    QList<Message>* m_message;
    QMutex m_readingMutex;
    QMutex m_writingMutex;
};

#endif // RCLIENT_H
