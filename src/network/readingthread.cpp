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
#include "readingthread.h"
#include <QTcpSocket>
ReadingThread::ReadingThread(QTcpSocket* l,QObject *parent) :
    QThread(parent),m_socket(l)
{
    m_messageQueue = new QList<Message*>;
}

void ReadingThread::setConnection(Connection& m)
{
    m_conn = m;
}

void ReadingThread::run()
{
    //m_socket->connectToHost(m_conn.getAddress(),m_conn.getPort());
    exec();
}
void ReadingThread::readDataFromSocket()
{
    Message* msg = new Message;
    QByteArray array = m_socket->readAll();
    msg->append(array);
    m_messageQueue->append(msg);

}
