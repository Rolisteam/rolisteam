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

#include "data/cleveruri.h"
#include "map/map.h"

#include "map/charactertoken.h"
#include "initialisation.h"
#include "preferences/preferencesmanager.h"
#include "services/ipchecker.h"

#include "network/networkreceiver.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "data/mediacontainer.h"

#ifndef NULL_PLAYER
#include "audioPlayer.h"
#endif

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
class NetworkManager;
class TextEdit;
class PlayersList;
/**
 * @brief Main widget for rolisteam, it herits from QMainWindow.
 */
class MainWindow : public QMainWindow, public NetWorkReceiver
{
    Q_OBJECT

public :

	/**
	*
	*/
    ~MainWindow();
    /**
     * @brief getInstance
     * @return
     */
    static MainWindow* getInstance();
//    /**
//     * @brief majCouleursPersonnelles
//     */
//    void majCouleursPersonnelles();

    /**
     * @brief addMap
	 * @param BipMapWindow
     * @param titre
     * @param mapsize
     * @param pos
     */
	QWidget* addMap(MapFrame *MapFrame, QString titre,QSize mapsize=QSize(),QPoint pos=QPoint());
    /**
     * @brief addImage
     * @param imageFenetre
     * @param titre
     */
    void addImage(Image *imageFenetre, QString titre);
    /**
     * @brief mettreAJourEspaceTravail
     */
    void updateWorkspace();
    /**
     * @brief trouverCarte
     * @param idCarte
     * @return
     */
    Map *findMapById(QString idCarte);
    /**
     * @brief removeMapFromId
     * @param idCarte
     * @return
     */
    void removeMapFromId(QString idCarte);

    /**
     * @brief removePictureFromId
     * @param idImage
     * @return
     */
    void removePictureFromId(QString idImage);

    /**
     * @brief enleverImageDeLaListe
     * @param idImage
     * @return
     */
    //bool enleverImageDeLaListe(QString idImage);

    /**
     * @brief registerSubWindow
     * @param subWindow
     */
    QWidget* registerSubWindow(QWidget * subWindow, QAction* action);
    /**
     * @brief showConnectionDialog
     * @return
     */
    bool showConnectionDialog();
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
    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);

    /**
     * @brief prepareMap
     * @param mapFrame
     */
    void prepareMap(MapFrame* mapFrame);
    /**
     * @brief addMediaToMdiArea
     * @param mediac
     */
    void addMediaToMdiArea(MediaContainer* mediac );

signals:
    void closing();

public slots :
    /**
     * @brief notifyUser
     * @param msg
     */
    void notifyUser(QString msg) const;
    /**
	 * @brief displayMinutesEditor
     * @param afficher
     * @param cocherAction
     */
    void displayMinutesEditor(bool afficher, bool cocherAction = false);
    /**
     * @brief quitterApplication
     * @param perteConnexion
     */
    void quitterApplication(bool perteConnexion = false);
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
    void extractCharacter(Map* map,NetworkMessageReader* msg);
    CleverURI* contentToPath(CleverURI::ContentType type,bool save);

private slots :
    void changementNatureUtilisateur();
    void changementFenetreActive(QMdiSubWindow* widget);
    void newMap();
    void openStory();
    void openNote();
    void closeMapOrImage();
    void saveMap();
    void updateMayBeNeeded();
	void emettreTousLesPlans(NetworkLink * link);
	void emettreToutesLesImages(NetworkLink * link);
    void updateSessionToNewClient(Player* player);
    void receiveData(quint64 readData,quint64 size);
    void openContent();

    //Network private Slot
    void stopReconnection();
    void closeConnection();
    void startReconnection();
    void networkStateChanged(bool state);
    void openContentFromType(CleverURI::ContentType type);
    void openCleverURI(CleverURI* uri);
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

private :
    MainWindow();
    static MainWindow* m_singleton;
    void creerLogUtilisateur();
    void linkActionToMenu();

	//QMdiSubWindow*  readMapAndNpc(QDataStream &file, bool masquer = false, QString nomFichier = "");
    void readImageFromStream(QDataStream &file);
    void saveAllMap(QDataStream &file);
    void saveAllImages(QDataStream &file);
    /**
     * @brief workspace
     */
	ImprovedWorkspace* m_mdiArea;
    PlayersListWidget * m_playersListWidget;
	ToolsBar* m_toolBar;
    QMap<QString,MapFrame *> m_mapWindowMap;
    QList <Image*> m_pictureList;
    QMap<MediaContainer*,QAction*>* m_mapAction;
#ifndef NULL_PLAYER   
    AudioPlayer* m_audioPlayer;
#endif

    PreferencesDialog * m_preferencesDialog;
	PreferencesManager* m_preferences;
    ChatListWidget * m_chatListWidget;
    UpdateChecker* m_updateChecker;

    QString m_version;
    QDockWidget* m_dockLogUtil;
	NetworkManager* m_networkManager;
    QTextEdit* m_notifierDisplay;
    TextEdit* m_noteEditor;
    PlayersList* m_playerList;
	//QMenuBar* m_menuBar;
    /// @brief get the server IP.
    IpChecker* m_ipChecker;
    //subwindow
    QMdiSubWindow* m_noteEditorSub;
    QProgressBar* m_downLoadProgressbar;
    bool m_shownProgress;
    QString m_localPlayerId;
	bool m_resetSettings;
    Ui::MainWindow* m_ui;
//filters
	QString m_supportedImage;
	QString m_supportedCharacterSheet;
	QString m_supportedNotes;
	QString m_supportedMap;
	QString m_pdfFiles;

    //Recent files managment
    int m_maxSizeRecentFile;
    QList<QAction*> m_recentFileActs;
};

#endif
