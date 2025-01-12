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
#include <QTest>

#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/campaignmanager.h"
#include "data/cleveruri.h"
#include <QMouseEvent>
#include <controller/contentcontroller.h>
#include <controller/playercontroller.h>
#include <data/character.h>
#include <helper.h>
#include <map>
#include <model/contentmodel.h>

#include "media/mediatype.h"

class ContentControllerTest : public QObject
{
    Q_OBJECT

public:
    ContentControllerTest();

private slots:
    void init();
    void serializeTest();
    void serializeTest_data();

    void completeSerializationTest();
    void completeSerializationTest_data();

    void saveLoadImage();

private:
    std::unique_ptr<ContentController> m_ctrl;
    std::unique_ptr<PlayerController> m_playerCtrl;
    QUndoStack m_stack;
};

Q_DECLARE_METATYPE(std::vector<Core::ContentType>)

ContentControllerTest::ContentControllerTest() {}

void ContentControllerTest::init()
{
    m_playerCtrl.reset(new PlayerController(nullptr));
    m_ctrl.reset(new ContentController(nullptr, m_playerCtrl->model(), m_playerCtrl->characterModel(), nullptr));
    connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });

    m_ctrl->setLocalId("localid");
}

void ContentControllerTest::saveLoadImage()
{
    m_ctrl.reset(new ContentController(new campaign::CampaignManager(nullptr), m_playerCtrl->model(),
                                       m_playerCtrl->characterModel(), nullptr));
    m_ctrl->setLocalId("localid");
    /*connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });
    {
        auto imgParams
            = std::map<QString, QVariant>({{Core::keys::KEY_UUID, "test_unit_vmap"},
                                           {Core::keys::KEY_PATH, ":/img/girafe3.jpg"},
                                           {Core::keys::KEY_RECT, QRectF(0, 0, 326, 244)},
                                           {Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::VECTORIALMAP)},
                                           {Core::keys::KEY_POSITION, QPointF(500, 200)},
                                           {Core::keys::KEY_TOOL, Core::SelectableTool::IMAGE}});
        auto mapParams= std::map<QString, QVariant>({{"name", QString("Unit Test Map")}});

        m_ctrl->newMedia(nullptr, mapParams);

        auto ctrls= m_ctrl->contentModel()->controllers();

        QCOMPARE(ctrls.size(), 1);

        auto ctrl= ctrls.front();

        auto mapCtrl= dynamic_cast<VectorialMapController*>(ctrl);

        QVERIFY(nullptr != mapCtrl);
        QSignalSpy spy(mapCtrl, &VectorialMapController::visualItemControllerCreated);
        mapCtrl->addItemController(imgParams);

        QCOMPARE(spy.count(), 1);
        auto itemCtrl= mapCtrl->itemController("test_unit_vmap");
        QVERIFY(nullptr != itemCtrl);

        auto rectCtrl= dynamic_cast<vmap::ImageController*>(itemCtrl);
        QCOMPARE(rectCtrl->rect(), QRectF(0, 0, 326, 244));
        QCOMPARE(rectCtrl->pos(), QPointF(500, 200));
    }
    auto path= QStringLiteral("%1/%2").arg(QDir::tempPath()).arg("scenario.sce");

    m_ctrl.reset(new ContentController(m_playerCtrl->model(), m_playerCtrl->characterModel(), nullptr));
    m_ctrl->setLocalId("localid");
    connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });
    m_ctrl->setSessionPath(path);
    m_ctrl->loadSession();

    {
        auto ctrls= m_ctrl->contentModel()->controllers();
        QCOMPARE(ctrls.size(), 1);

        auto ctrl= ctrls.front();
        auto mapCtrl= dynamic_cast<VectorialMapController*>(ctrl);
        QVERIFY(nullptr != mapCtrl);
        auto itemCtrl= mapCtrl->itemController("test_unit_vmap");
        auto rectCtrl= dynamic_cast<vmap::ImageController*>(itemCtrl);
        QCOMPARE(rectCtrl->rect(), QRectF(0, 0, 326, 244));
        QCOMPARE(rectCtrl->pos(), QPointF(500, 200));
    }

    QCOMPARE(m_ctrl->contentCount(), 1);*/
}

void ContentControllerTest::serializeTest()
{
    /*QFETCH(QVector<Core::ContentType>, contentType);
    using DataVec= QVector<std::map<QString, QVariant>>;
    QFETCH(DataVec, map);
    QFETCH(int, count);

    QCOMPARE(contentType.size(), map.size());

    for(int i= 0; i < contentType.size(); ++i)
    {
        auto mapItem= map[i];
        mapItem.insert({Core::keys::KEY_TYPE, QVariant::fromValue(contentType[i])});
        m_ctrl->newMedia(nullptr, mapItem);
    }
    QVERIFY2(m_ctrl->contentCount() == count, "After manual insertion");

    auto path= QStringLiteral("%1/%2").arg(QDir::tempPath()).arg("scenario.sce");

    m_ctrl->setSessionPath(path);
    m_ctrl->saveSession();

    m_ctrl.reset(new ContentController(m_playerCtrl->model(), m_playerCtrl->characterModel(), nullptr));
    connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });
    m_ctrl->setSessionPath(path);
    m_ctrl->loadSession();

    QCOMPARE(m_ctrl->contentCount(), count);*/
}

