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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <data/charactersheet.h>

#define COUNT_TURN 2000
class DataCharacterTest : public QObject
{
    Q_OBJECT

public:
    DataCharacterTest();

private Q_SLOTS:
    void testAddRemoveAtSection();
    void testAddRemoveOneSection();
    void initTestCase();
    void cleanupTestCase();

private:
    CharacterSheet* m_charactersheet;
};

DataCharacterTest::DataCharacterTest()
{

}
void DataCharacterTest::initTestCase()
{
     m_charactersheet = new CharacterSheet();
}

void DataCharacterTest::cleanupTestCase()
{
    delete m_charactersheet;
}
void DataCharacterTest::testAddRemoveAtSection()
{
    for(int i = 0; i< COUNT_TURN; i++)
    {
        Section* sectemp= new Section();
        sectemp->setName(QString("Section %1").arg(i));
        QString a("%1");
                a=a.arg(i);
        sectemp->setValue(a);
        m_charactersheet->appendSection(sectemp);
    }

    QVERIFY2(m_charactersheet->getSectionCount()==COUNT_TURN,"The number of section is not as expected");
    for(int i = COUNT_TURN; i>=0; i--)
    {
        m_charactersheet->removeSectionAt(i);
    }
    QVERIFY2(m_charactersheet->getSectionCount()==0,"The number of section is not 0");
}
void DataCharacterTest::testAddRemoveOneSection()
{
    QList<Section*> sectionlist;
    for(int i = 0; i< COUNT_TURN; i++)
    {
        Section* sectemp= new Section();
        sectemp->setName(QString("Section %1").arg(i));
        QString a("%1");
                a=a.arg(i);
        sectemp->setValue(a);

        sectionlist.append(sectemp);
        m_charactersheet->appendSection(sectemp);
    }
    QVERIFY2(m_charactersheet->getSectionCount()==COUNT_TURN,"The number of section is not as expected");
    for(int i = 0; i< COUNT_TURN; i++)
    {
        m_charactersheet->removeSection(sectionlist.at(i));
    }
    QVERIFY2(m_charactersheet->getSectionCount()==0,"The number of section is not 0");
}

QTEST_MAIN(DataCharacterTest);

#include "tst_datacharactertest.moc"
