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
#include <data/chapter.h>
#define COUNT_TURN 2000
class TestChapter : public QObject
{
    Q_OBJECT

public:
    TestChapter(QObject *parent=NULL);

private Q_SLOTS:
    /**
      * @brief initTestCase creates instance of classes to be tested
      */
    void initTestCase();

    /**
      * @brief clean data
      */
    void cleanupTestCase();

    /**
      * @brief add chapters and remove them.
      */
    void testAddChapter();

    /**
      * @brief initTestCase creates instance of classes to be tested
      */
    void testAddChapterHasChildren();

    /**
      * @brief test the set and get method for Chapter
      */
    void testSetGetName();

    /**
      * @brief Add cleveruri to chapter and subchapter and remove them.
      */
    void testAddCleverURIToChapter();


private:
    Chapter* m_chapter;
};


TestChapter::TestChapter(QObject *parent) :
    QObject(parent)
{
}

void TestChapter::initTestCase()
{
    m_chapter = new Chapter();
}

void TestChapter::cleanupTestCase()
{
    delete m_chapter;
}
void TestChapter::testAddChapterHasChildren()
{
    for(int i = 0; i< COUNT_TURN; i++)
    {
        QString temp("Chapiter %1");
        temp=temp.arg(i);
        RessourcesNode* item = m_chapter->addChapter(temp);
    }
    QVERIFY2(m_chapter->hasChildren(),"No Children");

}
void TestChapter::testSetGetName()
{
    for(int i = 0; i< COUNT_TURN; i++)
    {
        QString temp("Chapiter %1");
        temp=temp.arg(i);
        m_chapter->setShortName(temp);
        QVERIFY2(m_chapter->getShortName()==temp,"Name is not as expected!");
    }

}
void TestChapter::testAddCleverURIToChapter()
{
    for(int i = 0; i< COUNT_TURN; i++)
    {
        CleverURI* temp = new CleverURI("/foo",CleverURI::CHARACTERSHEET);
        m_chapter->addResource(temp);
        QVERIFY2(m_chapter->removeRessourcesNode(temp),"Removal of CleverURI fails, the item was not in the list");
    }
    for(int i = 0; i< COUNT_TURN; i++)
    {
        CleverURI* tempURI = new CleverURI("/foo",CleverURI::CHARACTERSHEET);
        QString temp("Chapiter %1");
        temp=temp.arg(i);
        Chapter* tmpchapter = m_chapter->addChapter(temp);

        tmpchapter->addResource(tempURI);
        QVERIFY2(m_chapter->removeRessourcesNode(tempURI),"Removal of subchapter fails, the item was not in the list");
    }
}

void TestChapter::testAddChapter()
{
    for(int i = 0; i< COUNT_TURN; i++)
    {
        QString temp("Chapiter %1");
        temp=temp.arg(i);
        RessourcesNode* item = m_chapter->addChapter(temp);
        QVERIFY2(m_chapter->removeRessourcesNode(item),"Removal fails, the item was not in the list");
    }
    QVERIFY2(!m_chapter->hasChildren(),"all Children have not been removed");

}

QTEST_MAIN(TestChapter);

#include "testChapter.moc"
