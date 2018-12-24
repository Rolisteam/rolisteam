/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
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
#include <QModelIndex>
#include <QModelIndexList>
#include <memory>
#include <QJsonObject>

#include "preferences/dicealiasmodel.h"
#include "diceparser/dicealias.h"
#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"

class TestDiceAliasModel : public QObject
{
    Q_OBJECT
public:
    TestDiceAliasModel();

    void addDefaultAlias();
private slots:
    void init();

    void addTest();
    void addTest_data();

    void moveTest();
    void removeTest();

    void saveModelTest();

private:
    std::unique_ptr<DiceAliasModel> m_model;
};

TestDiceAliasModel::TestDiceAliasModel()
{

}

void TestDiceAliasModel::init()
{
    m_model.reset(new DiceAliasModel());
}

void TestDiceAliasModel::addDefaultAlias()
{
    m_model->addAlias(new DiceAlias("l5r","D10k"));
    m_model->addAlias(new DiceAlias("l5R","D10K"));
    m_model->addAlias(new DiceAlias("DF","D[-1-1]"));
    m_model->addAlias(new DiceAlias("nwod","D10e10c[>7]"));
    m_model->addAlias(new DiceAlias("(.*)wod(.*)","\\1d10e[=10]c[>=\\2]-@c[=1]",false));
}

void TestDiceAliasModel::addTest()
{
    QFETCH(QString, pattern);
    QFETCH(QString, replace);
    QFETCH(bool, regexp);


    QVERIFY(m_model->rowCount() == 0);

    m_model->addAlias(new DiceAlias(pattern,replace,regexp));

    QCOMPARE(m_model->rowCount(), 1);
}

void TestDiceAliasModel::addTest_data()
{
   QTest::addColumn<QString>("pattern");
   QTest::addColumn<QString>("replace");
   QTest::addColumn<bool>("regexp");


   QTest::addRow("alias1") <<  "k"      << "d10k"  << false ;
   QTest::addRow("alias2") <<  "K"      << "d10e10k"  << false ;
   QTest::addRow("alias3") <<  "nwod"   << "d10e10c[>7]"  << false ;
   QTest::addRow("alias4") <<  "blue"   << "d10e10k"  << false ;
}

void TestDiceAliasModel::removeTest()
{
    addDefaultAlias();
    QCOMPARE(m_model->rowCount(), 5);


    m_model->clear();
    QCOMPARE(m_model->rowCount(), 0);

    addDefaultAlias();
    QCOMPARE(m_model->rowCount(), 5);

    QModelIndexList indexList;
    auto index = m_model->index(0, 0);
    indexList << index;
    m_model->deleteAlias(index);
    QCOMPARE(m_model->rowCount(), 4);

    m_model->clear();
    QCOMPARE(m_model->rowCount(), 0);

    addDefaultAlias();
    QCOMPARE(m_model->rowCount(), 5);

    index = m_model->index(0,0);
    auto index1 = m_model->index(1,0);
    m_model->deleteAlias(index1);
    m_model->deleteAlias(index);
    QCOMPARE(m_model->rowCount(), 3);
}

void TestDiceAliasModel::moveTest()
{
    auto listState = m_model->getAliases();

    addDefaultAlias();
    QCOMPARE(m_model->rowCount(), 5);

    auto index = m_model->index(1,0);
    m_model->upAlias(index);
    QCOMPARE(listState->at(0)->getCommand(),"l5R");

    index = m_model->index(1,0);
    m_model->downAlias(index);
    QCOMPARE(listState->at(1)->getCommand(),"DF");
    QCOMPARE(listState->at(2)->getCommand(),"l5r");

    index = m_model->index(4,0);
    m_model->topAlias(index);
    QCOMPARE(listState->at(0)->getCommand(),"(.*)wod(.*)");

    index = m_model->index(0,0);
    m_model->bottomAlias(index);
    QCOMPARE(listState->at(4)->getCommand(),"(.*)wod(.*)");
}

void TestDiceAliasModel::saveModelTest()
{
    addDefaultAlias();
    QCOMPARE(m_model->rowCount(), 5);

    QJsonObject obj;
    m_model->save(obj);

    DiceAliasModel model;

    model.load(obj);
    QCOMPARE(model.rowCount(), 5);
    auto list = model.getAliases();
    QCOMPARE(list->at(0)->getCommand(), "l5r");
    QCOMPARE(list->at(4)->getCommand(), "(.*)wod(.*)");

}

QTEST_MAIN(TestDiceAliasModel);

#include "tst_dicealiasmodel.moc"
