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
#include <QTest>
#include <QAbstractItemModelTester>

#include "controller/view_controller/mindmapcontroller.h"
#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/geometry/linknode.h"
#include "mindmap/model/minditemmodel.h"
#include "mindmap/model/nodestylemodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"
#include <helper.h>
#include <memory>

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QSignalSpy>
#include <QTimer>

class MindMapTest : public QObject
{
    Q_OBJECT
public:
    MindMapTest();

private slots:
    void init();

    void addRemoveImageTest();
    void addItemAndUndoTest();
    void reparentTest();
    void AddLinkTest();

    void selectionCtrl();
    void selectionCtrl_data();

    void removeNodeAndLinkTest();
    void removeNodeAndLinkTest_data();

    void remoteAddTest();

    void linkGeometryTest();
    void getAndSetTest();

private:
    std::unique_ptr<MindMapController> m_ctrl;
};

void MindMapTest::init()
{
    m_ctrl.reset(new MindMapController("test_id"));
}

MindMapTest::MindMapTest() {}

void MindMapTest::addRemoveImageTest()
{
    auto node= std::make_unique<mindmap::MindNode>();
    auto id= Helper::randomString();
    node->setId(id);
    m_ctrl->addNode({node.get()}, false);

    auto imagePath= Helper::imagePath();
    m_ctrl->addImageFor(id, imagePath, {});

    QVERIFY(m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(!m_ctrl->canRedo());

    m_ctrl->undo();

    QVERIFY(!m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(!m_ctrl->canUndo());
    QVERIFY(m_ctrl->canRedo());

    m_ctrl->redo();

    QVERIFY(m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(!m_ctrl->canRedo());

    m_ctrl->removeImageFor(id);

    QVERIFY(!m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(!m_ctrl->canRedo());

    m_ctrl->undo();

    QVERIFY(m_ctrl->imgModel()->hasPixmap(id));
    QVERIFY(m_ctrl->canUndo());
    QVERIFY(m_ctrl->canRedo());

    m_ctrl->redo();
    QVERIFY(!m_ctrl->imgModel()->hasPixmap(id));
}

void MindMapTest::AddLinkTest()
{
    auto node= std::make_unique<mindmap::MindNode>();
    auto id= Helper::randomString();
    node->setId(id);

    auto node2= std::make_unique<mindmap::MindNode>();
    auto id2= Helper::randomString();
    node2->setId(id2);

    m_ctrl->addNode({node.get(), node2.get()}, false);

    m_ctrl->addLink(id, id2);

    auto items= m_ctrl->itemModel()->items(mindmap::MindItem::LinkType);
    QCOMPARE(items.size(), 1);
    QVERIFY(m_ctrl->canUndo());

    m_ctrl->undo();

    items= m_ctrl->itemModel()->items(mindmap::MindItem::LinkType);
    QCOMPARE(items.size(), 0);
    QVERIFY(m_ctrl->canRedo());

    m_ctrl->redo();
    items= m_ctrl->itemModel()->items(mindmap::MindItem::LinkType);
    QCOMPARE(items.size(), 1);
}

void MindMapTest::addItemAndUndoTest()
{
    m_ctrl->addNode(QString{});
    QVERIFY(m_ctrl->canUndo());
    QCOMPARE(m_ctrl->itemModel()->rowCount(), 1);

    m_ctrl->undo();

    QVERIFY(m_ctrl->canRedo());
    QCOMPARE(m_ctrl->itemModel()->rowCount(), 0);

    m_ctrl->redo();
    QVERIFY(m_ctrl->canUndo());
    QCOMPARE(m_ctrl->itemModel()->rowCount(), 1);
}

void MindMapTest::reparentTest()
{
    /*auto node = std::make_unique<mindmap::MindNode>();
    auto fatherId = Helper::randomString();
    node->setId(fatherId);

    auto node1 = std::make_unique<mindmap::MindNode>();
    auto child1Id = Helper::randomString();
    node1->setId(child1Id);

    auto node2 = std::make_unique<mindmap::MindNode>();
    auto child2Id = Helper::randomString();
    node2->setId(child2Id);*/

    m_ctrl->addNode(QString());
    auto items= m_ctrl->itemModel()->items(mindmap::MindItem::NodeType);
    QCOMPARE(items.size(), 1);

    auto main= dynamic_cast<mindmap::MindNode*>(items[0]);
    auto mainId= main->id();

    m_ctrl->addNode(mainId);
    items= m_ctrl->itemModel()->items(mindmap::MindItem::NodeType);
    QCOMPARE(items.size(), 2);

    auto second= dynamic_cast<mindmap::MindNode*>(items[1]);
    // auto secondId = second->id();

    m_ctrl->addNode(mainId);
    items= m_ctrl->itemModel()->items(mindmap::MindItem::NodeType);
    QCOMPARE(items.size(), 3);

    auto third= dynamic_cast<mindmap::MindNode*>(items[2]);
    auto thirdId= third->id();

    QSet<mindmap::LinkController*> a;
    QCOMPARE(main->subNodeCount(a), 2);

    QSet<mindmap::LinkController*> a1;
    QCOMPARE(second->subNodeCount(a1), 0);

    QSet<mindmap::LinkController*> a2;
    QCOMPARE(third->subNodeCount(a2), 0);
    QCOMPARE(third->parentNode(), main);
    QCOMPARE(second->parentNode(), main);

    m_ctrl->reparenting(second, thirdId);

    QSet<mindmap::LinkController*> b;
    QCOMPARE(main->subNodeCount(b), 2);

    QSet<mindmap::LinkController*> b1;
    QCOMPARE(second->subNodeCount(b1), 1);

    QSet<mindmap::LinkController*> b2;
    QCOMPARE(third->subNodeCount(b2), 0);
    QCOMPARE(third->parentNode(), main);
    QCOMPARE(second->parentNode(), main);

    m_ctrl->undo();

    QSet<mindmap::LinkController*> c;
    QCOMPARE(main->subNodeCount(c), 2);

    QSet<mindmap::LinkController*> c1;
    QCOMPARE(second->subNodeCount(c1), 0);

    QSet<mindmap::LinkController*> c2;
    QCOMPARE(third->subNodeCount(c2), 0);
    QCOMPARE(third->parentNode(), main);
    QCOMPARE(second->parentNode(), main);
}

void MindMapTest::selectionCtrl()
{
    QFETCH(int, nodeCount);
    auto ctrl= new mindmap::SelectionController();
    auto undo= new QUndoStack();
    ctrl->setUndoStack(undo);

    QVERIFY(!ctrl->enabled());
    QVERIFY(!ctrl->hasSelection());

    QList<QPointF> positions;
    positions.reserve(nodeCount);
    QList<mindmap::MindNode*> nodes;
    for(int i= 0; i < nodeCount; ++i)
    {
        auto node= new mindmap::MindNode();
        node->setText(Helper::randomString());
        QPointF pos(Helper::generate(0, 1000), Helper::generate(0, 1000));
        positions.append(pos);
        node->setPosition(pos);

        ctrl->addToSelection(node);
        nodes.append(node);
    }
    if(nodeCount > 0)
        QVERIFY(ctrl->hasSelection());
    QCOMPARE(ctrl->selectedNodes().size(), nodes.size());

    QPointF move{0, 0};
    for(int i= 0; i < nodeCount; ++i)
    {
        QPointF pos(Helper::generate(0, 100), Helper::generate(0, 100));
        ctrl->movingNode(pos);
        move+= pos;
    }

    auto selection= ctrl->selectedNodes();
    int i= 0;
    for(auto item : selection)
    {
        auto pos= positions[i];
        auto pItem= dynamic_cast<mindmap::PositionedItem*>(item);
        QCOMPARE(pItem->position(), pos + move);
        ++i;
    }

    for(auto item : selection)
    {
        ctrl->removeFromSelection(item);
    }
    QVERIFY(!ctrl->hasSelection());

    for(int i= 0; i < nodeCount; ++i)
    {
        auto node= new mindmap::MindNode();
        node->setText(Helper::randomString());
        QPointF pos(Helper::generate(0, 1000), Helper::generate(0, 1000));
        node->setPosition(pos);
        ctrl->addToSelection(node);
    }
    if(nodeCount > 0)
        QVERIFY(ctrl->hasSelection());
    ctrl->clearSelection();
    QVERIFY(!ctrl->hasSelection());

    while(undo->canUndo())
    {
        undo->undo();
    }

    delete ctrl;
    delete undo;
}

void MindMapTest::selectionCtrl_data()
{
    QTest::addColumn<int>("nodeCount");

    for(int i= 0; i < 100; ++i)
    {
        QTest::addRow("cmd %d", i) << i;
    }
}

void MindMapTest::remoteAddTest()
{
    // auto ctrl= new MindMapController("test_id_tmp");
    // auto spacerCtrl= m_ctrl->spacingController();
    auto itemModel= dynamic_cast<mindmap::MindItemModel*>(m_ctrl->itemModel());

    QSignalSpy itemAdded(itemModel, &mindmap::MindItemModel::rowsInserted);

    auto node1= new mindmap::MindNode();
    node1->setText("Node1");
    node1->setPosition(QPointF(0, 0));

    auto node2= new mindmap::MindNode();
    node2->setText("Node2");
    node2->setPosition(QPointF(0, 0));

    auto link= new mindmap::LinkController();
    link->setStart(node1);
    link->setEnd(node2);

    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddItem);
    MessageHelper::buildAddItemMessage(msg, {node1, node2, link});

    auto dataMsg= msg.data();
    NetworkMessageReader readMsg;
    readMsg.setData(dataMsg);
    MessageHelper::readMindMapAddItem(m_ctrl.get(), &readMsg);

    QCOMPARE(itemModel->rowCount(), 3);
    QCOMPARE(itemAdded.count(), 3);

    auto links= itemModel->items(mindmap::MindItem::LinkType);
    QCOMPARE(links.size(), 1);

    QTimer timer;
    QSignalSpy spacer(&timer, &QTimer::timeout);
    timer.start(5000);

    spacer.wait(5001);

    auto realNode1= dynamic_cast<mindmap::MindNode*>(itemModel->item(node1->id()));
    auto realNode2= dynamic_cast<mindmap::MindNode*>(itemModel->item(node2->id()));
    auto realLink= dynamic_cast<mindmap::LinkController*>(itemModel->item(link->id()));

    QCOMPARE(qIsNaN(realNode1->position().x()), false);
    QCOMPARE(qIsNaN(realNode1->position().y()), false);
    QCOMPARE(qIsNaN(realNode2->position().x()), false);
    QCOMPARE(qIsNaN(realNode2->position().y()), false);

    if(realLink)
    {
        QVERIFY(realLink->startPoint() != QPointF());
        QVERIFY(realLink->endPoint() != QPointF());
    }
}
void MindMapTest::removeNodeAndLinkTest()
{
    QFETCH(QList<QList<int>>, nodetree);
    QFETCH(int, nodeCount);
    QFETCH(int, linkCount);
    QFETCH(int, indexToRemove);
    QFETCH(int, restingNodes);
    QFETCH(int, restingLinks);

    auto itemModel= dynamic_cast<mindmap::MindItemModel*>(m_ctrl->itemModel());

    auto& nodes= itemModel->items(mindmap::MindItem::NodeType);

    QCOMPARE(itemModel->rowCount(), 0);

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
                    m_ctrl->addNode(QString());
                    if(!nodes.empty())
                    {
                        auto last= nodes[nodes.size() - 1];

                        currentLevelIds << last->id();
                    }
                }
                else
                {
                    // qDebug() << "parentId: "<< c-1 << index;
                    auto parentId= idLevels[idLevels.size() - 1].at(index);
                    m_ctrl->addNode(parentId);
                    auto last= nodes[nodes.size() - 1];
                    currentLevelIds << last->id();
                }
            }
            index++;
        }
        idLevels << currentLevelIds;
        ++d;
    }

    QCOMPARE(itemModel->rowCount(), linkCount + nodeCount);

    m_ctrl->undo();

    QCOMPARE(itemModel->rowCount(), std::max(nodeCount - 1, 0) + std::max(linkCount - 1, 0));

    m_ctrl->redo();

    QCOMPARE(itemModel->rowCount(), nodeCount + linkCount);

    if(indexToRemove < 0)
        return;
    // Remove

    QSignalSpy nodeSpy(itemModel, &mindmap::MindItemModel::rowsAboutToBeRemoved);

    auto selectionCtrl= m_ctrl->selectionController();

    auto last= nodes[indexToRemove];
    selectionCtrl->addToSelection(last);
    m_ctrl->removeSelection();

    QCOMPARE(itemModel->rowCount(), restingNodes + restingLinks);

    QCOMPARE(nodeSpy.count(), nodeCount - restingNodes + linkCount - restingLinks);

    m_ctrl->undo();

    QCOMPARE(itemModel->rowCount(), nodeCount + linkCount);
    nodeSpy.clear();

    m_ctrl->redo();

    QCOMPARE(itemModel->rowCount(), restingNodes + restingLinks);

    QCOMPARE(nodeSpy.count(), nodeCount - restingNodes + linkCount - restingLinks);
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
    QTest::addRow("cmd 3") << QList<QList<int>>({{1}, {1}}) << 2 << 1 << 0 << 1 << 0;
    QTest::addRow("cmd 4") << QList<QList<int>>({{1}, {1}}) << 2 << 1 << 1 << 1 << 0;

    QTest::addRow("cmd 5") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 0 << 0 << 2; // last was 0
    QTest::addRow("cmd 6") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 1 << 2 << 1;
    QTest::addRow("cmd 7") << QList<QList<int>>({{1}, {2}}) << 3 << 2 << 2 << 2 << 1;

    QTest::addRow("cmd 8") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 0 << 0 << 3; // last was 0
    QTest::addRow("cmd 9") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 1 << 1 << 1; // last was 0
    QTest::addRow("cmd 10") << QList<QList<int>>({{1}, {1}, {1}}) << 3 << 2 << 2 << 2 << 1;

    QTest::addRow("cmd 11") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 0 << 0 << 8; // last was 0
    QTest::addRow("cmd 12") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 1 << 3 << 4; // last was 2
    QTest::addRow("cmd 13") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 2 << 4 << 4; // last was 3
    QTest::addRow("cmd 14") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 3 << 5 << 4;
    QTest::addRow("cmd 15") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 4 << 5 << 4;
    QTest::addRow("cmd 16") << QList<QList<int>>({{1}, {2}, {2, 1}}) << 6 << 5 << 5 << 5 << 4;
}

