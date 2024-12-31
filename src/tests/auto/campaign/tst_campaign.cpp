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
#include "diceparser/dicealias.h"
#include "diceparser_qobject/diceroller.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "model/mediamodel.h"
#include "model/nonplayablecharactermodel.h"
#include "utils/iohelper.h"
#include "worker/fileserializer.h"
#include <helper.h>

#include <QCryptographicHash>
#include <QQuickStyle>

constexpr int delay{2000};

QFileInfoList fileAndDirRecusive(const QString& path)
{
    QFileInfoList list;
    QDir dir(path);

    auto importedFile= dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    list << importedFile;
    for(auto const& res : std::as_const(importedFile))
    {
        if(res.isDir())
        {
            qDebug() << res.absoluteFilePath();
            QDir subdir(res.absoluteFilePath());
            list << subdir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        }
    }

    return list;
}

class CampaignTest : public QObject
{
    Q_OBJECT

public:
    CampaignTest();

    void create(const QString& path);

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void createCampaign();
    void createCampaign_data();

    void campaignModel();
    void campaignModel_data();

    void addAndRemoveMedia();
    void addAndRemoveMedia_data();

    void copyCampaign();
    void copyCampaign_data();

    void importFromAnotherCampaign();
    void importFromAnotherCampaign_data();

    void editorCampaign();
    void campaignTest();
    void campaignManagerTest();

private:
    std::unique_ptr<campaign::CampaignManager> m_manager;
    std::unique_ptr<DiceRoller> m_diceparser;
};

using TupleMedia= QList<std::tuple<QString, QString, QByteArray>>;
using TupleState= QList<std::tuple<QString, QString, QColor>>;
using TupleDice= QList<std::tuple<QString, QString, bool>>;
using TupleNPC= QList<std::tuple<QString, QString, QString, QColor>>;
Q_DECLARE_METATYPE(TupleMedia)
Q_DECLARE_METATYPE(TupleState)
Q_DECLARE_METATYPE(TupleDice)
Q_DECLARE_METATYPE(TupleNPC)

CampaignTest::CampaignTest() {}

void CampaignTest::initTestCase()
{
    QQuickStyle::setStyle(":/rolistyle");
    QQuickStyle::setFallbackStyle("Default");
}

