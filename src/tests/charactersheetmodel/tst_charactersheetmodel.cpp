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
#include <QString>
#include <QVector>
#include <QtTest>
#include <memory>

#include "charactersheetitem.h"
#include "charactersheetmodel.h"
#include "field.h"
#include "networkmessagereader.h"
#include "networkmessagewriter.h"
#include "section.h"
#include "tablefield.h"
#include <helper.h>

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

    void removeDataTest();
    void removeDataTest_data();

    void saveModelTest();
    void saveModelTest_data();

    void readAndWriteModelTest();
    void readAndWriteModelTest_data();

    void clearModel();
    void clearModel_data();

private:
    std::unique_ptr<CharacterSheetModel> m_model;
};
Q_DECLARE_METATYPE(CharacterSheetItem*);

TestCharacterSheetModel::TestCharacterSheetModel() {}

Field* fieldFactory(const CharacterSheetItem::TypeField& currentType)
{
    auto field= new Field(true);
    field->setCurrentType(currentType);
    return field;
}

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
    QFETCH(Field*, field);
    QFETCH(int, expected);

    QVERIFY(m_model->rowCount() == 0);

    auto root= new Section();
    root->appendChild(field);

    m_model->setRootSection(root);

    QCOMPARE(m_model->rowCount(), expected);
}

void TestCharacterSheetModel::addField_data()
{
    QTest::addColumn<Field*>("field");
    QTest::addColumn<int>("expected");

    QTest::addRow("list1") << fieldFactory(CharacterSheetItem::TEXTINPUT) << 1;
    QTest::addRow("list2") << fieldFactory(CharacterSheetItem::TEXTFIELD) << 1;
    QTest::addRow("list3") << fieldFactory(CharacterSheetItem::TEXTAREA) << 1;
    QTest::addRow("list4") << fieldFactory(CharacterSheetItem::SELECT) << 1;
    QTest::addRow("list5") << fieldFactory(CharacterSheetItem::CHECKBOX) << 1;
    QTest::addRow("list6") << fieldFactory(CharacterSheetItem::IMAGE) << 1;
    QTest::addRow("list7") << fieldFactory(CharacterSheetItem::DICEBUTTON) << 1;
    QTest::addRow("list8") << fieldFactory(CharacterSheetItem::FUNCBUTTON) << 1;
    QTest::addRow("list9") << fieldFactory(CharacterSheetItem::WEBPAGE) << 1;
    QTest::addRow("list10") << fieldFactory(CharacterSheetItem::NEXTPAGE) << 1;
    QTest::addRow("list11") << fieldFactory(CharacterSheetItem::PREVIOUSPAGE) << 1;
    QTest::addRow("list12") << fieldFactory(CharacterSheetItem::TABLE) << 1;
}

void TestCharacterSheetModel::addCharacter()
{
    auto sheet= m_model->addCharacterSheet();
    auto uuid= sheet->getUuid();
    QCOMPARE(m_model->getCharacterSheetCount(), 1);

    auto sheet2= m_model->getCharacterSheetById(uuid);
    QCOMPARE(sheet, sheet2);

    m_model->removeCharacterSheet(sheet);
    QCOMPARE(m_model->getCharacterSheetCount(), 0);
}

void TestCharacterSheetModel::removeDataTest()
{
    QFETCH(QVector<Field*>, fields);
    QFETCH(int, expected);

    auto sheet= m_model->addCharacterSheet();

    auto root= new Section();
    for(auto field : fields)
    {
        root->appendChild(field);
    }
    m_model->setRootSection(root);

    QCOMPARE(sheet->getFieldCount(), expected);

    for(auto field : fields)
    {
        root->removeChild(field);
    }
    m_model->setRootSection(root);
    QCOMPARE(sheet->getFieldCount(), 0);
}
void TestCharacterSheetModel::removeDataTest_data()
{
    QTest::addColumn<QVector<Field*>>("fields");
    QTest::addColumn<int>("expected");

    QTest::addRow("remove0") << QVector<Field*>() << 0;

    std::vector<CharacterSheetItem::TypeField> data(
        {CharacterSheetItem::TEXTINPUT, CharacterSheetItem::TEXTFIELD, CharacterSheetItem::TEXTAREA,
            CharacterSheetItem::SELECT, CharacterSheetItem::CHECKBOX, CharacterSheetItem::IMAGE,
            CharacterSheetItem::DICEBUTTON, CharacterSheetItem::FUNCBUTTON, CharacterSheetItem::WEBPAGE,
            CharacterSheetItem::NEXTPAGE, CharacterSheetItem::PREVIOUSPAGE, CharacterSheetItem::TABLE});

    QVector<Field*> list;

    int index= 0;
    for(std::size_t i= 0; i < data.size(); ++i)
    {
        std::size_t comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                list.push_back(fieldFactory(*it));
            }
            auto title= QStringLiteral("remove%1").arg(++index);
            QTest::addRow(title.toStdString().c_str()) << list << list.size();
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}
void TestCharacterSheetModel::saveModelTest()
{
    QJsonObject obj;
    {
        QFETCH(QVector<Field*>, fields);
        QFETCH(int, expected);

        auto sheet= m_model->addCharacterSheet();

        auto root= new Section();
        for(auto field : fields)
        {
            root->appendChild(field);
        }
        m_model->setRootSection(root);

        QCOMPARE(sheet->getFieldCount(), expected);

        m_model->writeModel(obj, true);
    }
    CharacterSheetModel model;
    model.readModel(obj, true);

    QCOMPARE(model.getCharacterSheetCount(), m_model->getCharacterSheetCount());
    auto rootSec= model.getRootSection();
    auto rootSec2= m_model->getRootSection();
    QCOMPARE(rootSec->getChildrenCount(), rootSec2->getChildrenCount());
}

