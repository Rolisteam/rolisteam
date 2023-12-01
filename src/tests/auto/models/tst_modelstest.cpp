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

#include "controller/view_controller/mindmapcontrollerbase.h"
#include "controller/view_controller/webpagecontroller.h"
#include "data/player.h"
#include "data/shortcutmodel.h"
#include "model/filteredplayermodel.h"
#include "model/historymodel.h"
#include "model/participantsmodel.h"
#include "model/playermodel.h"
#include "model/playerproxymodel.h"
#include "model/contentmodel.h"
#include "model/singlecontenttypemodel.h"
#include "model/languagemodel.h"
#include "model/actiononlistmodel.h"
#include "data/shortcutmodel.h"
#include "rwidgets/customs/shortcutvisitor.h"
// #include "test_root_path.h"
// #include "utils/iohelper.h"
// #include "worker/iohelper.h"
#include <QAbstractItemModelTester>
#include <QClipboard>
#include <QGuiApplication>
#include <QtTest/QtTest>
#include <helper.h>
#include <memory>

class ModelTest : public QObject
{
    Q_OBJECT

public:
    ModelTest();

private slots:
    void init();

    void participantsModel();
    void playerProxyModel();
    void historyModel();
    void filteredPlayerModel();

    void shortcutModel();
    void shortcutVisitor();

    void singleContentTypeModel();

    void languageModel();
    void actionListModel();

private:
};

ModelTest::ModelTest() {}

void ModelTest::init() {}

void ModelTest::participantsModel()
{
    std::unique_ptr<PlayerModel> player(new PlayerModel());
    std::unique_ptr<ParticipantsModel> model(new ParticipantsModel());

    model->setSourceModel(player.get());

    Player* p1= new Player;
    p1->setGM(true);
    p1->setName("GM");
    p1->setUuid("a");
    player->addPlayer(p1);

    model->setOwnerId(p1->uuid());
    QCOMPARE(model->ownerId(), p1->uuid());

    new QAbstractItemModelTester(player.get());
    new QAbstractItemModelTester(model.get());

    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::Hidden);

    model->promotePlayerToRead(QModelIndex());
    model->promotePlayerToReadWrite(QModelIndex());
    model->demotePlayerToRead(QModelIndex());
    model->demotePlayerToHidden(QModelIndex());
    model->promotePlayer(QModelIndex());
    model->demotePlayer(QModelIndex());

    QJsonObject obj;
    model->saveModel(obj);
    QVERIFY(!obj.isEmpty());
    model->loadModel(obj);

    //    qDebug() << "Parent" << model->rowCount(QModelIndex());
    //    qDebug() << "Write" << model->rowCount(model->index(0, 0, QModelIndex()));
    //    qDebug() << "Read" << model->rowCount(model->index(1, 0, QModelIndex()));
    //    qDebug() << "Hidden" << model->rowCount(model->index(2, 0, QModelIndex()));

    model->promotePlayerToRead(model->index(0, 0, model->index(2, 0, QModelIndex())));

    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::ReadOnly);

    model->promotePlayerToReadWrite(model->index(0, 0, model->index(1, 0, QModelIndex())));
    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::ReadWrite);

    model->demotePlayerToRead(model->index(0, 0, model->index(0, 0, QModelIndex())));
    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::ReadOnly);

    model->demotePlayerToHidden(model->index(0, 0, model->index(1, 0, QModelIndex())));
    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::Hidden);

    model->promotePlayer(model->index(0, 0, model->index(2, 0, QModelIndex())));
    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::ReadOnly);

    model->demotePlayer(model->index(0, 0, model->index(1, 0, QModelIndex())));
    QCOMPARE(model->getPermissionFor(p1), ParticipantsModel::Hidden);

    Player* p2= new Player;
    p2->setGM(false);
    p2->setName("Player");
    p2->setUuid("b");
    player->addPlayer(p2);
    QCOMPARE(model->getPermissionFor(p2), ParticipantsModel::Hidden);
}

void ModelTest::playerProxyModel()
{
    std::unique_ptr<PlayerModel> player(new PlayerModel());
    std::unique_ptr<PlayerProxyModel> model(new PlayerProxyModel());

    model->setSourceModel(player.get());

    new QAbstractItemModelTester(player.get());
    new QAbstractItemModelTester(model.get());

    Player* p1= new Player;
    p1->setGM(true);
    p1->setName("GM");
    p1->setUuid("a");
    player->addPlayer(p1);

    Player* p2= new Player;
    p2->setGM(false);
    p2->addCharacter(Helper::randomString(), Helper::randomString(), Helper::randomColor(), {}, {}, false);
    p2->setName("Player");
    p2->setUuid("b");
    player->addPlayer(p2);

    QCOMPARE(model->rowCount(), 2);

    QVERIFY(!model->hasChildren(model->index(0, 0, QModelIndex())));
    QCOMPARE(model->rowCount(model->index(1, 0, QModelIndex())), 0);
}

