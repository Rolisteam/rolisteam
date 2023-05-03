#include <QObject>
#include <QtCore/QCoreApplication>
#include <QtTest/QtTest>

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "rwidgets/customs/vmap.h"
#include "model/vmapitemmodel.h"
#include "rwidgets/graphicsItems/lineitem.h"
#include "controller/item_controllers/linecontroller.h"
#include "rwidgets/mediacontainers/vmapframe.h"
#include "undoCmd/addvmapitem.h"
#include "controller/item_controllers/vmapitemfactory.h"
// #include "test_root_path.h"
#include "helper.h"

constexpr int shortTime{5};
using ParamMap= QList<std::map<QString, QVariant>>;


class FakeItem : public vmap::VisualItemController
{
public:
    FakeItem(VisualItemController::ItemType itemType,VectorialMapController* ctrl) : VisualItemController(itemType, {}, ctrl) {};
    void aboutToBeRemoved() {};
    void setCorner(const QPointF& move, int corner,
        Core::TransformType transformType= Core::TransformType::NoTransform)
        {};
    QRectF rect() const {return {};};
};


class VMapTest : public QObject
{
    Q_OBJECT
public:
    VMapTest();

private slots:
    void init();
    void cleanup();

    void addItems();
    void addItems_data();

    void addNullItems();

private:
    std::unique_ptr<VectorialMapController> m_ctrl;
    std::unique_ptr<VMapFrame> m_media;
    std::unique_ptr<QUndoStack> m_stack;
    QPointer<VectorialMapController> m_pctrl;
    QPointer<VMapFrame> m_pmedia;
};

VMapTest::VMapTest()
{
    std::string duration("3600000"); // 3600 seconds -> 60 min
    QByteArray timeoutDuration(duration.c_str(), static_cast<int>(duration.length()));
    qputenv("QTEST_FUNCTION_TIMEOUT", timeoutDuration);
    { // just for checking ..
        auto result= qgetenv("QTEST_FUNCTION_TIMEOUT");
        qDebug() << "timeout set to:" << result << "ms";
    }
}

void VMapTest::init()
{
    if(m_pmedia)
    {
        delete m_media.release();
    }

    if(m_pctrl)
    {
        delete m_ctrl.release();
    }

    m_ctrl.reset(new VectorialMapController());
    m_pctrl= m_ctrl.get();
    m_media.reset(new VMapFrame(m_ctrl.get()));
    m_pmedia= m_media.get();
    m_stack.reset(new QUndoStack);

    connect(m_ctrl.get(), &VectorialMapController::performCommand, this,
            [this](QUndoCommand* cmd) { m_stack->push(cmd); });

    connect(m_ctrl.get(), &VectorialMapController::destroyed, this, []() { qDebug() << "ctrl destroyed"; });
    connect(m_media.get(), &VMapFrame::destroyed, this, []() { qDebug() << "VMapFrame destroyed"; });
}
void VMapTest::cleanup() {}

void VMapTest::addNullItems()
{
    auto map= m_media->map();
    auto model = m_ctrl->model();

    model->appendItemController(nullptr);

    QList<vmap::VisualItemController::ItemType> types{vmap::VisualItemController::PATH,
                                                      vmap::VisualItemController::LINE,
                                                      vmap::VisualItemController::ELLIPSE,
                                                      vmap::VisualItemController::CHARACTER,
                                                      vmap::VisualItemController::TEXT,
                                                      vmap::VisualItemController::RECT,
                                                      vmap::VisualItemController::RULE,
                                                      vmap::VisualItemController::IMAGE,
                                                      vmap::VisualItemController::SIGHT,
                                                      vmap::VisualItemController::ANCHOR,
                                                      vmap::VisualItemController::GRID,
                                                      vmap::VisualItemController::HIGHLIGHTER};

    for(auto type : types)
        model->appendItemController(new FakeItem(type, m_ctrl.get()));

    auto line= vmap::VmapItemFactory::createVMapItem(m_ctrl.get(), Core::SelectableTool::LINE, Helper::buildLineController(Helper::randomPoint(), Helper::randomPoint(), Helper::randomPoint()));
    model->appendItemController(line);

    m_ctrl->showTransparentItems();
    line->setOpacity(0);
}

