/***************************************************************************
 *	Copyright (C) 2011 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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
#ifndef READINGTHREAD_H
#define READINGTHREAD_H
#include "connection.h"
#include <QThread>
#include "message.h"
class QTcpSocket;
class ReadingThread : public QThread
{
    Q_OBJECT
public:
    explicit ReadingThread(QTcpSocket* l,QObject *parent = 0);
    void setConnection(Connection& m);

protected:
    void run();

signals:
    //void readingFinished();
    void messageRecieved(Message&);

public slots:
    void readDataFromSocket();

private:
    QTcpSocket* m_socket;
    QList<Message*>* m_messageQueue;
    Connection m_conn;
};

#endif // READINGTHREAD_H
