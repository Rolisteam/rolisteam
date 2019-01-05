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

#include "die.h"
#include "dicealias.h"
#include "diceparser.h"

class TestDice : public QObject
{
    Q_OBJECT

public:
    TestDice();

private slots:
    void initTestCase();
    void getAndSetTest();
    void diceRollD10Test();
    void diceRollD20Test();
    void commandEndlessLoop();

    void mathPriority();
    void mathPriority_data();

    void commandsTest();
    void commandsTest_data();

    void dangerousCommandsTest();
    void dangerousCommandsTest_data();

    void wrongCommandsTest();
    void wrongCommandsTest_data();

    void wrongCommandsExecutionTimeTest();
    void scopeDF();
    void severalInstruction();
    void testAlias();
    void cleanupTestCase();

private:
    std::unique_ptr<Die> m_die;
    std::unique_ptr<DiceParser> m_diceParser;
};

TestDice::TestDice()
{
}

void TestDice::initTestCase()
{
    m_die.reset(new Die());
    m_diceParser.reset(new DiceParser());
}

void TestDice::getAndSetTest()
{
    for(unsigned int i = 0; i<2000;i++)
    {
        m_die->setMaxValue(i);
        QVERIFY(m_die->getMaxValue()==i);
    }

    m_die->setSelected(true);
    QVERIFY(m_die->isSelected()==true);

    m_die->setSelected(false);
    QVERIFY(m_die->isSelected()==false);

}

void TestDice::diceRollD10Test()
{
    m_die->setMaxValue(10);
    for(int i = 0; i< 2000; i++)
    {
        m_die->roll(false);
        QVERIFY(m_die->getValue()>0);
        QVERIFY(m_die->getValue()<11);
    }

}
void TestDice::diceRollD20Test()
{
    m_die->setMaxValue(20);
    for(int i = 0; i< 2000; i++)
    {
        m_die->roll(false);
        QVERIFY(m_die->getValue()>0);
        QVERIFY(m_die->getValue()<21);
    }
}
void TestDice::commandEndlessLoop()
{
    bool a = m_diceParser->parseLine("1D10e[>0]");
    QVERIFY(!a);
}

void TestDice::commandsTest()
{
    QFETCH(QString, cmd);

    bool a = m_diceParser->parseLine(cmd);
    QVERIFY2(a,"parsing");

    m_diceParser->start();
    QVERIFY2(m_diceParser->humanReadableError().isEmpty(),"no error");

    QVERIFY2(m_diceParser->humanReadableWarning().isEmpty(),"no warning");
}

