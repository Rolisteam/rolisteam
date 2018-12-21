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
#include <QtCore/QCoreApplication>

#include "charactersheet.h"
#include "field.h"

#include "formula/formulamanager.h"

class TestCharacterSheet : public QObject
{
    Q_OBJECT

public:
    TestCharacterSheet();

private slots:
    void initTestCase();
    void getAndSetTest();
    void cleanupTestCase();
    void changeValue();
    void changeReadOnly();
    void changeLabel();
    void insertField();
    void commandsTest();
    void commandsTest_data();
    void wrongCommandsTest();

private:
    CharacterSheet* m_sheet = nullptr;
    QList<CharacterSheetItem*> m_list;
    Formula::FormulaManager* m_formulaMan = nullptr;
    QHash<QString,QString>* m_variable = nullptr;
};

TestCharacterSheet::TestCharacterSheet()
{

}

void TestCharacterSheet::initTestCase()
{
    m_sheet = new CharacterSheet();
    Field* field = new Field();
    field->setLabel("name");
    field->setId("id_1");
    m_list << field;

    field = new Field();
    field->setLabel("last name");
    field->setId("id_2");
    m_list << field;


    field = new Field();
    field->setLabel("age");
    field->setId("id_3");
    m_list << field;

    field = new Field();
    field->setLabel("Species");
    field->setId("id_4");
    m_list << field;


    field = new Field();
    field->setLabel("Intelligence");
    field->setId("id_5");
    m_list << field;


    //Formula
    m_formulaMan = new Formula::FormulaManager();
    m_variable = new QHash<QString,QString>();

    m_variable->insert("Name","John Doe");
    m_variable->insert("weight","85kg");
    m_variable->insert("age","18");
    m_variable->insert("size","1.69");
    m_variable->insert("strenght","4");
    m_variable->insert("intelligence","5");
    m_variable->insert("speed","59");
    m_variable->insert("ecole","Akodo");
    m_variable->insert("arme","Katana");
    m_variable->insert("XP","0");
    m_variable->insert("max health","82");
    m_variable->insert("mana","10000");
    m_variable->insert("level","-1");
    m_variable->insert("agility","-2.5");
    m_variable->insert("manipulation","10");
    m_variable->insert("mental health","90%");

    m_formulaMan->setConstantHash(m_variable);

}
void TestCharacterSheet::insertField()
{
    for(auto field : m_list)
    {
        m_sheet->insertCharacterItem(field);
    }
    QVERIFY2(m_sheet->getFieldCount() == m_list.size(),"The number of field is not right.");
}
void TestCharacterSheet::getAndSetTest()
{

}
void TestCharacterSheet::changeValue()
{

}
void TestCharacterSheet::changeReadOnly()
{

}
void TestCharacterSheet::changeLabel()
{

}
void TestCharacterSheet::commandsTest()
{
    QFETCH(QString, cmd);
    QFETCH(qreal, result);

    auto a = m_formulaMan->getValue(cmd).toDouble();
    QVERIFY(qFuzzyCompare(a,result));
}

void TestCharacterSheet::commandsTest_data()
{
    QTest::addColumn<QString>("cmd");
    QTest::addColumn<qreal>("result");
    QTest::addColumn<bool>("expected");

    QTest::addRow("cmd1") << "=4*4" << 16. << true;
    QTest::addRow("cmd2") << "=10+8" << 18. << true;
    QTest::addRow("cmd3") << "=10-7" << 3. << true;
    QTest::addRow("cmd4") << "=50/5" << 10. << true;
    QTest::addRow("cmd5") << "=25-52" << -27. << true;
    QTest::addRow("cmd6") << "=${speed}+10" << 69. << true;
    QTest::addRow("cmd7") << "=${size}*10" << 16.9 << true;
    QTest::addRow("cmd8") << "=${level}-100" << -101. << true;
    QTest::addRow("cmd9") << "=2+4/4" << 3. << true;
    QTest::addRow("cmd10") << "=${manipulation}/100" << 0.1 << true;
    QTest::addRow("cmd11") << "=${size}*${speed}" << 99.71 << true;
    QTest::addRow("cmd12") << "=${agility}-100" << -102.5 << true;
    QTest::addRow("cmd13") << "=${agility}*100" << -250. << true;
    QTest::addRow("cmd14") << "=min(${intelligence},${strenght})" << 4. << true;
    QTest::addRow("cmd15") << "=max(${intelligence},${strenght})" << 5. << true;
    QTest::addRow("cmd16") << "=abs(${size})" << 1.69 << true;
    QTest::addRow("cmd17") << "=18*1.69-10/-1" << 40.42 << true;
    QTest::addRow("cmd18") << "=avg(${age},20})" << 19. << true;
    QTest::addRow("cmd19") << "=(10+2)*3" << 36. << true;
    QTest::addRow("cmd20") << "=(10-4)*3" << 18. << true;
    QTest::addRow("cmd21") << "=((10-4)+6)*2" << 24. << true;
    QTest::addRow("cmd22") << "=(10+2)*(10-8)" << 24. << true;

}
void TestCharacterSheet::wrongCommandsTest()
{
    QStringList commands;

    for(QString cmd: commands)
    {
        QVariant a = m_formulaMan->getValue(cmd);

        QCOMPARE(a.toString(),cmd.toStdString().c_str());
    }
}
void TestCharacterSheet::cleanupTestCase()
{
    if(m_sheet)
        delete m_sheet;

    if(m_formulaMan)
        delete m_formulaMan;

  /*  if(m_variable)
        delete m_variable;*/
    //qDeleteAll(m_list);
}

QTEST_MAIN(TestCharacterSheet);

#include "tst_charactersheet.moc"
