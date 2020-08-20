/*************************************************************************
 *     Copyright (C) 2007 by Romain Campioni                             *
 *     Copyright (C) 2009 by Renaud Guezennec                            *
 *     Copyright (C) 2010 by Berenger Morel                              *
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QAction>
#include <QMainWindow>
#include <QMdiSubWindow>
#include <QMenu>
#include <QProgressBar>
#include <QStackedWidget>
#include <QTextEdit>
#include <QUndoStack>
#include <memory>

#include "data/cleveruri.h"
#include "data/mediacontainer.h"

#include "network/networkreceiver.h"
#include "preferences/preferencesmanager.h"

#include "vmap/vmap.h"
#include "vmap/vmapframe.h"
#include "vmap/vmaptoolbar.h"
#include "vmap/vtoolbar.h"

#include "network/selectconnectionprofiledialog.h"
#include "noteeditor/src/notecontainer.h"
#include "sharededitor/sharednotecontainer.h"

#ifndef NULL_PLAYER
#include "audio/audioPlayer.h"
#endif

#include "network/channellistpanel.h"

#include "controller/gamecontroller.h"
#include "userlist/playerspanel.h"

namespace Ui
{
class MainWindow;
}

struct FileInfo
{
    QString path;
    Core::ContentType type;
};

class UpdateChecker;
// class ChatListWidget;
class Image;
class NetworkLink;
class PreferencesDialog;
class Player;
class PlayersListWidget;
class Workspace;
class TextEdit;
class PlayerModel;
class ConnectionProfile;
class SessionDock;
class CharacterSheetWindow;
class GameMasterTool;
class LogPanel;
class ServerManager;
class NotificationZone;

struct CommandLineProfile
{
    QString m_ip;
    int m_port;
    QByteArray m_pass;
};
/**
 * @brief Main widget for rolisteam, it herits from QMainWindow.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief MainWindow
     */
    MainWindow(const QStringList& args);
    /**
     * @brief ~MainWindow
     */
    virtual ~MainWindow();
    /**
     * @brief
     */
    void updateWorkspace();
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief readSettings
     */
    void readSettings();
    /**
     * @brief writeSettings
     */
    void writeSettings();
    /**
     * @brief setUpNetworkConnection
     */
    void setUpNetworkConnection();
    /**
     * @brief parseCommandLineArguments
     */
    void parseCommandLineArguments(const QStringList&);
    /**
     * @brief setLatestFile
     * @param fileName
     */
    void setLatestFile(CleverURI* fileName);

    void openRecentScenario();
    void updateRecentScenarioAction();
    void openGenericContent();
    void openOnlineImage();
    void openVMap();
    void openMap();
    void newMap();
public slots:
    /**
     * @brief
     * @param
     */
    bool mayBeSaved(bool connectionLost= false);
    /**
     * @brief updateUi
     */
    void updateUi();
    /**
     * @brief updateWindowTitle
     */
    void updateWindowTitle();
    /**
     * @brief closeAllImagesAndMap - remove all maps and Pictures
     */
    void closeAllMediaContainer();

    /**
     * @brief showConnectionDialog
     */
    void showConnectionDialog(bool forced= false);
    /**
     * @brief postConnection
     */
    void postConnection();
    /**
     * @brief openImageAs
     * @param pix
     * @param type
     */
    void openImageAs(const QPixmap& pix, Core::ContentType type);
    void showAsPreferences();

protected:
    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent* event);
    virtual void focusInEvent(QFocusEvent* event);
    virtual void focusOutEvent(QFocusEvent* event);
    /**
     * @brief contentToPath
     * @param type
     * @param save
     * @return
     */
    CleverURI* contentToPath(Core::ContentType type, bool save);
    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QDropEvent* event);
    /**
     * @brief dragEnterEvent
     * @param ev
     */
    void dragEnterEvent(QDragEnterEvent* ev);
    /**
     * @brief saveAllMediaContainer
     */
    void saveAllMediaContainer();
    /**
     * @brief saveMedia
     * @param mediaC
     * @param AskPath
     * @param saveAs
     */
    void saveMedia(MediaContainer* mediaC, bool saveAs);
    void readStory(QString fileName);
    void processSharedNoteMessage(NetworkMessageReader* msg);
    void showTipChecker();
    virtual void mouseMoveEvent(QMouseEvent* event);
    void createPostSettings();
