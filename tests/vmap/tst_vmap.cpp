#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QObject>

#include "vmap/vmap.h"
#include "vmap/items/rectitem.h"
#include "vmap/items/ellipsitem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/ruleitem.h"
#include "vmap/items/textitem.h"
#include "vmap/items/lineitem.h"
#include "vmap/items/imageitem.h"
#include "vmap/items/characteritem.h"
#include "vmap/items/highlighteritem.h"
#include "undoCmd/addvmapitem.h"
#include "data/character.h"
#include "data/player.h"

class VMapTest : public QObject
{
    Q_OBJECT
public:
    VMapTest();
    VisualItem* getItemFromId(int i);

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void getAndSetTest();
    void cleanupTestCase();

    void addRect();
    void addEllipse();
    void addPath();
    void addHighLighter();
    void addRule();
    void addLine();
    void addImage();
    void addCharacter();
    void addText();

    void saveAndLoad();
    void saveAndLoad_data();

    void zOrderTest();

    void fogTest();
    void fogTest_data();

    void testOption();
    void testOption_data();

    void testMovableItems();
    void testMovableItems_data();
private:
    std::unique_ptr<VMap> m_vmap;
};

Q_DECLARE_METATYPE(VMap::GRID_PATTERN);
Q_DECLARE_METATYPE(VMap::SCALE_UNIT);
Q_DECLARE_METATYPE(VMap::VisibilityMode);
Q_DECLARE_METATYPE(VisualItem::Layer);
Q_DECLARE_METATYPE(VisualItem*);
Q_DECLARE_METATYPE(VisualItem::Properties);
Q_DECLARE_METATYPE(Map::PermissionMode);

VMapTest::VMapTest()
{

}

void VMapTest::initTestCase()
{

}

void VMapTest::cleanupTestCase()
{

}
void VMapTest::init()
{
    m_vmap.reset(new VMap());
}
void VMapTest::cleanup()
{

}
void VMapTest::getAndSetTest()
{
    VToolsBar::EditionMode mode = VToolsBar::Painting;
    m_vmap->setEditionMode(mode);
    QVERIFY2(m_vmap->getEditionMode() == mode, "Normal edition mode");

    mode = VToolsBar::Mask;
    m_vmap->setEditionMode(mode);
    QVERIFY2(m_vmap->getEditionMode() == mode, "Mask edition mode");

    int height = 800;
    m_vmap->setHeight(height);
    QVERIFY(m_vmap->mapHeight() == height);

    int width = 1200;
    m_vmap->setWidth(width);
    QVERIFY(m_vmap->mapWidth() == width);

    QColor color(Qt::red);
    m_vmap->setBackGroundColor(color);
    QVERIFY(m_vmap->getBackGroundColor() == color);

    QString title("Test");
    m_vmap->setTitle(title);
    QVERIFY(m_vmap->getMapTitle() == title);

    Map::PermissionMode permission = Map::PC_ALL;
    m_vmap->setPermissionMode(permission);
    QVERIFY(m_vmap->getPermissionMode() == permission);
    m_vmap->setPermissionMode(Map::GM_ONLY);
    QVERIFY(m_vmap->getPermissionModeText() == "No Right");
    m_vmap->setPermissionMode(Map::PC_MOVE);
    QVERIFY(m_vmap->getPermissionModeText() == "His character");
    m_vmap->setPermissionMode(Map::PC_ALL);
    QVERIFY(m_vmap->getPermissionModeText() == "All Permissions");

    VMap::VisibilityMode visiMode = VMap::HIDDEN;
    m_vmap->setVisibilityMode(visiMode);
    QVERIFY(m_vmap->getVisibilityModeText() == "Hidden");
    visiMode = VMap::FOGOFWAR;
    m_vmap->setVisibilityMode(visiMode);
    QVERIFY(m_vmap->getVisibilityModeText() == "Fog Of War");
    visiMode = VMap::ALL;
    m_vmap->setVisibilityMode(visiMode);
    QVERIFY(m_vmap->getVisibilityModeText() == "All visible");
}