void TestDice::commandsTest_data()
{
     QTest::addColumn<QString>("cmd");


    QTest::addRow("cmd1") << "1L[cheminée,chocolat,épée,arc,chute de pierre]";
    QTest::addRow("cmd2") << "10d10c[>=6]-@c[=1]";
    QTest::addRow("cmd3") << "10d10c[>=6]-@c[=1]-@c[=1]";
    QTest::addRow("cmd4") << "10d10c[>6]+@c[=10]";
    QTest::addRow("cmd5") << "1+1D10";
    QTest::addRow("cmd6") << "3d10c[>=5]";
    QTest::addRow("cmd7") << "1+(4*3)D10";
    QTest::addRow("cmd8") << "2+4/4";
    QTest::addRow("cmd9") << "2D10*2D20*8";
    QTest::addRow("cmd10") <<"1+(4*3)D10";
    QTest::addRow("cmd11") <<"(4D6)D10";
    QTest::addRow("cmd12") << "1D100a[>=95]a[>=96]a[>=97]a[>=98]a[>=99]e[>=100]";
    QTest::addRow("cmd13") << "3D100";
    QTest::addRow("cmd14") << "4k3";
    QTest::addRow("cmd15") << "10D10e[>=6]sc[>=6]";
    QTest::addRow("cmd16") << "10D10e10s";
    QTest::addRow("cmd17") << "10D10s";
    QTest::addRow("cmd18") << "15D10e10c[8-10]";
    QTest::addRow("cmd19") << "10d10e10";
    QTest::addRow("cmd30") << "(4+4)^4";
    QTest::addRow("cmd31") << "(1d20+20)*7/10";
    QTest::addRow("cmd32") << "20*7/10";
    QTest::addRow("cmd33") << "1D8+2D6+7";
    QTest::addRow("cmd34") << "D25";
    QTest::addRow("cmd35") << "1L[tete[10],ventre[50],jambe[40]]";
    QTest::addRow("cmd36") << "2d6c[%2=0]";
    QTest::addRow("cmd37") << "D25+D10";
    QTest::addRow("cmd38") << "D25;D10";
    QTest::addRow("cmd39") << "8+8+8";
    QTest::addRow("cmd40") << "1D20-88";
    QTest::addRow("cmd41") << "100*1D20*2D6";
    QTest::addRow("cmd42") << "2D6 # two 6sided dice";
    QTest::addRow("cmd43") << "100/28*3";
    QTest::addRow("cmd44") << "100/8";
    QTest::addRow("cmd45") << "100*3*8";
    QTest::addRow("cmd46") << "help";
    QTest::addRow("cmd47") << "la";
    QTest::addRow("cmd48") << "10D10c[<2|>7]";
    QTest::addRow("cmd49") << "10D6c[=2|=4|=6]";
    QTest::addRow("cmd50") << "10D10e[=1|=10]k4";
    QTest::addRow("cmd51") << "1L[tete,bras droit,bras gauche,jambe droite,jambe gauche,ventre[6-7],buste[8-10]]";
    QTest::addRow("cmd52") << "10+10s";
    QTest::addRow("cmd53") << "1d6e6;1d4e4mk1";
    QTest::addRow("cmd54") << "1d6e6;1d4e4mk1";
    QTest::addRow("cmd55") << "400D20/400000";
    QTest::addRow("cmd56") << "1d100e[>=95]i[<5]{-1d100e95}";
    QTest::addRow("cmd57") << "100*3*8";
    QTest::addRow("cmd58") << "1d100i[<70]{1d10i[=10]{1d100i[<70]{1d10e10}}}";
    QTest::addRow("cmd59") << "10d6c[<2|>5]";
    QTest::addRow("cmd60") << "5-5*5+5";
    QTest::addRow("cmd61") << "((3+4)*2)d6";
    QTest::addRow("cmd62") << "4d6i[=6]{+1d6}";
    QTest::addRow("cmd63") << "10d[-8--1]";
    QTest::addRow("cmd64") << "4d6e6i[=4]{-4}+2";
    QTest::addRow("cmd65") << "4d6e6f[!=4]+2";
    QTest::addRow("cmd66") << "5d10g10";
    QTest::addRow("cmd67") << "4d6p[4:blue]c[>=4];1d6p[1:#FFFFFF]c6-@c1;1d6p[1:#FF0000]c[>=4]+@c6-@c1";
    QTest::addRow("cmd68") << "10d[0-9]";
    QTest::addRow("cmd69") << "1d8e8;1d6e6mk1+2";
    QTest::addRow("cmd70") << "3d100g50";
    QTest::addRow("cmd71") << "3d100g33";
    QTest::addRow("cmd72") << "3d100g5";
    QTest::addRow("cmd73") << "3d100g40";
}


void TestDice::wrongCommandsTest()
{
//            << "pajaejlbnmàw";
    QFETCH(QString, cmd);
    QFETCH(bool, valid);
    bool a = m_diceParser->parseLine(cmd);
    if(a)
    {
        m_diceParser->start();
        auto map = m_diceParser->getErrorMap();
        a = !map.isEmpty();
    }
    QCOMPARE(a,valid);
}


void TestDice::wrongCommandsTest_data()
{
    QTest::addColumn<QString>("cmd");
    QTest::addColumn<bool>("valid");

    QTest::newRow("test1") << "1L[cheminée,chocolat,épée,arc,chute de pierre" << false;
    QTest::newRow("test2") << "10d10c" << false;
    QTest::newRow("test3") << "10d10a" << false;
    QTest::newRow("test4") << "10d0a[>7]" << false;
    QTest::newRow("test5") << "aiteanetauearuteurn" << false;
    QTest::newRow("test6") << "meregue" << false;
    QTest::newRow("test7") << "p i follow rivers" << false;
    QTest::newRow("test8") << "manga violet evergarden" << false;
    QTest::newRow("test9") << "((1d8e8+2);(1d6e6+2))" << false;
}

