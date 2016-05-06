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

#include "map/map.h"
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
#include "audioPlayer.h"
#endif

#include "network/networkmanager.h"

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
    QWidget* registerSubWindow(QWidget * subWindow, QAction* action);
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
    void notifyUser(QString msg,MessageType msgType = Information) const;
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
	 * @return
	 */
    bool showConnectionDialog(bool forced = false);

    void startConnection();
protected :
    void closeEvent(QCloseEvent *event);
    void prepareImage(Image *imageFenetre);
	void processImageMessage(NetworkMessageReader* msg);
    void processMapMessage(NetworkMessageReader* msg);
    void processNpcMessage(NetworkMessageReader* msg);
    void processPaintingMessage(NetworkMessageReader* msg);
    void processCharacterMessage(NetworkMessageReader* msg);
    void processConnectionMessage(NetworkMessageReader* msg);
    void processCharacterPlayerMessage(NetworkMessageReader* msg);
    NetWorkReceiver::SendType processVMapMessage(NetworkMessageReader* msg);
    void extractCharacter(Map* map,NetworkMessageReader* msg);
    CleverURI* contentToPath(CleverURI::ContentType type,bool save);
    void dropEvent(QDropEvent* event);
    void dragEnterEvent(QDragEnterEvent* ev);

    void prepareCharacterSheetWindow(CharacterSheetWindow *window);
private slots :
    void userNatureChange();
    void activeWindowChanged(QMdiSubWindow* widget);
    void newMap();
    void newVectorialMap();
    void openStory();
    void openNote();
    void closeMapOrImage();
    void updateMayBeNeeded();
	void sendOffAllMaps(NetworkLink * link);
	void sendOffAllImages(NetworkLink * link);
    void updateSessionToNewClient(Player* player);
    void receiveData(quint64 readData,quint64 size);
    void openContent();



    //save methods
    void saveCurrentMedia();
    //void saveMap(MapFrame* mapWindow);




    //Network private Slot
    void stopReconnection();
    void closeConnection();
    void startReconnection();
    void networkStateChanged(NetworkManager::ConnectionState state);
    void openContentFromType(CleverURI::ContentType type);
    void openCleverURI(CleverURI* uri,bool force = false);
    void newNoteDocument();
    void setLatestFile(CleverURI* fileName);
    void updateRecentFileActions();
    void openRecentFile();

    /**
    * \brief Show the about dialog
    *
    */
    void aboutRolisteam();

    /// \brief open the Qt assistant with the rolisteam documentation
    void helpOnLine();
    bool saveStory();
    bool saveMinutes();
    void notifyAboutAddedPlayer(Player * player) const;
    void notifyAboutDeletedPlayer(Player * player) const;

    /**
     * @brief showIp displays ip address of the server - feature request #76
     * @param ip
     */
    void showIp(QString ip);
    void newCharacterSheetWindow();



private :
    MainWindow();
    void showCleverUri(CleverURI *uri);

private:
    static MainWindow* m_singleton;
    void createNotificationZone();
    void linkActionToMenu();

	//QMdiSubWindow*  readMapAndNpc(QDataStream &file, bool masquer = false, QString nomFichier = "");
    void readImageFromStream(QDataStream &file);
    void saveAllMap(QDataStream &file);
    void saveAllImages(QDataStream &file);
    CleverURI::ContentType getContentType(QString str);
    /**
     * @brief workspace
     */
	ImprovedWorkspace* m_mdiArea;
    PlayersListWidget* m_playersListWidget;

    //toolbar
    ToolsBar* m_toolBar;
    VToolsBar* m_vToolBar;
    QStackedWidget* m_toolBarStack;


    QMap<QString,MapFrame *> m_mapWindowMap;
    QMap<QString,VMapFrame *> m_mapWindowVectorialMap;
    QMap<QString,NoteContainer*> m_noteMap;
    QHash<QString,CharacterSheetWindow*> m_sheetHash;
    QHash<QString,Image*> m_pictureHash;
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

    CharacterSheetWindow *findCharacterSheetWindowById(QString id);
};

#endif