/*void VMapTest::gridTest()
{
    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    auto map= m_media->map();
    auto gridCtrl = m_ctrl->gridController();
    auto gridItem= map->gridItem();

    gridItem->setNewEnd(QPoint());

    QVERIFY(!gridItem->isVisible());

    QSignalSpy spyGrid(m_ctrl.get(), &VectorialMapController::gridVisibilityChanged);

    m_ctrl->setGridPattern(Core::GridPattern::SQUARE);
    m_ctrl->setGridAbove(true);
    m_ctrl->setGridVisibility(true);
    spyGrid.wait(10);

    QCOMPARE(spyGrid.count(), 1);

    QSignalSpy gridResize(gridItem, &GridItem::parentChanged);
    QSignalSpy spyGridResize(gridCtrl, &vmap::GridController::rectChanged);

    auto size= m_media->size();
    m_media->resize(size * 1.5);

    spyGridResize.wait(10);
    spyGridResize.clear();
    gridCtrl->setRect(m_media->geometry());

    spyGridResize.wait(10);
    QCOMPARE(spyGridResize.count(), 1);
}

void VMapTest::lineTest()
{
    {
        LineItem item(nullptr);
    }
    vmap::LineController ctrl({}, m_ctrl.get());
    LineItem item(&ctrl);

    item.setNewEnd(QPointF{200., 300.});

    ctrl.setStartPoint(QPointF{20., 30.});
}

void VMapTest::ellispeTest()
{
    {
        EllipsItem item(nullptr);
        item.shape();
    }
    {
        vmap::EllipseController ctrl({}, m_ctrl.get());
        EllipsItem item(&ctrl);
        item.shape();
    }
}

void VMapTest::toolBoxTest()
{
    auto toolbox= m_media->toolBox();

    QSignalSpy spy(m_ctrl.get(), &VectorialMapController::permissionChanged);
    QSignalSpy spy2(m_ctrl.get(), &VectorialMapController::editionModeChanged);
    QSignalSpy spy3(m_ctrl.get(), &VectorialMapController::visibilityChanged);

    m_ctrl->setPermission(Core::PermissionMode::PC_ALL);
    spy.wait(shortTime);
    m_ctrl->setEditionMode(Core::EditionMode::Mask);
    spy2.wait(shortTime);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy2.count(), 1);

    m_ctrl->setPermission(Core::PermissionMode::PC_ALL);
    spy.wait(shortTime);
    m_ctrl->setEditionMode(Core::EditionMode::Mask);
    spy2.wait(shortTime);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy2.count(), 1);

    m_ctrl->setVisibility(Core::VisibilityMode::FOGOFWAR);
    spy3.wait(shortTime);
    m_ctrl->setVisibility(Core::VisibilityMode::NONE);
    spy3.wait(shortTime);
    m_ctrl->setVisibility(Core::VisibilityMode::ALL);
    spy3.wait(shortTime);
    m_ctrl->setVisibility(Core::VisibilityMode::ALL);
    spy3.wait(shortTime);

    QCOMPARE(spy3.count(), 3);

    auto acts= toolbox->actions();

    for(auto act : acts)
    {
        if(!act->objectName().isEmpty())
            continue;

        auto tool= act->data().value<Core::SelectableTool>();
        QSignalSpy toolSpy(m_ctrl.get(), &VectorialMapController::toolChanged);
        QSignalSpy actSpy(act, &QAction::triggered);
        act->trigger();
        toolSpy.wait(shortTime);
        if(m_ctrl->tool() != tool)
            qDebug() << tool << "action:" << act;

        QCOMPARE(toolSpy.count(), 1);
        QCOMPARE(actSpy.count(), 1);
        QCOMPARE(m_ctrl->tool(), tool);

        act->trigger();
        toolSpy.wait(10);
        QCOMPARE(toolSpy.count(), 1);
        QCOMPARE(actSpy.count(), 2);
        QCOMPARE(m_ctrl->tool(), tool);
    }
    QSignalSpy spy4(m_ctrl.get(), &VectorialMapController::zoomLevelChanged);

    m_ctrl->setZoomLevel(1.1);
    spy4.wait(shortTime);
    m_ctrl->setZoomLevel(1.1);
    spy4.wait(shortTime);
    m_ctrl->setZoomLevel(1.0);
    spy4.wait(shortTime);

    QCOMPARE(spy4.count(), 2);
}
void VMapTest::topBarTest()
{
    auto topBar= m_media->topBar();
    QSignalSpy spyGM(m_ctrl.get(), &VectorialMapController::localGMChanged);

    m_ctrl->setLocalGM(!m_ctrl->localGM());
    spyGM.wait(shortTime);
    QCOMPARE(spyGM.count(), 1);
    m_ctrl->setLocalGM(m_ctrl->localGM());
    spyGM.wait(shortTime);
    QCOMPARE(spyGM.count(), 1);

    auto acts= topBar->actions();

    QSignalSpy spyPerm(m_ctrl.get(), &VectorialMapController::permissionChanged);
    QSignalSpy spyVisibility(m_ctrl.get(), &VectorialMapController::visibilityChanged);
    QSignalSpy spyLayer(m_ctrl.get(), &VectorialMapController::layerChanged);

    for(auto const& p : acts)
    {
        auto name= p->objectName();

        if(name.startsWith("permission_") || name.startsWith("Visibility_") || name.startsWith("layer_"))
            p->trigger();
    }

    spyPerm.wait(shortTime);
    spyVisibility.wait(shortTime);
    spyLayer.wait(shortTime);
    QVERIFY(spyPerm.count() > 0);
    QVERIFY(spyVisibility.count() > 0);
    QVERIFY(spyLayer.count() > 0);
}

void VMapTest::createEmptyTopBarTest()
{
    auto topBar= std::make_unique<VmapTopBar>(nullptr);

    topBar->updateUI();
}
void VMapTest::createEmptyToolBoxTest()
{
    auto toolBox= std::make_unique<ToolBox>(nullptr);
}


void VMapTest::rulerTest()
{
    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    QSignalSpy gridSizeChanged(m_ctrl.get(), &VectorialMapController::gridSizeChanged);
    QSignalSpy gridScaleChanged(m_ctrl.get(), &VectorialMapController::gridScaleChanged);
    QSignalSpy scaleUnitChanged(m_ctrl.get(), &VectorialMapController::scaleUnitChanged);

    auto rule= new RuleItem(m_ctrl.get());
    auto map= m_media->map();
    map->addItem(rule);

    rule->setNewEnd(QPointF(), true);
    rule->setNewEnd(QPointF(), false);

    rule->setPos(QPointF(100., 100.));
    rule->setNewEnd(QPointF(150., 250.), true);

    QCOMPARE(rule->boundingRect(), QRectF(0., 0., 0., 250.));

    rule->setNewEnd(QPointF(1500., 250.), false);

    QCOMPARE(rule->boundingRect(), QRect(0, 0, 1500, 500));

    // addAndInit(parentItemAnchor);
    auto items= map->items();

    QCOMPARE(items.count(), 3);

    QList<Core::ScaleUnit> units{Core::ScaleUnit::CM, Core::ScaleUnit::FEET, Core::ScaleUnit::INCH,
                                 Core::ScaleUnit::KM, Core::ScaleUnit::M,    Core::ScaleUnit::MILE,
                                 Core::ScaleUnit::PX, Core::ScaleUnit::YARD};

    //    Core::ScaleUnit::CM ,m_ctrl->scaleUnitChanged(Core::ScaleUnit::CM
    int i= 0;
    for(auto const& unit : units)
    {
        if(m_ctrl->scaleUnit() == unit)
            continue;

        m_ctrl->setScaleUnit(unit);
        i++;
        scaleUnitChanged.wait(10);
    }
    QCOMPARE(scaleUnitChanged.count(), i);
}
void VMapTest::anchorTest()
{
    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    auto anchor= new AnchorItem();
    anchor->setNewEnd(QPointF());

    anchor->setPos(QPointF(100., 100.));
    anchor->setNewEnd(QPointF(200., 250.));

    auto map= m_media->map();
    map->addItem(anchor);
    anchor->setVisible(true);

    // addAndInit(parentItemAnchor);

    anchor->boundingRect();

    QCOMPARE(anchor->getEnd(), QPointF(300., 350.));
    QCOMPARE(anchor->getStart(), QPointF(100., 100.));

    auto items= map->items();

    QCOMPARE(items.count(), 3);
}*/