void ModelTest::historyModel()
{
    history::HistoryModel model;
    new QAbstractItemModelTester(&model);

    auto max= Helper::generate<int>(1, 10);
    model.setMaxCapacity(max);
    QCOMPARE(model.maxCapacity(), max);

    model.setMaxCapacity(max);

    model.addLink(Helper::randomUrl(), Helper::randomString(),
                  Helper::randomFromList<Core::ContentType>(
                      {Core::ContentType::VECTORIALMAP, Core::ContentType::PICTURE, Core::ContentType::NOTES,
                       Core::ContentType::CHARACTERSHEET, Core::ContentType::SHAREDNOTE, Core::ContentType::PDF,
                       Core::ContentType::WEBVIEW, Core::ContentType::INSTANTMESSAGING, Core::ContentType::MINDMAP,
                       Core::ContentType::UNKNOWN}));

    QCOMPARE(model.rowCount(), 1);
    model.addLink(Helper::randomUrl(), Helper::randomString(),
                  Helper::randomFromList<Core::ContentType>(
                      {Core::ContentType::VECTORIALMAP, Core::ContentType::PICTURE, Core::ContentType::NOTES,
                       Core::ContentType::CHARACTERSHEET, Core::ContentType::SHAREDNOTE, Core::ContentType::PDF,
                       Core::ContentType::WEBVIEW, Core::ContentType::INSTANTMESSAGING, Core::ContentType::MINDMAP,
                       Core::ContentType::UNKNOWN}));

    QCOMPARE(model.rowCount(), 2);

    auto infos= model.data();

    for(const auto& info : infos)
    {
        QCOMPARE(model.idToPath(info.id).bookmarked, info.bookmarked);
        QCOMPARE(model.idToPath(info.id).displayName, info.displayName);
        QCOMPARE(model.idToPath(info.id).id, info.id);
        QCOMPARE(model.idToPath(info.id).lastAccess, info.lastAccess);
        QCOMPARE(model.idToPath(info.id).type, info.type);
        QCOMPARE(model.idToPath(info.id).url, info.url);
    }

    for(int i= 0; i < model.rowCount(); i++)
    {
        for(int j= 0; j < model.columnCount(); j++)
        {
            auto val1= model.data(model.index(i, j), Qt::DisplayRole);
            auto val2= model.data(model.index(i, j), Qt::UserRole + 1 + j);
            QCOMPARE(val2, val1);
        }

        for(int k= Qt::UserRole + 1; k < history::HistoryModel::BookmarkRole; ++k)
        {
            model.data(model.index(i, 0), k);
        }
    }

    model.clear();
    model.setLinks(infos);

    model.refreshAccess(infos[0].id);
    model.refreshAccess(infos[1].id);
}

void ModelTest::filteredPlayerModel()
{
    std::unique_ptr<PlayerModel> player(new PlayerModel());
    std::unique_ptr<InstantMessaging::FilteredPlayerModel> model(
        new InstantMessaging::FilteredPlayerModel(QStringList{"b"}, nullptr));

    model->setSourceModel(player.get());

    new QAbstractItemModelTester(player.get());
    new QAbstractItemModelTester(model.get());

    Player* p1= new Player;
    p1->setGM(true);
    p1->setName("GM");
    p1->setUuid("a");
    player->addPlayer(p1);

    Player* p2= new Player;
    p2->setGM(false);
    p2->addCharacter(Helper::randomString(), Helper::randomString(), Helper::randomColor(), {}, {}, false);
    p2->setName("Player");
    p2->setUuid("b");
    player->addPlayer(p2);

    QCOMPARE(model->rowCount(), 1);

    QCOMPARE(model->recipiantIds(), QStringList{"b"});
    QVERIFY(model->hasRecipiant(QString("b")));
    QCOMPARE(model->recipiantName(QString("b")), QString("Player"));
    QVERIFY(!model->hasRecipiant(Helper::randomString()));
}

void ModelTest::shortcutModel()
{
    ShortCutModel model;

    new QAbstractItemModelTester(&model);

    QCOMPARE(model.rowCount(), 0);

    auto cat1= Helper::randomString();

    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());

    QCOMPARE(model.rowCount(), 0);

    model.addCategory(cat1);
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat1, Helper::randomString(), Helper::randomString());

    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.rowCount(model.index(0, 0, QModelIndex())), 5);

    auto cat2= Helper::randomString();
    model.addCategory(cat2);

    auto cat3= Helper::randomString();
    model.addCategory(cat3);

    model.insertShortCut(cat2, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat2, Helper::randomString(), Helper::randomString());
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.rowCount(model.index(1, 0, QModelIndex())), 2);

    model.insertShortCut(cat3, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat3, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat3, Helper::randomString(), Helper::randomString());
    model.insertShortCut(cat3, Helper::randomString(), Helper::randomString());

    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.rowCount(model.index(2, 0, QModelIndex())), 4);

    model.removeCategory(cat2);
    model.removeCategory(Helper::randomString());
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.rowCount(model.index(0, 0, QModelIndex())), 5);
    QCOMPARE(model.rowCount(model.index(1, 0, QModelIndex())), 4);

    QVERIFY(!model.headerData(0, Qt::Vertical, Qt::EditRole).isValid());
    QVERIFY(!model.headerData(0, Qt::Vertical, Qt::DisplayRole).isValid());

    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString(), "Action");
    QCOMPARE(model.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString(), "Key");

    model.index(-1, -1, QModelIndex());

    auto name= Helper::randomString();

    Category cat(name);

    QCOMPARE(cat.name(), name);
    name= Helper::randomString();
    cat.setName(name);
    QCOMPARE(cat.name(), name);

    {
        auto seq= Helper::randomString();
        auto name= Helper::randomString();
        ShortCut cut(name, seq);

        QCOMPARE(cut.getName(), name);
        QCOMPARE(cut.getSequence(), QKeySequence(seq));
        cat.insertShortcut(name, seq);
        auto shortcut= cat.getShortCut(0);

        QVERIFY(cat.hasShortCut(shortcut));
        QCOMPARE(cat.indexOf(shortcut), 0);

        QVERIFY(!cat.hasShortCut(nullptr));
        QCOMPARE(cat.indexOf(nullptr), -1);
    }
}

