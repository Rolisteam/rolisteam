/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "controller/gamecontroller.h"

#include <QTimer>

#include "common/logcontroller.h"
#include "common/remotelogcontroller.h"

#include "data/campaigneditor.h"

#include "data/media.h"
#include "model/profilemodel.h"
#include "preferences/preferencesmanager.h"
#include "services/tipchecker.h"
#include "services/updatechecker.h"
#include "updater/controller/dicephysicupdater.h"
#include "utils/iohelper.h"
#include "worker/autosavecontroller.h"
#include "worker/iohelper.h"
#include "worker/mediahelper.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"
#include "worker/utilshelper.h"

#include "network/connectionprofile.h"

GameController::GameController(const QString& appname, const QString& version, QClipboard* clipboard, QObject* parent)
    : QObject(parent)
    , m_diceParser(new DiceRoller)
    , m_logController(new LogController(true))
    , m_remoteLogCtrl(new RemoteLogController())
    , m_networkCtrl(new NetworkController)
    , m_playerController(new PlayerController(m_networkCtrl.get()))
    , m_preferencesDialogController(new PreferencesController)
    , m_campaignManager(new campaign::CampaignManager(m_diceParser.get()))
    , m_contentCtrl(new ContentController(m_campaignManager.get(), m_playerController->model(),
                                          m_playerController->characterModel(), clipboard, m_networkCtrl.get()))
    , m_preferences(new PreferencesManager(appname, QString("%1_%2/preferences").arg(appname, version)))
    , m_instantMessagingCtrl(new InstantMessagingController(m_diceParser.get(), m_playerController->model()))
    , m_imUpdater(new InstantMessaging::InstantMessagingUpdater(m_instantMessagingCtrl.get()))
    , m_audioCtrl(new AudioController(m_campaignManager.get(), m_preferences.get()))
    , m_dicePhysicController(new Dice3DController)
    , m_version(version)
    , m_undoStack(new QUndoStack)
    , m_autoSaveCtrl(new AutoSaveController(m_preferences.get()))
{
    new DicePhysicUpdater(m_dicePhysicController.get(), this);
    m_preferences->readSettings();
    postSettingInit();

    m_networkCtrl->setGameController(this);
    m_playerController->setGameController(this);
    m_contentCtrl->setGameController(this);
    m_preferencesDialogController->setGameController(this);

    connect(m_logController.get(), &LogController::sendOffMessage, m_remoteLogCtrl.get(), &RemoteLogController::addLog);
    connect(m_networkCtrl.get(), &NetworkController::connectedChanged, this,
            [this](bool b)
            {
                if(b && m_playerController->localIsGm())
                {
                    m_campaignManager->shareModels();
                }
                emit connectedChanged(b);
            });

    connect(m_campaignManager.get(), &campaign::CampaignManager::campaignChanged, this,
            [this]()
            {
                auto cm= m_campaignManager->campaign();
                m_contentCtrl->setMediaRoot(cm->directory(campaign::Campaign::Place::MEDIA_ROOT));
                m_logController->setCurrentPath(QString("%1/rolisteam.log").arg(cm->rootDirectory()));
            });
    connect(m_campaignManager->campaign(), &campaign::Campaign::rootDirectoryChanged, this,
            [this]()
            {
                auto cm= m_campaignManager->campaign();
                m_contentCtrl->setMediaRoot(cm->directory(campaign::Campaign::Place::MEDIA_ROOT));
                emit campaignRootChanged();
            });
    connect(m_campaignManager->editor(), &campaign::CampaignEditor::performCommand, this, &GameController::addCommand);

    // clang-format off
    connect(m_autoSaveCtrl.get(), &AutoSaveController::saveData, m_campaignManager.get(), &campaign::CampaignManager::saveCampaign);
    connect(m_playerController.get(), &PlayerController::gameMasterIdChanged, m_contentCtrl.get(), &ContentController::setGameMasterId);
    connect(m_playerController.get(), &PlayerController::performCommand, this, &GameController::addCommand);
    connect(m_playerController.get(), &PlayerController::localPlayerIdChanged, m_remoteLogCtrl.get(), &RemoteLogController::setLocalUuid);
    connect(m_playerController.get(), &PlayerController::localPlayerIdChanged, m_remoteLogCtrl.get(), &RemoteLogController::setLocalUuid);
    connect(m_playerController.get(), &PlayerController::localPlayerIdChanged, m_instantMessagingCtrl.get(), &InstantMessagingController::setLocalId);
    connect(m_playerController.get(), &PlayerController::localPlayerIdChanged, m_contentCtrl.get(), &ContentController::setLocalId);
    connect(m_playerController.get(), &PlayerController::localPlayerChanged, m_contentCtrl.get(), [this](){
        auto local = m_playerController->localPlayer();
        if(!local)
            return;
        m_contentCtrl->setLocalColor(local->getColor());
    });

    connect(m_contentCtrl.get(), &ContentController::performCommand, this, &GameController::addCommand);
    auto popCommand = [this]()
    {
        m_undoStack->setIndex(m_undoStack->index()-1);
    };
    connect(m_contentCtrl.get(), &ContentController::popCommand, this, popCommand);
    connect(m_instantMessagingCtrl.get(), &InstantMessagingController::openWebPage, this, &GameController::openPageWeb);
    connect(m_dicePhysicController.get(), &Dice3DController::diceRolled, m_instantMessagingCtrl.get(), &InstantMessagingController::translateDiceResult, Qt::QueuedConnection);

    connect(m_networkCtrl.get(), &NetworkController::isGMChanged, m_campaignManager.get(), &campaign::CampaignManager::setLocalIsGM);
    connect(m_networkCtrl.get(), &NetworkController::isGMChanged, m_audioCtrl.get(), &AudioController::setLocalIsGM);
    connect(m_campaignManager.get(), &campaign::CampaignManager::campaignLoaded, this, [this](campaign::CampaignInfo info){
        emit dataLoaded(info.missingFiles, info.unmanagedFiles);
        m_campaignManager->diceparser();
        ModelHelper::fetchDice3d(m_dicePhysicController.get(),
                                         utils::IOHelper::loadFile(m_campaignManager->placeDirectory(campaign::Campaign::Place::DICE_3D_FILE)));
        m_imUpdater->load(m_campaignManager->placeDirectory(campaign::Campaign::Place::IM_FILE));

        auto c = m_campaignManager->campaign();
        if(!c || !c->loadSession())
            return;
        auto list = info.openDocuments;
        auto assets = info.assets;
        std::for_each(std::begin(assets), std::end(assets),[this, list](const QJsonValue& ref){
                auto obj = ref.toObject();
                auto root = m_campaignManager->placeDirectory(campaign::Campaign::Place::MEDIA_ROOT);
                auto path = QString("%1/%2").arg(root,obj[Core::keys::KEY_PATH].toString());
                auto uuid = obj[Core::keys::KEY_UUID].toString();
                if(list.contains(uuid))
                    openInternalResources(uuid, path, helper::utils::extensionToContentType(path));
        });
    });
    // clang-format on

    m_preferences->registerLambda("Messaging::SaveChatrooms",
                                  [this](const QVariant& var) { m_imUpdater->setSaveChatrooms(var.toBool()); });

    m_contentCtrl->setGameMasterId(m_playerController->gameMasterId());
    m_remoteLogCtrl->setLocalUuid(m_playerController->localPlayerId());
    m_contentCtrl->setLocalId(m_playerController->localPlayerId());
    m_instantMessagingCtrl->setLocalId(m_playerController->localPlayerId());
    m_audioCtrl->setLocalIsGM(m_networkCtrl->isGM());
    m_campaignManager->setLocalIsGM(m_networkCtrl->isGM());

    m_remoteLogCtrl->setAppId(0);
}
GameController::~GameController()= default;

