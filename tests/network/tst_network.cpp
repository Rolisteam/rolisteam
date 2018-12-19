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

#include "network/networkmessagewriter.h"
//#include <networkmessage.h>
//
//Accepter
#include "network/timeaccepter.h"
#include "network/ipbanaccepter.h"
#include "network/iprangeaccepter.h"
#include "network/passwordaccepter.h"

class TestNetwork : public QObject
{
    Q_OBJECT

public:
    TestNetwork();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void writeTest();
   /* void writeAndReadTest();
    void writeAndReadTest_data();*/
    void messageRecipiantTest();
    void ipBanAccepterTest();
    void ipBanAccepterTest_data();
    void passwordAccepterTest();
    void passwordAccepterTest_data();
    void ipRangeAccepterTest();
    void ipRangeAccepterTest_data();
    void timeAccepterTest();
    void timeAccepterTest_data();
private:
    std::unique_ptr<NetworkMessageWriter> m_writer;
    std::unique_ptr<IpBanAccepter> m_ipBanAccepter;
    std::unique_ptr<PasswordAccepter> m_passwordAccepter;
    std::unique_ptr<IpRangeAccepter> m_ipRangeAccepter;
    std::unique_ptr<TimeAccepter> m_timeAccepter;
};

Q_DECLARE_METATYPE(PasswordAccepter::Level);

TestNetwork::TestNetwork()
{
}

void TestNetwork::initTestCase()
{

}

void TestNetwork::cleanupTestCase()
{

}

void TestNetwork::init()
{
    m_writer.reset(new NetworkMessageWriter(NetMsg::ChatCategory,NetMsg::AddEmptyMap));
    m_ipBanAccepter.reset(new IpBanAccepter());
    m_passwordAccepter.reset(new PasswordAccepter());
    m_ipRangeAccepter.reset(new IpRangeAccepter());
    m_timeAccepter.reset(new TimeAccepter());
}
void TestNetwork::writeTest()
{
    for(quint8 i = 0;i<255;++i)
    {
        m_writer->uint8(i);
        QCOMPARE(m_writer->getDataSize(), (1+i)*sizeof(quint8)+1);//+sizeof(NetworkMessageHeader)
    }
}
/*
void TestNetwork::writeAndReadTest()
{

}

void TestNetwork::writeAndReadTest_data()
{
    QTest::addColumn<QString>("currentIp");
    QTest::addColumn<QStringList>("ipBan");
}*/

void TestNetwork::cleanup()
{
}

void TestNetwork::messageRecipiantTest()
{
    QStringList list = {"client1","client2"};
    QCOMPARE(m_writer->getRecipientMode(), NetworkMessage::All);

    m_writer->setRecipientList(list,NetworkMessage::OneOrMany);

    QCOMPARE(m_writer->getRecipientMode(), NetworkMessage::OneOrMany);
}


void TestNetwork::ipBanAccepterTest()
{
    QFETCH( QString, currentIp);
    QFETCH( QStringList, ipBan);
    QFETCH( bool, expected);

    QMap<QString,QVariant> data = {{"currentIp",currentIp},{"IpBan",ipBan}};

    QCOMPARE(m_ipBanAccepter->isValid(data), expected);

    data.remove("IpBan");

    QCOMPARE(m_ipBanAccepter->isValid(data), true);
}

void TestNetwork::ipBanAccepterTest_data()
{
    QTest::addColumn<QString>("currentIp");
    QTest::addColumn<QStringList>("ipBan");
    QTest::addColumn<bool>("expected");

    QTest::addRow("localhost") << "127.0.0.1" << QStringList() << true;
    QTest::addRow("ipv4 ban") << "192.168.0.25" << QStringList({"192.168.0.25"}) << false;
    QTest::addRow("ipv4") << "192.168.0.24" << QStringList() << true;
    QTest::addRow("ipv6") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334" << QStringList() << true;
    QTest::addRow("ipv6 ban") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334" << QStringList({"2001:0db8:85a3:0000:0000:8a2e:0370:7334"}) << false;
    QTest::addRow("any") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334:192.168.0.27" << QStringList() << true;
    QTest::addRow("any ban v6") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334:192.168.1.45" << QStringList({"2001:0db8:85a3:0000:0000:8a2e:0370:7334","192.168.1.44"}) << false;
    QTest::addRow("any ban v4") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334:192.168.1.45" << QStringList({"2001:0db8:85a3:0000:0000:4a2e:0370:7334","192.168.1.45"}) << false;
}                                                                                                          //2001:0db8:85a3:0000:0000:8a2e:0370:7334