void VMapTest::testOption()
{
    QFETCH(VisualItem::Properties,key);
    QFETCH(QVariant,value);

    VisualItem::Properties pop = static_cast<VisualItem::Properties>(key);

    m_vmap->setOption(pop,value);
    QVERIFY(m_vmap->getOption(pop) == value);

    QFETCH(QVariant,value1);
    bool b = m_vmap->setOption(pop,value1);
    QVERIFY(b);
    QVERIFY(m_vmap->getOption(pop) == value1);
}
void VMapTest::testOption_data()
{
    QTest::addColumn<VisualItem::Properties>("key");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QVariant>("value1");

    QTest::newRow("ShowNpcName")     << VisualItem::ShowNpcName          << QVariant(true) << QVariant::fromValue(false);
    QTest::newRow("ShowPcName")      << VisualItem::ShowPcName           << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("ShowNpcNumber")   << VisualItem::ShowNpcNumber        << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("ShowHealthStatus")<< VisualItem::ShowHealthStatus     << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("ShowInitScore")   << VisualItem::ShowInitScore        << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("ShowGrid")        << VisualItem::ShowGrid             << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("LocalIsGM")       << VisualItem::LocalIsGM            << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("GridPattern")     << VisualItem::GridPattern          << QVariant::fromValue(VMap::SQUARE) << QVariant::fromValue(VMap::NONE);
    QTest::newRow("GridColor")       << VisualItem::GridColor            << QVariant::fromValue(QColor(Qt::blue)) << QVariant::fromValue(QColor(Qt::yellow));
    QTest::newRow("GridSize")        << VisualItem::GridSize             << QVariant::fromValue(50) << QVariant::fromValue(100);
    QTest::newRow("Scale")           << VisualItem::Scale                << QVariant::fromValue(1.0) << QVariant::fromValue(0.5);
    QTest::newRow("Unit")            << VisualItem::Unit                 << QVariant::fromValue(VMap::M) << QVariant::fromValue(VMap::KM);
    QTest::newRow("CharacterVision") << VisualItem::EnableCharacterVision<< QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("PermissionMode")  << VisualItem::PermissionMode       << QVariant::fromValue(Map::PC_ALL) << QVariant::fromValue(Map::GM_ONLY);
    QTest::newRow("FogOfWarStatus")  << VisualItem::FogOfWarStatus       << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("CollisionStatus") << VisualItem::CollisionStatus      << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("GridAbove")       << VisualItem::GridAbove            << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("HideOtherLayers") << VisualItem::HideOtherLayers      << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("VisibilityMode")  << VisualItem::VisibilityMode       << QVariant::fromValue(VMap::HIDDEN) << QVariant::fromValue(VMap::ALL);
    QTest::newRow("ShowHealthBar")   << VisualItem::ShowHealthBar        << QVariant::fromValue(true) << QVariant::fromValue(false);
    QTest::newRow("MapLayer")        << VisualItem::MapLayer             <<  QVariant::fromValue(VisualItem::GROUND) <<  QVariant::fromValue(VisualItem::OBJECT);
}

void  VMapTest::saveAndLoad()
{
    auto itemCountAtStart = m_vmap->items().size();

    QFETCH(int, id);
    auto item = getItemFromId(id);
    AddVmapItemCommand cmd(item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(itemCountAtStart != m_vmap->items().size());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount() , 0);

    auto itemCountAfterInsertion = m_vmap->items().size();

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

    if(array.size() != array2.size())
        qDebug() << "arrays size"<<array.size() << array2.size();

    QCOMPARE(array.size(), array2.size());

    delete item;
}
VisualItem* VMapTest::getItemFromId(int i)
{
    VisualItem* item = nullptr;
    switch(i)
    {
    case 1:
        item = new RectItem(QPointF(100,100),QPointF(200,200),true,18,QColor(Qt::red));
        break;
case 2:
        item = new PathItem(QPointF(100,100),QColor(Qt::red),true,18);
        break;
    case 3:
        item =  new LineItem(QPointF(100,100),QColor(Qt::red),18);
        break;
    case 4:
        item = new TextItem(QPointF(100,100),18,QColor(Qt::red));
        break;
    case 5:
        item = new EllipsItem(QPointF(100,100),true,18,QColor(Qt::red));
        break;
    case 6:
    {
        auto img = new ImageItem();
        img->setImageUri(":/assets/img/girafe.jpg");
        item = img;
    }
        break;
    case 7:
    {
        auto character = new Character("Obi",QColor(Qt::darkBlue),false);
        item = new CharacterItem(character,QPoint(100,100));
    }
        break;
    }
    item->setLayer(m_vmap->currentLayer());
    return item;
}

