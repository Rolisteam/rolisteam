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

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>

#include "data/character.h"
#include "network/ipbanaccepter.h"
#include "network/iprangeaccepter.h"
#include "network/messagedispatcher.h"
#include "network/networkmessagewriter.h"
#include "network/passwordaccepter.h"
#include "network/timeaccepter.h"
#include "worker/messagehelper.h"
#include "worker/playermessagehelper.h"

#include <helper.h>

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
    void messageRecipiantTest();
    void ipBanAccepterTest();
    void ipBanAccepterTest_data();
    void passwordAccepterTest();
    void passwordAccepterTest_data();
    void ipRangeAccepterTest();
    void ipRangeAccepterTest_data();
    void timeAccepterTest();
    void timeAccepterTest_data();

    void messageDispatcherTest();

    void messageHeaderTest();
    void messageHeaderTest_data();

    void messageWriterTest();
    void playerMessageHelper();

    void messageHelperTest();

private:
    std::unique_ptr<NetworkMessageWriter> m_writer;
    std::unique_ptr<IpBanAccepter> m_ipBanAccepter;
    std::unique_ptr<PasswordAccepter> m_passwordAccepter;
    std::unique_ptr<IpRangeAccepter> m_ipRangeAccepter;
    std::unique_ptr<TimeAccepter> m_timeAccepter;
};

Q_DECLARE_METATYPE(PasswordAccepter::Level)

TestNetwork::TestNetwork() {}

void TestNetwork::initTestCase() {}

void TestNetwork::cleanupTestCase() {}

