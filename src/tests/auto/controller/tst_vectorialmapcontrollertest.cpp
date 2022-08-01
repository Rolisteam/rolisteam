/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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

#include <QColor>
#include <QRectF>
#include <QSignalSpy>
#include <QTest>
#include <QUndoStack>
#include <helper.h>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/textcontroller.h"
#include "controller/item_controllers/vmapitemfactory.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "media/mediafactory.h"
#include "media/mediatype.h"
#include "model/charactermodel.h"
#include "model/playermodel.h"
#include "model/vmapitemmodel.h"
#include "network/networkmessagereader.h"
#include "undoCmd/changesizevmapitem.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/vectorialmapmessagehelper.h"

#include <map>
#include <memory>
#include <vector>

const std::map<QString, QVariant> buildRectController(bool filled, const QRectF& rect,
                                                      const QPointF& pos= QPointF(0, 0))
{
    return {{"filled", filled},
            {"tool", filled ? Core::SelectableTool::FILLRECT : Core::SelectableTool::EMPTYRECT},
            {"rect", rect},
            {"position", pos}};
}

const std::map<QString, QVariant> buildTextController(bool border, const QString& text, const QRectF& rect,
                                                      const QPointF& pos= QPointF(0, 0))
{
    return {{"border", border},
            {"rect", rect},
            {"position", pos},
            {"text", text},
            {"tool", border ? Core::SelectableTool::TEXT : Core::SelectableTool::TEXTBORDER}};
}

const std::map<QString, QVariant> buildEllipseController(bool filled, qreal rx, qreal ry,
                                                         const QPointF& pos= QPointF(0, 0))
{
    return {{"filled", filled},
            {"tool", filled ? Core::SelectableTool::FILLEDELLIPSE : Core::SelectableTool::EMPTYELLIPSE},
            {"rx", rx},
            {"ry", ry},
            {"position", pos}};
}

const std::map<QString, QVariant> buildImageController(const QString& path, const QRectF& rect,
                                                       const QPointF& pos= QPointF(0, 0))
{
    return {{"path", path}, {"rect", rect}, {"tool", Core::SelectableTool::IMAGE}, {"position", pos}};
}

const std::map<QString, QVariant> buildPathController(bool filled, const std::vector<QPointF>& points,
                                                      const QPointF& pos= QPointF(0, 0))
{
    return {{"filled", filled},
            {"tool", Core::SelectableTool::PEN},
            {"points", QVariant::fromValue(points)},
            {"position", pos}};
}
const std::map<QString, QVariant> buildLineController(const QPointF& p1, const QPointF& p2,
                                                      const QPointF& pos= QPointF(0, 0))
{
    return {{"tool", Core::SelectableTool::LINE},
            {"start", QVariant::fromValue(p1)},
            {"end", QVariant::fromValue(p2)},
            {"position", pos}};
}

class VectorialMapControllerTest : public QObject
{
    Q_OBJECT
public:
    VectorialMapControllerTest()= default;

private slots:
    void init();
    void cleanupTestCase();

    void propertyTest();

    void addItemTest();
    void addItemTest_data();

    void normalSize();
    void normalSize_data();

    void serialization();
    void serialization_data();

    // void serialization_sight();
    // void serialization_sight_data();

    void networkMessage();
    void networkMessage_data();

private:
    std::unique_ptr<VectorialMapController> m_ctrl;
    std::unique_ptr<QUndoStack> m_stack;
    std::unique_ptr<Helper::TestMessageSender> m_sender;
};

void VectorialMapControllerTest::init()
{
    qRegisterMetaType<QUndoCommand*>("QUndoCommand*");
    m_ctrl.reset(new VectorialMapController("map"));
    m_stack.reset(new QUndoStack);
    m_sender.reset(new Helper::TestMessageSender);

    NetworkMessage::setMessageSender(m_sender.get());

    connect(m_ctrl.get(), &VectorialMapController::performCommand, m_stack.get(),
            [this](QUndoCommand* cmd) { m_stack->push(cmd); });
}

void VectorialMapControllerTest::cleanupTestCase()
{
    m_stack->clear();
}

