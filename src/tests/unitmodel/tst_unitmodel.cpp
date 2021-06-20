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

#include <QModelIndex>
#include <QModelIndexList>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <memory>

#include "widgets/gmtoolbox/UnitConvertor/unit.h"
#include "widgets/gmtoolbox/UnitConvertor/unitmodel.h"

using namespace GMTOOL;
class TestUnitModel : public QObject
{
    Q_OBJECT
public:
    TestUnitModel();

private slots:
    void init();

    void addTest();
    void addTest_data();

    void insertTest();
    void removeTest();

    void saveModelTest();

private:
    std::unique_ptr<UnitModel> m_model;
};

TestUnitModel::TestUnitModel() {}

void TestUnitModel::init()
{
    m_model.reset(new UnitModel());
}

void TestUnitModel::addTest()
{
    QFETCH(QStringList, nameList);
    QFETCH(QStringList, symbolList);
    QFETCH(QList<int>, category);
    QFETCH(int, expected);

    QVERIFY(m_model->rowCount() == 0);

    int i= 0;
    for(auto name : nameList)
    {
        auto symbol= symbolList[i];
        auto cat= category[i];
        m_model->insertData(new Unit(name, symbol, static_cast<Unit::Category>(cat)));
    }

    QCOMPARE(m_model->rowCount(), expected);
}

void TestUnitModel::addTest_data()
{
    QTest::addColumn<QStringList>("nameList");
    QTest::addColumn<QStringList>("symbolList");
    QTest::addColumn<QList<int>>("category");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << QStringList() << QStringList() << QList<int>() << 0;
    QTest::addRow("list2") << QStringList({"Kilometer"}) << QStringList({"km"}) << QList<int>({Unit::DISTANCE}) << 1;
    QTest::addRow("list3") << QStringList({"Kilometer", "meter"}) << QStringList({"km", "m"})
                           << QList<int>({Unit::DISTANCE, Unit::DISTANCE}) << 2;
    QTest::addRow("list4") << QStringList({"Kilometer", "meter", "gram"}) << QStringList({"km", "m", "g"})
                           << QList<int>({Unit::DISTANCE, Unit::DISTANCE, Unit::MASS}) << 3;
}

void TestUnitModel::removeTest()
{
    /* QStringList list;
     list << "song1.mp3"
          << "song2.ogg";
     m_model->addSong(list);

     QCOMPARE(m_model->rowCount(), 2);

     m_model->removeAll();

     QCOMPARE(m_model->rowCount(), 0);

     m_model->addSong(list);

     QCOMPARE(m_model->rowCount(), 2);

     QModelIndexList indexList;
     auto index= m_model->index(0, 0);
     indexList << index;
     m_model->removeSong(indexList);

     QCOMPARE(m_model->rowCount(), 1);

     m_model->removeSong(indexList);

     QCOMPARE(m_model->rowCount(), 0);

     m_model->addSong(list);

     QCOMPARE(m_model->rowCount(), 2);

     index= m_model->index(0, 0);
     auto index1= m_model->index(1, 0);
     indexList << index << index1;
     m_model->removeSong(indexList);
     QCOMPARE(m_model->rowCount(), 0);*/
}

void TestUnitModel::insertTest()
{
    /*    QStringList list;
        list << "/home/file/song1.mp3"
             << "/home/file/song2.ogg";
        m_model->addSong(list);

        QCOMPARE(m_model->rowCount(), 2);
        QString song("/home/file/songInserted.mp3");
        m_model->insertSong(1, song);

        QCOMPARE(m_model->rowCount(), 3);

        auto index= m_model->index(1, 0);
        QCOMPARE(index.data().toString(), "songInserted.mp3");*/
}

void TestUnitModel::saveModelTest()
{
    /*   QStringList list;
       list << "http://song1.mp3"
            << "http://song2.ogg";
       m_model->addSong(list);

       QString data;
       QTextStream out(&data, QIODevice::WriteOnly);
       m_model->saveIn(out);
       auto list2= data.split('\n', QString::SkipEmptyParts);

       QCOMPARE(list, list2);*/
}

QTEST_MAIN(TestUnitModel);

#include "tst_unitmodel.moc"
