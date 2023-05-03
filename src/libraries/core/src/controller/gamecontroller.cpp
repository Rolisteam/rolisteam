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
#include "worker/autosavecontroller.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"

#include "network/connectionprofile.h"

GameController::GameController(const QString& appname, const QString& version, QClipboard* clipboard, QObject* parent)
    : QObject(parent)
    , m_diceParser(new DiceRoller)
    , m_logController(new LogController(true))
    , m_remoteLogCtrl(new RemoteLogController())
    , m_networkCtrl(new NetworkController)
    , m_playerController(new PlayerController)
    , m_preferencesDialogController(new PreferencesController)
    , m_campaignManager(new campaign::CampaignManager(m_diceParser.get()))
    , m_contentCtrl(new ContentController(m_campaignManager.get(), m_playerController->model(),
                                          m_playerController->characterModel(), clipboard, m_networkCtrl.get()))
    , m_preferences(new PreferencesManager(appname, QString("%1_%2/preferences").arg(appname, version)))
    , m_instantMessagingCtrl(new InstantMessagingController(m_playerController->model()))
    , m_audioCtrl(new AudioController(m_campaignManager.get(), m_preferences.get()))
    , m_version(version)
    , m_undoStack(new QUndoStack)
    , m_autoSaveCtrl(new AutoSaveController(m_preferences.get()))
{
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
                if(b)
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
    connect(m_networkCtrl.get(), &NetworkController::isGMChanged, m_campaignManager.get(), &campaign::CampaignManager::setLocalIsGM);
    connect(m_networkCtrl.get(), &NetworkController::isGMChanged, m_audioCtrl.get(), &AudioController::setLocalIsGM);
    connect(m_campaignManager.get(), &campaign::CampaignManager::campaignLoaded, this, &GameController::dataLoaded);
    // clang-format on

    m_contentCtrl->setGameMasterId(m_playerController->gameMasterId());
    m_remoteLogCtrl->setLocalUuid(m_playerController->localPlayerId());
    m_contentCtrl->setLocalId(m_playerController->localPlayerId());
    m_instantMessagingCtrl->setLocalId(m_playerController->localPlayerId());
    m_instantMessagingCtrl->setDiceParser(m_diceParser.get());
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
    // Log controller
    auto logDebug= m_preferences->value(QStringLiteral("LogDebug"), false).toBool();
    // m_logController->setMessageHandler(logDebug);

    auto LogResearch= m_preferences->value(QStringLiteral("LogResearch"), false).toBool();
    auto dataCollection= m_preferences->value(QStringLiteral("dataCollection"), false).toBool();
    m_logController->setSignalInspection(logDebug && (LogResearch || dataCollection));

    LogController::StorageModes mode= LogController::Gui | LogController::Console;

    if(localIsGM())
        mode|= LogController::File;

    if((LogResearch && dataCollection))
    {
        /*      m_logController->listenObjects(this);
              mode= LogController::Gui | LogController::Network;
              setFocusPolicy(Qt::StrongFocus);
              auto clipboard= QGuiApplication::clipboard();
              connect(clipboard, &QClipboard::dataChanged, this, [clipboard, this]() {
                  auto text= clipboard->text();
                  auto mime= clipboard->mimeData();
                  text.append(QStringLiteral("\n%1").arg(mime->formats().join("|")));
                  m_logController->manageMessage(QStringLiteral("Clipboard data changed: %1").arg(text),
                      LogController::Search);
              });*/
    }
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
    m_contentCtrl->openMedia(other.toStdMap());
}

void GameController::save()
{
    ModelHelper::saveSession(m_contentCtrl.get());
    ModelHelper::saveAudioController(m_audioCtrl.get());
    m_campaignManager->saveCampaign();
}

void GameController::saveAs(const QString& path)
{
    ModelHelper::saveSession(m_contentCtrl.get());
    ModelHelper::saveAudioController(m_audioCtrl.get());
    m_campaignManager->copyCampaign(path);
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
        std::for_each(characters.begin(), characters.end(),
                      [local](const connection::CharacterData& data)
                      { local->addCharacter(data.m_name, data.m_color, data.m_avatarData, data.m_params, false); });
    }
    m_networkCtrl->setHost(profile->address());
    m_networkCtrl->setPort(profile->port());
    m_networkCtrl->setServerPassword(profile->password());
    m_networkCtrl->setIsGM(profile->isGM());
    m_networkCtrl->setHosting(profile->isServer());
    m_networkCtrl->setAskForGM(profile->isGM());

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

void GameController::postConnection() {}

void GameController::aboutToClose()
{
    save();
    // save data
    m_networkCtrl->saveData();

    // close connection
    MessageHelper::sendOffGoodBye();
    m_networkCtrl->closeServer();
    m_preferences->writeSettings();
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
