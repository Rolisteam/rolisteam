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

#include "dicealias.h"
#include "diceparser.h"
#include "die.h"

// node
#include "node/bind.h"
#include "node/countexecutenode.h"
#include "node/explodedicenode.h"
#include "node/filternode.h"
#include "node/groupnode.h"
#include "node/ifnode.h"
#include "node/jumpbackwardnode.h"
#include "node/keepdiceexecnode.h"
#include "node/numbernode.h"
#include "node/occurencecountnode.h"
#include "node/rerolldicenode.h"
#include "node/sortresult.h"
#include "node/stringnode.h"
#include "node/testnode.h"
#include "result/stringresult.h"

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
    void scopeDF_data();

    void severalInstruction();
    void testAlias();
    void cleanupTestCase();

    void keepTest();
    void keepTest_data();

    void sortTest();
    void sortTest_data();

    void countTest();
    void countTest_data();

    void rerollTest();
    void rerollTest_data();

    void explodeTest();
    void explodeTest_data();

    void rerollUntilTest();
    void rerollUntilTest_data();

    void rerollAddTest();
    void rerollAddTest_data();

    void mergeTest();
    void mergeTest_data();

    void ifTest();
    void ifTest_data();

    void paintTest();
    void paintTest_data();

    void filterTest();
    void filterTest_data();

    void splitTest();
    void splitTest_data();

    void uniqueTest();
    void uniqueTest_data();

    void groupTest();
    void groupTest_data();

    void bindTest();
    void bindTest_data();

    void occurenceTest();
    void occurenceTest_data();

private:
    std::unique_ptr<Die> m_die;
    std::unique_ptr<DiceParser> m_diceParser;
};

TestDice::TestDice() {}

void TestDice::initTestCase()
{
    m_die.reset(new Die());
    m_diceParser.reset(new DiceParser());
}

void TestDice::getAndSetTest()
{
    for(unsigned int i= 0; i < 2000; i++)
    {
        m_die->setMaxValue(i);
        QVERIFY(m_die->getMaxValue() == i);
    }

    m_die->setSelected(true);
    QVERIFY(m_die->isSelected() == true);

    m_die->setSelected(false);
    QVERIFY(m_die->isSelected() == false);
}

void TestDice::diceRollD10Test()
{
    m_die->setMaxValue(10);
    for(int i= 0; i < 2000; i++)
    {
        m_die->roll(false);
        QVERIFY(m_die->getValue() > 0);
        QVERIFY(m_die->getValue() < 11);
    }
}
void TestDice::diceRollD20Test()
{
    m_die->setMaxValue(20);
    for(int i= 0; i < 2000; i++)
    {
        m_die->roll(false);
        QVERIFY(m_die->getValue() > 0);
        QVERIFY(m_die->getValue() < 21);
    }
}
void TestDice::commandEndlessLoop()
{
    bool a= m_diceParser->parseLine("1D10e[>0]");
    QVERIFY(!a);
}

void TestDice::commandsTest()
{
    QFETCH(QString, cmd);

    bool a= m_diceParser->parseLine(cmd);
    QVERIFY2(a, "parsing");

    m_diceParser->start();
    QVERIFY2(m_diceParser->humanReadableError().isEmpty(), "no error");

    QVERIFY2(m_diceParser->humanReadableWarning().isEmpty(), "no warning");
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
    QTest::addRow("cmd10") << "1+(4*3)D10";
    QTest::addRow("cmd11") << "(4D6)D10";
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
    QTest::addRow("cmd74") << "2d10k1+2d10k1+2d10k1";
}

void TestDice::wrongCommandsTest()
{
    //            << "pajaejlbnmàw";
    QFETCH(QString, cmd);
    QFETCH(bool, valid);
    bool a= m_diceParser->parseLine(cmd);
    if(a)
    {
        m_diceParser->start();
        auto map= m_diceParser->getErrorMap();
        a= map.isEmpty();
    }
    QCOMPARE(a, valid);
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
    QTest::newRow("test9") << "((1d8e8+2);(1d6e6+2))" << true;
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

    for(QString cmd : commands)
    {
        bool test= m_diceParser->parseLine(cmd);
        m_diceParser->start();

        QVERIFY2(m_diceParser->getErrorMap().isEmpty() == false || !test, cmd.toStdString().c_str());
    }
}
void TestDice::scopeDF()
{
    QFETCH(QString, cmd);
    QFETCH(int, min);
    QFETCH(int, max);
    QFETCH(bool, valid);

    bool test= m_diceParser->parseLine(cmd);
    QVERIFY2(test == valid, cmd.toStdString().c_str());
    m_diceParser->start();
    auto results= m_diceParser->getLastIntegerResults();

    for(auto result : results)
        QVERIFY(result >= min && result <= max);
}

