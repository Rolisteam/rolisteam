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

//#include "networklink.h"

#include <chat/chat.h>
#include <chat/chatwindow.h>
#include <chat/improvedtextedit.h>
#include <data/character.h>
#include <data/localpersonmodel.h>
#include <data/person.h>
#include <data/player.h>
#include <userlist/playersList.h>

class Player;
class NetworkLink;
class NetworkMessage;

class ChatWindowTest : public QObject
{
    Q_OBJECT

public:
    ChatWindowTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void changeUser();
    void showMessage();
    void enterText();
    void resendPrevious();

    void localPersonModelTest();

private:
    ImprovedTextEdit* m_impTextEditor;
    ChatWindow* m_chatWindow;
    Player* m_player;
    Character* m_character;
    std::unique_ptr<LocalPersonModel> m_localPersonModel;
};
ChatWindowTest::ChatWindowTest() {}
void ChatWindowTest::initTestCase()
{
    m_player= new Player("bob", Qt::black, false);
    m_character= new Character("character", Qt::red, false);
    m_player->addCharacter(m_character);
    PlayersList::instance()->setLocalPlayer(m_player);

    m_localPersonModel.reset(new LocalPersonModel);

    m_chatWindow= new ChatWindow(new PublicChat());
    m_chatWindow->setLocalPlayer(m_player);
    m_impTextEditor= m_chatWindow->getEditionZone();
}

void ChatWindowTest::enterText()
{
    QString test= QStringLiteral("Text Test Text Test");
    m_impTextEditor->setText(test);
    QSignalSpy spy(m_impTextEditor, SIGNAL(textValidated(bool, QString)));
    QTest::keyPress(m_impTextEditor, Qt::Key_Enter);
    QCOMPARE(spy.count(), 1);
}

void ChatWindowTest::changeUser()
{
    QSignalSpy spy(m_impTextEditor, SIGNAL(ctrlUp()));
    QTest::keyPress(m_impTextEditor, Qt::Key_Up, Qt::ControlModifier);
    QTest::keyPress(m_impTextEditor, Qt::Key_Enter);
    QCOMPARE(spy.count(), 1);
}

void ChatWindowTest::resendPrevious()
{
    QSignalSpy spy(m_impTextEditor, SIGNAL(textValidated(bool, QString)));
    QTest::keyPress(m_impTextEditor, Qt::Key_Up);
    QString txt= m_impTextEditor->document()->toPlainText();
    QCOMPARE(txt, QStringLiteral("Text Test Text Test"));
    QTest::keyPress(m_impTextEditor, Qt::Key_Enter);
    QCOMPARE(spy.count(), 1);
}

void ChatWindowTest::showMessage()
{
    QSignalSpy spy(m_chatWindow, SIGNAL(ChatWindowHasChanged(ChatWindow*)));
    QTest::keyPress(m_impTextEditor, Qt::Key_Up, Qt::ControlModifier);
    QString test= QStringLiteral("/me draws his weapons");
    m_impTextEditor->setText(test);
    QSignalSpy spy2(m_impTextEditor, SIGNAL(textValidated(bool, QString)));
    QTest::keyPress(m_impTextEditor, Qt::Key_Enter);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy2.count(), 1);
}

void ChatWindowTest::localPersonModelTest()
{
    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(0, 0), Qt::DisplayRole).toString(), m_player->name());
    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(0, 0), PlayersList::IdentifierRole).toString(),
             m_player->getUuid());

    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(1, 0), Qt::DisplayRole).toString(),
             m_character->name());
    QCOMPARE(m_localPersonModel->data(m_localPersonModel->index(1, 0), PlayersList::IdentifierRole).toString(),
             m_character->getUuid());
}

void ChatWindowTest::cleanupTestCase()
{
    delete m_impTextEditor;
}

QTEST_MAIN(ChatWindowTest);

#include "tst_chat.moc"
