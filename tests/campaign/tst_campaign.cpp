/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   Copyright (C) 2015 by Benoit Lagarde                                  *
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

#include <QDir>
#include <QFileInfo>
#include <QTemporaryDir>
#include <QTreeView>
#include <memory>

#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "data/campaignmanager.h"
#include "data/media.h"

#include "model/mediamodel.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"

#include <QQuickStyle>

class CampaignTest : public QObject
{
    Q_OBJECT

public:
    CampaignTest();

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void createCampaign();
    void createCampaign_data();

    void campaignModel();
    void campaignModel_data();

private:
    std::unique_ptr<campaign::CampaignManager> m_manager;
};
CampaignTest::CampaignTest() {}

void CampaignTest::initTestCase()
{
    QQuickStyle::setStyle(":/rolistyle");
    QQuickStyle::setFallbackStyle("Default");
}

void CampaignTest::init()
{
    m_manager.reset(new campaign::CampaignManager(nullptr));
}

void CampaignTest::cleanup() {}

void CampaignTest::createCampaign()
{
    QFETCH(QString, subdir);
    QTemporaryDir dir;
    if(dir.isValid())
    {
        auto path= dir.path();
        path+= subdir;
        auto b= m_manager->createCampaign(QUrl::fromLocalFile(path));
        QVERIFY(b);
        QVERIFY(QFileInfo::exists(path + "/" + campaign::TRASH_FOLDER));
        QVERIFY(QFileInfo::exists(path + "/" + campaign::MEDIA_ROOT));
        QVERIFY(QFileInfo::exists(path + "/" + campaign::STATE_ROOT));
    }
}

void CampaignTest::createCampaign_data()
{
    QTest::addColumn<QString>("subdir");

    QTest::addRow("cmd1") << QString{};
    QTest::addRow("cmd2") << QString{"/campaign"};
}

void CampaignTest::campaignModel()
{
    QFETCH(QStringList, paths);
    QFETCH(QStringList, ids);
    QFETCH(QStringList, names);
    QFETCH(int, expected);

    auto campaign= m_manager->campaign();
    campaign->setRootDirectory(QDir::homePath());
    auto model= new campaign::MediaModel(campaign);

    int i= 0;
    for(const auto& path : paths)
    {
        auto id= ids[i];
        auto name= names[i];
        std::unique_ptr<campaign::Media> media(
            new campaign::Media(id, name, path, campaign::FileSerializer::typeFromExtention(path)));
        campaign->addMedia(std::move(media));
        ++i;
    }

    auto model2= new campaign::MediaModel(campaign);

    QCOMPARE(model->rowCount(), expected);
    QCOMPARE(model2->rowCount(), expected);

    QTreeView* view= new QTreeView();
    view->setModel(model);
    view->setVisible(true);

    QTest::qWait(2000);
    delete view;
    delete model;
}

void CampaignTest::campaignModel_data()
{
    QTest::addColumn<QStringList>("paths");
    QTest::addColumn<QStringList>("ids");
    QTest::addColumn<QStringList>("names");
    QTest::addColumn<int>("expected");

    auto campaign= m_manager->campaign();
    campaign->setRootDirectory(QDir::homePath());

    auto root= campaign->directory(campaign::Campaign::Place::MEDIA_ROOT);

    QTest::addRow("cmd1") << QStringList{} << QStringList{} << QStringList{} << 0;
    QTest::addRow("cmd2") << QStringList{root + "/image.jpg"} << QStringList{"image"} << QStringList{"image"} << 1;
    QTest::addRow("cmd3") << QStringList{root + "/image.jpg", root + "/image.jpg"} << QStringList{"image", "image"}
                          << QStringList{"image", "image"} << 1;

    QTest::addRow("cmd4") << QStringList{root + "/image.jpg", root + "/tavern.vmap"} << QStringList{"image", "tavern"}
                          << QStringList{"image", "tavern"} << 2;

    QTest::addRow("cmd5") << QStringList{root + "/image.jpg", root + "/chapter1/tavern.vmap"}
                          << QStringList{"image", "tavern"} << QStringList{"image", "tavern"} << 2;

    QTest::addRow("cmd6") << QStringList{root + "/chapter1/image.jpg", root + "/chapter1/tavern.vmap"}
                          << QStringList{"image", "tavern"} << QStringList{"image", "tavern"} << 1;
}

QTEST_MAIN(CampaignTest);

#include "tst_campaign.moc"