void TestDice::scopeDF_data()
{
    QTest::addColumn<QString>("cmd");
    QTest::addColumn<int>("min");
    QTest::addColumn<int>("max");
    QTest::addColumn<bool>("valid");

    QTest::newRow("test1") << "1D[-1-1]" << -1 << 1 << true;
    QTest::newRow("test2") << "1D[-10--5]" << -10 << -5 << true;
    QTest::newRow("test3") << "1D[-100-100]" << -100 << 100 << true;
    QTest::newRow("test4") << "1D[-1-0]" << -1 << 0 << true;
    QTest::newRow("test5") << "1D[10-20]" << 10 << 20 << true;
    QTest::newRow("test6") << "1D[30-100]" << 30 << 100 << true;
    QTest::newRow("test7") << "1D[0-99]" << 0 << 99 << true;
    QTest::newRow("test8") << "5-5*5+5" << -15 << -15 << true;
    QTest::newRow("test9") << "2d20c[<=13]+@c[<=3]" << 0 << 4 << true;
    QTest::newRow("test10") << "6d10c[>=6]-@c1" << -6 << 6 << true;
    QTest::newRow("test11") << "2d6k1+2d8k1+2d10k1" << 3 << 30 << true;
    QTest::newRow("test12") << "1D[-2-50]" << -2 << 50 << true;
}
void TestDice::testAlias()
{
    m_diceParser->insertAlias(new DiceAlias("!", "3d6c"), 0);
    m_diceParser->insertAlias(new DiceAlias("g", "d10k"), 1);
    m_diceParser->insertAlias(new DiceAlias("(.*)C(.*)", QStringLiteral("\\1d10e10c[>=\\2]"), false), 2);

    QStringList cmds;
    cmds << "!2"
         << "${rang}g4"
         << "${rang}g4 # gerald"
         << "5C3"
         << "1d100i:[<101]{\"great!\"}{\"try again\"}";

    QStringList expected;
    expected << "3d6c2"
             << "${rang}d10k4"
             << "${rang}d10k4 # gerald"
             << "5d10e10c[>=3]"
             << "1d100i:[<101]{\"great!\"}{\"try again\"}";

    int i= 0;
    for(auto cmd : cmds)
    {
        auto result= m_diceParser->convertAlias(cmd);
        QVERIFY2(result == expected[i], result.toLatin1());
        ++i;
    }
}
void TestDice::severalInstruction()
{
    QStringList commands;

    commands << "1d10;2d20;$1+$2";

    QList<int> results;
    results << 3;

    int i= 0;
    for(auto cmd : commands)
    {
        auto test= m_diceParser->parseLine(cmd);
        QVERIFY2(test, cmd.toStdString().c_str());
        QVERIFY2(m_diceParser->getStartNodeCount() == results[i], "Wrong number of instruction");
    }
}
void TestDice::mathPriority()
{
    QFETCH(QString, cmd);
    QFETCH(int, expected);

    bool test= m_diceParser->parseLine(cmd);
    QVERIFY(test);
    m_diceParser->start();
    auto resultList= m_diceParser->getLastIntegerResults();
    QCOMPARE(resultList.size(), 1);

    auto value= resultList.first();
    QVERIFY(qFuzzyCompare(value, expected) == 1);
}

void TestDice::mathPriority_data()
{
    QTest::addColumn<QString>("cmd");
    QTest::addColumn<int>("expected");

    QTest::addRow("cmd1") << "10+2" << 12;
    QTest::addRow("cmd2") << "2-10" << -8;
    QTest::addRow("cmd3") << "5+2*3" << 11;
    QTest::addRow("cmd4") << "5-5*5+5" << -15;
    QTest::addRow("cmd5") << "5-5/5+5" << 9;
    QTest::addRow("cmd6") << "10*(3*2)" << 60;
    QTest::addRow("cmd7") << "60/(3*2)" << 10;
    QTest::addRow("cmd8") << "5-(5*5+5)" << -25;
}

