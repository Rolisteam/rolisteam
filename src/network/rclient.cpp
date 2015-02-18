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

#include "rclient.h"
#include <QHostAddress>
#include <QDebug>

RClient::RClient(Connection& connectpar,QObject *parent) :
    QObject(parent),m_connection(connectpar)
{
    m_client = new QTcpSocket(this);
    m_currentState =DISCONNECTED;
    connect(m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorOccurs()));
    connect(m_client,SIGNAL(connected()),this,SLOT(isConnected()));
    connect(m_client,SIGNAL(readyRead()),this,SLOT(readData()));
}
RClient::~RClient()
{

}
void RClient::errorOccurs()
{
    m_currentState=ERROR;
    qDebug() << m_client->errorString();
}

void RClient::startConnection()
{
    QHostAddress addr(m_connection.getAddress());
    qDebug() << m_connection.getAddress() << m_connection.getPort();
    m_client->connectToHost(addr,m_connection.getPort());
}
void RClient::isConnected()
{
    m_currentState=CONNECTED;
    qDebug() << "connected established";
}
void RClient::readData()
{
    m_currentState=CONNECTED;
    qDebug() << "ready to read";
}
