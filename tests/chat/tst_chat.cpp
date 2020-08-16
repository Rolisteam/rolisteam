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

#include <data/character.h>
#include <data/localpersonmodel.h>
#include <data/person.h>
#include <data/player.h>
#include <userlist/playermodel.h>

#include "controller/instantmessagingcontroller.h"
#include "qmlchat/avatarprovider.h"
#include "qmlchat/chatroom.h"
#include "qmlchat/chatroomfactory.h"
#include "qmlchat/chatroomsplittermodel.h"
#include "qmlchat/dicemessage.h"
#include "qmlchat/filteredplayermodel.h"
#include "qmlchat/filterinstantmessagingmodel.h"
#include "qmlchat/instantmessagingmodel.h"
#include "qmlchat/instantmessagingview.h"
#include "qmlchat/messagefactory.h"
#include "qmlchat/messageinterface.h"
#include "qmlchat/messagemodel.h"
#include "qmlchat/textmessage.h"
#include "qmlchat/textwritercontroller.h"

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

void ChatWindowTest::diceOutputTest()
{
    QFETCH(QString, command);
    QFETCH(QString, result);
    QFETCH(bool, regex);

    QString title;
    QString message;
    m_chatWindow->manageDiceRoll(command, title, message, true, false);

    if(!regex)
        QCOMPARE(result, message);
    else
    {
        QRegularExpression exp(result);
        auto match= exp.match(message);
        qDebug() << result;
        QVERIFY2(match.hasMatch(), message.toUtf8());
    }
}

void ChatWindowTest::diceOutputTest_data()
{
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("result");
    QTest::addColumn<bool>("regex");

    QTest::addRow("cmd1") << "\"text\""
                          << "got <span class=\"dice\">text</span> [&quot;text&quot;]" << false;
    QTest::addRow("cmd2") << "8;9;19"
                          << "got <span class=\"dice\">8,9,19</span> [8;9;19]" << false;
    QTest::addRow("cmd3") << "1d[101..101];1d[2..2]"
                          << "got <span class=\"dice\">101,2</span> [1d[101..101];1d[2..2] (<span "
                             "class=\"dice\">101</span> - <span class=\"dice\">2</span>)]"
                          << false;
    QTest::addRow("cmd4") << "[7,7,6,4,3,2,2,1]k2"
                          << "got <span class=\"dice\">14</span> [[7,7,6,4,3,2,2,1]k2 (<span "
                             "class=\"dice\">7,7</span>,6,4,3,2,2,1)]"
                          << false;

    QTest::addRow("cmd5") << "[1,4,7,9,2]c[<5]"
                          << "got <span class=\"dice\">3</span> [[1,4,7,9,2]c[&lt;5] (<span "
                             "class=\"dice\">1,4</span>,7,9,<span class=\"dice\">2</span>)]"
                          << false;

    QTest::addRow("cmd6") << "[2,5,5,5,6,6,7,8,9,10]o"
                          << "got <span class=\"dice\">1x2,3x5,2x6,1x7,1x8,1x9,1x10 - [2,5,5,5,6,6,7,8,9,10]</span> "
                             "[[2,5,5,5,6,6,7,8,9,10]o]"
                          << false;

    QTest::addRow("cmd7") << "[2,2,8,9,7,4,1,1]p[1:blue]"
                          << "got <span class=\"dice\">34</span> [[2,2,8,9,7,4,1,1]p[1:blue] (<span "
                             "style=\"color:blue;font-weight:bold\">2</span>,<span "
                             "class=\"dice\">2,8,9,7,4,1,1</span>)]"
                          << false;

    QTest::addRow("cmd8")
        << "[2,2,8,9,7,4,1,1]f[!=7]"
        << "got <span class=\"dice\">27</span> [[2,2,8,9,7,4,1,1]f[!=7] (<span class=\"dice\">2,2,8,9,4,1,1</span>)]"
        << false;

    QTest::addRow("cmd9")
        << "1d[6..6];1d[8..8]mk1"
        << "got <span class=\"dice\">8</span> [1d[6..6];1d[8..8]mk1 (<span class=\"dice\">8</span>,6)]"
        << false; // (d6<span class=\"dice\">6</span>)]";

    QTest::addRow("cmd10")
        << "1d6;1d8mk1"
        << "got <span class=\\\"dice\\\">\\d+</span> \\[1d6;1d8mk1 \\(d6:\\((<span "
           "class=\\\"dice\\\">\\d+</span>|\\d+)\\) d8:\\((\\d+|<span class=\\\"dice\\\">\\d+</span>)\\)\\)\\]"
        << true;

    QTest::addRow("cmd11")
        << "repeat(1d10,5)"
        << "got <span class=\\\"dice\\\">\\d+\n\\d+\n\\d+\n\\d+\n\\d+</span> \\[repeat\\(1d10,5\\)\\]" << true;

    QTest::addRow("cmd12") << "1d1i:[>3]{\"Success\"}{\"Fail\"}"
                           << "got <span class=\"dice\">Fail</span> "
                              "[1d1i:[&gt;3]{&quot;Success&quot;}{&quot;Fail&quot;} (<span class=\"dice\">1</span>)]"
                           << false;

    QTest::addRow("cmd13") << "2Lu[cat,dog]"
                           << "got <span class=\\\"dice\\\">(cat,dog|dog,cat)</span>"
                              " \\[2Lu\\[cat,dog\\]\\]"
                           << true;

    QTest::addRow("cmd14") << "2d6t"
                           << "got <span class=\\\"dice\\\">\\d+</span> \\[2d6t \\(.*\\)\\]" << true;

    QTest::addRow("cmd15")
        << "2d8;2d12b;$2k2;$2k2kl1;\"your total is $3 with lowest: $4\""
        << "got <span class=\\\"dice\\\">your total is \\d+ with lowest: \\d+</span> "
           "\\[2d8;2d12b;\\$2k2;\\$2k2kl1;&quot;your total is \\$3 with lowest: \\$4&quot; \\(.*\\)\\]"
        << true;

    QTest::addRow("cmd16") << "[10,7,7,3,2]gs10"
                           << "got <span class=\\\"dice\\\">2 \\(\\{10\\},\\{7,3\\} - \\[7,2\\]\\)</span> "
                              "\\[\\[10,7,7,3,2\\]gs10 \\(<span class=\\\"dice\\\">.*</span>\\)\\]"
                           << true;
}

QTEST_MAIN(ChatWindowTest);

#include "tst_chat.moc"