void TestDice::dangerousCommandsTest()
{
    QFETCH(QString, cmd);

    for(int i= 0; i < 1000; ++i)
    {
        auto b= m_diceParser->parseLine(cmd);
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
    // QTest::addRow("cmd4") << "10d10g10";
    // QTest::addRow("cmd5") << "10d10g10";
}

void makeResult(DiceResult& result, const QVector<int>& values)
{
    for(int val : values)
    {
        auto die= new Die();
        die->setBase(1);
        die->setMaxValue(10);
        die->insertRollValue(val);
        result.insertResult(die);
    }
}

Validator* makeValidator(int number, BooleanCondition::LogicOperator op)
{
    BooleanCondition* validator= new BooleanCondition();
    NumberNode* node= new NumberNode();
    node->setNumber(number);
    validator->setValueNode(node);
    validator->setOperator(op);
    return validator;
}

void TestDice::keepTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, keep);
    QFETCH(int, score);
    QFETCH(bool, error);

    TestNode node;
    KeepDiceExecNode keepN;
    keepN.setDiceKeepNumber(keep);

    DiceResult result;

    makeResult(result, values);

    node.setResult(&result);
    node.setNextNode(&keepN);

    node.run(nullptr);

    bool isErrorEmpty= !keepN.getExecutionErrorMap().isEmpty();

    QCOMPARE(isErrorEmpty, error);

    if(error)
        return;

    auto resultScore= keepN.getResult()->getResult(Result::SCALAR).toInt();

    QCOMPARE(score, resultScore);
}

void TestDice::keepTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("keep");
    QTest::addColumn<int>("score");
    QTest::addColumn<bool>("error");

    QTest::addRow("cmd1") << QVector<int>({10, 9, 2}) << 1 << 10 << false;
    QTest::addRow("cmd2") << QVector<int>({10, 9, 2}) << 2 << 19 << false;
    QTest::addRow("cmd3") << QVector<int>({10, 9, 2}) << 3 << 21 << false;
    QTest::addRow("cmd4") << QVector<int>({10, 9, 2}) << 4 << 0 << true;
}

void TestDice::sortTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(bool, ascending);
    QFETCH(QVector<int>, scores);

    TestNode node;
    SortResultNode sortN;
    sortN.setSortAscending(ascending);

    DiceResult result;

    makeResult(result, values);

    node.setResult(&result);
    node.setNextNode(&sortN);

    DiceResult expectedScore;
    makeResult(expectedScore, scores);

    node.run(nullptr);

    auto list= dynamic_cast<DiceResult*>(sortN.getResult())->getResultList();

    int i= 0;
    auto expected= expectedScore.getResultList();
    for(auto sortedDie : list)
    {
        QCOMPARE(expected[i]->getValue(), sortedDie->getValue());
        ++i;
    }
}

void TestDice::sortTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<bool>("ascending");
    QTest::addColumn<QVector<int>>("scores");

    QTest::addRow("cmd1") << QVector<int>({10, 9, 2}) << true << QVector<int>({2, 9, 10});
    QTest::addRow("cmd2") << QVector<int>({1, 2, 3}) << false << QVector<int>({3, 2, 1});
    QTest::addRow("cmd3") << QVector<int>({10, 9, 2}) << false << QVector<int>({10, 9, 2});
    QTest::addRow("cmd4") << QVector<int>({2, 9, 10}) << true << QVector<int>({2, 9, 10});
    QTest::addRow("cmd5") << QVector<int>({1, 25, 10}) << false << QVector<int>({25, 10, 1});
    QTest::addRow("cmd6") << QVector<int>({10, 2, 100}) << false << QVector<int>({100, 10, 2});
}

void TestDice::countTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(int, score);

    TestNode node;
    CountExecuteNode countN;

    auto validator= makeValidator(condition, BooleanCondition::GreaterThan);

    countN.setValidator(validator);
    DiceResult result;
    node.setResult(&result);
    node.setNextNode(&countN);

    makeResult(result, values);

    node.run(nullptr);

    QCOMPARE(score, countN.getResult()->getResult(Result::SCALAR).toInt());

    countN.setValidator(nullptr);
}