void GameController::clear()
{
    m_playerController->clear();
    m_undoStack->clear();
}

void GameController::postSettingInit()
{
    m_logController->setLogLevel(
        m_preferences->value(QStringLiteral("LogLevel"), LogController::Error).value<LogController::LogLevel>());

    LogController::StorageModes mode= LogController::Gui | LogController::Console;

    if(localIsGM())
        mode|= LogController::File;

    m_logController->setCurrentModes(mode);
}

PreferencesManager* GameController::preferencesManager() const
{
    return m_preferences.get();
}

void GameController::setCampaignRoot(const QString& path)
{
    auto campaign= m_campaignManager->campaign();
    campaign->setRootDirectory(path);
}

void GameController::setVersion(const QString& version)
{
    if(version == m_version)
        return;
    m_version= version;
    emit versionChanged();
}

void GameController::setLocalPlayerId(const QString& id)
{
    m_remoteLogCtrl->setLocalUuid(id);
}

void GameController::newMedia(const std::map<QString, QVariant>& map)
{
    m_contentCtrl->newMedia(m_campaignManager->editor(), map);
}

void GameController::openMedia(const std::map<QString, QVariant>& map)
{
    QMap<QString, QVariant> other(map);
    if(localIsGM())
    {
        if(other.contains(Core::keys::KEY_URL) && !other.contains(Core::keys::KEY_INTERNAL))
        {
            auto path= other.value(Core::keys::KEY_URL).toUrl().toLocalFile();
            other[Core::keys::KEY_PATH]= m_campaignManager->importFile(QUrl::fromLocalFile(path));
        }
        else if(other.contains(Core::keys::KEY_DATA))
        {
            auto name= other.value(Core::keys::KEY_NAME).toString();
            auto data= other.value(Core::keys::KEY_DATA).toByteArray();
            other[Core::keys::KEY_PATH]= m_campaignManager->createFileFromData(name, data);
        }
    }

    namespace hu= helper::utils;
    namespace cv= Core::keys;
    using std::placeholders::_1;
    other.insert(Core::keys::KEY_LOCALISGM, localIsGM());

    if(!other.contains(Core::keys::KEY_SERIALIZED))
        hu::setParamIfAny<QUrl>(cv::KEY_URL, map,
                                [&other](const QUrl& path)
                                { other.insert(Core::keys::KEY_SERIALIZED, utils::IOHelper::loadFile(path.path())); });

    m_contentCtrl->openMedia(other.toStdMap());
}