void VectorialMapControllerTest::propertyTest()
{
    { // NpcNameVisible
        QCOMPARE(m_ctrl->npcNameVisible(), true);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::npcNameVisibleChanged);

        m_ctrl->setNpcNameVisible(false);

        QCOMPARE(m_ctrl->npcNameVisible(), false);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setNpcNameVisible(false);

        QCOMPARE(m_ctrl->npcNameVisible(), false);
        QCOMPARE(spy.count(), 1);
    }

    { // PcNameVisible
        QCOMPARE(m_ctrl->pcNameVisible(), true);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::pcNameVisibleChanged);

        m_ctrl->setPcNameVisible(false);

        QCOMPARE(m_ctrl->pcNameVisible(), false);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setPcNameVisible(false);

        QCOMPARE(m_ctrl->pcNameVisible(), false);
        QCOMPARE(spy.count(), 1);
    }

    { // npcNumberVisible
        QCOMPARE(m_ctrl->npcNumberVisible(), true);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::npcNumberVisibleChanged);

        m_ctrl->setNpcNumberVisible(false);

        QCOMPARE(m_ctrl->npcNumberVisible(), false);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setNpcNumberVisible(false);

        QCOMPARE(m_ctrl->npcNumberVisible(), false);
        QCOMPARE(spy.count(), 1);
    }

    { // healthBarVisible
        QCOMPARE(m_ctrl->healthBarVisible(), true);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::healthBarVisibleChanged);

        m_ctrl->setHealthBarVisible(false);

        QCOMPARE(m_ctrl->healthBarVisible(), false);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setHealthBarVisible(false);

        QCOMPARE(m_ctrl->healthBarVisible(), false);
        QCOMPARE(spy.count(), 1);
    }

    { // initScoreVisible
        QCOMPARE(m_ctrl->initScoreVisible(), true);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::initScoreVisibleChanged);

        m_ctrl->setInitScoreVisible(false);

        QCOMPARE(m_ctrl->initScoreVisible(), false);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setInitScoreVisible(false);

        QCOMPARE(m_ctrl->initScoreVisible(), false);
        QCOMPARE(spy.count(), 1);
    }

    { // stateLabelVisible
        QCOMPARE(m_ctrl->stateLabelVisible(), false);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::stateLabelVisibleChanged);

        m_ctrl->setStateLabelVisible(true);

        QCOMPARE(m_ctrl->stateLabelVisible(), true);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setStateLabelVisible(true);

        QCOMPARE(m_ctrl->stateLabelVisible(), true);
        QCOMPARE(spy.count(), 1);
    }

    { // collision
        QCOMPARE(m_ctrl->collision(), false);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::collisionChanged);

        m_ctrl->setCollision(true);

        QCOMPARE(m_ctrl->collision(), true);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setCollision(true);

        QCOMPARE(m_ctrl->collision(), true);
        QCOMPARE(spy.count(), 1);
    }

    { // characterVision
        QCOMPARE(m_ctrl->characterVision(), false);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::characterVisionChanged);

        m_ctrl->setCharacterVision(true);

        QCOMPARE(m_ctrl->characterVision(), true);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setCharacterVision(true);

        QCOMPARE(m_ctrl->characterVision(), true);
        QCOMPARE(spy.count(), 1);
    }

    { // gridColor
        QCOMPARE(m_ctrl->gridColor(), QColor(Qt::black));

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::gridColorChanged);

        m_ctrl->setGridColor(QColor(Qt::red));

        QCOMPARE(m_ctrl->gridColor(), QColor(Qt::red));
        QCOMPARE(spy.count(), 1);

        m_ctrl->setGridColor(QColor(Qt::red));

        QCOMPARE(m_ctrl->gridColor(), QColor(Qt::red));
        QCOMPARE(spy.count(), 1);
    }

    { // gridScale
        QCOMPARE(m_ctrl->gridScale(), 5.0);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::gridScaleChanged);

        m_ctrl->setGridScale(100.0);

        QCOMPARE(m_ctrl->gridScale(), 100.0);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setGridScale(100.0);

        QCOMPARE(m_ctrl->gridScale(), 100.0);
        QCOMPARE(spy.count(), 1);
    }

    { // gridSize
        QCOMPARE(m_ctrl->gridSize(), 50);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::gridSizeChanged);

        m_ctrl->setGridSize(100.0);

        QCOMPARE(m_ctrl->gridSize(), 100.0);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setGridSize(100.0);

        QCOMPARE(m_ctrl->gridSize(), 100.0);
        QCOMPARE(spy.count(), 1);
    }

    { // gridVisibility
        QCOMPARE(m_ctrl->gridVisibility(), false);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::gridVisibilityChanged);

        m_ctrl->setGridVisibility(true);

        QCOMPARE(m_ctrl->gridVisibility(), true);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setGridVisibility(true);

        QCOMPARE(m_ctrl->gridVisibility(), true);
        QCOMPARE(spy.count(), 1);
    }

    { // gridAbove
        QCOMPARE(m_ctrl->gridAbove(), false);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::gridAboveChanged);

        m_ctrl->setGridAbove(true);

        QCOMPARE(m_ctrl->gridAbove(), true);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setGridAbove(true);

        QCOMPARE(m_ctrl->gridAbove(), true);
        QCOMPARE(spy.count(), 1);
    }

    { // scaleUnit
        QCOMPARE(m_ctrl->scaleUnit(), Core::M);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::scaleUnitChanged);

        m_ctrl->setScaleUnit(Core::KM);

        QCOMPARE(m_ctrl->scaleUnit(), Core::KM);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setScaleUnit(Core::KM);

        QCOMPARE(m_ctrl->scaleUnit(), Core::KM);
        QCOMPARE(spy.count(), 1);
    }

    { // npcName
        QCOMPARE(m_ctrl->npcName(), "");

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::npcNameChanged);

        m_ctrl->setNpcName("tata");

        QCOMPARE(m_ctrl->npcName(), "tata");
        QCOMPARE(spy.count(), 1);

        m_ctrl->setNpcName("tata");

        QCOMPARE(m_ctrl->npcName(), "tata");
        QCOMPARE(spy.count(), 1);
    }

    { // permission
        QCOMPARE(m_ctrl->permission(), Core::GM_ONLY);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::permissionChanged);

        m_ctrl->setPermission(Core::PC_ALL);

        QCOMPARE(m_ctrl->permission(), Core::PC_ALL);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setPermission(Core::PC_ALL);

        QCOMPARE(m_ctrl->permission(), Core::PC_ALL);
        QCOMPARE(spy.count(), 1);
    }

    { // gridPattern
        QCOMPARE(m_ctrl->gridPattern(), Core::GridPattern::NONE);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::gridPatternChanged);

        m_ctrl->setGridPattern(Core::GridPattern::SQUARE);

        QCOMPARE(m_ctrl->gridPattern(), Core::GridPattern::SQUARE);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setGridPattern(Core::GridPattern::SQUARE);

        QCOMPARE(m_ctrl->gridPattern(), Core::GridPattern::SQUARE);
        QCOMPARE(spy.count(), 1);
    }
    { // visibility
        QCOMPARE(m_ctrl->visibility(), Core::HIDDEN);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::visibilityChanged);

        m_ctrl->setVisibility(Core::ALL);

        QCOMPARE(m_ctrl->visibility(), Core::ALL);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setVisibility(Core::ALL);

        QCOMPARE(m_ctrl->visibility(), Core::ALL);
        QCOMPARE(spy.count(), 1);
    }

    { // backgroundColor
        QCOMPARE(m_ctrl->backgroundColor(), Qt::white);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::backgroundColorChanged);

        m_ctrl->setBackgroundColor(Qt::red);

        QCOMPARE(m_ctrl->backgroundColor(), Qt::red);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setBackgroundColor(Qt::red);

        QCOMPARE(m_ctrl->backgroundColor(), Qt::red);
        QCOMPARE(spy.count(), 1);
    }

    { // toolColor
        QCOMPARE(m_ctrl->toolColor(), Qt::black);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::toolColorChanged);

        m_ctrl->setToolColor(Qt::red);

        QCOMPARE(m_ctrl->toolColor(), Qt::red);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setToolColor(Qt::red);

        QCOMPARE(m_ctrl->toolColor(), Qt::red);
        QCOMPARE(spy.count(), 1);
    }

    { // penSize
        QCOMPARE(m_ctrl->penSize(), 15);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::penSizeChanged);

        m_ctrl->setPenSize(100);

        QCOMPARE(m_ctrl->penSize(), 100);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setPenSize(100);

        QCOMPARE(m_ctrl->penSize(), 100);
        QCOMPARE(spy.count(), 1);
    }

    { // npcNumber
        QCOMPARE(m_ctrl->npcNumber(), 1);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::npcNumberChanged);

        m_ctrl->setNpcNumber(100);

        QCOMPARE(m_ctrl->npcNumber(), 100);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setNpcNumber(100);

        QCOMPARE(m_ctrl->npcNumber(), 100);
        QCOMPARE(spy.count(), 1);
    }

    { // zoomLevel
        QCOMPARE(m_ctrl->zoomLevel(), 1.0);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::zoomLevelChanged);

        m_ctrl->setZoomLevel(4.0);

        QCOMPARE(m_ctrl->zoomLevel(), 4.0);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setZoomLevel(10.0);

        QCOMPARE(m_ctrl->zoomLevel(), 4.0);
        QCOMPARE(spy.count(), 1);
    }

    { // layer
        QCOMPARE(m_ctrl->layer(), Core::Layer::GROUND);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::layerChanged);

        m_ctrl->setLayer(Core::Layer::OBJECT);

        QCOMPARE(m_ctrl->layer(), Core::Layer::OBJECT);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setLayer(Core::Layer::OBJECT);

        QCOMPARE(m_ctrl->layer(), Core::Layer::OBJECT);
        QCOMPARE(spy.count(), 1);
    }

    { // SelectableTool
        QCOMPARE(m_ctrl->tool(), Core::HANDLER);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::toolChanged);

        m_ctrl->setTool(Core::PEN);

        QCOMPARE(m_ctrl->tool(), Core::PEN);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setTool(Core::PEN);

        QCOMPARE(m_ctrl->tool(), Core::PEN);
        QCOMPARE(spy.count(), 1);
    }

    { // editionMode
        QCOMPARE(m_ctrl->editionMode(), Core::EditionMode::Painting);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::editionModeChanged);

        m_ctrl->setEditionMode(Core::EditionMode::Unmask);

        QCOMPARE(m_ctrl->editionMode(), Core::EditionMode::Unmask);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setEditionMode(Core::EditionMode::Unmask);

        QCOMPARE(m_ctrl->editionMode(), Core::EditionMode::Unmask);
        QCOMPARE(spy.count(), 1);
    }

    { // opacity
        QCOMPARE(m_ctrl->opacity(), 1.0);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::opacityChanged);

        m_ctrl->setOpacity(0.4);

        QCOMPARE(m_ctrl->opacity(), 0.4);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setOpacity(0.4);

        QCOMPARE(m_ctrl->opacity(), 0.4);
        QCOMPARE(spy.count(), 1);
    }

    { // visualRect
        QCOMPARE(m_ctrl->visualRect(), QRectF());

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::visualRectChanged);

        m_ctrl->setVisualRect(QRectF(0, 0, 100, 100));

        QCOMPARE(m_ctrl->visualRect(), QRectF(0, 0, 100, 100));
        QCOMPARE(spy.count(), 1);

        m_ctrl->setVisualRect(QRectF(0, 0, 100, 100));

        QCOMPARE(m_ctrl->visualRect(), QRectF(0, 0, 100, 100));
        QCOMPARE(spy.count(), 1);
    }

    { // idle
        QCOMPARE(m_ctrl->idle(), true);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::idleChanged);

        m_ctrl->setIdle(false);

        QCOMPARE(m_ctrl->idle(), false);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setIdle(false);

        QCOMPARE(m_ctrl->idle(), false);
        QCOMPARE(spy.count(), 1);
    }

    { // zIndex
        QCOMPARE(m_ctrl->zIndex(), 0);

        QSignalSpy spy(m_ctrl.get(), &VectorialMapController::zIndexChanged);

        m_ctrl->setZindex(2);

        QCOMPARE(m_ctrl->zIndex(), 2);
        QCOMPARE(spy.count(), 1);

        m_ctrl->setZindex(2);

        QCOMPARE(m_ctrl->zIndex(), 2);
        QCOMPARE(spy.count(), 1);
    }
}

