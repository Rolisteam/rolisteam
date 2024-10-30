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
#include <QtTest/QtTest>

#include <QSignalSpy>
#include <QUrl>

#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/sidemenucontroller.h"
#include <QAbstractItemModelTester>
#include <memory>

using namespace mindmap;

class SideMenuControllerTest : public QObject
{
    Q_OBJECT

public:
    SideMenuControllerTest();

private slots:
    void init();
    void criteria();
    void criteria_data();

private:
    std::unique_ptr<mindmap::SideMenuController> m_ctrl;
    std::unique_ptr<MindMapController> m_mindmap;
};

SideMenuControllerTest::SideMenuControllerTest() {}

void SideMenuControllerTest::init()
{
    m_mindmap.reset(new MindMapController("mindmaptest"));
    m_ctrl.reset(new mindmap::SideMenuController());
}

void initModel(mindmap::MindItemModel* model)
{
    new QAbstractItemModelTester(model);

    // l.first->setText("foo");
    // l.first->setId("toto");
    auto node1= new MindNode();
    node1->setTags({"tag1", "tag2", "blue", "yellow"});
    node1->setDescription("Description of node1");
    node1->setText("foo");
    node1->setId("toto");

    auto node2= new MindNode();
    node2->setText("bar");
    node2->setId("toto2");
    node2->setTags({"tag1", "yellow", "mouse"});
    node2->setDescription("bla.. bla..");

    auto node3= new MindNode();
    node3->setParentNode(node1);
    node3->setText("characters");
    node3->setId("charactersId");
    node3->setTags({"pc", "failure", "coffe"});
    node3->setDescription("The characters");

    auto node4= new MindNode();
    node4->setParentNode(node1);
    node4->setText("sentiment");
    node4->setId("id4");
    node4->setTags({"samourai", "sword", "killer"});
    node4->setDescription("hello world!");

    model->appendItem({node1, node2, node3, node4}, false);
}

void SideMenuControllerTest::criteria()
{
    QFETCH(FilteredModel::Criteria, criteria);
    QFETCH(QString, pattern);
    QFETCH(int, count);

    initModel(m_mindmap->itemModel());

    QSignalSpy spyCtrl(m_ctrl.get(), &SideMenuController::controllerChanged);
    QSignalSpy spyPat(m_ctrl.get(), &SideMenuController::patternChanged);
    QSignalSpy spyCrit(m_ctrl.get(), &SideMenuController::criteriaChanged);

    m_ctrl->setController(m_mindmap.get());
    spyCtrl.wait(10);
    QCOMPARE(spyCtrl.count(), 1);
    m_ctrl->setController(m_mindmap.get());
    spyCtrl.wait(10);
    QCOMPARE(spyCtrl.count(), 1);
    QCOMPARE(m_ctrl->controller(), m_mindmap.get());

    if(m_ctrl->pattern() != pattern)
    {
        m_ctrl->setPattern(pattern);
        spyPat.wait(10);
        QCOMPARE(spyPat.count(), 1);
        m_ctrl->setPattern(pattern);
        spyPat.wait(10);
        QCOMPARE(spyPat.count(), 1);
        QCOMPARE(m_ctrl->pattern(), pattern);
    }

    if(m_ctrl->criteria() != criteria)
    {
        m_ctrl->setCriteria(static_cast<FilteredModel::Criteria>(criteria));
        spyCrit.wait(10);
        QCOMPARE(spyCrit.count(), 1);
        m_ctrl->setCriteria(static_cast<FilteredModel::Criteria>(criteria));
        spyCrit.wait(10);
        QCOMPARE(spyCrit.count(), 1);
        QCOMPARE(m_ctrl->criteria(), criteria);
    }

    if(m_ctrl->model()->rowCount() != count)
    {
        for(int i= 0; i < m_ctrl->model()->rowCount(); ++i)
        {
            qDebug() << m_ctrl->model()->data(m_ctrl->model()->index(i, 0));
        }
    }
    QCOMPARE(m_ctrl->model()->rowCount(), count);
}
void SideMenuControllerTest::criteria_data()
{
    QTest::addColumn<FilteredModel::Criteria>("criteria");
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<int>("count");

    QTest::addRow("empty") << FilteredModel::NoCrit << QString() << 4;
    QTest::addRow("empty name") << FilteredModel::NameCrit << QString() << 4;
    QTest::addRow("empty tag") << FilteredModel::TagCrit << QString() << 4;
    QTest::addRow("empty desc") << FilteredModel::DescriptionCrit << QString() << 4;
    QTest::addRow("empty parent") << FilteredModel::ParentOfCrit << QString() << 4;
    QTest::addRow("empty all") << FilteredModel::AllCrit << QString() << 4;

    QTest::addRow("one letter") << FilteredModel::NoCrit << QString("a") << 4;
    QTest::addRow("one letter name") << FilteredModel::NameCrit << QString("a") << 2;
    QTest::addRow("one letter tag") << FilteredModel::TagCrit << QString("a") << 4;
    QTest::addRow("one letter desc") << FilteredModel::DescriptionCrit << QString("a") << 2;
    QTest::addRow("one letter all") << FilteredModel::AllCrit << QString("a") << 4;

    QTest::addRow("two letters") << FilteredModel::NoCrit << QString("ta") << 4;
    QTest::addRow("two letters name") << FilteredModel::NameCrit << QString("ta") << 0;
    QTest::addRow("two letters tag") << FilteredModel::TagCrit << QString("ta") << 2;
    QTest::addRow("two letters desc") << FilteredModel::DescriptionCrit << QString("ta") << 0;
    QTest::addRow("two letters all") << FilteredModel::AllCrit << QString("ta") << 2;

    QTest::addRow("three letters") << FilteredModel::NoCrit << QString("har") << 4;
    QTest::addRow("three letters name") << FilteredModel::NameCrit << QString("har") << 1;
    QTest::addRow("three letters tag") << FilteredModel::TagCrit << QString("har") << 0;
    QTest::addRow("three letters desc") << FilteredModel::DescriptionCrit << QString("har") << 1;
    QTest::addRow("three letters all") << FilteredModel::AllCrit << QString("har") << 1;

    QTest::addRow("ParentOfCrit 1") << FilteredModel::ParentOfCrit << QString("tatz!") << 0;
    QTest::addRow("ParentOfCrit 2") << FilteredModel::ParentOfCrit << QString("toto") << 2;
}

QTEST_MAIN(SideMenuControllerTest);

#include "tst_mindmapsidemenucontroller.moc"
