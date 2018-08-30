/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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

#include <character.h>
#include <genericmodel.h>

#define COUNT_TURN 2000
class FieldModelTest : public QObject
{
    Q_OBJECT

public:
    FieldModelTest();

private Q_SLOTS:
    void testHeader();
    void testAdd();
    void testDelete();
    void initTestCase();

private:
    GenericModel* m_model;
    CharacterProperty propriety;
    CharacterProperty propriety1;
    CharacterProperty propriety2;
};

FieldModelTest::FieldModelTest()
{

}
void FieldModelTest::initTestCase()
{
    QStringList list;
    list << "column1" << "column2" ;
     m_model = new GenericModel(list,this);
}

void FieldModelTest::testHeader()
{
    QVERIFY(m_model->columnCount() == 2);
    auto col1 = m_model->headerData(0,Qt::Horizontal, Qt::DisplayRole).toString();
    auto col2 = m_model->headerData(1,Qt::Horizontal, Qt::DisplayRole).toString();

    QVERIFY(col1 == "column1");
    QVERIFY(col2 == "column2");
}
void FieldModelTest::testAdd()
{
    QSignalSpy spy(m_model, &GenericModel::rowsAboutToBeInserted);
    QVERIFY(m_model->rowCount() == 0);
    m_model->addData(&propriety);
    QVERIFY(m_model->rowCount() == 1);
    m_model->addData(&propriety1);
    QVERIFY(m_model->rowCount() == 2);
    m_model->addData(&propriety2);
    QVERIFY(m_model->rowCount() == 3);

    QVERIFY(spy.count() == 3);

}
void FieldModelTest::testDelete()
{
    QSignalSpy spy(m_model, &GenericModel::rowsAboutToBeRemoved);
    QVERIFY(m_model->rowCount() == 3);

    m_model->removeData(m_model->index(0,0));
    QVERIFY(m_model->rowCount() == 2);
    m_model->removeData(m_model->index(0,0));
    QVERIFY(m_model->rowCount() == 1);
    m_model->removeData(m_model->index(0,0));
    QVERIFY(m_model->rowCount() == 0);

    QVERIFY(spy.count() == 3);

}




QTEST_MAIN(FieldModelTest);

#include "tst_fieldmodel.moc"
