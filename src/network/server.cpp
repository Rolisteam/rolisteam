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

#include "server.h"
#include <QTcpSocket>

Server::Server(int port,QObject *parent) :
    QTcpServer(parent),m_port(port)
{
    m_list = new QList<QTcpSocket*>;
    connect(this,SIGNAL(newConnection()),this,SLOT(incommingTcpConnection()));
}
void Server::startConnection()
{
    if(!listen(QHostAddress::Any,m_port))
    {
        emit error(tr("Enable to start the server"));
    }
}
void Server::incommingTcpConnection()
{
    if(hasPendingConnections())
    {
        QTcpSocket* tmp = nextPendingConnection();
        connect(tmp,SIGNAL(readyRead()),this,SLOT(readDataFromClient()));
        m_list->append(tmp);
    }
}
void Server::readDataFromClient()
{

    QTcpSocket* tmp = static_cast<QTcpSocket*>(sender());
    /// @todo: base line of server forward messages to all other clients.
    QByteArray array = tmp->readAll();

    QDataStream stream(array);
    QString name;
    QString type;
    QString data;
    stream >> type;
    stream >> name;
    foreach(QTcpSocket* tmpclient, *m_list)
    {



        quint64 size=tmpclient->write(array);
        qDebug() << "taille envoyé" <<size << m_list->size()<< type<<name ;
    }

}