void CampaignTest::init()
{
    m_diceparser.reset(new DiceRoller);
    m_manager.reset(new campaign::CampaignManager(m_diceparser.get()));
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

void CampaignTest::create(const QString& path)
{
    m_manager->createCampaign(QUrl::fromLocalFile(path));
    QSignalSpy spy(m_manager.get(), &campaign::CampaignManager::campaignLoaded);

    QVERIFY(spy.wait(delay));
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
    campaign->setRootDirectory(QDir::tempPath());
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

    // auto model2= new campaign::MediaModel(campaign);

    QCOMPARE(model->rowCount(), expected);
    // QCOMPARE(model2->rowCount(), expected);

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
    campaign->setRootDirectory(QDir::tempPath());

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

void CampaignTest::addAndRemoveMedia()
{
    QFETCH(QStringList, paths);
    QFETCH(QStringList, ids);
    QFETCH(QStringList, names);
    QFETCH(int, expected);

    auto campaign= m_manager->campaign();
    campaign->setRootDirectory(QDir::tempPath());
    auto model= new campaign::MediaModel(campaign);
    QTreeView* view= new QTreeView();
    view->setModel(model);
    view->setVisible(true);
    QCOMPARE(model->rowCount(), 0);

    int i= 0;
    for(const auto& path : paths)
    {
        auto id= ids[i];
        auto name= names[i];
        std::unique_ptr<campaign::Media> media(
            new campaign::Media(id, name, path, campaign::FileSerializer::typeFromExtention(path)));
        campaign->addMedia(std::move(media));
        campaign->removeMedia(id);
        ++i;
    }

    QCOMPARE(model->rowCount(), expected);

    QTest::qWait(2000);
    delete view;
    delete model;
}

void CampaignTest::addAndRemoveMedia_data()
{
    QTest::addColumn<QStringList>("paths");
    QTest::addColumn<QStringList>("ids");
    QTest::addColumn<QStringList>("names");
    QTest::addColumn<int>("expected");

    auto campaign= m_manager->campaign();
    campaign->setRootDirectory(QDir::tempPath());

    auto root= campaign->directory(campaign::Campaign::Place::MEDIA_ROOT);

    QTest::addRow("cmd1") << QStringList{} << QStringList{} << QStringList{} << 0;
    QTest::addRow("cmd2") << QStringList{root + "/image.jpg"} << QStringList{"image"} << QStringList{"image"} << 0;
    QTest::addRow("cmd3") << QStringList{root + "/image.jpg", root + "/image.jpg"} << QStringList{"image", "image"}
                          << QStringList{"image", "image"} << 0;

    QTest::addRow("cmd4") << QStringList{root + "/image.jpg", root + "/tavern.vmap"} << QStringList{"image", "tavern"}
                          << QStringList{"image", "tavern"} << 0;

    QTest::addRow("cmd5") << QStringList{root + "/image.jpg", root + "/chapter1/tavern.vmap"}
                          << QStringList{"image", "tavern"} << QStringList{"image", "tavern"} << 1;

    QTest::addRow("cmd6") << QStringList{root + "/chapter1/image.jpg", root + "/chapter1/tavern.vmap"}
                          << QStringList{"image", "tavern"} << QStringList{"image", "tavern"} << 1;
}

void CampaignTest::copyCampaign()
{
    QFETCH(TupleMedia, medias);
    QFETCH(TupleDice, dices);
    QFETCH(TupleState, states);
    QFETCH(TupleNPC, npcs);

    QTemporaryDir root1;
    if(root1.isValid())
    {
        auto path= root1.path();
        create(path);
    }

    auto campaign= m_manager->campaign();
    auto edit= m_manager->editor();
    auto stateModel= campaign->stateModel();
    auto diceModel= campaign->diceAliases();
    auto npcModel= campaign->npcModel();

    // add states
    for(auto const& t : states)
    {
        CharacterState state;
        state.setId(std::get<0>(t));
        state.setLabel(std::get<1>(t));
        state.setColor(std::get<2>(t));
        stateModel->appendState(std::move(state));
    }
    // add dices
    for(auto const& a : dices)
    {
        DiceAlias alias(std::get<0>(a), std::get<1>(a), {}, std::get<2>(a));
        diceModel->appendAlias(std::move(alias));
    }
    // add NPC
    for(auto const& n : npcs)
    {
        auto npc= new campaign::NonPlayableCharacter();
        npc->setUuid(std::get<0>(n));
        npc->setName(std::get<1>(n));
        npc->setDescription(std::get<2>(n));
        npc->setColor(std::get<3>(n));
        npcModel->addCharacter(npc);
    }
    // add medias
    for(auto const& m : medias)
    {
        edit->addMedia(std::get<0>(m), std::get<1>(m), std::get<2>(m));
    }
    m_manager->saveCampaign();

    QTest::qWait(2000);

    QTemporaryDir dir;
    if(dir.isValid())
    {
        auto path= dir.path();
        m_manager->copyCampaign(QUrl::fromUserInput(path));
        QTest::qWait(2000);

        QDir copyDir(path);

        auto copyListFile= copyDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        QDir sourceDir(campaign->rootDirectory());
        auto sourceListFile= sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        // if(copyListFile.size() != sourceListFile.size())
        //     qDebug() << copyListFile << sourceListFile << states.size();

        QCOMPARE(copyListFile.size(), sourceListFile.size());

        for(int i= 0; i < copyListFile.size(); ++i)
        {
            auto copy= copyListFile[i];
            auto source= sourceListFile[i];
            QVERIFY2(copy.size() == source.size(), copy.absoluteFilePath().toStdString().c_str());

            QFile copyFile(copy.absoluteFilePath());
            QFile sourceFile(source.absoluteFilePath());
            if(copyFile.open(QFile::ReadOnly) && sourceFile.open(QFile::ReadOnly))
            {
                QCOMPARE(QCryptographicHash::hash(copyFile.readAll(), QCryptographicHash::Md5),
                         QCryptographicHash::hash(sourceFile.readAll(), QCryptographicHash::Md5));
            }
        }
    }
}

void CampaignTest::copyCampaign_data()
{

    QTest::addColumn<TupleMedia>("medias");
    QTest::addColumn<TupleDice>("dices");
    QTest::addColumn<TupleState>("states");
    QTest::addColumn<TupleNPC>("npcs");

    QTest::addRow("cmd1") << TupleMedia{} << TupleDice{} << TupleState{} << TupleNPC{};
    QTest::addRow("cmd2") << TupleMedia{{"girafe1", "girafe1", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{} << TupleState{} << TupleNPC{};

    QTest::addRow("cmd3") << TupleMedia{{"girafe1", "girafe1", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}} << TupleState{} << TupleNPC{};

    QTest::addRow("cmd4") << TupleMedia{{"girafe1", "girafe1", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}}
                          << TupleState{{"state1", "state1", QColor(Qt::blue)}} << TupleNPC{};

    QTest::addRow("cmd5") << TupleMedia{{"girafe1", "girafe1", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}}
                          << TupleState{{"state1", "state1", QColor(Qt::blue)}}
                          << TupleNPC{{"character1", "character1", "character1", QColor(Qt::blue)}};
}

void CampaignTest::importFromAnotherCampaign()
{
    QFETCH(TupleMedia, medias);
    QFETCH(TupleDice, dices);
    QFETCH(TupleState, states);
    QFETCH(TupleNPC, npcs);
    QFETCH(QVector<Core::CampaignDataCategory>, categories);
    QFETCH(bool, expectedEguality);

    QTemporaryDir root1;
    if(root1.isValid())
    {
        auto path= root1.path();
        create(path);
    }

    auto campaign= m_manager->campaign();
    auto edit= m_manager->editor();
    auto stateModel= campaign->stateModel();
    auto diceModel= campaign->diceAliases();
    auto npcModel= campaign->npcModel();

    auto oldCampaignPath= campaign->rootDirectory();

    // add states
    for(auto const& t : states)
    {
        CharacterState state;
        state.setId(std::get<0>(t));
        state.setLabel(std::get<1>(t));
        state.setColor(std::get<2>(t));
        stateModel->appendState(std::move(state));
    }
    // add dices
    for(auto const& a : dices)
    {
        DiceAlias alias(std::get<0>(a), std::get<1>(a), {}, std::get<2>(a));
        diceModel->appendAlias(std::move(alias));
    }
    // add NPC
    for(auto const& n : npcs)
    {
        auto npc= new campaign::NonPlayableCharacter();
        npc->setUuid(std::get<0>(n));
        npc->setName(std::get<1>(n));
        npc->setDescription(std::get<2>(n));
        npc->setColor(std::get<3>(n));
        npcModel->addCharacter(npc);
    }
    // add medias
    for(auto const& m : medias)
    {
        edit->addMedia(std::get<0>(m), QString("%1/media/%2").arg(oldCampaignPath, std::get<1>(m)), std::get<2>(m));
    }
    m_manager->saveCampaign();

    QTest::qWait(delay);

    QTemporaryDir dir;
    if(dir.isValid())
    {
        m_manager.reset(new campaign::CampaignManager(m_diceparser.get()));
        auto path= dir.path();
        create(path);
        m_manager->importDataFrom(oldCampaignPath, categories);
        // m_manager->reload();
        QTest::qWait(delay);
        m_manager->saveCampaign();
        QTest::qWait(delay);

        auto importedFile= fileAndDirRecusive(path);
        auto sourceListFile= fileAndDirRecusive(root1.path());

        if(expectedEguality)
        {
            // if(importedFile.size() != sourceListFile.size())
            //                qDebug() << importedFile << sourceListFile;
            QCOMPARE(importedFile.size(), sourceListFile.size());
        }

        if(expectedEguality)
        {
            for(int i= 0; i < importedFile.size(); ++i)
            {
                auto copy= importedFile[i];
                auto source= sourceListFile[i];
                if(copy.absoluteFilePath().endsWith(campaign::MODEL_FILE))
                    continue;

                // if(copy.size() != source.size())
                //                    qDebug() << "size issue" << copy.size() << source.size();
                QVERIFY2(copy.size() == source.size(), copy.absoluteFilePath().toStdString().c_str());

                QFile copyFile(copy.absoluteFilePath());
                QFile sourceFile(source.absoluteFilePath());
                if(copyFile.open(QFile::ReadOnly) && sourceFile.open(QFile::ReadOnly))
                {
                    QCOMPARE(QCryptographicHash::hash(copyFile.readAll(), QCryptographicHash::Md5),
                             QCryptographicHash::hash(sourceFile.readAll(), QCryptographicHash::Md5));
                }
            }
        }
        else
        {
            if(importedFile.size() != sourceListFile.size())
                return;
            int i= 0;
            QVERIFY2(!std::all_of(std::begin(importedFile), std::end(importedFile),
                                  [sourceListFile, &i](const QFileInfo& copy)
                                  {
                                      auto source= sourceListFile[i];

                                      ++i;
                                      return copy.size() == source.size();
                                  }),
                     "All files have the same size");
        }
    }
}
void CampaignTest::importFromAnotherCampaign_data()
{
    QTest::addColumn<TupleMedia>("medias");
    QTest::addColumn<TupleDice>("dices");
    QTest::addColumn<TupleState>("states");
    QTest::addColumn<TupleNPC>("npcs");
    QTest::addColumn<QVector<Core::CampaignDataCategory>>("categories");
    QTest::addColumn<bool>("expectedEguality");

    QTest::addRow("cmd1") << TupleMedia{} << TupleDice{} << TupleState{} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{} << true;

    QTest::addRow("cmd2") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{} << TupleState{} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images} << true;

    QTest::addRow("cmd3") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{} << TupleState{} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::PDFDoc} << false;

    QTest::addRow("cmd4") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}} << TupleState{} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images,
                                                                 Core::CampaignDataCategory::DiceAlias}
                          << true;

    QTest::addRow("cmd5") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}} << TupleState{} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images} << false;

    QTest::addRow("cmd6") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}}
                          << TupleState{{"state1", "state1", QColor(Qt::blue)}} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images,
                                                                 Core::CampaignDataCategory::CharacterStates,
                                                                 Core::CampaignDataCategory::DiceAlias}
                          << true;

    QTest::addRow("cmd7") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}}
                          << TupleState{{"state1", "state1", QColor(Qt::blue)}} << TupleNPC{}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images,
                                                                 Core::CampaignDataCategory::DiceAlias}
                          << false;

    QTest::addRow("cmd8") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}}
                          << TupleState{{"state1", "state1", QColor(Qt::blue)}}
                          << TupleNPC{{"character1", "character1", "character1", QColor(Qt::blue)}}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images,
                                                                 Core::CampaignDataCategory::CharacterStates,
                                                                 Core::CampaignDataCategory::DiceAlias,
                                                                 Core::CampaignDataCategory::AntagonistList}
                          << true;

    QTest::addRow("cmd9") << TupleMedia{{"girafe1", "girafe1.jpg", utils::IOHelper::loadFile(":/img/girafe.jpg")}}
                          << TupleDice{{"pattern", "dicecommand", false}}
                          << TupleState{{"state1", "state1", QColor(Qt::blue)}}
                          << TupleNPC{{"character1", "character1", "character1", QColor(Qt::blue)}}
                          << QVector<Core::CampaignDataCategory>{Core::CampaignDataCategory::Images,
                                                                 Core::CampaignDataCategory::CharacterStates,
                                                                 Core::CampaignDataCategory::DiceAlias}
                          << false;

    /* {
         Core::CampaignDataCategory::AudioPlayer1, Core::CampaignDataCategory::AudioPlayer2,
             Core::CampaignDataCategory::AudioPlayer3, Core::CampaignDataCategory::Images,
             Core::CampaignDataCategory::Maps, Core::CampaignDataCategory::MindMaps, Core::CampaignDataCategory::Notes,
             Core::CampaignDataCategory::WebLink, Core::CampaignDataCategory::PDFDoc,
             Core::CampaignDataCategory::DiceAlias, Core::CampaignDataCategory::CharacterStates,
             Core::CampaignDataCategory::Themes, Core::CampaignDataCategory::CharacterSheets,
             Core::CampaignDataCategory::AntagonistList
     }*/
}

