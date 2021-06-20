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

#include "core/data/cleveruri.h"
#include "rwidgets/mediacontainers/mediacontainer.h"

#include "network/networkreceiver.h"
#include "preferences/preferencesmanager.h"

#include "rwidgets/customs/vmap.h"
#include "rwidgets/docks/vmaptoolbar.h"
#include "rwidgets/mediacontainers/vmapframe.h"
#include "rwidgets/toolbars/vtoolbar.h"

#include "rwidgets/dialogs/selectconnectionprofiledialog.h"
#include "rwidgets/editors/noteeditor/src/notecontainer.h"
#include "rwidgets/editors/sharededitor/sharednotecontainer.h"

#ifndef NULL_PLAYER
#include "rwidgets/docks/audioPlayer.h"
#endif

#include "rwidgets/docks/channellistpanel.h"

#include "controller/gamecontroller.h"
#include "rwidgets/docks/playerspanel.h"

namespace Ui
{
class MainWindow;
}

struct FileInfo
{
    QString path;
    Core::ContentType type;
};

namespace campaign
{
class CampaignDock;
class AntagonistBoard;
} // namespace campaign

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
    MainWindow(GameController* game, const QStringList& args);
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
     * @brief postConnection
     */
    void postConnection();
    /**
     * @brief openImageAs
     * @param pix
     * @param type
     */
    void openImageAs(const QPixmap& pix, Core::ContentType type);
    void makeVisible(bool value);

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
    void processSharedNoteMessage(NetworkMessageReader* msg);
    void showTipChecker();
    virtual void mouseMoveEvent(QMouseEvent* event);
    void createPostSettings();

private slots:
    void cleanUpData();
    void showSupportPage();
    /**
     * @brief userNatureChange
     * @param isGM
     */
    void userNatureChange();
    void openCampaign();
    /**
     * @brief showUpdateNotification
     */
    void showUpdateNotification();
    /**
     * @brief stopReconnection
     */
    void stopReconnection();
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
     * @brief findCharacterSheetWindowById
     * @param id
     * @return
     */
    CharacterSheetWindow* findCharacterSheetWindowById(const QString& idMedia, const QString& idSheet);

private:
    QPointer<GameController> m_gameController;
#ifndef NULL_PLAYER
    std::unique_ptr<AudioPlayer> m_audioPlayer;
#endif
    std::unique_ptr<NotificationZone> m_dockLogUtil;
    std::unique_ptr<CommandLineProfile> m_commandLineProfile;
    std::unique_ptr<campaign::CampaignDock> m_campaignDock;
    std::unique_ptr<campaign::AntagonistBoard> m_antagonistWidget;
    std::unique_ptr<QSystemTrayIcon> m_systemTray;
    std::unique_ptr<Workspace> m_mdiArea;

    // subwindow
    Ui::MainWindow* m_ui;
    QPointer<PreferencesDialog> m_preferencesDialog;
    PreferencesManager* m_preferences;

    // Recent files managment
    int m_maxSizeRecentFile;
    QList<QAction*> m_recentFileActs;
    std::vector<QAction*> m_recentScenarioActions;
    QStringList m_recentScenarios;
    QAction* m_separatorAction;
    std::vector<FileInfo> m_recentFiles;
    QList<GameMasterTool*> m_gmToolBoxList;
    QDockWidget* m_roomPanelDockWidget;
    ChannelListPanel* m_roomPanel;

    bool m_isOut= false;
};

#endif
