/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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

#include "controller/view_controller/mindmapcontroller.h"
#include "mindmap/src/controller/selectioncontroller.h"
#include "mindmap/src/data/link.h"
#include "mindmap/src/data/mindnode.h"
#include "mindmap/src/geometry/linknode.h"
#include "mindmap/src/model/boxmodel.h"
#include "mindmap/src/model/linkmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"
#include <memory>

#include <QSignalSpy>
#include <QTimer>

class MindMapTest : public QObject
{
    Q_OBJECT
public:
    MindMapTest();

private slots:
    void init();
    void remoteAddTest();
    void removeNodeAndLinkTest();
    void removeNodeAndLinkTest_data();

private:
    std::unique_ptr<MindMapController> m_ctrl;
};

void MindMapTest::init()
{
    m_ctrl.reset(new MindMapController("test_id"));
}

MindMapTest::MindMapTest() {}

void MindMapTest::remoteAddTest()
{
    // auto ctrl= new MindMapController("test_id_tmp");
    // auto spacerCtrl= m_ctrl->spacingController();
    auto nodeModel= dynamic_cast<mindmap::BoxModel*>(m_ctrl->nodeModel());
    auto linkModel= dynamic_cast<mindmap::LinkModel*>(m_ctrl->linkModel());

    QSignalSpy nodeAdded(nodeModel, &mindmap::BoxModel::rowsInserted);
    QSignalSpy linkAdded(linkModel, &mindmap::LinkModel::rowsInserted);

    auto node1= new mindmap::MindNode();
    node1->setText("Node1");
    node1->setPosition(QPointF(0, 0));

    auto node2= new mindmap::MindNode();
    node2->setText("Node2");
    node2->setPosition(QPointF(0, 0));

    auto link= new mindmap::Link();
    link->setStart(node1);
    link->setEnd(node2);

    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddMessage);
    MessageHelper::buildAddItemMessage(msg, {node1, node2}, {link});

    auto dataMsg= msg.getData();
    NetworkMessageReader readMsg;
    readMsg.setData(dataMsg);
    MessageHelper::readMindMapAddItem(m_ctrl.get(), &readMsg);

    /*  auto dataMsg1= msg1.getData();
      NetworkMessageReader readMsg1;
      readMsg1.setData(dataMsg1);
      MessageHelper::readAddMindMapNode(m_ctrl.get(), &readMsg1);

      auto dataMsgLink= msgLink.getData();
      NetworkMessageReader readMsgLink;
      readMsgLink.setData(dataMsgLink);
      MessageHelper::readMindMapLink(m_ctrl.get(), &readMsgLink);*/

    QCOMPARE(nodeModel->rowCount(), 2);
    QCOMPARE(linkModel->rowCount(), 1);

    QCOMPARE(nodeAdded.count(), 2);
    QCOMPARE(linkAdded.count(), 1);

    QTimer timer;
    QSignalSpy spacer(&timer, &QTimer::timeout);
    timer.start(5000);

    spacer.wait(5001);

    auto realNode1= nodeModel->nodeFromId(node1->id());
    auto realNode2= nodeModel->nodeFromId(node2->id());
    auto realLink= linkModel->linkFromId(link->id());
    qDebug() << " ##########" << realNode1->centerPoint() << realNode1->position() << realNode1->boundingRect();

    QCOMPARE(qIsNaN(realNode1->position().x()), false);
    QCOMPARE(qIsNaN(realNode1->position().y()), false);
    QCOMPARE(qIsNaN(realNode2->position().x()), false);
    QCOMPARE(qIsNaN(realNode2->position().y()), false);

    // QCOMPARE(node1->position(), realNode1->position());
    // QCOMPARE(node1->boundingRect(), realNode1->boundingRect());
    // QCOMPARE(realLink->startPoint(), link->startPoint());
    // QCOMPARE(realLink->endPoint(), link->endPoint());
    QVERIFY(realLink->p1() != QPointF());
    QVERIFY(realLink->p2() != QPointF());
}