void TestDice::countTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<int>("score");

    QTest::addRow("cmd1") << QVector<int>({10, 9, 2}) << 3 << 2;
    QTest::addRow("cmd2") << QVector<int>({1, 2, 3}) << 3 << 0;
}

void TestDice::rerollTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(bool, different);

    TestNode node;
    RerollDiceNode reroll(true, false);

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    auto validator= makeValidator(condition, BooleanCondition::GreaterThan);
    reroll.setValidator(validator);
    node.setNextNode(&reroll);

    node.run(nullptr);

    auto list= dynamic_cast<DiceResult*>(reroll.getResult())->getResultList();

    int i= 0;
    auto expected= result.getResultList();
    bool resultDiff= false;
    for(auto rerolled : list)
    {
        if(!resultDiff && rerolled->getValue() != expected[i]->getValue())
            resultDiff= true;
        ++i;
    }

    QCOMPARE(different, resultDiff);
}

void TestDice::rerollTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<bool>("different");

    QTest::addRow("cmd1") << QVector<int>({8, 9, 2}) << 10 << false;
    QTest::addRow("cmd2") << QVector<int>({0, 0, 0}) << -1 << true;
}

void TestDice::explodeTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(bool, different);

    TestNode node;
    ExplodeDiceNode explode;

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    auto validator= makeValidator(condition, BooleanCondition::Equal);
    explode.setValidator(validator);
    node.setNextNode(&explode);

    node.run(nullptr);

    auto list= dynamic_cast<DiceResult*>(explode.getResult())->getResultList();

    int i= 0;
    auto expected= result.getResultList();
    bool resultDiff= false;
    for(auto rerolled : list)
    {
        if(!resultDiff && rerolled->getValue() != expected[i]->getValue())
            resultDiff= true;
        ++i;
    }

    QCOMPARE(different, resultDiff);
}

void TestDice::explodeTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<bool>("different");

    QTest::addRow("cmd1") << QVector<int>({8, 9, 2}) << 10 << false;
    QTest::addRow("cmd2") << QVector<int>({0, 0, 0}) << 0 << true;
}

void TestDice::rerollUntilTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(bool, different);

    TestNode node;
    RerollDiceNode reroll(false, false);

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    auto validator= makeValidator(condition, BooleanCondition::Equal);
    reroll.setValidator(validator);
    node.setNextNode(&reroll);

    node.run(nullptr);

    auto list= dynamic_cast<DiceResult*>(reroll.getResult())->getResultList();

    int i= 0;
    auto expected= result.getResultList();
    bool resultDiff= false;
    for(auto rerolled : list)
    {
        if(!resultDiff && rerolled->getValue() != expected[i]->getValue())
            resultDiff= true;
        ++i;
    }

    QCOMPARE(different, resultDiff);
}
void TestDice::rerollUntilTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<bool>("different");

    QTest::addRow("cmd1") << QVector<int>({8, 9, 2}) << 10 << false;
    QTest::addRow("cmd2") << QVector<int>({0, 0, 0}) << 0 << true;
}

void TestDice::rerollAddTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(bool, different);

    TestNode node;
    RerollDiceNode reroll(true, true);

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    auto validator= makeValidator(condition, BooleanCondition::Equal);
    reroll.setValidator(validator);
    node.setNextNode(&reroll);

    node.run(nullptr);

    auto list= dynamic_cast<DiceResult*>(reroll.getResult())->getResultList();

    int i= 0;
    auto expected= result.getResultList();
    bool resultDiff= false;
    for(auto rerolled : list)
    {
        if(!resultDiff && rerolled->getValue() != expected[i]->getValue())
            resultDiff= true;
        ++i;
    }

    QCOMPARE(different, resultDiff);
}
void TestDice::rerollAddTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<bool>("different");

    QTest::addRow("cmd1") << QVector<int>({8, 9, 2}) << 10 << false;
    QTest::addRow("cmd2") << QVector<int>({0, 0, 0}) << 0 << true;
}

void TestDice::mergeTest() {}
void TestDice::mergeTest_data() {}