void VectorialMapControllerTest::addItemTest() {}

void VectorialMapControllerTest::addItemTest_data()
{
    // QTest::addColumn("");
}

void VectorialMapControllerTest::normalSize()
{
    QFETCH(QList<vmap::VisualItemController*>, list);
    QFETCH(VectorialMapController::Method, method);
    QFETCH(QPointF, click);
    QFETCH(QRectF, result);
    QFETCH(int, call);
    m_stack.reset(nullptr);

    connect(m_ctrl.get(), &VectorialMapController::performCommand, this, [list, result](QUndoCommand* cmd) {
        QUndoStack stack;
        stack.push(cmd);

        std::for_each(list.begin(), list.end(),
                      [result](vmap::VisualItemController* ctrl) { QCOMPARE(ctrl->rect(), result); });
    });

    QSignalSpy spy(m_ctrl.get(), &VectorialMapController::performCommand);

    m_ctrl->normalizeSize(list, method, click);

    QCOMPARE(spy.count(), call);
    spy.wait(1000);
}

void VectorialMapControllerTest::normalSize_data()
{
    QTest::addColumn<QList<vmap::VisualItemController*>>("list");
    QTest::addColumn<VectorialMapController::Method>("method");
    QTest::addColumn<QPointF>("click");
    QTest::addColumn<QRectF>("result");
    QTest::addColumn<int>("call");

    QTest::addRow("cmd1") << QList<vmap::VisualItemController*>() << VectorialMapController::Bigger << QPointF()
                          << QRectF() << 0;
    QTest::addRow("cmd2") << QList<vmap::VisualItemController*>() << VectorialMapController::Smaller << QPointF()
                          << QRectF() << 0;
    QTest::addRow("cmd3") << QList<vmap::VisualItemController*>() << VectorialMapController::UnderMouse << QPointF()
                          << QRectF() << 0;
    QTest::addRow("cmd4") << QList<vmap::VisualItemController*>() << VectorialMapController::Average << QPointF()
                          << QRectF() << 0;

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd5") << vec << VectorialMapController::Bigger << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd6") << vec << VectorialMapController::Smaller << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd7") << vec << VectorialMapController::UnderMouse << QPointF(0, 0) << QRectF(0, 0, 100, 100)
                              << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd8") << vec << VectorialMapController::Average << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 10, 10))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd9") << vec << VectorialMapController::Bigger << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 10, 10))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd10") << vec << VectorialMapController::Smaller << QPointF() << QRectF(0, 0, 10, 10) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 10, 10))));
        QTest::addRow("cmd11") << vec << VectorialMapController::UnderMouse << QPointF(0, 0) << QRectF(0, 0, 100, 100)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 10, 10))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                            buildRectController(true, QRectF(0, 0, 100, 100))));
        QTest::addRow("cmd12") << vec << VectorialMapController::Average << QPointF() << QRectF(0, 0, 55, 55) << 1;
    }

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd13") << vec << VectorialMapController::Bigger << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd14") << vec << VectorialMapController::Smaller << QPointF() << QRectF(0, 0, 10, 10) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 15, 15), QPointF(0, 0))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd15") << vec << VectorialMapController::UnderMouse << QPointF(10, 10) << QRectF(0, 0, 15, 15)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd16") << vec << VectorialMapController::UnderMouse << QPointF(30, 30) << QRectF(0, 0, 100, 100)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd17") << vec << VectorialMapController::Average << QPointF() << QRectF(0, 0, 55, 55) << 1;
    }

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd18") << vec << VectorialMapController::Bigger << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 1000, 1000), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd19") << vec << VectorialMapController::Smaller << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 300, 300), QPointF(20, 20))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 200, 200), QPointF(0, 0))));
        QTest::addRow("cmd20") << vec << VectorialMapController::UnderMouse << QPointF(30, 30) << QRectF(0, 0, 300, 300)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        QTest::addRow("cmd21") << vec << VectorialMapController::UnderMouse << QPointF(30, 30) << QRectF(0, 0, 100, 100)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(
            vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLRECT,
                                                  buildRectController(true, QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd22") << vec << VectorialMapController::Average << QPointF() << QRectF(0, 0, 55, 55) << 1;
    }

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd23") << vec << VectorialMapController::Bigger << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd24") << vec << VectorialMapController::Smaller << QPointF() << QRectF(0, 0, 10, 10) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd25") << vec << VectorialMapController::UnderMouse << QPointF(0, 0) << QRectF(0, 0, 10, 10)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd26") << vec << VectorialMapController::UnderMouse << QPointF(30, 30) << QRectF(0, 0, 100, 100)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 10, 10), QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::TEXTBORDER,
            buildTextController(true, "hello world", QRectF(0, 0, 100, 100), QPointF(20, 20))));
        QTest::addRow("cmd27") << vec << VectorialMapController::Average << QPointF() << QRectF(0, 0, 55, 55) << 1;
    }

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 10, 10, QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 100, 100, QPointF(0, 0))));
        QTest::addRow("cmd28") << vec << VectorialMapController::Bigger << QPointF() << QRectF(-100, -100, 200, 200)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 10, 10, QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 100, 100, QPointF(20, 20))));
        QTest::addRow("cmd29") << vec << VectorialMapController::Smaller << QPointF() << QRectF(-10, -10, 20, 20) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 10, 10, QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 100, 100, QPointF(20, 20))));
        QTest::addRow("cmd30") << vec << VectorialMapController::UnderMouse << QPointF(0, 0) << QRectF(-10, -10, 20, 20)
                               << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 10, 10, QPointF(0, 0))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 100, 100, QPointF(20, 20))));
        QTest::addRow("cmd31") << vec << VectorialMapController::UnderMouse << QPointF(30, 30)
                               << QRectF(-100, -100, 200, 200) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 10., 10.)));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::FILLEDELLIPSE,
                                                            buildEllipseController(true, 100, 100, QPointF(0, 0))));
        QTest::addRow("cmd32") << vec << VectorialMapController::Average << QPointF() << QRectF(-55, -55, 110, 110)
                               << 1;
    }

    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/girafe3.jpg", QRectF(0, 0, 100, 100), QPointF(2000, 2000))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/lion3.jpg", QRectF(0, 0, 700, 700), QPointF(300, 300))));
        QTest::addRow("cmd33") << vec << VectorialMapController::Bigger << QPointF() << QRectF(0, 0, 700, 700) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/girafe3.jpg", QRectF(0, 0, 100, 100), QPointF(2000, 2000))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/lion3.jpg", QRectF(0, 0, 700, 700), QPointF(300, 300))));
        QTest::addRow("cmd34") << vec << VectorialMapController::Smaller << QPointF() << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/girafe3.jpg", QRectF(0, 0, 100, 100), QPointF(2000, 2000))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/lion3.jpg", QRectF(0, 0, 700, 700), QPointF(300, 300))));
        QTest::addRow("cmd35") << vec << VectorialMapController::UnderMouse << QPointF(2010, 2010)
                               << QRectF(0, 0, 100, 100) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/girafe3.jpg", QRectF(0, 0, 100, 100), QPointF(2000, 2000))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/lion3.jpg", QRectF(0, 0, 700, 700), QPointF(300, 300))));
        QTest::addRow("cmd36") << vec << VectorialMapController::UnderMouse << QPointF(310, 310)
                               << QRectF(0, 0, 700, 700) << 1;
    }
    {
        QList<vmap::VisualItemController*> vec;
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/girafe3.jpg", QRectF(0, 0, 100, 100), QPointF(2000, 2000))));
        vec.push_back(vmap::VmapItemFactory::createVMapItem(
            m_ctrl.get(), Core::SelectableTool::IMAGE,
            buildImageController(":/img/lion3.jpg", QRectF(0, 0, 700, 700), QPointF(300, 300))));
        QTest::addRow("cmd37") << vec << VectorialMapController::Average << QPointF() << QRectF(0, 0, 400, 400) << 1;
    }
}