void ModelTest::shortcutVisitor()
{
    QWidget wid;
    wid.setObjectName(Helper::randomString());

    auto act= new QAction(Helper::randomString());
    act->setShortcut(QKeySequence::New);
    wid.addAction(act);

    act= new QAction(Helper::randomString());
    act->setShortcut(QKeySequence::Open);
    wid.addAction(act);

    act= new QAction(Helper::randomString());
    act->setShortcut(QKeySequence("Ctrl+E"));
    wid.addAction(act);

    ShortcutVisitor visitor;
    QWidget wid2;

    act= new QAction(Helper::randomString());
    act->setShortcut(Helper::randomString(1));
    wid2.addAction(act);

    QWidget wid22(&wid2);

    act= new QAction(Helper::randomString());
    act->setShortcut(Helper::randomString(1));
    wid22.addAction(act);

    QWidget wid4;

    {
        QWidget wid3;

        act= new QAction(Helper::randomString());
        act->setShortcut(Helper::randomString(1));
        wid3.addAction(act);

        act= new QAction(Helper::randomString());
        act->setShortcut(Helper::randomString(1));
        wid3.addAction(act);

        visitor.registerWidget(&wid, Helper::randomString(), true);
        visitor.registerWidget(&wid, Helper::randomString(), true);
        visitor.registerWidget(&wid2, Helper::randomString(), true);
        visitor.registerWidget(&wid3, Helper::randomString(), true);
        visitor.registerWidget(&wid4, Helper::randomString(), true);

        auto model= visitor.getModel();

        QCOMPARE(model->rowCount(), 3);
        QCOMPARE(model->rowCount(model->index(0, 0, QModelIndex())), 3);
        QCOMPARE(model->rowCount(model->index(1, 0, QModelIndex())), 1);
        QCOMPARE(model->rowCount(model->index(2, 0, QModelIndex())), 2);
    }
    auto model= visitor.getModel();

    QCOMPARE(model->rowCount(), 2);
    visitor.unregisterWidget(&wid2);
    visitor.unregisterWidget(&wid2);
    QCOMPARE(model->rowCount(), 1);
}

void ModelTest::singleContentTypeModel()
{
    auto singleModel = std::make_unique<SingleContentTypeModel>(Core::ContentType::WEBVIEW);
    auto contentModel = std::make_unique<ContentModel>();

    singleModel->setSourceModel(contentModel.get());

    new QAbstractItemModelTester(singleModel.get());
    new QAbstractItemModelTester(contentModel.get());

    auto id = Helper::randomString();
    auto data = new WebpageController(id);
    contentModel->appendMedia(data);
    contentModel->appendMedia(new WebpageController());
    contentModel->appendMedia(new WebpageController());
    contentModel->appendMedia(new mindmap::MindMapControllerBase(true, Helper::randomString()));

    QCOMPARE(singleModel->rowCount(), 3);
    QVERIFY(singleModel->contains(id));
    QVERIFY(!singleModel->contains(Helper::randomString(12)));
    QCOMPARE(dynamic_cast<WebpageController*>(singleModel->controller(id)), data);


}
void ModelTest::languageModel()
{
    Q_INIT_RESOURCE(translations);

    auto langModel = std::make_unique<LanguageModel>();

    new QAbstractItemModelTester(langModel.get());
    auto syst = QLocale::system();

    if(langModel->rowCount()==0) {
        qDebug() << "Language model is Empty!!";
        return;
    }

    auto idx = langModel->indexSystemLocale(QLocale::languageToCode(syst.language()));
    qDebug() << "id" << idx << QLocale::languageToCode(syst.language());
    QVERIFY(idx > -1);
    QVERIFY(langModel->indexSystemLocale(Helper::randomString()) == -1);

    langModel->pathFromIndex(langModel->index(idx,0));
}

void ModelTest::actionListModel()
{
    auto actModel = std::make_unique<ActionOnListModel>();
    new QAbstractItemModelTester(actModel.get());
}
QTEST_MAIN(ModelTest);

#include "tst_modelstest.moc"