void GameController::openInternalResources(const QString& id, const QString& path, Core::ContentType type)
{
    std::map<QString, QVariant> vec;
    vec.insert({Core::keys::KEY_PATH, path});
    vec.insert({Core::keys::KEY_UUID, id});
    vec.insert({Core::keys::KEY_TYPE, QVariant::fromValue(type)});
    vec.insert({Core::keys::KEY_SERIALIZED, utils::IOHelper::loadFile(path)});
    vec.insert({Core::keys::KEY_INTERNAL, true});
    auto localId= localPlayerId();
    vec.insert({Core::keys::KEY_OWNERID, localId});
    vec.insert({Core::keys::KEY_LOCALID, localId});
    openMedia(vec);
}

void GameController::save()
{
    if(!localIsGM())
        return;

    ModelHelper::saveSession(m_contentCtrl.get());
    ModelHelper::saveDice3d(m_dicePhysicController.get(),
                            m_campaignManager->placeDirectory(campaign::Campaign::Place::DICE_3D_FILE));
    m_audioCtrl->saveStates();

    m_campaignManager->saveCampaign();
    m_imUpdater->save(m_campaignManager->placeDirectory(campaign::Campaign::Place::IM_FILE));
}

void GameController::saveAs(const QString& path)
{
    if(!localIsGM())
        return;

    m_campaignManager->copyCampaign(path);
    save();
    /*ModelHelper::saveSession(m_contentCtrl.get());
    m_audioCtrl->saveStates();
    ModelHelper::saveDice3d(m_dicePhysicController.get(),
                            m_campaignManager->placeDirectory(campaign::Campaign::Place::DICE_3D_FILE));*/
}

void GameController::openPageWeb(const QString& urlText)
{
    newMedia(MediaHelper::prepareWebView(urlText, m_playerController->localIsGm()));
}

void GameController::setUpdateAvailable(bool available)
{
    if(available == m_updateAvailable)
        return;
    m_updateAvailable= available;
    emit updateAvailableChanged();
}

ContentController* GameController::contentController() const
{
    return m_contentCtrl.get();
}

LogController* GameController::logController() const
{
    return m_logController.get();
}

void GameController::addErrorLog(const QString& message, const QString& cat)
{
    m_logController->manageMessage(message, cat, LogController::Error);
}

void GameController::addWarningLog(const QString& message, const QString& cat)
{
    m_logController->manageMessage(message, cat, LogController::Warning);
}

void GameController::addFeatureLog(const QString& message, const QString& cat)
{
    m_logController->manageMessage(message, cat, LogController::Features);
}

void GameController::addInfoLog(const QString& message, const QString& cat)
{
    m_logController->manageMessage(message, cat, LogController::Info);
}

void GameController::addSearchLog(const QString& message, const QString& cat)
{
    m_logController->manageMessage(message, cat, LogController::Search);
}

NetworkController* GameController::networkController() const
{
    return m_networkCtrl.get();
}

PlayerController* GameController::playerController() const
{
    return m_playerController.get();
}

QString GameController::remoteVersion() const
{
    return m_remoteVersion;
}

