/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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

#include <QObject>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <network/server.h>
#include <network/rclient.h>
#include <data/connection.h>

class NetworkServer : public QObject
{
    Q_OBJECT

public:
    /**
      * @brief constructor
      */
    NetworkServer(QObject *parent=NULL);

private Q_SLOTS:
    /**
      * @brief initTestCase creates instance of classes to be tested
      */
    void initTestCase();

    /**
      * @brief clean data
      */
    void cleanupTestCase();




private:
    Server* m_server;
    RClient* m_sender;
    RClient* m_receiver;
    Connection* m_connection;
};


NetworkServer::NetworkServer(QObject *parent) :
    QObject(parent)
{
}

void NetworkServer::initTestCase()
{
    m_server = new Server();
    m_server->startConnection();

    //waitForSignal(m_server,SIGNAL(messageToAnalyse()));

    m_sender = new RClient();
    m_receiver = new RClient();

    m_connection = new Connection("test","localhost",6666);

    m_sender->startConnection(*m_connection);

    int i =0;
    while (m_sender->isConnected() && i++ < 50)
        QTest::qWait(250);
    QVERIFY2(m_sender->isConnected(),"Sender Connection fails");

    i =0;
    while (m_receiver->isConnected() && i++ < 50)
        QTest::qWait(250);
    m_receiver->startConnection(*m_connection);


    QVERIFY2(m_receiver->isConnected(),"receiver Connection fails");

}

void NetworkServer::cleanupTestCase()
{
    delete m_server;
    delete m_sender;
    delete m_receiver;
    delete m_connection;
}


QTEST_MAIN(NetworkServer);

#include "networkserver.moc"
