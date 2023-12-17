/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
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

#include <QColor>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <data/character.h>
#include <data/player.h>
#include <helper.h>

#define COUNT_TURN 2000

class DataplayerTest : public QObject
{
    Q_OBJECT

public:
    DataplayerTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testGetSet();
    void testChildrenAddAndRemove();

private:
    Player* m_player;
};

DataplayerTest::DataplayerTest() {}

void DataplayerTest::initTestCase()
{
    m_player= new Player();
}

void DataplayerTest::cleanupTestCase()
{
    delete m_player;
}

void DataplayerTest::testGetSet()
{
    QColor tmpcolor(Qt::green);
    m_player->setColor(tmpcolor);
    QVERIFY2(m_player->getColor() == tmpcolor, "Colors are not the same! Failure");

    QString tmpname("nametest");
    m_player->setName(tmpname);
    QVERIFY2(m_player->name() == tmpname, "Names are different! Failure");

    QVERIFY(m_player->isFullyDefined());

    auto v= Helper::randomString();
    m_player->setUserVersion(v);
    QCOMPARE(m_player->userVersion(), v);

    Player p2;
    p2.copyPlayer(m_player);

    m_player->getVariableDictionnary();
    m_player->removeChild(nullptr);
    m_player->addCharacter(nullptr);
    auto c= new Character(Helper::randomString(), Helper::randomColor(), true);
    m_player->addCharacter(c);
    m_player->addCharacter(c);
    QCOMPARE(c->getParentId(), m_player->uuid());
    QCOMPARE(c->getParentPlayer(), m_player);
    QCOMPARE(m_player->indexOf(c), 0);
    QCOMPARE(m_player->characterById(c->uuid()), c);
    QCOMPARE(m_player->getCharacterByIndex(0), c);
    QCOMPARE(m_player->getCharacterByIndex(80), nullptr);
    auto const& children= m_player->children();
    QCOMPARE(children.size(), 1);
    // QCOMPARE(children[0], c);

    m_player->removeChild(c);

    Player p3(Helper::randomString(), Helper::randomString(), Helper::randomColor(), false);

    auto uuid= Helper::randomString();
    auto nameC= Helper::randomString();
    auto colorC= Helper::randomColor();
    auto dataC= Helper::imageData();

    m_player->addCharacter(uuid, nameC, colorC, dataC, {}, true);
    m_player->addCharacter(uuid, nameC, colorC, dataC, {}, true);
}
void DataplayerTest::testChildrenAddAndRemove()
{
    QVERIFY2(m_player->isLeaf() == false, "It has children! Failure");
    QList<Character*> list;
    m_player->clearCharacterList();
    for(int i= 0; i < COUNT_TURN; i++)
    {
        Character* tmp= new Character("Unknown", Qt::blue, "/fake/path/");
        list.append(tmp);
        m_player->addCharacter(tmp);
        QVERIFY2(m_player->characterCount() == list.size(), "The children count is not as expected! Failure");
    }

    m_player->removeChild(list.at(0));
    QVERIFY2(m_player->characterCount() == (list.size() - 1), "The children count is not as expected! Failure");
    m_player->clearCharacterList();
    QVERIFY2(m_player->characterCount() == 0, "The children count is not as expected! 0 Failure");
}

QTEST_MAIN(DataplayerTest);

#include "tst_dataplayertest.moc"