void TestCharacterSheetModel::saveModelTest_data()
{
    QTest::addColumn<QVector<Field*>>("fields");
    QTest::addColumn<int>("expected");

    QTest::addRow("save0") << QVector<Field*>() << 0;

    std::vector<CharacterSheetItem::TypeField> data(
        {CharacterSheetItem::TEXTINPUT, CharacterSheetItem::TEXTFIELD, CharacterSheetItem::TEXTAREA,
            CharacterSheetItem::SELECT, CharacterSheetItem::CHECKBOX, CharacterSheetItem::IMAGE,
            CharacterSheetItem::DICEBUTTON, CharacterSheetItem::FUNCBUTTON, CharacterSheetItem::WEBPAGE,
            CharacterSheetItem::NEXTPAGE, CharacterSheetItem::PREVIOUSPAGE, CharacterSheetItem::TABLE});

    QVector<Field*> list;

    int index= 0;
    for(std::size_t i= 0; i < data.size(); ++i)
    {
        std::size_t comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                list.push_back(fieldFactory(*it));
            }
            auto title= QStringLiteral("save%1").arg(++index);
            QTest::addRow(title.toStdString().c_str()) << list << list.size();
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}

void TestCharacterSheetModel::readAndWriteModelTest()
{
    NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::addCharacterSheet);
    {
        QFETCH(QVector<Field*>, fields);
        QFETCH(int, expected);

        auto sheet= m_model->addCharacterSheet();

        auto root= new Section();
        for(auto field : fields)
        {
            root->appendChild(field);
        }
        m_model->setRootSection(root);

        QCOMPARE(sheet->getFieldCount(), expected);

        m_model->fillRootSection(&msg);
    }
    CharacterSheetModel model;
    NetworkMessageReader reader;
    auto const array= msg.getData();
    reader.setInternalData(array);
    reader.resetToData();
    model.readRootSection(&reader);

    auto rootSec= model.getRootSection();
    auto rootSec2= m_model->getRootSection();
    QCOMPARE(rootSec->getChildrenCount(), rootSec2->getChildrenCount());
}
void TestCharacterSheetModel::readAndWriteModelTest_data()
{
    QTest::addColumn<QVector<Field*>>("fields");
    QTest::addColumn<int>("expected");

    QTest::addRow("network0") << QVector<Field*>() << 0;

    std::vector<CharacterSheetItem::TypeField> data(
        {CharacterSheetItem::TEXTINPUT, CharacterSheetItem::TEXTFIELD, CharacterSheetItem::TEXTAREA,
            CharacterSheetItem::SELECT, CharacterSheetItem::CHECKBOX, CharacterSheetItem::IMAGE,
            CharacterSheetItem::DICEBUTTON, CharacterSheetItem::FUNCBUTTON, CharacterSheetItem::WEBPAGE,
            CharacterSheetItem::NEXTPAGE, CharacterSheetItem::PREVIOUSPAGE, CharacterSheetItem::TABLE});

    QVector<Field*> list;

    int index= 0;
    for(std::size_t i= 0; i < data.size(); ++i)
    {
        std::size_t comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                list.push_back(fieldFactory(*it));
            }
            auto title= QStringLiteral("network%1").arg(++index);
            QTest::addRow(title.toStdString().c_str()) << list << list.size();
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}
void TestCharacterSheetModel::clearModel()
{
    QFETCH(QVector<Field*>, fields);
    QFETCH(int, expected);

    QPointer<CharacterSheet> sheet= m_model->addCharacterSheet();

    auto root= new Section();
    for(auto field : fields)
    {
        root->appendChild(field);
    }
    m_model->setRootSection(root);

    QCOMPARE(sheet->getFieldCount(), expected);

    m_model->clearModel();
    QCOMPARE(sheet.isNull(), true);
    QCOMPARE(m_model->getCharacterSheetCount(), 0);
    QCOMPARE(m_model->getRootSection()->getChildrenCount(), 0);
}
void TestCharacterSheetModel::clearModel_data()
{
    QTest::addColumn<QVector<Field*>>("fields");
    QTest::addColumn<int>("expected");

    QTest::addRow("clear0") << QVector<Field*>() << 0;

    std::vector<CharacterSheetItem::TypeField> data(
        {CharacterSheetItem::TEXTINPUT, CharacterSheetItem::TEXTFIELD, CharacterSheetItem::TEXTAREA,
            CharacterSheetItem::SELECT, CharacterSheetItem::CHECKBOX, CharacterSheetItem::IMAGE,
            CharacterSheetItem::DICEBUTTON, CharacterSheetItem::FUNCBUTTON, CharacterSheetItem::WEBPAGE,
            CharacterSheetItem::NEXTPAGE, CharacterSheetItem::PREVIOUSPAGE, CharacterSheetItem::TABLE});

    QVector<Field*> list;

    int index= 0;
    for(std::size_t i= 0; i < data.size(); ++i)
    {
        std::size_t comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                list.push_back(fieldFactory(*it));
            }
            auto title= QStringLiteral("clear%1").arg(++index);
            QTest::addRow(title.toStdString().c_str()) << list << list.size();
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}
QTEST_MAIN(TestCharacterSheetModel);

#include "tst_charactersheetmodel.moc"
