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
#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QUndoStack>
#include <memory>

struct TipOfDay
{
    QString title;
    QString content;
    QString url;
    int id;
};

class QAbstractItemModel;
class ResourcesController;
class LogController;
class LogSenderScheduler;
class RemoteLogController;
class PreferencesManager;
class NetworkController;
class PlayerController;
class PreferencesController;
class ContentController;
class QSystemTrayIcon;
class InstantMessagingController;
class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PreferencesController* preferencesController READ preferencesController CONSTANT)
    Q_PROPERTY(PlayerController* playerController READ playerController CONSTANT)
    Q_PROPERTY(ContentController* contentController READ contentController CONSTANT)
    Q_PROPERTY(QString currentScenario READ currentScenario WRITE setCurrentScenario NOTIFY currentScenarioChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QString localPlayerId READ localPlayerId NOTIFY localPlayerIdChanged)
    Q_PROPERTY(QString remoteVersion READ remoteVersion NOTIFY remoteVersionChanged)
    Q_PROPERTY(TipOfDay tipOfDay READ tipOfDay NOTIFY tipOfDayChanged)
    Q_PROPERTY(bool localIsGM READ localIsGM NOTIFY localIsGMChanged)
    Q_PROPERTY(bool updateAvailable READ updateAvailable WRITE setUpdateAvailable NOTIFY updateAvailableChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
public:
    explicit GameController(QObject* parent= nullptr);
    ~GameController();

    NetworkController* networkController() const;
    PlayerController* playerController() const;
    ContentController* contentController() const;
    PreferencesController* preferencesController() const;
    PreferencesManager* preferencesManager() const;
    InstantMessagingController* instantMessagingController() const;

    QString version() const;
    QString currentScenario() const;
    QString localPlayerId() const;
    QString remoteVersion() const;
    bool localIsGM() const;
    bool updateAvailable() const;
    bool tipAvailable() const;
    bool connected() const;
    QUndoStack* undoStack() const;

    LogController* logController() const;
    TipOfDay tipOfDay() const;

    void clear();

signals:
    void currentScenarioChanged();
    void versionChanged();
    void localPlayerIdChanged();
    void localIsGMChanged(bool);
    void updateAvailableChanged();
    void connectedChanged(bool b);
    void remoteVersionChanged();
    void tipOfDayChanged();

public slots:
    void addErrorLog(const QString& message);
    void addWarningLog(const QString& message);
    void addFeatureLog(const QString& message);
    void addInfoLog(const QString& message);
    void addSearchLog(const QString& message);

    void startCheckForUpdates();
    void startIpRetriever();
    void startTipOfDay();
    void postSettingInit();

    void setCurrentScenario(const QString& path);
    void setVersion(const QString& version);
    void setUpdateAvailable(bool available);
    void startConnection(int profileIndex);
    void stopConnection();
    void sendDataToServerAtConnection();
    void aboutToClose();
    void setLocalPlayerId(const QString& id);

private:
    void addCommand(QUndoCommand* cmd);

private:
    std::unique_ptr<ResourcesController> m_resourcesController;
    std::unique_ptr<LogController> m_logController;
    std::unique_ptr<RemoteLogController> m_remoteLogCtrl;
    std::unique_ptr<NetworkController> m_networkCtrl;
    std::unique_ptr<PlayerController> m_playerController;
    std::unique_ptr<PreferencesController> m_preferencesDialogController;
    std::unique_ptr<ContentController> m_contentCtrl;
    std::unique_ptr<PreferencesManager> m_preferences;
    std::unique_ptr<InstantMessagingController> m_instantMessagingCtrl;

    QString m_currentScenario;
    QString m_version;
    QString m_remoteVersion;
    bool m_updateAvailable= false;
    TipOfDay m_tipOfTheDay;
    std::unique_ptr<QUndoStack> m_undoStack;
};

#endif // GAMECONTROLLER_H