void TestDice::wrongCommandsExecutionTimeTest()
{
    QStringList commands;

    //<< "8D20+10*@c[=20]"
    commands << "1/0"
             << ""
             << "0d10"
             << "10d10k11"
             << "!!!!";

    for(QString cmd: commands)
    {
        bool test = m_diceParser->parseLine(cmd);
        m_diceParser->start();

        QVERIFY2(m_diceParser->getErrorMap().isEmpty() == false || !test,cmd.toStdString().c_str());
    }
}
void TestDice::scopeDF()
{
    QStringList commands;

    commands << "1D[-1-1]"
             << "1D[-10--5]"
             << "1D[-100-100]"
             << "1D[-1-0]"
             << "1D[10-20]"
             << "1D[30-100]"
             << "1D[0-99]"
             << "5-5*5+5"
             << "2d20c[<=13]+@c[<=3]"
             << "6d10c[>=6]-@c1"
             << "1D[-2-50]";

    QList< QPair<int,int> > pairMinMax;
    pairMinMax  << QPair<int,int>(-1,1)
                << QPair<int,int>(-10,-5)
                << QPair<int,int>(-100,100)
                << QPair<int,int>(-1,0)
                << QPair<int,int>(10,20)
                << QPair<int,int>(30,100)
                << QPair<int,int>(0,99)
                << QPair<int,int>(-15,-15)
                << QPair<int,int>(0,4)
                << QPair<int,int>(-6,6)
                << QPair<int,int>(-2,50);


    int index = 0;
    for(QString cmd: commands)
    {
        bool test = m_diceParser->parseLine(cmd);
        QVERIFY2(test,cmd.toStdString().c_str());
        m_diceParser->start();
        QPair<int,int> expect = pairMinMax.at(index);
        int min = expect.first;
        int max = expect.second;
        auto  results = m_diceParser->getLastIntegerResults();

        for(auto result: results)
            QVERIFY2(result >= min && result <= max,cmd.toStdString().c_str());
        index++;
    }
}
void TestDice::testAlias()
{
    m_diceParser->insertAlias(new DiceAlias("!","3d6c"),0);
    m_diceParser->insertAlias(new DiceAlias("g","d10k"),1);
    m_diceParser->insertAlias(new DiceAlias("(.*)C(.*)",QStringLiteral("\\1d10e10c[>=\\2]"),false),2);


    QStringList cmds;
    cmds << "!2"
         << "${rang}g4"
         << "${rang}g4 # gerald"
         << "5C3"
         << "1d100i:[<101]{\"great!\"}{\"try again\"}";


    QStringList expected;
    expected << "3d6c2" << "${rang}d10k4" << "${rang}d10k4 # gerald" << "5d10e10c[>=3]" << "1d100i:[<101]{\"great!\"}{\"try again\"}";

    int i=0;
    for(auto cmd : cmds)
    {
        auto result = m_diceParser->convertAlias(cmd);
        QVERIFY2(result == expected[i],result.toLatin1());
        ++i;
    }


}
void TestDice::severalInstruction()
{

    QStringList commands;

    commands << "1d10;2d20;$1+$2";
    
    QList<int> results;
    results << 3;

    int i = 0;
    for(auto cmd : commands)
    {
        auto test = m_diceParser->parseLine(cmd);
        QVERIFY2(test,cmd.toStdString().c_str());
        QVERIFY2(m_diceParser->getStartNodeCount() == results[i], "Wrong number of instruction");
    }

}
void TestDice::mathPriority()
{
    QFETCH(QString, cmd);
    QFETCH(int, expected);

    bool test = m_diceParser->parseLine(cmd);
    QVERIFY(test);
    m_diceParser->start();
    auto resultList = m_diceParser->getLastIntegerResults();
    QCOMPARE(resultList.size(),1);

    auto value = resultList.first();
    QVERIFY(qFuzzyCompare(value, expected)==1);
}

void TestDice::mathPriority_data()
{
    QTest::addColumn<QString>("cmd");
    QTest::addColumn<int>("expected");

    QTest::addRow("cmd1") << "10+2"<<12 ;
    QTest::addRow("cmd2")  << "2-10"<<-8 ;
    QTest::addRow("cmd3")  << "5+2*3"<< 11 ;
    QTest::addRow("cmd4")  << "5-5*5+5"<< -15 ;
    QTest::addRow("cmd5")  << "5-5/5+5"<< 9 ;
    QTest::addRow("cmd6")  << "10*(3*2)"<< 60 ;
    QTest::addRow("cmd7")  << "60/(3*2)"<< 10 ;
    QTest::addRow("cmd8")  << "5-(5*5+5)" << -25;
}

void TestDice::dangerousCommandsTest()
{
    QFETCH(QString, cmd);

    for(int i = 0; i < 1000 ; ++i)
    {
        auto b = m_diceParser->parseLine(cmd);
        QVERIFY(b);
        m_diceParser->start();
    }
}
void TestDice::dangerousCommandsTest_data()
{
    QTest::addColumn<QString>("cmd");


    QTest::addRow("cmd1") << "10d6g10";
    QTest::addRow("cmd2") << "10d2g10";
    QTest::addRow("cmd3") << "10d10g10";
    //QTest::addRow("cmd4") << "10d10g10";
    //QTest::addRow("cmd5") << "10d10g10";

}

void TestDice::cleanupTestCase()
{
}

QTEST_MAIN(TestDice);

#include "tst_dice.moc"
