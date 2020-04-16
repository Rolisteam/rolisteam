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
#include "gamecontroller.h"

#include "common/controller/logcontroller.h"
#include "common/controller/remotelogcontroller.h"
#include "controller/contentcontroller.h"
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"
#include "controller/preferencescontroller.h"
#include "model/profilemodel.h"
#include "preferences/preferencesmanager.h"
#include "resourcescontroller.h"
#include "services/tipchecker.h"
#include "services/updatechecker.h"
#include "worker/messagehelper.h"

#include "network/connectionprofile.h"

GameController::GameController(QObject* parent)
    : QObject(parent)
    , m_logController(new LogController(false))
    , m_remoteLogCtrl(new RemoteLogController())
    , m_networkCtrl(new NetworkController)
    , m_playerController(new PlayerController)
    , m_preferencesDialogController(new PreferencesController)
    , m_contentCtrl(new ContentController(m_playerController->characterModel(), m_networkCtrl.get()))
    , m_preferences(new PreferencesManager)
    , m_undoStack(new QUndoStack)
{
#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    m_version= QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
#endif
#endif
#endif
    m_preferences->readSettings(m_version);

    m_networkCtrl->setGameController(this);
    m_playerController->setGameController(this);
    m_contentCtrl->setGameController(this);
    m_preferencesDialogController->setGameController(this);

    connect(m_logController.get(), &LogController::sendOffMessage, m_remoteLogCtrl.get(), &RemoteLogController::addLog);
    // connect(, &NetworkController::isGMChanged, this, &GameController::localIsGMChanged);
    connect(m_networkCtrl.get(), &NetworkController::connectedChanged, this, [this](bool b) {
        if(b)
            sendDataToServerAtConnection();

        emit connectedChanged(b);
    });

    connect(m_playerController.get(), &PlayerController::gameMasterIdChanged, m_contentCtrl.get(),
            &ContentController::setGameMasterId);
    connect(m_playerController.get(), &PlayerController::performCommand, this, &GameController::addCommand);
    connect(m_contentCtrl.get(), &ContentController::performCommand, this, &GameController::addCommand);

    m_contentCtrl->setGameMasterId(m_playerController->gameMasterId());

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
    m_logController->setMessageHandler(logDebug);

    auto LogResearch= m_preferences->value(QStringLiteral("LogResearch"), false).toBool();
    auto dataCollection= m_preferences->value(QStringLiteral("dataCollection"), false).toBool();
    m_logController->setSignalInspection(logDebug && (LogResearch || dataCollection));

    LogController::StorageModes mode= LogController::Gui;
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

void GameController::setCurrentScenario(const QString& path)
{
    if(m_currentScenario == path)
        return;
    m_currentScenario= path;
    emit currentScenarioChanged();
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
    /*if(m_localId == id)
        return;
    m_localId= id;
    emit lorcalPlayerIdChanged();*/

    m_remoteLogCtrl->setLocalUuid(id);
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

void GameController::addErrorLog(const QString& message)
{
    m_logController->manageMessage(message, LogController::Error);
}

void GameController::addWarningLog(const QString& message)
{
    m_logController->manageMessage(message, LogController::Warning);
}

void GameController::addFeatureLog(const QString& message)
{
    m_logController->manageMessage(message, LogController::Features);
}

void GameController::addInfoLog(const QString& message)
{
    m_logController->manageMessage(message, LogController::Info);
}

void GameController::addSearchLog(const QString& message)
{
    m_logController->manageMessage(message, LogController::Search);
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
    connect(updateChecker, &UpdateChecker::checkFinished, this, [this, updateChecker]() {
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

    connect(tipChecker, &TipChecker::checkFinished, this, [=]() {
        auto id= m_preferences->value(QStringLiteral("MainWindow::lastTips"), 0).toInt();
        if(tipChecker->hasArticle() && tipChecker->getId() + 1 > id)
        {
            m_tipOfTheDay= {tipChecker->getArticleTitle(), tipChecker->getArticleContent(), tipChecker->getUrl(),
                            tipChecker->getId()};

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

QString GameController::currentScenario() const
{
    return m_currentScenario;
}

QString GameController::localPlayerId() const
{
    return tr("idnull");
    // return m_networkCtrl->localPlayer()->getId();
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

void GameController::startConnection(int profileIndex)
{
    auto profile= m_networkCtrl->profileModel()->getProfile(profileIndex);

    if(profile == nullptr)
        return;

    auto local= m_playerController->localPlayer();

    local->setName(profile->playerName());
    local->setColor(profile->playerColor());
    local->setAvatarPath(profile->playerAvatar());

    if(!local->isGM())
    {
        auto characters= profile->characters();
        std::for_each(characters.begin(), characters.end(), [local](const CharacterData& data) {
            local->addCharacter(data.m_name, data.m_color, data.m_avatarPath, data.m_params, false);
        });
    }
    m_networkCtrl->setHost(profile->address());
    m_networkCtrl->setPort(profile->port());
    m_networkCtrl->setServerPassword(profile->password());
    m_networkCtrl->setIsGM(profile->isGM());
    m_networkCtrl->setHosting(profile->isServer());

    m_playerController->addPlayer(local);

    m_networkCtrl->startConnection();
}

void GameController::stopConnection()
{
    networkController()->stopConnecting();
}

void GameController::sendDataToServerAtConnection()
{
    /*m_preferencesDialog->sendOffAllDiceAlias();
    m_preferencesDialog->sendOffAllState();*/
    if(localIsGM())
        m_preferencesDialogController->shareModels();
}

void GameController::aboutToClose()
{
    // save data
    m_networkCtrl->saveData();

    // close connection
    MessageHelper::sendOffGoodBye();
    m_networkCtrl->closeServer();
    m_preferences->writeSettings(m_version);
}

PreferencesController* GameController::preferencesController() const
{
    return m_preferencesDialogController.get();
}
void GameController::addCommand(QUndoCommand* cmd)
{
    m_undoStack->push(cmd);
}