void ContentControllerTest::serializeTest_data()
{
    /*QTest::addColumn<QVector<Core::ContentType>>("contentType");
    QTest::addColumn<QVector<std::map<QString, QVariant>>>("map");
    QTest::addColumn<int>("count");

    auto func= [] {
        static int counter= 0;
        return counter++;
    };
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>() << QVector<std::map<QString, QVariant>>() << 0;
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::PICTURE})
                                    << QVector<std::map<QString, QVariant>>(
                                           {{{"path", ":/img/girafe3.jpg"}, {"name", "girafe"}}})
                                    << 1;
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::CHARACTERSHEET})
                                    << QVector<std::map<QString, QVariant>>(
                                           {{{"path", ":/charactersheet/bitume_fixed.rcs"}, {"name", "bitume"}}})
                                    << 1;
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::PDF})
                                    << QVector<std::map<QString, QVariant>>(
                                           {{{"path", ":/pdf/01_personnages.pdf"}, {"name", "personnages"}}})
                                    << 1;
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::SHAREDNOTE})
                                    << QVector<std::map<QString, QVariant>>(
                                           {{{"path", ":/sharednotes/test.rsn"}, {"name", "RSN file"}}})
                                    << 1;
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::SHAREDNOTE})
                                    << QVector<std::map<QString, QVariant>>(
                                           {{{"path", ":/sharednotes/scenario.md"}, {"name", "Markdown file"}}})
                                    << 1;
    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::VECTORIALMAP})
                                    << QVector<std::map<QString, QVariant>>({{{"path", ""}, {"name", ""}}}) << 1;

    QTest::addRow("cmd %d", func()) << QVector<Core::ContentType>({Core::ContentType::WEBVIEW})
                                    << QVector<std::map<QString, QVariant>>(
                                           {{{"path", "https://rolisteam.org"}, {"name", "rolisteam"}}})
                                    << 1;*/
}

void ContentControllerTest::completeSerializationTest()
{
    /*  QFETCH(std::vector<Core::ContentType>, list);

      for(auto content : list)
      {
          std::map<QString, QVariant> mapItem;
          switch(content)
          {
          case Core::ContentType::PICTURE:
              mapItem.insert({{Core::keys::KEY_PATH, ":/img/girafe3.jpg"}, {Core::keys::KEY_NAME, "girafe"}});
              break;
          case Core::ContentType::CHARACTERSHEET:
              mapItem.insert(
                  {{Core::keys::KEY_PATH, ":/charactersheet/bitume_fixed.rcs"}, {Core::keys::KEY_NAME, "bitume"}});
              break;
          case Core::ContentType::PDF:
              mapItem.insert({{Core::keys::KEY_PATH, ":/pdf/01_personnages.pdf"}, {Core::keys::KEY_NAME,
      "personnages"}}); break; case Core::ContentType::VECTORIALMAP: mapItem.insert({{Core::keys::KEY_PATH,
      ":/sharednotes/test.vmap"}, {Core::keys::KEY_NAME, "Test VMap"}}); break; case Core::ContentType::SHAREDNOTE:
              mapItem.insert(
                  {{Core::keys::KEY_PATH, ":/sharednotes/scenario.md"}, {Core::keys::KEY_NAME, "Markdown file"}});
              break;
          case Core::ContentType::NOTES:
              mapItem.insert(
                  {{Core::keys::KEY_PATH, ":/sharednotes/scenario.md"}, {Core::keys::KEY_NAME, "Markdown file"}});
              break;
          case Core::ContentType::WEBVIEW:
              mapItem.insert({{Core::keys::KEY_PATH, "https://rolisteam.org"}, {Core::keys::KEY_NAME, "rolisteam"}});
              break;
          default:
              break;
          }
          mapItem.insert({{Core::keys::KEY_TYPE, QVariant::fromValue(content)}});
          m_ctrl->newMedia(nullptr, mapItem);
      }

      QVERIFY2(m_ctrl->contentCount() == static_cast<int>(list.size()), "After manual insertion");

      auto path= QStringLiteral("%1/%2").arg(QDir::tempPath()).arg("scenario.sce");

      m_ctrl->setSessionPath(path);
      m_ctrl->saveSession();

      m_ctrl.reset(new ContentController(m_playerCtrl->model(), m_playerCtrl->characterModel(), nullptr));
      connect(m_ctrl.get(), &ContentController::performCommand, this, [this](QUndoCommand* cmd) { m_stack.push(cmd); });
      m_ctrl->setSessionPath(path);
      m_ctrl->loadSession();

      QCOMPARE(m_ctrl->contentCount(), static_cast<int>(list.size()));*/
}

void ContentControllerTest::completeSerializationTest_data()
{
    QTest::addColumn<std::vector<Core::ContentType>>("list");

    std::vector<Core::ContentType> data({Core::ContentType::VECTORIALMAP, Core::ContentType::SHAREDNOTE,
                                         Core::ContentType::CHARACTERSHEET, Core::ContentType::NOTES,
                                         Core::ContentType::PICTURE, Core::ContentType::WEBVIEW});

    // auto list = new std::vector<CleverURI*>();
    std::vector<Core::ContentType> list;

    int index= 0;
    for(unsigned int i= 0; i < data.size(); ++i)
    {
        auto comb_size= i + 1;
        do
        {
            list.clear();
            for(auto it= data.begin(); it < data.begin() + comb_size; ++it)
            {
                list.push_back(*it);
            }
            QTest::addRow("save %d", ++index) << list;
        } while(Helper::next_combination(data.begin(), data.begin() + comb_size, data.end()));
    }
}
QTEST_MAIN(ContentControllerTest)

#include "tst_serialization.moc"