void TestNetwork::init()
{
    m_writer.reset(new NetworkMessageWriter(NetMsg::MediaCategory, NetMsg::AddMedia));
    m_ipBanAccepter.reset(new IpBanAccepter());
    m_passwordAccepter.reset(new PasswordAccepter());
    m_ipRangeAccepter.reset(new IpRangeAccepter());
    m_timeAccepter.reset(new TimeAccepter());
}
void TestNetwork::writeTest()
{
    for(quint8 i= 0; i < 255; ++i)
    {
        m_writer->uint8(i);
        QCOMPARE(m_writer->getDataSize(), (1 + i) * sizeof(quint8) + 1); //+sizeof(NetworkMessageHeader)
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

void TestNetwork::cleanup() {}

void TestNetwork::messageRecipiantTest()
{
    QStringList list= {"client1", "client2"};
    QCOMPARE(m_writer->getRecipientMode(), NetworkMessage::All);

    m_writer->setRecipientList(list, NetworkMessage::OneOrMany);

    QCOMPARE(m_writer->getRecipientMode(), NetworkMessage::OneOrMany);
}

void TestNetwork::ipBanAccepterTest()
{
    QFETCH(QString, currentIp);
    QFETCH(QStringList, ipBan);
    QFETCH(bool, expected);

    QMap<QString, QVariant> data= {{"currentIp", currentIp}, {"IpBan", ipBan}};

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
    QTest::addRow("ipv6 ban") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334"
                              << QStringList({"2001:0db8:85a3:0000:0000:8a2e:0370:7334"}) << false;
    QTest::addRow("any") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334:192.168.0.27" << QStringList() << true;
    QTest::addRow("any ban v6") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334:192.168.1.45"
                                << QStringList({"2001:0db8:85a3:0000:0000:8a2e:0370:7334", "192.168.1.44"}) << false;
    QTest::addRow("any ban v4") << "2001:0db8:85a3:0000:0000:8a2e:0370:7334:192.168.1.45"
                                << QStringList({"2001:0db8:85a3:0000:0000:4a2e:0370:7334", "192.168.1.45"}) << false;
} // 2001:0db8:85a3:0000:0000:8a2e:0370:7334

void TestNetwork::passwordAccepterTest()
{
    QFETCH(QString, currentPw);
    QFETCH(QString, expectedPw);
    QFETCH(PasswordAccepter::Level, level);
    QFETCH(bool, expected);

    m_passwordAccepter.reset(new PasswordAccepter(level));

    QString key;

    if(PasswordAccepter::Connection == level)
    {
        key= "ServerPassword";
    }
    else if(PasswordAccepter::Admin == level)
    {
        key= "AdminPassword";
    }
    else if(PasswordAccepter::Channel == level)
    {
        key= "ChannelPassword";
    }

    QVERIFY(!key.isEmpty());

    QMap<QString, QVariant> data= {{"userpassword", currentPw}, {key, expectedPw}};

    QCOMPARE(m_passwordAccepter->isValid(data), expected);
}
void TestNetwork::passwordAccepterTest_data()
{
    QTest::addColumn<QString>("currentPw");
    QTest::addColumn<QString>("expectedPw");
    QTest::addColumn<PasswordAccepter::Level>("level");
    QTest::addColumn<bool>("expected");

    QTest::addRow("server") << "tagada"
                            << "tagada" << PasswordAccepter::Connection << true;
    QTest::addRow("server1") << "tagada"
                             << "tagada1" << PasswordAccepter::Connection << false;
    QTest::addRow("server2") << "tagada"
                             << "" << PasswordAccepter::Connection << false;

    QTest::addRow("admin") << "tagada"
                           << "tagada" << PasswordAccepter::Admin << true;
    QTest::addRow("admin1") << "tagada"
                            << "tagada1" << PasswordAccepter::Admin << false;
    QTest::addRow("admin2") << "tagada"
                            << "" << PasswordAccepter::Admin << false;

    QTest::addRow("channel") << "tagada"
                             << "tagada" << PasswordAccepter::Channel << true;
    QTest::addRow("channel1") << "tagada"
                              << "tagada1" << PasswordAccepter::Channel << false;
    QTest::addRow("channel2") << "tagada"
                              << "" << PasswordAccepter::Channel << false;
}

void TestNetwork::ipRangeAccepterTest()
{
    QFETCH(QString, currentIp);
    QFETCH(QString, range);
    QFETCH(bool, expected);

    QMap<QString, QVariant> data= {{"currentIp", currentIp}, {"rangeIp", range}};

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
    QTest::addRow("range2") << "79.80.80.80"
                            << "80.0.0.0.0/255.0.0.0" << false;

    QTest::addRow("range3") << "192.168.1.15"
                            << "192.168.1.0/24" << true;
    QTest::addRow("range3") << "192.168.0.15"
                            << "192.168.1.0/24" << false;
    QTest::addRow("no ip") << ""
                           << "192.168.1.0/24" << false;
    QTest::addRow("no range") << "192.168.0.15" << QString() << true;
}
void TestNetwork::timeAccepterTest()
{
    bool expected= false;
    QFETCH(QString, start);
    QFETCH(QString, end);

    QTime time= QTime::currentTime();
    const QString format= QStringLiteral("hh:mm");
    QTime startT= QTime::fromString(start, format);
    QTime endT= QTime::fromString(end, format);

    if(start.isEmpty() && end.isEmpty())
        expected= true;
    else if(time >= startT && time <= endT)
        expected= true;

    QMap<QString, QVariant> data= {{"TimeStart", start}, {"TimeEnd", end}};

    QCOMPARE(m_timeAccepter->isValid(data), expected);
}
void TestNetwork::timeAccepterTest_data()
{
    QTest::addColumn<QString>("start");
    QTest::addColumn<QString>("end");

    int count= 0;
    for(int i= 0; i < 24; ++i)
    {
        QTest::addRow("time %d", ++count) << QStringLiteral("%1:00").arg(i, 2, 10, QLatin1Char('0'))
                                          << QStringLiteral("%1:30").arg(i, 2, 10, QLatin1Char('0'));

        QTest::addRow("time %d", ++count) << QStringLiteral("%1:30").arg(i, 2, 10, QLatin1Char('0'))
                                          << QStringLiteral("%1:00").arg(i + 1, 2, 10, QLatin1Char('0'));
    }

    QTest::addRow("time_null") << ""
                               << "";
}

void TestNetwork::messageDispatcherTest()
{
    MessageDispatcher dispatch;

    dispatch.dispatchMessage(QByteArray(), nullptr, nullptr);
    dispatch.dispatchMessage(Helper::randomData(Helper::generate(100, 500)), nullptr, nullptr);

    Channel channel;
    ServerConnection connect(nullptr);
    channel.addChild(&connect);
    dispatch.dispatchMessage(Helper::randomData(Helper::generate(100, 500)), &channel, nullptr);
    dispatch.dispatchMessage(Helper::randomData(Helper::generate(100, 500)), &channel, &connect);

    /*
     * struct NETWORK_EXPORT NetworkMessageHeader
{
    quint8 category;
    quint8 action;
    quint32 dataSize;
};
*/
}

void TestNetwork::messageWriterTest()
{
    NetworkMessageWriter writer(NetMsg::Category::AdministrationCategory, NetMsg::Action::EndConnectionAction,
                                NetworkMessage::All, 0);

    QCOMPARE(writer.category(), NetMsg::Category::AdministrationCategory);
    QCOMPARE(writer.action(), NetMsg::Action::EndConnectionAction);

    auto header= writer.buffer();

    QCOMPARE(header->category, NetMsg::Category::AdministrationCategory);
    QCOMPARE(header->action, NetMsg::Action::EndConnectionAction);
    QCOMPARE(writer.bufferSize(), 128);

    writer.int16(18);
    writer.int32(200);

    QCOMPARE(writer.currentPos(), sizeof(NetworkMessageHeader) + sizeof(qint32) + sizeof(qint16) + 1);
    QCOMPARE(writer.getRecipientList(), QStringList{});

    QPixmap pix;
    pix.loadFromData(Helper::imageData(false));
    writer.pixmap(pix);
}

void TestNetwork::messageHeaderTest()
{
    QFETCH(quint8, cat);
    QFETCH(quint8, action);
    QFETCH(QString, resultCat);
    QFETCH(QString, resultAct);

    NetworkMessageHeader header{cat, action, 0};

    QCOMPARE(MessageDispatcher::act2String(&header), resultAct);
    QCOMPARE(MessageDispatcher::cat2String(&header), resultCat);
}

void TestNetwork::messageHeaderTest_data()
{
    QTest::addColumn<quint8>("cat");
    QTest::addColumn<quint8>("action");
    QTest::addColumn<QString>("resultCat");
    QTest::addColumn<QString>("resultAct");

    QStringList cats{"AdministrationCategory",
                     "PlayerCategory",
                     "CharacterPlayerCategory",
                     "NPCCategory",
                     "CharacterCategory",
                     "DrawCategory",
                     "MapCategory",
                     "InstantMessageCategory",
                     "MusicCategory",
                     "SetupCategory",
                     "CampaignCategory",
                     "VMapCategory",
                     "MediaCategory",
                     "SharedNoteCategory",
                     "WebPageCategory",
                     "MindMapCategory"};

    QList<QStringList> actionPerCategorie;

    actionPerCategorie.append({"EndConnectionAction",
                               "Heartbeat",
                               "ConnectionInfo",
                               "Goodbye",
                               "Kicked",
                               "MoveChannel",
                               "SetChannelList",
                               "RenameChannel",
                               "NeedPassword",
                               "AuthentificationSucessed",
                               "AuthentificationFail",
                               "LockChannel",
                               "UnlockChannel",
                               "JoinChannel",
                               "DeleteChannel",
                               "AddChannel",
                               "ChannelPassword",
                               "ResetChannelPassword",
                               "BanUser",
                               "ClearTable",
                               "AdminPassword",
                               "AdminAuthSucessed",
                               "AdminAuthFail",
                               "MovedIntoChannel",
                               "GMStatus",
                               "ResetChannel"});

    actionPerCategorie.append({"PlayerConnectionAction", "DelPlayerAction", "ChangePlayerProperty"});

    actionPerCategorie.append({"AddPlayerCharacterAction", "DelPlayerCharacterAction",
                               "ToggleViewPlayerCharacterAction", "ChangePlayerCharacterSizeAction",
                               "ChangePlayerCharacterProperty"});

    actionPerCategorie.append(QStringList{"addNpc", "delNpc"});

    actionPerCategorie.append({"addCharacterList", "moveCharacter", "changeCharacterState",
                               "changeCharacterOrientation", "showCharecterOrientation", "addCharacterSheet",
                               "updateFieldCharacterSheet", "closeCharacterSheet"});

    actionPerCategorie.append({"penPainting", "linePainting", "emptyRectanglePainting", "filledRectanglePainting",
                               "emptyEllipsePainting", "filledEllipsePainting", "textPainting", "handPainting"});

    actionPerCategorie.append({"AddEmptyMap", "LoadMap", "ImportMap", "CloseMap"});

    actionPerCategorie.append({
        "InstantMessageAction",
        "AddChatroomAction",
        "RemoveChatroomAction",
        "UpdateChatAction",
    });

    actionPerCategorie.append({
        "StopSong",
        "PlaySong",
        "PauseSong",
        "NewSong",
        "ChangePositionSong",
    });
    actionPerCategorie.append(QStringList{"AddFeatureAction"});
    actionPerCategorie.append({
        "addDiceAlias",
        "moveDiceAlias",
        "removeDiceAlias",
        "addCharacterState",
        "moveCharacterState",
        "removeCharacterState",
        "DiceAliasModel",
        "CharactereStateModel",
    });
    actionPerCategorie.append({
        "AddItem",
        "DeleteItem",
        "UpdateItem",
        "AddPoint",
        "DeletePoint",
        "MovePoint",
        "SetParentItem",
        "CharcaterVisionChanged",
        "CharacterStateChanged",
        "CharacterChanged",
        "HighLightPosition",
    });
    actionPerCategorie.append({
        "AddMedia",
        "UpdateMediaProperty",
        "CloseMedia",
        "AddSubImage",
        "RemoveSubImage",
    });
    actionPerCategorie.append({
        "updateTextAndPermission",
        "updateText",
        "updatePermissionOneUser",
    });
    actionPerCategorie.append(QStringList{"UpdateContent"});
    actionPerCategorie.append(
        {"AddMessage", "RemoveMessage", "UpdateNode", "UpdatePackage", "UpdateLink", "UpdateMindMapPermission"});

    QCOMPARE(cats.size(), actionPerCategorie.size());

    for(quint8 i= 0; i < std::numeric_limits<quint8>::max(); ++i)
    {
        QString cat("UnknownCategory");
        QStringList actionList;
        if(i < cats.size())
        {
            actionList= actionPerCategorie[i];
            cat= cats[i];
        }

        for(quint8 j= 0; j < std::numeric_limits<quint8>::max(); ++j)
        {
            QString act("Unknown Action");
            if(j < actionList.size())
                act= actionList[j];

            QTest::addRow("test: %d, %d", i, j) << i << j << cat << act;
        }
    }
}

void TestNetwork::playerMessageHelper()
{

    // Player info
    Helper::TestMessageSender sender;
    NetworkMessage::setMessageSender(&sender);

    Player player;
    player.setName(Helper::randomString());
    player.setUuid(Helper::randomString());

    auto pw= Helper::randomData();
    PlayerMessageHelper::sendOffConnectionInfo(&player, pw);

    auto msgData= sender.messageData();

    PlayerMessageHelper::sendOffConnectionInfo(nullptr, pw);

    {
        NetworkMessageReader reader;
        reader.setData(msgData);

        QCOMPARE(reader.byteArray32(), pw);
        QCOMPARE(reader.string32(), player.name());
        QCOMPARE(reader.string32(), player.uuid());
    }

    // Player info
    player.setGM(true);
    player.setAvatar(Helper::imageData(true));
    player.setColor(Helper::randomColor());
    PlayerMessageHelper::sendOffPlayerInformations(&player);

    {
        NetworkMessageReader reader;
        reader.setData(sender.messageData());

        Player tempPlayer;

        PlayerMessageHelper::readPlayer(reader, &tempPlayer);

        QCOMPARE(player.isGM(), tempPlayer.isGM());
        QCOMPARE(player.avatar(), tempPlayer.avatar());
        QCOMPARE(player.getColor(), tempPlayer.getColor());
        QCOMPARE(player.name(), tempPlayer.name());
        QCOMPARE(player.uuid(), tempPlayer.uuid());

        PlayerMessageHelper::sendOffPlayerInformations(nullptr);
    }

    {

        auto character= new Character();
        character->setUuid(Helper::randomString());
        character->setName(Helper::randomString());
        character->setColor(Helper::randomColor());
        character->setLifeColor(Helper::randomColor());
        character->setNpc(false);
        character->setAvatar(Helper::randomData());

        Player player;
        player.setName(Helper::randomString());
        player.setUuid(Helper::randomString());
        player.setGM(true);
        player.setAvatar(Helper::imageData(true));
        player.setColor(Helper::randomColor());
        player.addCharacter(character);

        PlayerMessageHelper::sendOffPlayerInformations(&player);

        NetworkMessageReader reader;
        reader.setData(sender.messageData());
        Player tempPlayer;
        PlayerMessageHelper::readPlayer(reader, &tempPlayer);

        QCOMPARE(player.isGM(), tempPlayer.isGM());
        QCOMPARE(player.avatar(), tempPlayer.avatar());
        QCOMPARE(player.getColor(), tempPlayer.getColor());
        QCOMPARE(player.name(), tempPlayer.name());
        QCOMPARE(player.uuid(), tempPlayer.uuid());

        QCOMPARE(tempPlayer.characterCount(), 1);
        auto tempCharacter= tempPlayer.characterById(character->uuid());

        QCOMPARE(character->name(), tempCharacter->name());
        QCOMPARE(character->getLifeColor(), tempCharacter->getLifeColor());
        QCOMPARE(character->getColor(), tempCharacter->getColor());
        QCOMPARE(character->avatar(), tempCharacter->avatar());
        QCOMPARE(character->uuid(), tempCharacter->uuid());
    }
}

void TestNetwork::messageHelperTest()
{
    Helper::TestMessageSender sender;
    NetworkMessage::setMessageSender(&sender);

    {
        MessageHelper::sendOffGoodBye();

        NetworkMessageReader reader;
        reader.setData(sender.messageData());
        QCOMPARE(reader.action(), NetMsg::Goodbye);
        QCOMPARE(reader.category(), NetMsg::AdministrationCategory);
    }

    {
        auto id= Helper::randomString();
        MessageHelper::closeMedia(id, Core::ContentType::CHARACTERSHEET);

        NetworkMessageReader reader;
        reader.setData(sender.messageData());

        QCOMPARE(reader.action(), NetMsg::CloseMedia);
        QCOMPARE(reader.category(), NetMsg::MediaCategory);

        QCOMPARE(reader.uint8(), static_cast<int>(Core::ContentType::CHARACTERSHEET));
        QCOMPARE(reader.string8(), id);
    }
}

QTEST_MAIN(TestNetwork);

#include "tst_network.moc"