void CampaignTest::editorCampaign()
{
    auto editor= m_manager->editor();

    editor->mergeAudioFile(Helper::randomString(), Helper::randomString());

    QVERIFY(!editor->copyTheme(Helper::randomString(), Helper::randomString()));
    QVERIFY(!editor->mergeJsonArrayFile(Helper::randomString(), Helper::randomString()));
    QVERIFY(!editor->removeFile(Helper::randomString()));
    QVERIFY(!editor->removeMedia(Helper::randomString()));
    QVERIFY(!editor->mediaFullPath(Helper::randomString()).isEmpty());
    QCOMPARE(editor->currentDir(), QString());
    // QVERIFY(!editor->currentDir().isEmpty());
    QVERIFY(!editor
                 ->mediaFullPathWithExtension(
                     Helper::randomString(),
                     Helper::randomFromList<Core::ContentType>(
                         {Core::ContentType::CHARACTERSHEET, Core::ContentType::INSTANTMESSAGING,
                          Core::ContentType::MINDMAP, Core::ContentType::NOTES, Core::ContentType::PDF,
                          Core::ContentType::PICTURE, Core::ContentType::SHAREDNOTE, Core::ContentType::VECTORIALMAP,
                          Core::ContentType::WEBVIEW}))
                 .isEmpty());

    editor->saveAvatar(Helper::randomString(), {});
    editor->doCommand(nullptr);
}