void MindMapTest::linkGeometryTest()
{
    mindmap::LinkNode linkNode;

    linkNode.setColor(Helper::randomColor());
    linkNode.update(Helper::randomRect(), mindmap::LinkController::RightBottom, Helper::randomRect(),
                    Helper::randomRect());
    linkNode.update(Helper::randomRect(), mindmap::LinkController::LeftBottom, Helper::randomRect(),
                    Helper::randomRect());
    linkNode.update(Helper::randomRect(), mindmap::LinkController::RightTop, Helper::randomRect(),
                    Helper::randomRect());
    linkNode.update(Helper::randomRect(), mindmap::LinkController::LeftTop, Helper::randomRect(), Helper::randomRect());
}

void MindMapTest::getAndSetTest()
{
    auto space= m_ctrl->spacingCtrl();
    space->setRunning(false);
    space->setRunning(true);

    auto styleModel= dynamic_cast<mindmap::NodeStyleModel*>(m_ctrl->styleModel());
    new QAbstractItemModelTester(styleModel);

    for(int i= 0; i < styleModel->rowCount(); ++i)
    {
        qDebug() << i;
        QVERIFY(styleModel->getStyle(i) != nullptr);
        QVERIFY(m_ctrl->style(i) != nullptr);
        QCOMPARE(styleModel->getStyle(i), m_ctrl->style(i));
    }

    QCOMPARE(m_ctrl->errorMsg(), QString());
    QCOMPARE(m_ctrl->contentRect(), QRectF(0, 0, 100, 20));

    QSignalSpy spy(m_ctrl.get(), &MindMapController::errorMsgChanged);
    auto msg= Helper::randomString();
    m_ctrl->setErrorMsg(msg);
    m_ctrl->setErrorMsg(msg);

    QCOMPARE(m_ctrl->errorMsg(), msg);
    QCOMPARE(spy.count(), 1);

    QSignalSpy spy2(m_ctrl.get(), &MindMapController::defaultStyleIndexChanged);
    m_ctrl->setDefaultStyleIndex(8000);
    auto indexStyle= Helper::generate<int>(0, styleModel->rowCount());
    m_ctrl->setDefaultStyleIndex(indexStyle);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(indexStyle, m_ctrl->defaultStyleIndex());

    m_ctrl->setSpacing(false);
    QVERIFY(!m_ctrl->spacing());
    m_ctrl->setSpacing(true);
    QVERIFY(m_ctrl->spacing());

    m_ctrl->centerItems(200, 300);
    m_ctrl->refresh();

    auto imgModel= m_ctrl->imgModel();
    new QAbstractItemModelTester(imgModel);

    auto id= Helper::randomString();
    auto path= QUrl::fromLocalFile(Helper::imagePath());
    m_ctrl->openImage(id, path);

    m_ctrl->removeImage(id);
    m_ctrl->removeImage(Helper::randomString());

    m_ctrl->openImage(id, path);
    m_ctrl->removeImageFor(id);

    m_ctrl->removeLink({Helper::randomString()});
    m_ctrl->removeNode({Helper::randomString()});

    m_ctrl->updatePackage(Helper::randomPoint());
    m_ctrl->addPackage(Helper::randomPoint());

    m_ctrl->updatePackage(Helper::randomPoint());
    m_ctrl->updatePackage(Helper::randomPoint());
    m_ctrl->updatePackage(Helper::randomPoint());

    QVERIFY(!m_ctrl->pasteData(QMimeData()));

    m_ctrl->addLink(Helper::randomString(), Helper::randomString());
    m_ctrl->addLink({}, {});

    m_ctrl->setCurrentPackage(nullptr);

    QVERIFY(!m_ctrl->hasSelection());

    m_ctrl->setLinkLabelVisibility(false);
    QSignalSpy spy3(m_ctrl.get(), &MindMapController::linkLabelVisibilityChanged);

    m_ctrl->setLinkLabelVisibility(true);
    m_ctrl->setLinkLabelVisibility(true);
    m_ctrl->setLinkLabelVisibility(false);
    QVERIFY(!m_ctrl->linkLabelVisibility());
    QCOMPARE(spy3.count(), 2);

    m_ctrl->setLinkLabelVisibility(true);
    QVERIFY(m_ctrl->linkLabelVisibility());
    QCOMPARE(spy3.count(), 3);

    m_ctrl->addItemIntoPackage(Helper::randomString(), Helper::randomString());
    m_ctrl->subItem(Helper::randomString(), mindmap::MindItem::LinkType);

    QSignalSpy spyZoom(m_ctrl.get(), &MindMapController::zoomLevelChanged);
    m_ctrl->setZoomLevel(0.2);
    m_ctrl->setZoomLevel(0.2);
    spyZoom.wait(10);
    QCOMPARE(spyZoom.count(), 1);
    QCOMPARE(m_ctrl->zoomLevel(), 0.2);

    m_ctrl->hasNetwork();
}

QTEST_MAIN(MindMapTest);

#include "tst_mindmap.moc"