void VectorialMapControllerTest::serialization()
{
    using CustomMap= std::map<QString, QVariant>;
    QFETCH(QList<CustomMap>, list);

    for(const auto& item : list)
    {
        m_ctrl->insertItemAt(item);
    }

    auto byteArray= IOHelper::saveController(m_ctrl.get());

    VectorialMapController* ctrl2= new VectorialMapController();

    VectorialMapMessageHelper::readVectorialMapController(ctrl2, byteArray);

    QCOMPARE(ctrl2->model()->rowCount(), m_ctrl->model()->rowCount());
}
void VectorialMapControllerTest::serialization_data()
{
    using CustomMap= std::map<QString, QVariant>;
    QTest::addColumn<QList<CustomMap>>("list");

    std::vector<Core::SelectableTool> data(
        {Core::SelectableTool::FILLRECT, Core::SelectableTool::LINE, Core::SelectableTool::EMPTYELLIPSE,
         Core::SelectableTool::EMPTYRECT, Core::SelectableTool::FILLEDELLIPSE, Core::SelectableTool::IMAGE,
         Core::SelectableTool::TEXT, Core::SelectableTool::TEXTBORDER, Core::SelectableTool::PATH});
    /// TODO add:    Core::SelectableTool::PlayableCharacter, Core::SelectableTool::NonPlayableCharacter

    // auto list = new std::vector<CleverURI*>();
    QList<CustomMap> list;

    int index= 0;
    for(unsigned int i= 0; i < data.size(); ++i)
    {
        auto comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                CustomMap map;
                switch(*it)
                {
                case Core::SelectableTool::FILLRECT:
                    map= buildRectController(true, {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::LINE:
                    map= buildLineController({100, 100}, {500, 100}, {});
                    break;
                case Core::SelectableTool::EMPTYELLIPSE:
                    map= buildEllipseController(false, 200., 100., {500., 100.});
                    break;
                case Core::SelectableTool::EMPTYRECT:
                    map= buildRectController(false, {0, 0, 200, 200}, {300, 200});
                    break;
                case Core::SelectableTool::FILLEDELLIPSE:
                    map= buildEllipseController(true, 200., 100., {500., 100.});
                    break;
                case Core::SelectableTool::IMAGE:
                    map= buildImageController(":/img/girafe.jpg", {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::TEXT:
                    map= buildTextController(false, "Text without border", {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::TEXTBORDER:
                    map= buildTextController(true, "Text with border", {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::PATH:
                    map= buildPathController(true, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
                    break;
                default:
                    break;
                }
                list.append(map);
            }
            QTest::addRow("save %d", ++index) << list;
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}

/*void VectorialMapControllerTest::serialization_sight()
{
    QFETCH(QList<QPolygonF>, list);
    QFETCH(QList<bool>, masks);

    auto sightCtrl= new vmap::SightController(m_ctrl.get());

    int i= 0;
    for(auto const &poly, list)
    {
        auto b= masks[i];
        sightCtrl->addPolygon(poly, b);
        ++i;
    }

    auto byteArray= IOHelper::saveController(m_ctrl.get());

    VectorialMapMessageHelper::readVectorialMapController(ctrl2, byteArray);

    QCOMPARE(ctrl2->model()->rowCount(), m_ctrl->model()->rowCount());
}

void VectorialMapControllerTest::serialization_sight_data()
{
    QFETCH(QList<QPolygonF>, list);
    QFETCH(QList<bool>, masks);

    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
    QTest::addRow("cmd1") << QList<QPolygonF>{} << QList<bool>{};
}*/

void VectorialMapControllerTest::networkMessage()
{
    using CustomMap= std::map<QString, QVariant>;
    QFETCH(QList<CustomMap>, list);

    for(const auto& item : list)
    {
        m_ctrl->insertItemAt(item);
    }
    m_ctrl->setIdle(true);

    MessageHelper::sendOffVMap(m_ctrl.get());
    auto byteArray= m_sender->messageData();

    NetworkMessageReader msg;
    msg.setData(byteArray);

    auto type= static_cast<Core::ContentType>(msg.uint8());
    auto mediabase= Media::MediaFactory::createRemoteMedia(type, &msg, false);
    auto ctrl= dynamic_cast<VectorialMapController*>(mediabase);

    QCOMPARE(ctrl->scaleUnit(), m_ctrl->scaleUnit());
    QCOMPARE(ctrl->collision(), m_ctrl->collision());
    QCOMPARE(ctrl->gridColor(), m_ctrl->gridColor());
    QCOMPARE(ctrl->gridScale(), m_ctrl->gridScale());
    QCOMPARE(ctrl->gridSize(), m_ctrl->gridSize());
    QCOMPARE(ctrl->gridVisibility(), m_ctrl->gridVisibility());
    QCOMPARE(ctrl->gridAbove(), m_ctrl->gridAbove());
    QCOMPARE(ctrl->permission(), m_ctrl->permission());
    QCOMPARE(ctrl->gridPattern(), m_ctrl->gridPattern());
    QCOMPARE(ctrl->visibility(), m_ctrl->visibility());
    QCOMPARE(ctrl->backgroundColor(), m_ctrl->backgroundColor());
    QCOMPARE(ctrl->penSize(), m_ctrl->penSize());
    QCOMPARE(ctrl->npcNumber(), m_ctrl->npcNumber());
    QCOMPARE(ctrl->layer(), m_ctrl->layer());
    QCOMPARE(ctrl->opacity(), m_ctrl->opacity());
    QCOMPARE(ctrl->idle(), m_ctrl->idle());
    QCOMPARE(ctrl->zIndex(), m_ctrl->zIndex());
    QCOMPARE(ctrl->model()->rowCount(), m_ctrl->model()->rowCount());

    delete mediabase;
}
void VectorialMapControllerTest::networkMessage_data()
{
    using CustomMap= std::map<QString, QVariant>;
    QTest::addColumn<QList<CustomMap>>("list");

    std::vector<Core::SelectableTool> data(
        {Core::SelectableTool::FILLRECT, Core::SelectableTool::LINE, Core::SelectableTool::EMPTYELLIPSE,
         Core::SelectableTool::EMPTYRECT, Core::SelectableTool::FILLEDELLIPSE, Core::SelectableTool::IMAGE,
         Core::SelectableTool::TEXT, Core::SelectableTool::TEXTBORDER, Core::SelectableTool::PATH});
    /// TODO add:    Core::SelectableTool::PlayableCharacter, Core::SelectableTool::NonPlayableCharacter

    // auto list = new std::vector<CleverURI*>();
    QList<CustomMap> list;

    int index= 0;
    for(unsigned int i= 0; i < data.size(); ++i)
    {
        auto comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                CustomMap map;
                switch(*it)
                {
                case Core::SelectableTool::FILLRECT:
                    map= buildRectController(true, {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::LINE:
                    map= buildLineController({100, 100}, {500, 100}, {});
                    break;
                case Core::SelectableTool::EMPTYELLIPSE:
                    map= buildEllipseController(false, 200., 100., {500., 100.});
                    break;
                case Core::SelectableTool::EMPTYRECT:
                    map= buildRectController(false, {0, 0, 200, 200}, {300, 200});
                    break;
                case Core::SelectableTool::FILLEDELLIPSE:
                    map= buildEllipseController(true, 200., 100., {500., 100.});
                    break;
                case Core::SelectableTool::IMAGE:
                    map= buildImageController(":/img/girafe.jpg", {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::TEXT:
                    map= buildTextController(false, "Text without border", {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::TEXTBORDER:
                    map= buildTextController(true, "Text with border", {0, 0, 200, 200});
                    break;
                case Core::SelectableTool::PATH:
                    map= buildPathController(true, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
                    break;
                default:
                    break;
                }
                list.append(map);
            }
            QTest::addRow("save %d", ++index) << list;
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}

QTEST_MAIN(VectorialMapControllerTest);

#include "tst_vectorialmapcontrollertest.moc"