void CampaignTest::campaignTest()
{
    auto camp= m_manager->campaign();

    QVERIFY(!camp->currentStorePath().isEmpty());
    camp->renameMedia(Helper::randomString(), Helper::randomString());
    camp->pathFromUuid(Helper::randomString());

    QList<campaign::Campaign::Place> places{campaign::Campaign::Place::DICE_MODEL,
                                            campaign::Campaign::Place::NPC_MODEL,
                                            campaign::Campaign::Place::STATE_MODEL,
                                            campaign::Campaign::Place::FIRST_AUDIO_PLAYER_FILE,
                                            campaign::Campaign::Place::SECOND_AUDIO_PLAYER_FILE,
                                            campaign::Campaign::Place::THIRD_AUDIO_PLAYER_FILE,
                                            campaign::Campaign::Place::MEDIA_ROOT,
                                            campaign::Campaign::Place::STATE_MODEL,
                                            campaign::Campaign::Place::STATE_ROOT,
                                            campaign::Campaign::Place::NPC_MODEL,
                                            campaign::Campaign::Place::NPC_ROOT,
                                            campaign::Campaign::Place::THEME_FILE,
                                            campaign::Campaign::Place::TRASH_ROOT};

    for(auto type : places)
        QVERIFY(!camp->directory(type).isEmpty());

    camp->postError(Helper::randomString());

    camp->addAlias();
    camp->deleteAlias(QModelIndex());
    auto model= camp->diceAliases();
    camp->deleteAlias(model->index(0, 0, QModelIndex()));

    camp->convertAlias(Helper::randomString());

    camp->addState();
    camp->deleteState(QModelIndex());

    auto states= camp->stateModel();
    camp->deleteState(states->index(0, 0, QModelIndex()));

    QList<campaign::Campaign::Move> moves{campaign::Campaign::Move::UP, campaign::Campaign::Move::TOP,
                                          campaign::Campaign::Move::DOWN, campaign::Campaign::Move::BOTTOM};

    for(auto m : moves)
        camp->moveState(states->index(0, 0, QModelIndex()), m);

    for(auto m : moves)
        camp->moveAlias(model->index(0, 0, QModelIndex()), m);

    camp->addCharacter();
    camp->removeCharacter(Helper::randomString());
}

