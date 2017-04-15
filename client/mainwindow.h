/*************************************************************************
 *     Copyright (C) 2007 by Romain Campioni                             *
 *     Copyright (C) 2009 by Renaud Guezennec                            *
 *     Copyright (C) 2010 by Berenger Morel                              *
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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

#include <QMainWindow>
#include <QAction>
#include <QTextEdit>
#include <QMenu>
#include <QProgressBar>
#include <QMdiSubWindow>
#include <QStackedWidget>

#include "data/cleveruri.h"
#include "data/mediacontainer.h"

//#include "map/map.h"
#include "map/charactertoken.h"

#include "preferences/preferencesmanager.h"

#include "services/ipchecker.h"

#include "network/networkreceiver.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "vmap/vtoolbar.h"
#include "vmap/vmap.h"
#include "vmap/vmapframe.h"
#include "vmap/vmaptoolbar.h"

#include "notecontainer.h"
#include "network/selectconnectionprofiledialog.h"

#ifndef NULL_PLAYER
#include "audio/audioPlayer.h"
#endif

#include "network/networkmanager.h"
#include  "network/channellistpanel.h"

namespace Ui {
class MainWindow;
}

class UpdateChecker;
class ToolsBar;
class MapFrame;
class Map;
class ChatListWidget;
class Image;
class NetworkLink;
class PreferencesDialog;
class Player;
class PlayersListWidget;
class ImprovedWorkspace;
class TextEdit;
class PlayersList;
class ConnectionProfile;
class SessionManager;
class CharacterSheetWindow;
/**
 * @brief Main widget for rolisteam, it herits from QMainWindow.
 */
class MainWindow : public QMainWindow, public NetWorkReceiver
{
    Q_OBJECT

public :
    /**
     * @brief The MessageType enum
     */
    enum MessageType {Information,Notice,Warning,Error};
    /**
     * @brief ~MainWindow
     */
    virtual ~MainWindow();
    /**
     * @brief getInstance
     * @return
     */
    static MainWindow* getInstance();

    /**
     * @brief addMap
	 * @param BipMapWindow
     * @param titre
     * @param mapsize
     * @param pos
     */
	QWidget* addMap(MapFrame *MapFrame, QString titre,QSize mapsize=QSize(),QPoint pos=QPoint());
    /**
     * @brief mettreAJourEspaceTravail
     */
    void updateWorkspace();
    /**
     * @brief trouverCarte
     * @param idMap
     */
    Map* findMapById(QString idMap);
    /**
     * @brief removeMapFromId
     * @param idMap
     */
    void removeMapFromId(QString idMap);
    /**
     * @brief removeVMapFromId
     * @param idMap
     */
    void removeVMapFromId(QString idMap);
    /**
     * @brief removePictureFromId
     * @param idImage
     */
    void removePictureFromId(QString idImage);
    /**
     * @brief registerSubWindow
     * @param subWindow
     */
    QWidget* registerSubWindow(QWidget* subWindow, QAction* action);
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief getPermission
     * @param id
     * @return
     */
    Map::PermissionMode getPermission(int id);
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
	 * @brief getNetWorkManager
	 * @return
	 */
	NetworkManager* getNetWorkManager();
	/**
	 * @brief parseCommandLineArguments
	 */
    void parseCommandLineArguments(QStringList);

	/**
	 * @brief processMessage
	 * @param msg
	 */
    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg, NetworkLink* link);

    /**
     * @brief prepareMap
     * @param mapFrame
     */
    void prepareMap(MapFrame* mapFrame);
    /**
     * @brief prepareVMap
     * @param tmp
     */
    void prepareVMap(VMapFrame* tmp);
    /**
     * @brief addMediaToMdiArea
     * @param mediac
     */
    void addMediaToMdiArea(MediaContainer* mediac );
signals:
    /**
     * @brief closing
     */
    void closing();

