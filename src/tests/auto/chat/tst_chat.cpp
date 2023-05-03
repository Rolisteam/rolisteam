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
#include <helper.h>

#include "controller/instantmessagingcontroller.h"
#include "data/chatroom.h"
#include "data/chatroomfactory.h"
#include "instantmessaging/dicemessage.h"
#include "instantmessaging/errormessage.h"
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
#include <QAbstractItemModelTester>

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

    void textwriterControllerTest();
    void diceMessageTest();

    void errorMessageTest();

private:
    std::unique_ptr<PlayerModel> m_playerModel;
    std::unique_ptr<InstantMessagingController> m_imCtrl;
};
ChatWindowTest::ChatWindowTest() {}

void ChatWindowTest::init()
{
    m_playerModel.reset(new PlayerModel);
    new QAbstractItemModelTester(m_playerModel.get());
    m_imCtrl.reset(new InstantMessagingController(m_playerModel.get()));
}
void ChatWindowTest::errorMessageTest()
{
    auto owner= Helper::randomString();
    auto writer= Helper::randomString();
    auto time= QDateTime::currentDateTime();
    InstantMessaging::ErrorMessage msg(owner, writer, time);

    QSignalSpy spy(&msg, &InstantMessaging::ErrorMessage::textChanged);
    auto text1= Helper::randomString();
    msg.setText(text1);
    msg.setText(text1);

    spy.wait(10);
    QCOMPARE(spy.count(), 1);

    auto text2= Helper::randomString();
    msg.setText(text2);
    spy.wait(10);
    QCOMPARE(spy.count(), 2);

    QCOMPARE(msg.text(), text2);

    QCOMPARE(msg.owner(), owner);
    QCOMPARE(msg.writer(), writer);
    QCOMPARE(msg.dateTime(), time);
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

void ChatWindowTest::textwriterControllerTest()
{
    InstantMessaging::TextWriterController ctrl;

    QSignalSpy spy(&ctrl, &InstantMessaging::TextWriterController::urlChanged);
    QSignalSpy spy1(&ctrl, &InstantMessaging::TextWriterController::textComputed);
    QSignalSpy spy2(&ctrl, &InstantMessaging::TextWriterController::textChanged);
    QSignalSpy spy3(&ctrl, &InstantMessaging::TextWriterController::diceCommandChanged);

    auto str= Helper::randomString();
    ctrl.setText(str);
    ctrl.computeText();

    spy1.wait(100);
    QCOMPARE(spy1.count(), 1);

    spy1.clear();
    ctrl.setText(str);

    spy1.wait(10);
    QCOMPARE(spy1.count(), 0);

    auto url= Helper::randomUrl();
    ctrl.setText(url.toString());
    ctrl.computeText();

    spy.wait(10);
    spy1.wait(100);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(ctrl.imageLink(), QString());
    ctrl.setText("www.perdu.com");
    ctrl.computeText();

    spy.wait(10);
    spy1.wait(100);
    QCOMPARE(spy.count(), 3);

    auto cmd= QString("!%1").arg(Helper::randomString());
    ctrl.setText(cmd);

    spy3.wait(10);
    QCOMPARE(spy3.count(), 1);
    QVERIFY(ctrl.diceCommand());

    ctrl.setText(Helper::randomString());
    cmd= QString("!%1").arg(Helper::randomString());
    ctrl.setText(cmd);

    spy3.wait(10);
    QCOMPARE(spy3.count(), 3);

    QCOMPARE(spy2.count(), 10);

    spy2.clear();

    static QStringList msgs{Helper::randomString(), Helper::randomString(), Helper::randomString(),
                            Helper::randomString()};

    ctrl.setText(msgs[0]);
    QCOMPARE(ctrl.text(), msgs[0]);
    ctrl.send();

    ctrl.setText(msgs[1]);
    QCOMPARE(ctrl.text(), msgs[1]);
    ctrl.send();

    ctrl.setText(msgs[2]);
    QCOMPARE(ctrl.text(), msgs[2]);
    ctrl.send();

    ctrl.setText(msgs[3]);
    QCOMPARE(ctrl.text(), msgs[3]);
    ctrl.send();

    ctrl.up();
    QCOMPARE(ctrl.text(), msgs[3]);
    ctrl.up();
    QCOMPARE(ctrl.text(), msgs[2]);

    ctrl.down();
    QCOMPARE(ctrl.text(), msgs[3]);

    QCOMPARE(spy2.count(), 11);

    for(int i= 0; i < 200; ++i)
    {
        ctrl.setText(Helper::randomString());
        ctrl.send();
    }

    for(int i= 0; i < 200; ++i)
        ctrl.up();

    for(int i= 0; i < 200; ++i)
        ctrl.down();
}

void ChatWindowTest::diceMessageTest()
{
    auto owner= Helper::randomString();
    auto writer= Helper::randomString();

    InstantMessaging::DiceMessage msg(owner, writer, QDateTime::currentDateTime());

    msg.setText("{\n\"command\":\"8d10;\\\"Result: $1 [@1]\\\"\",\n   \"comment\":\"\",\n   \"error\":\"\",\n   "
                "\"instructions\":[\n      {\n         \"diceval\":[\n            {\n               \"color\":\"\",\n  "
                "             \"displayed\":false,\n               \"face\":10,\n               \"highlight\":true,\n  "
                "             \"string\":\"<span style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"b8f031dc-04ec-41b6-9752-ae93f0b4d64f\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>\",\n               "
                "\"uuid\":\"381cbf99-ba59-46d0-8350-596679315262\",\n               \"value\":2\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n               "
                "\"uuid\":\"c3632d2e-43af-41fb-97f3-d7e92d6d1f47\",\n               \"value\":4\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"f4962e7f-dd41-4fa9-afbe-f382a76274f0\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>\",\n               "
                "\"uuid\":\"53679f15-6dde-418b-9743-a030df721c14\",\n               \"value\":10\n            },\n     "
                "       {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"20c8fe23-da7a-4319-9694-b93e6c4762d2\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"28a8f62c-92b9-4784-bcb8-03d0880d6c14\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>\",\n               "
                "\"uuid\":\"5aa9094b-e5f2-48b7-99bf-7d8a21ad7361\",\n               \"value\":7\n            }\n       "
                "  ],\n         \"scalar\":53\n      },\n      {\n         \"string\":\"Result: $1 [@1]\"\n      }\n   "
                "],\n   \"scalar\":\"53\",\n   \"string\":\"Result: 53 [<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>]\",\n   \"warning\":\"\"\n}");

    msg.setText("{\n\"command\":\"8d10;\\\"Result: $1 [@1]\\\"\",\n   \"comment\":\"\",\n   \"error\":\"\",\n   "
                "\"instructions\":[\n      {\n         \"diceval\":[\n            {\n               \"color\":\"\",\n  "
                "             \"displayed\":false,\n               \"face\":10,\n               \"highlight\":true,\n  "
                "             \"string\":\"<span style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"b8f031dc-04ec-41b6-9752-ae93f0b4d64f\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>\",\n               "
                "\"uuid\":\"381cbf99-ba59-46d0-8350-596679315262\",\n               \"value\":2\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n               "
                "\"uuid\":\"c3632d2e-43af-41fb-97f3-d7e92d6d1f47\",\n               \"value\":4\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>\",\n               "
                "\"uuid\":\"f4962e7f-dd41-4fa9-afbe-f382a76274f0\",\n               \"value\":9\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>\",\n               "
                "\"uuid\":\"53679f15-6dde-418b-9743-a030df721c14\",\n               \"value\":10\n            },\n     "
                "       {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"20c8fe23-da7a-4319-9694-b93e6c4762d2\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>\",\n               "
                "\"uuid\":\"28a8f62c-92b9-4784-bcb8-03d0880d6c14\",\n               \"value\":6\n            },\n      "
                "      {\n               \"color\":\"\",\n               \"displayed\":false,\n               "
                "\"face\":10,\n               \"highlight\":true,\n               \"string\":\"<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>\",\n               "
                "\"uuid\":\"5aa9094b-e5f2-48b7-99bf-7d8a21ad7361\",\n               \"value\":7\n            }\n       "
                "  ],\n         \"scalar\":53\n      },\n      {\n         \"string\":\"Result: $1 [@1]\"\n      }\n   "
                "],\n   \"scalar\":\"53\",\n   \"string\":\"Result: 53 [<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">2</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">4</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">9</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">10</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">6</span>,<span "
                "style=\\\"color:red;font-weight:bold;\\\">7</span>]\",\n   \"warning\":\"\"\n}");

    msg.setText(
        "{\n    \"command\": \"2d6+2d8\",\n    \"comment\": \"\",\n    \"error\": \"\",\n    \"instructions\": [\n     "
        "   {\n            \"diceval\": [\n                {\n                    \"color\": \"\",\n                   "
        " \"displayed\": false,\n                    \"face\": 6,\n                    \"highlight\": true,\n          "
        "          \"string\": \"<span style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n                    "
        "\"uuid\": \"f8077eac-d1d3-4711-bc1e-1f2ff6532d0c\",\n                    \"value\": 4\n                },\n   "
        "             {\n                    \"color\": \"\",\n                    \"displayed\": false,\n             "
        "       \"face\": 6,\n                    \"highlight\": true,\n                    \"string\": \"<span "
        "style=\\\"color:red;font-weight:bold;\\\">5</span>\",\n                    \"uuid\": "
        "\"763015b9-3764-43d9-8ef9-ae55cde23094\",\n                    \"value\": 5\n                },\n             "
        "   {\n                    \"color\": \"\",\n                    \"displayed\": false,\n                    "
        "\"face\": 8,\n                    \"highlight\": true,\n                    \"string\": \"<span "
        "style=\\\"color:red;font-weight:bold;\\\">3</span>\",\n                    \"uuid\": "
        "\"ecacc278-f5b8-41bd-a936-df29101c6d25\",\n                    \"value\": 3\n                },\n             "
        "   {\n                    \"color\": \"\",\n                    \"displayed\": false,\n                    "
        "\"face\": 8,\n                    \"highlight\": true,\n                    \"string\": \"<span "
        "style=\\\"color:red;font-weight:bold;\\\">4</span>\",\n                    \"uuid\": "
        "\"ae62c050-9155-4e50-8b01-ee9a8f3e0906\",\n                    \"value\": 4\n                }\n            "
        "],\n            \"scalar\": 16\n        }\n    ],\n    \"scalar\": \"16\",\n    \"string\": \"16\",\n    "
        "\"warning\": \"\"\n}\n");

    QCOMPARE(msg.command(), "2d6+2d8");
    QCOMPARE(msg.comment(), "");
    QVERIFY(!msg.result().isEmpty());
    QVERIFY(!msg.details().isEmpty());
    QVERIFY(!msg.text().isEmpty());
}

void ChatWindowTest::cleanup() {}

QTEST_MAIN(ChatWindowTest);

#include "tst_chat.moc"
