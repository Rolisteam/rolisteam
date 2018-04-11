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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include <networkmessagewriter.h>
#include <networkmessage.h>

class TestNetwork : public QObject
{
    Q_OBJECT

public:
    TestNetwork();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void writeTest();
private:
    NetworkMessageWriter* m_writer;
};

TestNetwork::TestNetwork()
{
}

void TestNetwork::initTestCase()
{
    m_writer = new NetworkMessageWriter(NetMsg::ChatCategory,NetMsg::AddEmptyMap);
}
void TestNetwork::writeTest()
{
    for(int i = 0;i<256;++i)
    {
        m_writer->int8(i);
        QVERIFY2(m_writer->getDataSize()==i*sizeof(qint8),"");
    }
}


void TestNetwork::cleanupTestCase()
{
    delete m_writer;
}



QTEST_MAIN(TestNetwork);

#include "tst_network.moc"
