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
#include <QObject>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <chrono>
#include <data/resourcesnode.h>

#define COUNT_TURN 2000
/*class TestChapter : public QObject
{


public:
    TestChapter(QObject* parent= nullptr);

private Q_SLOTS:

    void initTestCase();


    void cleanupTestCase();

    void testAddChapterHasChildren();


    void testSetGetName();


    void testAddCleverURIToChapter();
    void testInsertAtAndIndexOf();
    void testClear();

private:
    //Chapter* m_chapter;
};

TestChapter::TestChapter(QObject* parent) : QObject(parent) {}

void TestChapter::initTestCase()
{
    m_chapter= new Chapter();
}

void TestChapter::cleanupTestCase()
{
    delete m_chapter;
}
void TestChapter::testAddChapterHasChildren()
{
    for(int i= 0; i < COUNT_TURN; i++)
    {
        QString temp("Chapiter %1");
        temp= temp.arg(i);
        auto child= new Chapter();
        child->setName(temp);
        m_chapter->addResource(child);
    }
    QVERIFY2(m_chapter->hasChildren(), "No Children");
    QVERIFY2(m_chapter->childrenCount() == COUNT_TURN, "Not the expect children count");
}
void TestChapter::testSetGetName()
{
    for(int i= 0; i < COUNT_TURN; i++)
    {
        QString temp("Chapiter %1");
        temp= temp.arg(i);
        m_chapter->setName(temp);
        QVERIFY2(m_chapter->name() == temp, "Name is not as expected!");
    }
}
void TestChapter::testAddCleverURIToChapter()
{
    for(int i= 0; i < COUNT_TURN; i++)
    {
        CleverURI* temp= new CleverURI(Core::ContentType::CHARACTERSHEET);
        temp->setContentType(Core::ContentType::CHARACTERSHEET);
        QString str("/foo");
        temp->setPath(str);
        m_chapter->addResource(temp);
        QVERIFY2(m_chapter->removeChild(temp), "Removal of CleverURI fails, the item was not in the list");
    }
    for(int i= 0; i < COUNT_TURN; i++)
    {
        CleverURI* tempURI= new CleverURI(Core::ContentType::CHARACTERSHEET);
        tempURI->setContentType(Core::ContentType::CHARACTERSHEET);
        QString str("/foo");
        tempURI->setPath(str);
        m_chapter->addResource(tempURI);
        QVERIFY2(m_chapter->removeChild(tempURI), "Removal of subchapter fails, the item was not in the list");
    }
}

void TestChapter::testInsertAtAndIndexOf()
{
    auto seed= std::chrono::high_resolution_clock::now().time_since_epoch().count();
    auto rng= std::mt19937(quintptr(this) + seed);
    for(int i= 0; i < COUNT_TURN; i++)
    {
        CleverURI* temp= new CleverURI(Core::ContentType::CHARACTERSHEET);
        std::uniform_int_distribution<int> dist(0, m_chapter->childrenCount());
        int j= dist(rng);
        m_chapter->insertChildAt(j, temp);
        QCOMPARE(m_chapter->indexOf(temp), j);
    }
}

void TestChapter::testClear()
{
    m_chapter->clear();
    QVERIFY2(!m_chapter->hasChildren(), "all Children have not been removed");
}*/

// QTEST_MAIN(TestChapter);

//#include "testChapter.moc"