void VMapTest::saveAndLoad_data()
{
    QTest::addColumn<int>("id");

    QTest::newRow("RectItem") <<1;
    QTest::newRow("PathItem") << 2;
    QTest::newRow("LineItem") <<3;
    QTest::newRow("TextItem") << 4;
    QTest::newRow("EllipsItem") << 5;
    QTest::newRow("ImageItem") << 6;
    QTest::newRow("characterItem") << 7;
}

void VMapTest::addRect()
{
    RectItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    cmd.undo();

    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addEllipse()
{
    EllipsItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    cmd.undo();

    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addPath()
{
    PathItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    cmd.undo();

    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addLine()
{
    LineItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    cmd.undo();

    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addImage()
{
    ImageItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    cmd.undo();

    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addCharacter()
{
    QSignalSpy spy(m_vmap.get(),&VMap::npcAdded);
    CharacterItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
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
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(true);
    cmd.redo();
    QVERIFY(cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount() , 1);
    QCOMPARE(m_vmap->getSortedItemCount() , 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    cmd.undo();

    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}
void VMapTest::addHighLighter()
{
    HighlighterItem item;
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
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
    AddVmapItemCommand cmd(&item,true,m_vmap.get());
    cmd.setUndoable(false);
    cmd.redo();
    QVERIFY(!cmd.isUndoable());
    QCOMPARE(m_vmap->getItemCount(), 0);
    QCOMPARE(m_vmap->getSortedItemCount(), 0);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);
}

void VMapTest::zOrderTest()
{
   auto item = new RectItem(QPointF(100,100),QPointF(200,200),true,18,QColor(Qt::red));
   auto item1 = new RectItem(QPointF(100,100),QPointF(200,200),true,18,QColor(Qt::blue));
   auto item2 = new RectItem(QPointF(100,100),QPointF(200,200),true,18,QColor(Qt::green));
   item->setVisible(true);
   item1->setVisible(true);
   item2->setVisible(true);

   m_vmap->setVisibilityMode(VMap::ALL);


   AddVmapItemCommand cmd(item,true,m_vmap.get());
   cmd.redo();

   QCOMPARE(m_vmap->getItemCount(), 1);
   QCOMPARE(m_vmap->getSortedItemCount(), 1);
   QCOMPARE(m_vmap->getOrderedItemCount(), 0);

   AddVmapItemCommand cmd1(item1,true,m_vmap.get());
   cmd1.redo();

   QCOMPARE(m_vmap->getItemCount(), 2);
   QCOMPARE(m_vmap->getSortedItemCount(), 2);
   QCOMPARE(m_vmap->getOrderedItemCount(), 0);


   AddVmapItemCommand cmd2(item2,true,m_vmap.get());
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


   item2->setPos(800,800);//move away the item
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
    auto sightItem = m_vmap->getFogItem();

    QFETCH(VMap::VisibilityMode, mode);
    QFETCH(bool, visibility);

    m_vmap->setVisibilityMode(mode);
    QCOMPARE(sightItem->isVisible(),visibility);
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

    AddVmapItemCommand cmd(item,true,m_vmap.get());
    cmd.redo();

    QCOMPARE(m_vmap->getItemCount(), 1);
    QCOMPARE(m_vmap->getSortedItemCount(), 1);
    QCOMPARE(m_vmap->getOrderedItemCount(), 0);

    QCOMPARE(item->flags() & QGraphicsItem::ItemIsMovable , movable);
}


void VMapTest::testMovableItems_data()
{
    QTest::addColumn<Map::PermissionMode>("mode");
    QTest::addColumn<bool>("movable");
    QTest::addColumn<bool>("isGM");
    QTest::addColumn<VisualItem*>("item");


    //rectangle for player
    QTest::addRow("Player wants to move GM item") << Map::GM_ONLY << false << false << getItemFromId(1);
    QTest::addRow("Player thinks item is his character") << Map::PC_MOVE << false << false << getItemFromId(1);
    QTest::addRow("Player move items") << Map::PC_ALL << true << false << getItemFromId(1);

    //rectangle for GM
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
    QTest::addRow("PC can move his character as all items") << Map::PC_ALL << true << false << itemVisual;*/
}

QTEST_MAIN(VMapTest);

#include "tst_vmap.moc"