void TestNetwork::passwordAccepterTest()
{
    QFETCH( QString, currentPw);
    QFETCH( QString, expectedPw);
    QFETCH( PasswordAccepter::Level, level);
    QFETCH( bool, expected);

    m_passwordAccepter.reset(new PasswordAccepter(level));

    QString key;

    if(PasswordAccepter::Connection == level)
    {
        key = "ServerPassword";
    }
    else if(PasswordAccepter::Admin == level)
    {
        key = "AdminPassword";
    }
    else if(PasswordAccepter::Channel == level)
    {
        key="ChannelPassword";
    }

    QVERIFY(!key.isEmpty());

    QMap<QString,QVariant> data = {{"userpassword",currentPw},{key,expectedPw}};

    QCOMPARE(m_passwordAccepter->isValid(data), expected);
}
void TestNetwork::passwordAccepterTest_data()
{
    QTest::addColumn<QString>("currentPw");
    QTest::addColumn<QString>("expectedPw");
    QTest::addColumn<PasswordAccepter::Level>("level");
    QTest::addColumn<bool>("expected");



    QTest::addRow("server") << "tagada" << "tagada" << PasswordAccepter::Connection << true;
    QTest::addRow("server1") << "tagada" << "tagada1" << PasswordAccepter::Connection << false;
    QTest::addRow("server2") << "tagada" << "" << PasswordAccepter::Connection << false;

    QTest::addRow("admin") << "tagada" << "tagada" << PasswordAccepter::Admin << true;
    QTest::addRow("admin1") << "tagada" << "tagada1" << PasswordAccepter::Admin << false;
    QTest::addRow("admin2") << "tagada" << "" << PasswordAccepter::Admin << false;

    QTest::addRow("channel") << "tagada" << "tagada" << PasswordAccepter::Channel << true;
    QTest::addRow("channel1") << "tagada" << "tagada1" << PasswordAccepter::Channel << false;
    QTest::addRow("channel2") << "tagada" << "" << PasswordAccepter::Channel << false;
}

void TestNetwork::ipRangeAccepterTest()
{
    QFETCH( QString, currentIp);
    QFETCH( QString, range);
    QFETCH( bool, expected);

    QMap<QString,QVariant> data = {{"currentIp",currentIp},{"rangeIp",range}};

    if(range.isNull())
        data.remove("IpRange");

    QCOMPARE(m_ipRangeAccepter->isValid(data), expected);
}
void TestNetwork::ipRangeAccepterTest_data()
{
    QTest::addColumn<QString>("currentIp");
    QTest::addColumn<QString>("range");
    QTest::addColumn<bool>("expected");

   // QTest::addRow("range1") << "80.80.80.80" << "80.0.0.0.0/255.0.0.0" << true;
    QTest::addRow("range2") << "79.80.80.80" << "80.0.0.0.0/255.0.0.0" << false;

    QTest::addRow("range3") << "192.168.1.15" << "192.168.1.0/24" << true;
    QTest::addRow("range3") << "192.168.0.15" << "192.168.1.0/24" << false;
    QTest::addRow("no ip") << "" << "192.168.1.0/24" << false;
    QTest::addRow("no range") << "192.168.0.15" << QString() << true;

}
void TestNetwork::timeAccepterTest()
{
    bool expected=false;
    QFETCH(QString, start);
    QFETCH(QString, end);

    QTime time(QTime::currentTime());
    const QString format = QStringLiteral("hh:mm");
    QTime startT= QTime::fromString(start,format);
    QTime endT= QTime::fromString(end,format);
    if(time >= startT && time <= endT)
        expected=true;


    QMap<QString,QVariant> data = {{"TimeStart",start},{"TimeEnd",end}};

    QCOMPARE(m_timeAccepter->isValid(data), expected);
}
void TestNetwork::timeAccepterTest_data()
{
    QTest::addColumn<QString>("start");
    QTest::addColumn<QString>("end");

    int count = 0;
    for(int i = 0; i < 3; ++i)
    {
        int end = i < 2 ? 9 : 3;
        for(int j = 0; j < end; ++j)
        {
            QTest::addRow(QStringLiteral("time%1").arg(++count).toStdString().c_str()) 
                << QStringLiteral("%1%2:00").arg(i).arg(j)
                << QStringLiteral("%1%2:30").arg(i).arg(j);
        
            QTest::addRow(QStringLiteral("time%1").arg(++count).toStdString().c_str()) 
                << QStringLiteral("%1%2:30").arg(i).arg(j)
                << QStringLiteral("%1%2:00").arg(i).arg(j+1);
        }
    }
}

QTEST_MAIN(TestNetwork);

#include "tst_network.moc"
