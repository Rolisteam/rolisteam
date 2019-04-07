/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
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

#include <QModelIndex>
#include <QModelIndexList>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <memory>

#include "network/channel.h"
#include "network/channelmodel.h"
#include "network/tcpclient.h"

class TestChannelModel : public QObject
{
    Q_OBJECT
public:
    TestChannelModel();

private slots:
    void init();

    void addTest();
    void addTest_data();

    void removeTest();
    void moveTest();

    void writeAndRead();
    void writeAndRead_data();

private:
    std::unique_ptr<ChannelModel> m_model;
};

TestChannelModel::TestChannelModel() {}

void TestChannelModel::init()
{
    m_model.reset(new ChannelModel());
}

void TestChannelModel::addTest()
{
    QFETCH(QStringList, channels);
    QFETCH(QByteArray, password);
    QFETCH(int, expected);

    for(auto channel : channels)
    {
        m_model->addChannel(channel, password);
    }
    QCOMPARE(m_model->rowCount(QModelIndex()), expected);
}

void TestChannelModel::addTest_data()
{
    QTest::addColumn<QStringList>("channels");
    QTest::addColumn<QByteArray>("password");
    QTest::addColumn<int>("expected");

    QByteArray array("password");
    QTest::addRow("list1") << QStringList() << array << 0;
    QTest::addRow("list2") << QStringList({"channel"}) << array << 1;
    QTest::addRow("list3") << QStringList({"channel1", "channel2"}) << array << 2;
    QTest::addRow("list3") << QStringList({"channel1", "channel2", "channel3"}) << array << 3;
}

void TestChannelModel::removeTest()
{
    m_model->addChannel("channel1", "password");
    m_model->addChannel("channel2", "password");
    QCOMPARE(m_model->rowCount(QModelIndex()), 2);

    m_model->cleanUp();
    QCOMPARE(m_model->rowCount(QModelIndex()), 0);

    m_model->addChannel("channel1", "password");
    auto id1= m_model->addChannel("channel2", "password");
    QCOMPARE(m_model->rowCount(QModelIndex()), 2);

    m_model->removeChild(id1);
    QCOMPARE(m_model->rowCount(QModelIndex()), 1);

    QCOMPARE(m_model->getItemById(id1), nullptr);
}

void TestChannelModel::moveTest()
{
    auto idC= m_model->addChannel("channel1", "password");
    auto idC2= m_model->addChannel("channel2", "password");
    QCOMPARE(m_model->rowCount(QModelIndex()), 2);

    TcpClient client(nullptr, nullptr);
    client.setIsAdmin(true);
    auto id= client.getId();
    m_model->setLocalPlayerId(id);
    m_model->addConnectionToDefaultChannel(&client);
    auto channel= m_model->getItemById(idC);
    QCOMPARE(client.getParentChannel(), channel);

    ClientMimeData data;
    auto parent= m_model->index(0, 0, QModelIndex());
    auto clientIndex= m_model->index(0, 0, parent);

    data.addClient(&client, clientIndex);
    auto b= m_model->dropMimeData(&data, Qt::MoveAction, 1, 1, m_model->index(1, 0, QModelIndex()));

    QVERIFY(b);
}

void TestChannelModel::writeAndRead()
{
    QFETCH(QStringList, channels);
    QFETCH(QByteArray, password);
    QFETCH(int, expected);

    for(auto channel : channels)
    {
        m_model->addChannel(channel, password);
    }
    QCOMPARE(m_model->rowCount(QModelIndex()), expected);

    QJsonObject obj;
    m_model->writeDataJson(obj);

    ChannelModel model1;
    model1.readDataJson(obj);

    QCOMPARE(model1.rowCount(QModelIndex()), expected);
}

void TestChannelModel::writeAndRead_data()
{
    QTest::addColumn<QStringList>("channels");
    QTest::addColumn<QByteArray>("password");
    QTest::addColumn<int>("expected");

    QByteArray array("password");
    QTest::addRow("list1") << QStringList() << array << 0;
    QTest::addRow("list2") << QStringList({"channel"}) << array << 1;
    QTest::addRow("list3") << QStringList({"channel1", "channel2"}) << array << 2;
    QTest::addRow("list3") << QStringList({"channel1", "channel2", "channel3"}) << array << 3;
}
QTEST_MAIN(TestChannelModel);

#include "tst_channelmodel.moc"
