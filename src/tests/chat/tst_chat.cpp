/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
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
#include <QtTest/QtTest>

#include <QMetaMethod>
#include <QMetaObject>
#include <QMetaProperty>

#include "data/character.h"
#include "data/localpersonmodel.h"
#include "data/person.h"
#include "data/player.h"
#include "model/playermodel.h"

#include "controller/instantmessagingcontroller.h"
#include "data/chatroom.h"
#include "data/chatroomfactory.h"
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/messagefactory.h"
#include "instantmessaging/messageinterface.h"
#include "instantmessaging/textmessage.h"
#include "instantmessaging/textwritercontroller.h"
#include "model/chatroomsplittermodel.h"
#include "model/filteredplayermodel.h"
#include "model/filterinstantmessagingmodel.h"
#include "model/instantmessagingmodel.h"
#include "model/messagemodel.h"
#include "qml_components/avatarprovider.h"
#include "rwidgets/mediacontainers/instantmessagingview.h"

class Player;
class NetworkLink;
class NetworkMessage;

class ChatWindowTest : public QObject
{
    Q_OBJECT

public:
    ChatWindowTest();

private slots:
    void init();
    void cleanup();

    void propertiesSetGetTest();
    void propertiesSetGetTest_data();

    void chatRoomCountTest();
    void chatRoomCountTest_data();

private:
    std::unique_ptr<PlayerModel> m_playerModel;
    std::unique_ptr<InstantMessagingController> m_imCtrl;
};
ChatWindowTest::ChatWindowTest() {}

void ChatWindowTest::init()
{
    m_playerModel.reset(new PlayerModel);
    m_imCtrl.reset(new InstantMessagingController(m_playerModel.get()));
}

void ChatWindowTest::propertiesSetGetTest()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, propertyVal);

    auto meta= m_imCtrl->metaObject();

    auto proper= meta->property(meta->indexOfProperty(propertyName.toUtf8()));

    auto signal= proper.notifySignal();

    QSignalSpy spy(m_imCtrl.get(), signal);

    auto b= m_imCtrl->setProperty(propertyName.toUtf8(), propertyVal);

    QVERIFY(b);
    QCOMPARE(spy.count(), 1);
}

void ChatWindowTest::propertiesSetGetTest_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("propertyVal");

    QTest::addRow("cmd 1") << "localId" << QVariant::fromValue(QString("tarniten"));
    QTest::addRow("cmd 2") << "nightMode" << QVariant::fromValue(true);
    QTest::addRow("cmd 3") << "visible" << QVariant::fromValue(true);
}

void ChatWindowTest::chatRoomCountTest()
{
    QFETCH(QList<Player*>, playerList);
    QFETCH(int, expected);

    auto model= m_imCtrl->mainModel();

    auto chatrooms= model->data(model->index(0, 0), InstantMessaging::ChatroomSplitterModel::FilterModelRole)
                        .value<InstantMessaging::FilterInstantMessagingModel*>();

    for(auto player : playerList)
        m_playerModel->addPlayer(player);

    auto c= chatrooms->rowCount();
    QCOMPARE(c, expected);
}

void ChatWindowTest::chatRoomCountTest_data()
{
    QTest::addColumn<QList<Player*>>("playerList");
    QTest::addColumn<int>("expected");

    QTest::addRow("cmd1") << QList<Player*>() << 1;

    QList<Player*> list;
    auto player= new Player();
    player->setUuid("1");
    list << player;
    QTest::addRow("cmd2") << list << 2;

    list.clear();
    player= new Player();
    player->setUuid("3");
    list << player;
    player= new Player();
    player->setUuid("2");
    list << player;
    QTest::addRow("cmd3") << list << 3;
}

/*void ChatWindowTest::localPersonModelTest()
{
    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(0, 0), Qt::DisplayRole).toString(), m_player->name());
    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(0, 0), PlayerModel::IdentifierRole).toString(),
             m_player->getUuid());

    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(1, 0), Qt::DisplayRole).toString(),
             m_character->name());
    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(1, 0), PlayerModel::IdentifierRole).toString(),
             m_character->getUuid());
}*/

void ChatWindowTest::cleanup() {}

QTEST_MAIN(ChatWindowTest);

#include "tst_chat.moc"