void MindMapTest::removeNodeAndLinkTest()
{
    QFETCH(QList<QList<int>>, nodetree);
    QFETCH(int, nodeCount);
    QFETCH(int, linkCount);
    QFETCH(int, indexToRemove);
    QFETCH(int, restingNodes);
    QFETCH(int, restingLinks);

    auto nodeModel= dynamic_cast<mindmap::BoxModel*>(m_ctrl->nodeModel());
    auto linkModel= dynamic_cast<mindmap::LinkModel*>(m_ctrl->linkModel());

    auto& nodes= nodeModel->nodes();

    QCOMPARE(linkModel->rowCount(), 0);
    QCOMPARE(linkModel->rowCount(), 0);

    int d= 0;
    QList<QStringList> idLevels;
    for(const auto& level : nodetree)
    {
        QStringList currentLevelIds;
        int index= 0;
        for(auto c : level)
        {
            for(int i= 0; i < c; ++i)
            {
                if(d == 0)
                {
                    m_ctrl->addBox(QString());
                    auto last= nodes[nodes.size() - 1];

                    currentLevelIds << last->id();
                }
                else
                {
                    // qDebug() << "parentId: "<< c-1 << index;
                    auto parentId= idLevels[idLevels.size() - 1].at(index);
                    m_ctrl->addBox(parentId);
                    auto last= nodes[nodes.size() - 1];
                    currentLevelIds << last->id();
                }
            }
            index++;
        }
        idLevels << currentLevelIds;
        ++d;
    }

    QCOMPARE(nodeModel->rowCount(), nodeCount);
    QCOMPARE(linkModel->rowCount(), linkCount);

    m_ctrl->undo();

    QCOMPARE(nodeModel->rowCount(), std::max(nodeCount - 1, 0));
    QCOMPARE(linkModel->rowCount(), std::max(linkCount - 1, 0));

    m_ctrl->redo();

    QCOMPARE(nodeModel->rowCount(), nodeCount);
    QCOMPARE(linkModel->rowCount(), linkCount);

    if(indexToRemove < 0)
        return;
    // Remove

    QSignalSpy nodeSpy(nodeModel, &mindmap::BoxModel::rowsAboutToBeRemoved);
    QSignalSpy nodeLink(linkModel, &mindmap::LinkModel::rowsAboutToBeRemoved);

    auto selectionCtrl= m_ctrl->selectionController();
    auto last= nodes[indexToRemove];
    selectionCtrl->addToSelection(last);
    m_ctrl->removeSelection();

    QCOMPARE(nodeModel->rowCount(), restingNodes);
    QCOMPARE(linkModel->rowCount(), restingLinks);

    QCOMPARE(nodeSpy.count(), nodeCount - restingNodes);
    QCOMPARE(nodeLink.count(), linkCount - restingLinks);

    m_ctrl->undo();

    QCOMPARE(nodeModel->rowCount(), nodeCount);
    QCOMPARE(linkModel->rowCount(), linkCount);
    nodeSpy.clear();
    nodeLink.clear();

    m_ctrl->redo();

    QCOMPARE(nodeModel->rowCount(), restingNodes);
    QCOMPARE(linkModel->rowCount(), restingLinks);

    QCOMPARE(nodeSpy.count(), nodeCount - restingNodes);
    QCOMPARE(nodeLink.count(), linkCount - restingLinks);
}

void MindMapTest::removeNodeAndLinkTest_data()
{
    QTest::addColumn<QList<QList<int>>>("nodetree");
    QTest::addColumn<int>("nodeCount");
    QTest::addColumn<int>("linkCount");
    QTest::addColumn<int>("indexToRemove");
    QTest::addColumn<int>("restingNodes");
    QTest::addColumn<int>("restingLinks");

    QTest::addRow("cmd 1") << QList<QList<int>>() << 0 << 0 << -1 << 0 << 0;
    QTest::addRow("cmd 2") << QList<QList<int>>({{1}}) << 1 << 0 << 0 << 0 << 0;
    QTest::addRow("cmd 3") << QList<QList<int>>({{1}, {1}}) << 2 << 1 << 0 << 0 << 0;
    QTest::addRow("cmd 4") << QList<QList<int>>({{1}, {1}}) << 2 << 1 << 1 << 1 << 0;

    QTest::addRow("cmd 5") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 0 << 0 << 0;
    QTest::addRow("cmd 6") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 1 << 2 << 1;
    QTest::addRow("cmd 7") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 2 << 2 << 1;

    QTest::addRow("cmd 8") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 0 << 0 << 0;
    QTest::addRow("cmd 9") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 1 << 1 << 0;
    QTest::addRow("cmd 10") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 2 << 2 << 1;

    QTest::addRow("cmd 11") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 0 << 0 << 0;
    QTest::addRow("cmd 12") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 1 << 3 << 2;
    QTest::addRow("cmd 13") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 2 << 4 << 3;
    QTest::addRow("cmd 13") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 3 << 5 << 4;
    QTest::addRow("cmd 13") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 4 << 5 << 4;
    QTest::addRow("cmd 13") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 5 << 5 << 4;
}

QTEST_MAIN(MindMapTest);

#include "tst_mindmap.moc"