protected slots:
    void cleanUpData();
private slots:
    void showSupportPage();
    /**
     * @brief userNatureChange
     * @param isGM
     */
    void userNatureChange(bool isGM);
    /**
     * @brief openStory
     */
    void openStory();
    /**
     * @brief showUpdateNotification
     */
    void showUpdateNotification();
    // save methods
    /**
     * @brief saveCurrentMedia
     */
    void saveCurrentMedia();
    // Network private Slot
    /**
     * @brief stopReconnection
     */
    void stopReconnection();
    void openResource(ResourcesNode* node, bool force);
    /**
     * @brief updateRecentFileActions
     */
    void updateRecentFileActions();
    /**
     * @brief openRecentFile
     */
    void openRecentFile();
    /**
     * @brief helpOnLine
     */
    void helpOnLine();
    /**
     * @brief saveStory - saves all media contener into the current story file.
     *
     */
    bool saveStory(bool saveAs);
    /**
     * @brief saveMinutes saves notes into file.
     * @return
     */
    bool saveMinutes();
    /**
     * @brief notifyAboutAddedPlayer
     * @param player
     */
    void notifyAboutAddedPlayer(Player* player) const;
    /**
     * @brief notifyAboutDeletedPlayer
     * @param player
     */
    void notifyAboutDeletedPlayer(Player* player) const;
    void showShortCutEditor();
    void newVMap();

private:
    /**
     * @brief createNotificationZone
     */
    void createNotificationZone();
    /**
     * @brief linkActionToMenu
     */
    void linkActionToMenu();
    /**
     * @brief getContentType
     * @param str
     * @return
     */
    Core::ContentType getContentType(QString str);
    /**
     * @brief findCharacterSheetWindowById
     * @param id
     * @return
     */
    CharacterSheetWindow* findCharacterSheetWindowById(const QString& idMedia, const QString& idSheet);
    /**
     * @brief getShortNameFromPath generic tool to translate filepath to short
     * name.
     * @param path
     * @return
     */
    QString getShortNameFromPath(QString path);

private:
    PlayersPanel* m_playersListWidget= nullptr;

#ifndef NULL_PLAYER
    AudioPlayer* m_audioPlayer;
#endif

    PreferencesDialog* m_preferencesDialog;
    PreferencesManager* m_preferences;
    NotificationZone* m_dockLogUtil;
    PlayerModel* m_playerModel;

    // subwindow
    Ui::MainWindow* m_ui;
    bool m_resetSettings;

    // Recent files managment
    int m_maxSizeRecentFile;
    QList<QAction*> m_recentFileActs;
    std::vector<QAction*> m_recentScenarioActions;
    QStringList m_recentScenarios;
    QAction* m_separatorAction;
    std::vector<FileInfo> m_recentFiles;

    ConnectionProfile* m_currentConnectionProfile= nullptr;
    QList<GameMasterTool*> m_gmToolBoxList;
    bool m_profileDefined= false;
    QDockWidget* m_roomPanelDockWidget;
    ChannelListPanel* m_roomPanel;
    QString m_connectionAddress;
    bool m_isOut= false;

    std::unique_ptr<CommandLineProfile> m_commandLineProfile;
    std::unique_ptr<GameController> m_gameController;
    std::unique_ptr<SelectConnectionProfileDialog> m_dialog;
    std::unique_ptr<SessionDock> m_sessionDock;
    std::unique_ptr<QSystemTrayIcon> m_systemTray;
    std::unique_ptr<Workspace> m_mdiArea;
};

#endif