void CampaignTest::campaignManagerTest()
{
    m_manager->importFile(Helper::randomUrl());
    auto file= Helper::randomString();
    m_manager->createFileFromData(file, Helper::imageData());

    m_manager->removeFile(file);
    m_manager->reload();

    m_manager->openCampaign(Helper::randomUrl());
    m_manager->shareModels();

    QList<QPair<QString, Core::CampaignAction>> actions{{Helper::randomString(), Core::CampaignAction::ForgetAction},
                                                        {Helper::randomString(), Core::CampaignAction::CreateAction},
                                                        {Helper::randomString(), Core::CampaignAction::ManageAction},
                                                        {Helper::randomString(), Core::CampaignAction::DeleteAction}};

    for(auto pair : actions)
    {
        m_manager->performAction(pair.first, pair.second);
    }

    m_manager->importDataFrom(
        Helper::randomString(),
        {Core::CampaignDataCategory::AudioPlayer1, Core::CampaignDataCategory::AudioPlayer2,
         Core::CampaignDataCategory::AudioPlayer3, Core::CampaignDataCategory::Images, Core::CampaignDataCategory::Maps,
         Core::CampaignDataCategory::MindMaps, Core::CampaignDataCategory::Notes, Core::CampaignDataCategory::WebLink,
         Core::CampaignDataCategory::PDFDoc, Core::CampaignDataCategory::DiceAlias,
         Core::CampaignDataCategory::CharacterStates, Core::CampaignDataCategory::Themes,
         Core::CampaignDataCategory::CharacterSheets, Core::CampaignDataCategory::AntagonistList});
}

QTEST_MAIN(CampaignTest)

#include "tst_campaign.moc"