public slots :
    /**
     * @brief notifyUser
     * @param msg
     */
    void notifyUser(QString msg, MainWindow::MessageType msgType = Information) const;
    /**
     * @brief quitterApplication
     * @param perteConnexion
     */
    bool mayBeSaved(bool perteConnexion = false);
    /**
     * @brief checkUpdate
     */
    void checkUpdate();
    /**
     * @brief setNetworkManager
     */
	void setNetworkManager(NetworkManager*);
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
    void closeAllImagesAndMaps();

	/**
     * @brief showConnectionDialog
	 */
    void showConnectionDialog(bool forced = false);

    /**
     * @brief startConnection
     */
    void startConnection();

protected :
    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent *event);
    /**
     * @brief prepareImage
     * @param imageFenetre
     */
    void prepareImage(Image *imageFenetre);
    /**
     * @brief processImageMessage
     * @param msg
     */
	void processImageMessage(NetworkMessageReader* msg);
    /**
     * @brief processMapMessage
     * @param msg
     */
    void processMapMessage(NetworkMessageReader* msg);
    /**
     * @brief processNpcMessage
     * @param msg
     */
    void processNpcMessage(NetworkMessageReader* msg);
    /**
     * @brief processPaintingMessage
     * @param msg
     */
    void processPaintingMessage(NetworkMessageReader* msg);
    /**
     * @brief processCharacterMessage
     * @param msg
     */
    void processCharacterMessage(NetworkMessageReader* msg);
    /**
     * @brief processConnectionMessage
     * @param msg
     */
    void processConnectionMessage(NetworkMessageReader* msg);
    /**
     * @brief processCharacterPlayerMessage
     * @param msg
     */
    void processCharacterPlayerMessage(NetworkMessageReader* msg);
    /**
     * @brief processMediaMessage
     * @param msg
     */
    void processMediaMessage(NetworkMessageReader *msg);
    /**
     * @brief processVMapMessage
     * @param msg
     * @return
     */
    NetWorkReceiver::SendType processVMapMessage(NetworkMessageReader* msg);
    /**
     * @brief extractCharacter
     * @param map
     * @param msg
     */
    void extractCharacter(Map* map,NetworkMessageReader* msg);
    /**
     * @brief contentToPath
     * @param type
     * @param save
     * @return
     */
    CleverURI* contentToPath(CleverURI::ContentType type,bool save);
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
     * @brief prepareCharacterSheetWindow
     * @param window
     */
    void prepareCharacterSheetWindow(CharacterSheetWindow *window);
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
    void saveMedia(MediaContainer *mediaC,bool AskPath, bool saveAs);
    void readStory(QString fileName);
    void prepareNote(NoteContainer *note);
protected slots:
    /**
     * @brief closeMediaContainer
     * @param id
     */
    void closeMediaContainer(QString id);
