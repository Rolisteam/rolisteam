/***************************************************************************
 *   Copyright (C) 2023 by Renaud Guezennec                                *
 *   renaud@rolisteam.org                                                  *
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

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QSignalSpy>
#include <QTest>
#include <memory>

#include "controller/instantmessagingcontroller.h"
//#include "rwidgets/mediacontainers/instantmessagingview.h"
#include "data/chatroom.h"
#include "data/player.h"
#include "diceparser_qobject/diceroller.h"
#include "helper.h"
#include "model/chatroomsplittermodel.h"
#include "model/messagemodel.h"
#include "model/playermodel.h"
#include "test_root_path.h"
#include <common_qml/theme.h>

void registerTypeTest()
{
    static bool registered= false;

    if(registered)
        return;

    Q_INIT_RESOURCE(viewsqml);
    Q_INIT_RESOURCE(textedit);
    Q_INIT_RESOURCE(rolisteam);
    Q_INIT_RESOURCE(resources);

    customization::Theme::setPath(QString("%1/resources/stylesheet/qml/theme.ini").arg(tests::root_path));
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");

    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);

    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject*
                                                   {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });

    QQuickStyle::setStyle("rolistyle");
    QQuickStyle::setFallbackStyle("Fusion");

    registered= true;
}

class InstantMessagingTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void sendMessage();
    void sendMessage_data();

    void splitScreen();
    void detachScreen();

private:
    std::unique_ptr<PlayerModel> m_model;
    std::unique_ptr<InstantMessagingController> m_ctrl;
    std::unique_ptr<Player> m_player1;
    std::unique_ptr<Player> m_player2;
    QString m_name1{Helper::randomString()};
    QString m_name2{Helper::randomString()};
};
void InstantMessagingTest::initTestCase()
{
    registerTypeTest();
}
void InstantMessagingTest::init()
{
    qDebug() << "INIT %%%%%%%%%%%%";
    m_player1= std::make_unique<Player>(m_name1, Qt::blue, true);
    m_player2= std::make_unique<Player>(m_name2, Qt::green, false);

    m_model.reset(new PlayerModel());
    m_ctrl.reset(new InstantMessagingController(m_model.get()));

    m_ctrl->setDiceParser(new DiceRoller(this));

    m_model->addPlayer(m_player1.get());
    m_model->addPlayer(m_player2.get());

    auto model= m_ctrl->mainModel();

    QCOMPARE(model->rowCount(), 1);
}

void InstantMessagingTest::cleanup()
{
    qDebug() << "CLEAN UP %%%%%%%%%%%%";
    m_ctrl.release();
    m_model.release();
    m_player1.release();
    m_player2.release();
}

void InstantMessagingTest::sendMessage()
{
    QFETCH(QString, text);
    QFETCH(QUrl, url);
    QFETCH(int, idx);
    QFETCH(QString, name);
    QFETCH(InstantMessaging::MessageInterface::MessageType, type);

    QList<Player*> list({m_player1.get(), m_player2.get()});
    auto personId= list[idx]->uuid();

    auto global= m_ctrl->globalChatroom();
    auto model= global->messageModel();

    QSignalSpy spy(model, &InstantMessaging::MessageModel::messageAdded);

    global->addMessage(text, url, personId, name);

    spy.wait(2000);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(model->rowCount(), 1);

    auto msgtype= model->data(model->index(0, 0), InstantMessaging::MessageModel::MessageTypeRole)
                      .value<InstantMessaging::MessageInterface::MessageType>();

    QCOMPARE(msgtype, type);
}
void InstantMessagingTest::sendMessage_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<int>("idx");
    QTest::addColumn<QString>("name");
    QTest::addColumn<InstantMessaging::MessageInterface::MessageType>("type");

    QTest::addRow("text1") << "hello there" << QUrl{} << 0 << m_name1 << InstantMessaging::MessageInterface::Text;
    QTest::addRow("text2") << "General Kenobi" << QUrl{} << 1 << m_name2 << InstantMessaging::MessageInterface::Text;

    QTest::addRow("dice1") << "!1d100" << QUrl{} << 0 << m_name1 << InstantMessaging::MessageInterface::Dice;
    QTest::addRow("dice2") << "!10d10" << QUrl{} << 1 << m_name2 << InstantMessaging::MessageInterface::Dice;

    QTest::addRow("cmd1") << "/e draws his sword" << QUrl{} << 0 << m_name1
                          << InstantMessaging::MessageInterface::Command;
    QTest::addRow("cmd2") << "/me pets the cat" << QUrl{} << 1 << m_name2
                          << InstantMessaging::MessageInterface::Command;

    QTest::addRow("error1") << "!4d10k8" << QUrl{} << 0 << m_name1 << InstantMessaging::MessageInterface::Error;
    QTest::addRow("error2") << "!4d10e[>0]" << QUrl{} << 1 << m_name2 << InstantMessaging::MessageInterface::Error;
}

void InstantMessagingTest::splitScreen() {}
void InstantMessagingTest::detachScreen() {}

QTEST_MAIN(InstantMessagingTest);

#include "tst_im.moc"
