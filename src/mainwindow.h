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




#include "map/DessinPerso.h"
#include "initialisation.h"
#include "preferencesmanager.h"
#include "ipchecker.h"
#include "map/mapwizzard.h"
#include "map/newemptymapdialog.h"

#include "network/networkreceiver.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#ifndef NULL_PLAYER
#include "audioPlayer.h"
#endif

class UpdateChecker;
class ToolBar;
class BipMapWindow;
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
    /**
     * @brief majCouleursPersonnelles
     */
    void majCouleursPersonnelles();

    /**
     * @brief addMap
	 * @param BipMapWindow
     * @param titre
     * @param mapsize
     * @param pos
     */
	QWidget* addMap(BipMapWindow *BipMapWindow, QString titre,QSize mapsize=QSize(),QPoint pos=QPoint());
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
    Map *trouverCarte(QString idCarte);
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
     * @brief enleverCarteDeLaListe
     * @param idCarte
     * @return
     */
    bool enleverCarteDeLaListe(QString idCarte);
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
     * @brief notifyUser
     * @param msg
     */
    static void notifyUser(QString msg);
    /**
     * @brief notifyUser_p
     * @param msg
     */
    void notifyUser_p(QString msg);

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

signals:
    void closing();

public slots :
    /**
	 * @brief buildNewMap
     * @param titre
     * @param idCarte
     * @param couleurFond
     * @param largeur
     * @param hauteur
     */
    void buildNewMap(QString titre, QString idCarte, QColor couleurFond, QSize size,Map::PermissionMode mode );
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

    void addImageToMdiArea(Image *imageFenetre, QString titre);

	void processImageMessage(NetworkMessageReader* msg);
    void processMapMessage(NetworkMessageReader* msg);
    void processNpcMessage(NetworkMessageReader* msg);
    void processPaintingMessage(NetworkMessageReader* msg);
    void processCharacterMessage(NetworkMessageReader* msg);
    void processConnectionMessage(NetworkMessageReader* msg);
    void processCharacterPlayerMessage(NetworkMessageReader* msg);


    void extractCharacter(Map* map,NetworkMessageReader* msg);
private slots :
    void changementNatureUtilisateur();
    void afficherNomsPj(bool afficher);
    void afficherNomsPnj(bool afficher);
    void afficherNumerosPnj(bool afficher);
    void changementFenetreActive(QMdiSubWindow* widget);
    void newMap();
    void openMap(Map::PermissionMode Permission,QString filepath,QString title,bool masquer = false);
    void openImage();
    void openMapWizzard();
    void ouvrirScenario();
    void openNote();
    void closeMapOrImage();
    void saveMap();
    void updateMayBeNeeded();
	void emettreTousLesPlans(NetworkLink * link);
	void emettreToutesLesImages(NetworkLink * link);
	void updateSessionToNewClient(NetworkLink* link);
    void receiveData(quint64 readData,quint64 size);


    //Network private Slot
    void stopReconnection();
    void closeConnection();
    void startReconnection();
    void networkStateChanged(bool state);





    /**
    * \brief Show the about dialog
    *
    */
    void aboutRolisteam();

    /// \brief open the Qt assistant with the rolisteam documentation
    void helpOnLine();
    bool sauvegarderScenario();
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
    void creerMenu();
    void linkActionToMenu();

    QMdiSubWindow*  readMapAndNpc(QDataStream &file, bool masquer = false, QString nomFichier = "");
    void readImageFromStream(QDataStream &file);
    void saveAllMap(QDataStream &file);
    void saveAllImages(QDataStream &file);



    /**
     * @brief Add a health state to a list
     *
     * @param &color QColor reference to use to initialise the state
     * @param &label QString reference that contain... the label of the state
     * @param &listHealthState reference to the list
     * @return Whether or not the windows was successfully hidden.
     */
    void AddHealthState(const QColor &color, const QString &label, QList<DessinPerso::etatDeSante> &listHealthState);

    /**
     * @brief Initialize a mouse pointer
     *
     * @param *pointer QCursor pointer to the cursor to initialise
     * @param iconFileName Filename of the icon to use
     * @param hotX same as in QCursor's constructor
     * @param hotY same as in QCursor's constructor
     * @return Whether or not the windows was successfully hidden.
     */
    void InitMousePointer(QCursor **pointer, const QString &iconFileName, const int hotX, const int hotY);

    /**
     * @brief workspace
     */
	ImprovedWorkspace* m_mdiArea;
    PlayersListWidget * m_playersListWidget;
    QMenu *m_windowMenu;
    ToolBar *m_toolBar;

	QList <BipMapWindow *> m_mapWindowList;
    //QList <Image *> listeImage;
    QList <QMdiSubWindow*> m_pictureList;
	QMap<BipMapWindow*,QAction*>* m_mapAction;
#ifndef NULL_PLAYER   
    AudioPlayer* m_audioPlayer;
#endif

    PreferencesDialog * m_preferencesDialog;

    ChatListWidget * m_chatListWidget;

    //submenu and action for map and parameters.
    QAction* m_newMapAct;
    QAction* m_openImageAct;
    QAction* m_openMapAct;
    //QAction* actionOuvrirEtMasquerPlan;
    QAction* m_openStoryAct;
    QAction* m_openMinutesAct;
    QAction* m_closeMap;
    QAction* m_saveMapAct;
    QAction* m_saveStoryAct;
    QAction* m_saveMinuteAct;
    QAction* m_preferencesAct;
    QAction* m_quitAct;

    QAction* m_reconnectAct;
    QAction* m_disconnectAct;

    QAction *m_showPCAct;
    QAction *m_showNpcNameAct;
    QAction *m_showNPCNumberAct;
    QAction *actionSansGrille;
    QAction *actionCarre;
    QAction *actionHexagones;

    QAction *m_cascadeAction;
    QAction *m_tuleAction;
    QAction* m_tabOrdering;
    QAction *m_noteEditorAct;

    QAction *m_helpAct;
    QAction *m_aboutAct;
    UpdateChecker* m_updateChecker;

    QString m_version;
    QDockWidget* m_dockLogUtil;
    PreferencesManager* m_preferences;

	NetworkManager* m_networkManager;

    QTextEdit* m_notifierDisplay;

    TextEdit* m_noteEditor;

    PlayersList* m_playerList;
    QMenuBar* m_menuBar;

    /// @brief get the server IP.
    IpChecker* m_ipChecker;


    NewEmptyMapDialog* m_newEmptyMapDialog;/// @brief dialog to create new map.

    MapWizzard* m_mapWizzard;

    //subwindow
    QMdiSubWindow* m_noteEditorSub;
    QProgressBar* m_downLoadProgressbar;
    bool m_shownProgress;
};

#endif