private slots :
    /**
     * @brief userNatureChange
     * @param isGM
     */
    void userNatureChange(bool isGM);
    /**
     * @brief activeWindowChanged
     * @param widget
     */
    void activeWindowChanged(QMdiSubWindow* widget);
    /**
     * @brief newMap
     */
    void newMap();
    /**
     * @brief newVectorialMap
     */
    void newVectorialMap();
    /**
     * @brief openStory
     */
    void openStory();
    /**
     * @brief openNote
     */
    void openNote();
    /**
     * @brief closeCurrentSubWindow
     */
    void closeCurrentSubWindow();
    /**
     * @brief updateMayBeNeeded
     */
    void updateMayBeNeeded();
    /**
     * @brief sendOffAllMaps
     * @param link
     */
	void sendOffAllMaps(NetworkLink * link);
    /**
     * @brief sendOffAllImages
     * @param link
     */
	void sendOffAllImages(NetworkLink * link);
    /**
     * @brief updateSessionToNewClient
     * @param player
     */
    void updateSessionToNewClient(Player* player);
    /**
     * @brief receiveData
     * @param readData
     * @param size
     */
    void receiveData(quint64 readData,quint64 size);
    /**
     * @brief openContent
     */
    void openContent();
    //save methods
    /**
     * @brief saveCurrentMedia
     */
    void saveCurrentMedia();
    //Network private Slot
    /**
     * @brief stopReconnection
     */
    void stopReconnection();
    /**
     * @brief closeConnection
     */
    void closeConnection();
    /**
     * @brief startReconnection
     */
    void startReconnection();
    /**
     * @brief networkStateChanged
     * @param state
     */
    void networkStateChanged(NetworkManager::ConnectionState state);
    /**
     * @brief openContentFromType
     * @param type
     */
    void openContentFromType(CleverURI::ContentType type);
    /**
     * @brief openCleverURI
     * @param uri
     * @param force
     */
    void openCleverURI(CleverURI* uri,bool force = false);
    /**
     * @brief newNoteDocument
     */
    void newNoteDocument();
    /**
     * @brief setLatestFile
     * @param fileName
     */
    void setLatestFile(CleverURI* fileName);
    /**
     * @brief updateRecentFileActions
     */
    void updateRecentFileActions();
    /**
     * @brief openRecentFile
     */
    void openRecentFile();

    /**
    * \brief Show the about dialog
    *
    */
    void aboutRolisteam();

    /// \brief open the Qt assistant with the rolisteam documentation
    /**
     * @brief helpOnLine
     */
    void helpOnLine();
    /**
     * @brief saveStory - saves all media contener into the current story file.
     *
     */
    bool saveStory();
    /**
     * @brief saveAsStory asks filename to save current story into.
     */
    void saveAsStory();
    /**
     * @brief saveMinutes saves notes into file.
     * @return
     */
    bool saveMinutes();
    /**
     * @brief notifyAboutAddedPlayer
     * @param player
     */
    void notifyAboutAddedPlayer(Player * player) const;
    /**
     * @brief notifyAboutDeletedPlayer
     * @param player
     */
    void notifyAboutDeletedPlayer(Player * player) const;

    /**
     * @brief showIp displays ip address of the server - feature request #76
     * @param ip
     */
    void showIp(QString ip);
    /**
     * @brief newCharacterSheetWindow
     */
    void newCharacterSheetWindow();



private :
    /**
     * @brief MainWindow
     */
    MainWindow();
    /**
     * @brief showCleverUri
     * @param uri
     */
    void showCleverUri(CleverURI *uri);

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
     * @brief readImageFromStream
     * @param file
     */
    void readImageFromStream(QDataStream &file);
    /**
     * @brief getContentType
     * @param str
     * @return
     */
    CleverURI::ContentType getContentType(QString str);
    /**
     * @brief findCharacterSheetWindowById
     * @param id
     * @return
     */
    CharacterSheetWindow *findCharacterSheetWindowById(QString id);
    /**
     * @brief getShortNameFromPath generic tool to translate filepath to short name.
     * @param path
     * @return
     */
    QString getShortNameFromPath(QString path);
private:
    static MainWindow* m_singleton;
	ImprovedWorkspace* m_mdiArea;
    PlayersListWidget* m_playersListWidget;

    //toolbar
    ToolsBar* m_toolBar;
    VToolsBar* m_vToolBar;
    QStackedWidget* m_toolBarStack;

    QHash<QString,MediaContainer*> m_mediaHash;
    QMap<MediaContainer*,QAction*>* m_mapAction;
#ifndef NULL_PLAYER   
    AudioPlayer* m_audioPlayer;
#endif

    PreferencesDialog* m_preferencesDialog;
	PreferencesManager* m_preferences;
    ChatListWidget* m_chatListWidget;
    UpdateChecker* m_updateChecker;

    QString m_version;
    QDockWidget* m_dockLogUtil;
	NetworkManager* m_networkManager;
    QTextEdit* m_notifierDisplay;
    PlayersList* m_playerList;
    IpChecker* m_ipChecker; /// @brief get the server IP.

    //subwindow
    QProgressBar* m_downLoadProgressbar;
    bool m_shownProgress;
    QString m_localPlayerId;
    Ui::MainWindow* m_ui;
    SessionManager* m_sessionManager;
    bool m_resetSettings;

    //Recent files managment
    int m_maxSizeRecentFile;
    QList<QAction*> m_recentFileActs;
    VmapToolBar* m_vmapToolBar;

    ConnectionProfile* m_currentConnectionProfile;
    QList<QWidget*> m_gmToolBoxList;
    SelectConnectionProfileDialog* m_dialog;
    bool m_profileDefined;
    CleverURI* m_currentStory;
    QDockWidget* m_roomPanelDockWidget;
};

#endif
