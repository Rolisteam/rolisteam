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

#include "charactersheetitem.h"
#include "charactersheetmodel.h"
#include "field.h"
#include "section.h"
#include "tablefield.h"

class TestCharacterSheetModel : public QObject
{
    Q_OBJECT
public:
    TestCharacterSheetModel();

private slots:
    void init();

    void creationTest();

    void addField();
    void addField_data();

    void addCharacter();
    void addCharacter_data();

    void addFieldOnCharacter();
    void addFieldOnCharacter_data();

    void removeDataTest();

    void saveModelTest();
    void readAndWriteModelTest();

    void clearModel();

private:
    std::unique_ptr<CharacterSheetModel> m_model;
};
Q_DECLARE_METATYPE(CharacterSheetItem*);

TestCharacterSheetModel::TestCharacterSheetModel() {}

void TestCharacterSheetModel::init()
{
    m_model.reset(new CharacterSheetModel());
}

void TestCharacterSheetModel::creationTest()
{
    QVERIFY(m_model->getRootSection() != nullptr);
}

void TestCharacterSheetModel::addField()
{
    QFETCH(CharacterSheetItem*, field);
    QFETCH(int, expected);

    QVERIFY(m_model->rowCount() == 0);

    auto root= new Section();
    root->appendChild(field);

    m_model->setRootSection(root);

    QCOMPARE(m_model->rowCount(), expected);
}

void TestCharacterSheetModel::addField_data()
{
    QTest::addColumn<CharacterSheetItem*>("field");
    QTest::addColumn<int>("expected");

    CharacterSheetItem* field= new Field(true);
    field->setCurrentType(CharacterSheetItem::TEXTINPUT);
    QTest::addRow("list1") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::TEXTFIELD);
    QTest::addRow("list2") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::TEXTAREA);
    QTest::addRow("list3") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::SELECT);
    QTest::addRow("list4") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::CHECKBOX);
    QTest::addRow("list5") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::IMAGE);
    QTest::addRow("list6") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::DICEBUTTON);
    QTest::addRow("list7") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::FUNCBUTTON);
    QTest::addRow("list8") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::WEBPAGE);
    QTest::addRow("list9") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::NEXTPAGE);
    QTest::addRow("list10") << field << 1;

    field= new Field(true);
    field->setCurrentType(CharacterSheetItem::PREVIOUSPAGE);
    QTest::addRow("list11") << field << 1;

    field= new TableField(true);
    field->setCurrentType(CharacterSheetItem::TABLE);
    QTest::addRow("list12") << field << 1;
}

void TestCharacterSheetModel::addCharacter() {}
void TestCharacterSheetModel::addCharacter_data() {}

void TestCharacterSheetModel::addFieldOnCharacter() {}
void TestCharacterSheetModel::addFieldOnCharacter_data() {}

void TestCharacterSheetModel::removeDataTest() {}

void TestCharacterSheetModel::saveModelTest() {}
void TestCharacterSheetModel::readAndWriteModelTest() {}

void TestCharacterSheetModel::clearModel() {}

QTEST_MAIN(TestCharacterSheetModel);

#include "tst_charactersheetmodel.moc"