void VMapTest::addItems()
{
    QFETCH(ParamMap, params);
    QFETCH(int, expected);

    m_ctrl->setVisibility(Core::ALL);
    m_ctrl->setLocalGM(true);
    m_ctrl->setPermission(Core::PermissionMode::GM_ONLY);
    m_media->setVisible(true);

    QSignalSpy performAction(m_ctrl.get(), &VectorialMapController::performCommand);
    QSignalSpy itemCreated(m_ctrl.get(), &VectorialMapController::visualItemControllerCreated);

    auto map= m_media->map();

    for(auto const& p : params)
    {
        performAction.clear();
        itemCreated.clear();
        m_ctrl->insertItemAt(p);
        performAction.wait(shortTime);
        QCOMPARE(performAction.count(), 1);

        itemCreated.wait(shortTime);
        QCOMPARE(itemCreated.count(), 1);

        auto list= map->items();
        QList<VisualItem*> items;
        std::transform(std::begin(list), std::end(list), std::back_inserter(items),
                       [](QGraphicsItem* item) -> VisualItem*
                       {
                           auto vitem= dynamic_cast<VisualItem*>(item);
                           if(!vitem)
                               return nullptr;
                           else
                           {
                               auto ctrl= vitem->controller();
                               if(ctrl->itemType() == vmap::VisualItemController::GRID
                                  || ctrl->itemType() == vmap::VisualItemController::SIGHT)
                               {
                                   return nullptr;
                               }
                               else
                                   return vitem;
                           }
                       });

        items.removeAll(nullptr);

        if(items.empty())
            continue;

        auto vitem= items.first();
        if(!vitem)
            continue;

        // vitem->setPenWidth(Helper::generate<int>(1, 50));

        vitem->getType();
        {
            auto c= Helper::randomColor();
            vitem->setColor(c);
            QCOMPARE(vitem->color(), c);
        }
        {
            auto c= Helper::randomColor();
            vitem->setHighlightColor(c);
            QCOMPARE(vitem->getHighlightColor(), c);
        }

        {
            auto c= Helper::generate<int>(0, 50);
            vitem->setHighlightWidth(c);
            QCOMPARE(vitem->getHighlightWidth(), c);
        }

        vitem->shape();

        QVERIFY(vitem->isLocal());

        QCOMPARE(vitem->promoteTo(vmap::VisualItemController::GRID), nullptr);

        vitem->setNewEnd(Helper::randomPoint());

        vitem->shape();

        auto ctrl= vitem->controller();
        if(!ctrl)
            continue;
        QSignalSpy isSelected(ctrl, &vmap::VisualItemController::selectedChanged);

        isSelected.clear();

        ctrl->setSelected(!ctrl->selected());
        ctrl->setSelected(ctrl->selected());

        isSelected.wait(shortTime);

        QCOMPARE(isSelected.count(), 1);

        QSignalSpy canUndo(m_stack.get(), &QUndoStack::canUndoChanged);
        m_ctrl->dupplicateItem(QList<vmap::VisualItemController*>{ctrl});

        canUndo.wait(shortTime);
        QCOMPARE(canUndo.count(), 1);
        m_stack->undo();
    }

    if(!params.isEmpty())
        qDebug() << params[0]["tool"].value<Core::SelectableTool>();

    QCOMPARE(map->items().count(), expected);
}

