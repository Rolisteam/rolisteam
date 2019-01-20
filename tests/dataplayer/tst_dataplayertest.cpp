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

#include <QColor>
#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <data/character.h>
#include <data/player.h>

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
        QVERIFY2(m_player->getChildrenCount() == list.size(), "The children count is not as expected! Failure");
    }

    m_player->removeChild(list.at(0));
    QVERIFY2(m_player->getChildrenCount() == (list.size() - 1), "The children count is not as expected! Failure");
    m_player->clearCharacterList();
    QVERIFY2(m_player->getChildrenCount() == 0, "The children count is not as expected! 0 Failure");
}

QTEST_MAIN(DataplayerTest);

#include "tst_dataplayertest.moc"
