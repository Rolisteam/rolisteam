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
#include "mindmap/src/data/link.h"
#include "mindmap/src/data/mindnode.h"
#include "mindmap/src/geometry/linknode.h"
#include "model/boxmodel.h"
#include "model/linkmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"
#include <memory>

#include <QSignalSpy>

class MindMapTest : public QObject
{
    Q_OBJECT
public:
    MindMapTest();

private slots:
    void init();
    void remoteAddTest();

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
    auto spacerCtrl= m_ctrl->spacingController();
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

    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddNode);
    MessageHelper::buildAddNodeMessage(msg, node1);

    NetworkMessageWriter msg1(NetMsg::MindMapCategory, NetMsg::AddNode);
    MessageHelper::buildAddNodeMessage(msg1, node2);

    NetworkMessageWriter msgLink(NetMsg::MindMapCategory, NetMsg::AddLink);
    MessageHelper::buildAddLinkMessage(msgLink, link);

    auto dataMsg= msg.getData();
    NetworkMessageReader readMsg;
    readMsg.setData(dataMsg);
    MessageHelper::readAddMindMapNode(m_ctrl.get(), &readMsg);

    auto dataMsg1= msg1.getData();
    NetworkMessageReader readMsg1;
    readMsg1.setData(dataMsg1);
    MessageHelper::readAddMindMapNode(m_ctrl.get(), &readMsg1);

    auto dataMsgLink= msgLink.getData();
    NetworkMessageReader readMsgLink;
    readMsgLink.setData(dataMsgLink);
    MessageHelper::readMindMapLink(m_ctrl.get(), &readMsgLink);

    QCOMPARE(nodeModel->rowCount(), 2);
    QCOMPARE(linkModel->rowCount(), 1);

    QCOMPARE(nodeAdded.count(), 2);
    QCOMPARE(linkAdded.count(), 1);

    QTimer timer;
    QSignalSpy spacer(&timer, &QTimer::timeout);
    timer.start(5000);

    spacer.wait(5001);

    auto realNode1= nodeModel->node(node1->id());
    auto realNode2= nodeModel->node(node2->id());
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

QTEST_MAIN(MindMapTest);

#include "tst_mindmap.moc"