void VMapTest::addItems_data()
{
    QTest::addColumn<ParamMap>("params");
    QTest::addColumn<int>("expected");

    QTest::addRow("empty") << ParamMap{} << 2;
    std::vector<Core::SelectableTool> data({Core::SelectableTool::FILLRECT, Core::SelectableTool::LINE,
                                            Core::SelectableTool::EMPTYELLIPSE, Core::SelectableTool::EMPTYRECT,
                                            Core::SelectableTool::FILLEDELLIPSE, Core::SelectableTool::IMAGE,
                                            Core::SelectableTool::TEXT, Core::SelectableTool::TEXTBORDER,
                                            Core::SelectableTool::PEN, Core::SelectableTool::PATH, Core::SelectableTool::NonPlayableCharacter});
    ParamMap list;
    int index= 0;
    for(unsigned int i= 0; i < data.size(); ++i)
    {
        auto comb_size= i + 1;
        do
        {
            int count= 2;
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            // for(auto val : data)
            {
                std::map<QString, QVariant> map;
                switch(*it) //
                {
                case Core::SelectableTool::FILLRECT:
                    map= Helper::buildRectController(true, {0, 0, 200, 200});
                    count+= 5;
                    break;
                case Core::SelectableTool::LINE:
                    map= Helper::buildLineController({100, 100}, {500, 100}, {});
                    count+= 3;
                    break;
                case Core::SelectableTool::EMPTYELLIPSE:
                    map= Helper::buildEllipseController(false, 200., 100., {500., 100.});
                    count+= 3;
                    break;
                case Core::SelectableTool::EMPTYRECT:
                    map= Helper::buildRectController(false, {0, 0, 200, 200}, {300, 200});
                    count+= 5;
                    break;
                case Core::SelectableTool::FILLEDELLIPSE:
                    map= Helper::buildEllipseController(true, 200., 100., {500., 100.});
                    count+= 3;
                    break;
                case Core::SelectableTool::IMAGE:
                    map= Helper::buildImageController(":/img/girafe.jpg", {0, 0, 200, 200}, {150, 200});
                    count+= 5;
                    break;
                case Core::SelectableTool::TEXT:
                    map= Helper::buildTextController(false, "Text without border", {0, 0, 200, 200});
                    count+= 6;
                    break;
                case Core::SelectableTool::TEXTBORDER:
                    map= Helper::buildTextController(true, "Text with border", {0, 0, 200, 200});
                    count+= 6;
                    break;
                case Core::SelectableTool::PATH:
                    map= Helper::buildPathController(true, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
                    count+= 5;
                    break;
                case Core::SelectableTool::PEN:
                    map= Helper::buildPenController(false, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
                    count+= 1;
                    break;
                case Core::SelectableTool::NonPlayableCharacter:
                    map= Helper::buildPenController(false, {{0, 0}, {10, 10}, {20, 0}, {30, 10}}, {0, 0});
                    count+= 1;
                    break;
                default:
                    break;
                }
                map.insert({QString("color"), QVariant::fromValue(Helper::randomColor())});
                map.insert({Core::vmapkeys::KEY_PENWIDTH, Helper::generate<int>(1, 50)});

                list.append(map);
            }
            QTest::addRow("save %d", ++index) << list << count;
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}

/*void VMapTest::getAndSetTest()
{
       ToolBox::EditionMode mode= Core::Painting;
       m_vmap->setEditionMode(mode);
       QVERIFY2(m_vmap->getEditionMode() == mode, "Normal edition mode");

       mode= ToolBox::Mask;
       m_vmap->setEditionMode(mode);
       QVERIFY2(m_vmap->getEditionMode() == mode, "Mask edition mode");

       int height= 800;
       m_vmap->setHeight(height);
       QVERIFY(m_vmap->mapHeight() == height);

       int width= 1200;
       m_vmap->setWidth(width);
       QVERIFY(m_vmap->mapWidth() == width);

       QColor color(Qt::red);
       m_vmap->setBackGroundColor(color);
       QVERIFY(m_vmap->getBackGroundColor() == color);

       QString title("Test");
       m_vmap->setTitle(title);
       QVERIFY(m_vmap->getMapTitle() == title);

       Map::PermissionMode permission= Map::PC_ALL;
       m_vmap->setPermissionMode(permission);
       QVERIFY(m_vmap->getPermissionMode() == permission);
       m_vmap->setPermissionMode(Map::GM_ONLY);
       QVERIFY(m_vmap->getPermissionModeText() == "No Right");
       m_vmap->setPermissionMode(Map::PC_MOVE);
       QVERIFY(m_vmap->getPermissionModeText() == "His character");
       m_vmap->setPermissionMode(Map::PC_ALL);
       QVERIFY(m_vmap->getPermissionModeText() == "All Permissions");

       VMap::VisibilityMode visiMode= VMap::HIDDEN;
       m_vmap->setVisibilityMode(visiMode);
       QVERIFY(m_vmap->getVisibilityModeText() == "Hidden");
       visiMode= VMap::FOGOFWAR;
       m_vmap->setVisibilityMode(visiMode);
       QVERIFY(m_vmap->getVisibilityModeText() == "Fog Of War");
       visiMode= VMap::ALL;
       m_vmap->setVisibilityMode(visiMode);
       QVERIFY(m_vmap->getVisibilityModeText() == "All visible");
}

void VMapTest::testOption()
{
      QFETCH(VisualItem::Properties, key);
       QFETCH(QVariant, value);

       VisualItem::Properties pop= static_cast<VisualItem::Properties>(key);

       m_vmap->setOption(pop, value);
       QVERIFY(m_vmap->getOption(pop) == value);

       QFETCH(QVariant, value1);
       bool b= m_vmap->setOption(pop, value1);
       QVERIFY(b);
       QVERIFY(m_vmap->getOption(pop) == value1);
}
void VMapTest::testOption_data()
{
      QTest::addColumn<VisualItem::Properties>("key");
      QTest::addColumn<QVariant>("value");
      QTest::addColumn<QVariant>("value1");

      QTest::newRow("ShowNpcName") << VisualItem::ShowNpcName << QVariant(true) << QVariant::fromValue(false);
      QTest::newRow("ShowPcName") << VisualItem::ShowPcName << QVariant::fromValue(true) << QVariant::fromValue(false);
      QTest::newRow("ShowNpcNumber") << VisualItem::ShowNpcNumber << QVariant::fromValue(true)
                                     << QVariant::fromValue(false);
      QTest::newRow("ShowHealthStatus") << VisualItem::ShowHealthStatus << QVariant::fromValue(true)
                                        << QVariant::fromValue(false);
      QTest::newRow("ShowInitScore") << VisualItem::ShowInitScore << QVariant::fromValue(true)
                                     << QVariant::fromValue(false);
      QTest::newRow("ShowGrid") << VisualItem::ShowGrid << QVariant::fromValue(true) << QVariant::fromValue(false);
      QTest::newRow("LocalIsGM") << VisualItem::LocalIsGM << QVariant::fromValue(true) << QVariant::fromValue(false);
      QTest::newRow("GridPattern") << VisualItem::GridPattern << QVariant::fromValue(VMap::SQUARE)
                                   << QVariant::fromValue(VMap::NONE);
      QTest::newRow("GridColor") << VisualItem::GridColor << QVariant::fromValue(QColor(Qt::blue))
                                 << QVariant::fromValue(QColor(Qt::yellow));
      QTest::newRow("GridSize") << VisualItem::GridSize << QVariant::fromValue(50) << QVariant::fromValue(100);
      QTest::newRow("Scale") << VisualItem::Scale << QVariant::fromValue(1.0) << QVariant::fromValue(0.5);
      QTest::newRow("Unit") << VisualItem::Unit << QVariant::fromValue(VMap::M) << QVariant::fromValue(VMap::KM);
      QTest::newRow("CharacterVision") << VisualItem::EnableCharacterVision << QVariant::fromValue(true)
                                       << QVariant::fromValue(false);
      QTest::newRow("PermissionMode") << VisualItem::PermissionMode << QVariant::fromValue(Map::PC_ALL)
                                      << QVariant::fromValue(Map::GM_ONLY);
      QTest::newRow("FogOfWarStatus") << VisualItem::FogOfWarStatus << QVariant::fromValue(true)
                                      << QVariant::fromValue(false);
      QTest::newRow("CollisionStatus") << VisualItem::CollisionStatus << QVariant::fromValue(true)
                                       << QVariant::fromValue(false);
      QTest::newRow("GridAbove") << VisualItem::GridAbove << QVariant::fromValue(true) << QVariant::fromValue(false);
      QTest::newRow("HideOtherLayers") << VisualItem::HideOtherLayers << QVariant::fromValue(true)
                                       << QVariant::fromValue(false);
      QTest::newRow("VisibilityMode") << VisualItem::VisibilityMode << QVariant::fromValue(VMap::HIDDEN)
                                      << QVariant::fromValue(VMap::ALL);
      QTest::newRow("ShowHealthBar") << VisualItem::ShowHealthBar << QVariant::fromValue(true)
                                     << QVariant::fromValue(false);
      QTest::newRow("MapLayer") << VisualItem::MapLayer << QVariant::fromValue(VisualItem::GROUND)
                                << QVariant::fromValue(VisualItem::OBJECT);
}

void VMapTest::saveAndLoad()
{
       auto itemCountAtStart= m_vmap->items().size();

        QFETCH(int, id);
        auto item= getItemFromId(id);
        if(item != nullptr)
        {
            AddVmapItemCommand cmd(item, true, m_vmap.get());
            cmd.setUndoable(true);
            cmd.redo();
            QVERIFY(itemCountAtStart != m_vmap->items().size());
            QCOMPARE(m_vmap->getItemCount(), 1);
            QCOMPARE(m_vmap->getSortedItemCount(), 1);
            QCOMPARE(m_vmap->getOrderedItemCount(), 0);
        }

        auto itemCountAfterInsertion= m_vmap->items().size();
        auto origWidth= m_vmap->mapWidth();

        QByteArray array;
        QDataStream in(&array, QIODevice::WriteOnly);
        in.setVersion(QDataStream::Qt_5_7);

        m_vmap->saveFile(in);

        VMap map;
        QDataStream out(&array, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_7);
        map.openFile(out);

        QCOMPARE(map.items().size(), itemCountAfterInsertion);

        QByteArray array2;
        QDataStream in2(&array2, QIODevice::WriteOnly);
        in2.setVersion(QDataStream::Qt_5_7);
        map.saveFile(in2);

        QCOMPARE(origWidth, map.mapWidth());

        // if(array.size() != array2.size())
        // qDebug() << "arrays size" << array.size() << array2.size();

        QCOMPARE(array.size(), array2.size());

        delete item;
}
VisualItem* VMapTest::getItemFromId(int i)
{
       VisualItem* item= nullptr;
       switch(i)
       {
       case 1:
           item= new RectItem(QPointF(100, 100), QPointF(200, 200), true, 18, QColor(Qt::red));
           break;
       case 2:
           item= new PathItem(QPointF(100, 100), QColor(Qt::red), true, 18);
           break;
       case 3:
           item= new LineItem(QPointF(100, 100), QColor(Qt::red), 18);
           break;
       case 4:
           item= new TextItem(QPointF(100, 100), 18, QColor(Qt::red));
           break;
       case 5:
           item= new EllipsItem(QPointF(100, 100), true, 18, QColor(Qt::red));
           break;
       case 6:
       {
           auto img= new ImageItem();
           img->setImageUri(":/assets/img/girafe.jpg");
           item= img;
       }
       break;
       case 7:
       {
           auto character= new Character("Obi", QColor(Qt::darkBlue), false);
           item= new CharacterItem(character, QPoint(100, 100));
       }
       break;
       }
       if(nullptr != item)
           item->setLayer(m_vmap->currentLayer());
       return item;
    return nullptr;
}

void VMapTest::saveAndLoad_data()
{
    QTest::addColumn<int>("id");

    QTest::newRow("Empty") << 0;
    QTest::newRow("RectItem") << 1;
    QTest::newRow("PathItem") << 2;
    QTest::newRow("LineItem") << 3;
    QTest::newRow("TextItem") << 4;
    QTest::newRow("EllipsItem") << 5;
    QTest::newRow("ImageItem") << 6;
    QTest::newRow("characterItem") << 7;
}


void VMapTest::addEllipse()
{
    /*    EllipsItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(true);
        cmd.redo();
        QVERIFY(cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        cmd.undo();

        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addPath()
{
        PathItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(true);
        cmd.redo();
        QVERIFY(cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        cmd.undo();

        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addLine()
{
        LineItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(true);
        cmd.redo();
        QVERIFY(cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        cmd.undo();

        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addImage()
{
    /*    ImageItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(true);
        cmd.redo();
        QVERIFY(cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        cmd.undo();

        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addCharacter()
{
        QSignalSpy spy(m_vmap.get(), &VMap::npcAdded);
        CharacterItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(true);
        cmd.redo();
        QVERIFY(cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
        QCOMPARE(spy.count(), 0);

        cmd.undo();

        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addText()
{
        TextItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(true);
        cmd.redo();
        QVERIFY(cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        cmd.undo();

        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addHighLighter()
{
        HighlighterItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(false);
        cmd.redo();
        QVERIFY(!cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addRule()
{
        RuleItem item;
        AddVmapItemCommand cmd(&item, true, m_vmap.get());
        cmd.setUndoable(false);
        cmd.redo();
        QVERIFY(!cmd.isUndoable());
        QCOMPARE(m_vmap->getItemCount(), 0);
        QCOMPARE(m_vmap->getSortedItemCount(), 0);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}

void VMapTest::zOrderTest()
{
        auto item= new RectItem(QPointF(100, 100), QPointF(200, 200), true, 18, QColor(Qt::red));
        auto item1= new RectItem(QPointF(100, 100), QPointF(200, 200), true, 18, QColor(Qt::blue));
        auto item2= new RectItem(QPointF(100, 100), QPointF(200, 200), true, 18, QColor(Qt::green));
        item->setVisible(true);
        item1->setVisible(true);
        item2->setVisible(true);

        m_vmap->setVisibilityMode(VMap::ALL);

        AddVmapItemCommand cmd(item, true, m_vmap.get());
        cmd.redo();

        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        AddVmapItemCommand cmd1(item1, true, m_vmap.get());
        cmd1.redo();

        QCOMPARE(m_vmap->getItemCount(), 2);
        QCOMPARE(m_vmap->getSortedItemCount(), 2);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        AddVmapItemCommand cmd2(item2, true, m_vmap.get());
        cmd2.redo();

        QCOMPARE(m_vmap->getItemCount(), 3);
        QCOMPARE(m_vmap->getSortedItemCount(), 3);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        m_vmap->changeStackOrder(item, VisualItem::FRONT);

        // item is in the front
        QVERIFY(item->zValue() > item1->zValue());
        QVERIFY(item->zValue() > item2->zValue());
        QVERIFY(item1->zValue() < item2->zValue());

        m_vmap->changeStackOrder(item, VisualItem::BACK);

        QVERIFY(item->zValue() < item1->zValue());
        QVERIFY(item->zValue() < item2->zValue());
        QVERIFY(item1->zValue() < item2->zValue());

        item2->setPos(800, 800); // move away the item
        m_vmap->changeStackOrder(item, VisualItem::RAISE);

        QVERIFY(item->isVisible());
        QVERIFY(item1->isVisible());
        QVERIFY(item2->isVisible());

        QVERIFY(item->zValue() > item1->zValue());
        QVERIFY(item->zValue() < item2->zValue());
        QVERIFY(item1->zValue() < item2->zValue());

        m_vmap->changeStackOrder(item, VisualItem::LOWER);

        QVERIFY(item->zValue() < item1->zValue());
        QVERIFY(item->zValue() < item2->zValue());
        QVERIFY(item1->zValue() < item2->zValue());

        delete item;
        delete item1;
        delete item2;
}

void VMapTest::fogTest()
{
        auto sightItem= m_vmap->getFogItem();

        QFETCH(VMap::VisibilityMode, mode);
        QFETCH(bool, visibility);

        m_vmap->setVisibilityMode(mode);
        QCOMPARE(sightItem->isVisible(), visibility);
}
void VMapTest::fogTest_data()
{
        QTest::addColumn<VMap::VisibilityMode>("mode");
        QTest::addColumn<bool>("visibility");

        QTest::addRow("Hidden") << VMap::HIDDEN << false;
        QTest::addRow("FogOfWar") << VMap::FOGOFWAR << true;
        QTest::addRow("All") << VMap::ALL << false;
}

void VMapTest::testMovableItems()
{
        QFETCH(Map::PermissionMode, mode);
        QFETCH(bool, movable);
        QFETCH(bool, isGM);
        QFETCH(VisualItem*, item);

        m_vmap->setPermissionMode(mode);
        m_vmap->setOption(VisualItem::LocalIsGM, isGM);

        AddVmapItemCommand cmd(item, true, m_vmap.get());
        cmd.redo();

        QCOMPARE(m_vmap->getItemCount(), 1);
        QCOMPARE(m_vmap->getSortedItemCount(), 1);
        QCOMPARE(m_vmap->getOrderedItemCount(), 0);

        QCOMPARE(item->flags() & QGraphicsItem::ItemIsMovable, movable);
}

void VMapTest::testMovableItems_data()
{
        QTest::addColumn<Map::PermissionMode>("mode");
        QTest::addColumn<bool>("movable");
        QTest::addColumn<bool>("isGM");
        QTest::addColumn<VisualItem*>("item");

        // rectangle for player
        QTest::addRow("Player wants to move GM item") << Map::GM_ONLY << false << false << getItemFromId(1);
        QTest::addRow("Player thinks item is his character") << Map::PC_MOVE << false << false << getItemFromId(1);
        QTest::addRow("Player move items") << Map::PC_ALL << true << false << getItemFromId(1);

        // rectangle for GM
        QTest::addRow("GM moves GM item") << Map::GM_ONLY << true << true << getItemFromId(1);
        QTest::addRow("GM moves even when player can move") << Map::PC_MOVE << true << true << getItemFromId(1);
        QTest::addRow("GM moves even when all permission") << Map::PC_ALL << true << true << getItemFromId(1);

        // PlayersList* model = PlayersList::instance();

        /*auto localPlayer = new Player(QStringLiteral("Obi"),QColor(Qt::darkBlue),false);
        auto localCharacter = new Character("Lynn Gray-Rike",QColor(Qt::darkBlue),false);
        auto item = new CharacterItem(localCharacter,QPoint(100,100));
        localPlayer->addCharacter(localCharacter);
        model->setLocalPlayer(localPlayer);

        VisualItem* itemVisual = item;
        //CharacterItem for GM
        QTest::addRow("PC can't move his character") << Map::GM_ONLY << false << false << itemVisual;
        QTest::addRow("PC can move his character") << Map::PC_MOVE << true << false << itemVisual;
        QTest::addRow("PC can move his character as all items") << Map::PC_ALL << true << false << itemVisual;
}*/

QTEST_MAIN(VMapTest);

#include "tst_vmap.moc"