void GameController::startCheckForUpdates()
{
    if(!m_preferences->value(QStringLiteral("MainWindow::MustBeChecked"), true).toBool())
        return;

    auto updateChecker= new UpdateChecker(m_version, this);
    updateChecker->startChecking();
    connect(updateChecker, &UpdateChecker::checkFinished, this,
            [this, updateChecker]()
            {
                setUpdateAvailable(updateChecker->needUpdate());
                m_remoteVersion= updateChecker->getLatestVersion();
                updateChecker->deleteLater();
            });
}

void GameController::startIpRetriever()
{
    if(m_preferences->value(QStringLiteral("MainWindow::neverDisplayTips"), false).toBool())
        return;
    TipChecker* tipChecker= new TipChecker(this);
    tipChecker->startChecking();

    connect(tipChecker, &TipChecker::checkFinished, this,
            [=]()
            {
                auto id= m_preferences->value(QStringLiteral("MainWindow::lastTips"), 0).toInt();
                if(tipChecker->hasArticle() && tipChecker->getId() + 1 > id)
                {
                    m_tipOfTheDay= {tipChecker->getArticleTitle(), tipChecker->getArticleContent(),
                                    tipChecker->getUrl(), tipChecker->getId()};

                    m_preferences->registerValue(QStringLiteral("MainWindow::lastTips"), m_tipOfTheDay.id);
                    emit tipOfDayChanged();
                }
                tipChecker->deleteLater();
            });
}

TipOfDay GameController::tipOfDay() const
{
    return m_tipOfTheDay;
}

void GameController::startTipOfDay() {}

QString GameController::version() const
{
    return m_version;
}

QString GameController::campaignRoot() const
{
    return m_campaignManager->campaignDir();
}

InstantMessagingController* GameController::instantMessagingController() const
{
    return m_instantMessagingCtrl.get();
}

campaign::CampaignManager* GameController::campaignManager() const
{
    return m_campaignManager.get();
}

campaign::Campaign* GameController::campaign() const
{
    return m_campaignManager->campaign();
}

AudioController* GameController::audioController() const
{
    return m_audioCtrl.get();
}

QString GameController::localPlayerId() const
{
    return m_playerController->localPlayerId();
}

bool GameController::localIsGM() const
{
    return m_networkCtrl->isGM();
}

bool GameController::updateAvailable() const
{
    return m_updateAvailable;
}

bool GameController::connected() const
{
    return m_networkCtrl->connected();
}

QUndoStack* GameController::undoStack() const
{
    return m_undoStack.get();
}

DiceRoller* GameController::diceParser() const
{
    return m_diceParser.get();
}

void GameController::setDataFromProfile(int profileIndex)
{
    auto profile= m_networkCtrl->profileModel()->getProfile(profileIndex);
    m_networkCtrl->setSelectedProfileIndex(profileIndex);

    if(profile == nullptr)
        return;

    auto local= m_playerController->localPlayer();

    local->setUuid(profile->playerId());
    local->setName(profile->playerName());
    local->setColor(profile->playerColor());
    local->setAvatar(profile->playerAvatar());
    local->setGM(profile->isGM());
    if(!local->isGM())
    {
        auto characters= profile->characters();
        std::for_each(
            characters.begin(), characters.end(),
            [local](const connection::CharacterData& data)
            { local->addCharacter(data.m_uuid, data.m_name, data.m_color, data.m_avatarData, data.m_params, false); });
    }

    m_playerController->addPlayer(local);
    if(profile->isGM())
    {
        m_campaignManager->openCampaign(QUrl::fromLocalFile(profile->campaignPath()));
    }
    else
    {
        emit dataLoaded({}, {});
    }
}

void GameController::startConnection()
{
    m_networkCtrl->startConnection();
}

void GameController::stopConnection()
{
    networkController()->stopConnecting();
}

void GameController::postConnection()
{
    // save data
    m_networkCtrl->saveData();
}

void GameController::aboutToClose()
{
    save();
    // save data
    m_networkCtrl->saveData();

    // close connection
    MessageHelper::sendOffGoodBye();
    m_networkCtrl->closeServer();
    m_preferences->writeSettings();
    m_preferences->registerValue(QStringLiteral("LogLevel"), QVariant::fromValue(m_logController->logLevel()));
    emit closingApp();
}

PreferencesController* GameController::preferencesController() const
{
    return m_preferencesDialogController.get();
}
void GameController::addCommand(QUndoCommand* cmd)
{
    m_undoStack->push(cmd);
}

Dice3DController* GameController::dicePhysicController() const
{
    return m_dicePhysicController.get();
}