void TestDice::ifTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(int, valCondition);
    QFETCH(QString, expectedResult);

    IfNode::ConditionType conditionType= static_cast<IfNode::ConditionType>(condition);

    TestNode node;
    IfNode ifNode;
    ifNode.setConditionType(conditionType);

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    StringNode trueNode;
    trueNode.setString(QStringLiteral("True"));
    StringNode falseNode;
    falseNode.setString(QStringLiteral("False"));

    ifNode.setInstructionTrue(&trueNode);
    ifNode.setInstructionFalse(&falseNode);

    auto validator= makeValidator(valCondition, BooleanCondition::Equal);
    ifNode.setValidator(validator);
    node.setNextNode(&ifNode);

    node.run(nullptr);

    auto text= dynamic_cast<StringResult*>(ifNode.getNextNode()->getResult())->getText();

    QCOMPARE(expectedResult, text);

    ifNode.setNextNode(nullptr);
}
void TestDice::ifTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<int>("valCondition");
    QTest::addColumn<QString>("expectedResult");

    int onEach= 0;
    int oneOfThem= 1;
    int allOfThem= 2;
    int onScalar= 3;

    QTest::addRow("cmd1") << QVector<int>({8, 9, 2}) << onEach << 0 << "False";
    QTest::addRow("cmd2") << QVector<int>({2, 2, 2}) << onEach << 2 << "True";

    QTest::addRow("cmd3") << QVector<int>({0, 0, 0}) << oneOfThem << 10 << "False";
    QTest::addRow("cmd4") << QVector<int>({10, 9, 5}) << oneOfThem << 10 << "True";
    QTest::addRow("cmd5") << QVector<int>({9, 9, 9}) << oneOfThem << 9 << "True";

    QTest::addRow("cmd6") << QVector<int>({8, 9, 2}) << allOfThem << 1 << "False";
    QTest::addRow("cmd7") << QVector<int>({8, 9, 2}) << allOfThem << 9 << "False";
    QTest::addRow("cmd8") << QVector<int>({8, 8, 8}) << allOfThem << 8 << "True";

    QTest::addRow("cmd9") << QVector<int>({25, 8, 14}) << onScalar << 1 << "False";
    QTest::addRow("cmd10") << QVector<int>({25, 8, 14}) << onScalar << 47 << "True";
}

void TestDice::paintTest() {}
void TestDice::paintTest_data() {}

void TestDice::filterTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(bool, different);

    TestNode node;
    FilterNode filter;

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    auto validator= makeValidator(condition, BooleanCondition::Different);
    filter.setValidator(validator);
    node.setNextNode(&filter);

    node.run(nullptr);

    auto list= dynamic_cast<DiceResult*>(filter.getResult())->getResultList();

    auto expected= result.getResultList();
    bool resultDiff= (list.size() != expected.size());

    QCOMPARE(different, resultDiff);
}

void TestDice::filterTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<bool>("different");

    QTest::addRow("cmd1") << QVector<int>({8, 4, 2}) << 4 << true;
    QTest::addRow("cmd2") << QVector<int>({0, 0, 0}) << 1 << false;
}

void TestDice::splitTest() {}
void TestDice::splitTest_data() {}

void TestDice::uniqueTest() {}
void TestDice::uniqueTest_data() {}

void TestDice::groupTest() {}
void TestDice::groupTest_data() {}

void TestDice::bindTest() {}
void TestDice::bindTest_data() {}

void TestDice::occurenceTest()
{
    QFETCH(QVector<int>, values);
    QFETCH(int, condition);
    QFETCH(QString, expected);

    TestNode node;
    OccurenceCountNode count;

    DiceResult result;
    makeResult(result, values);
    node.setResult(&result);

    auto validator= makeValidator(condition, BooleanCondition::GreaterThan);
    count.setValidator(validator);
    node.setNextNode(&count);

    node.run(nullptr);

    auto text= dynamic_cast<StringResult*>(count.getResult())->getText();

    QVERIFY(text.startsWith(expected));
}
void TestDice::occurenceTest_data()
{
    QTest::addColumn<QVector<int>>("values");
    QTest::addColumn<int>("condition");
    QTest::addColumn<QString>("expected");

    QTest::addRow("cmd1") << QVector<int>({8, 8, 2}) << 7 << "2x8";
    QTest::addRow("cmd2") << QVector<int>({0, 0, 0}) << 1 << "No matching result";
}

void TestDice::cleanupTestCase() {}

QTEST_MAIN(TestDice);

#include "tst_dice.moc"
