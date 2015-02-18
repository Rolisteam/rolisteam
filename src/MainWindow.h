/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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


/********************************************************************/
/*                                                                  */
/* Fenetre principale, contenant la palette d'outils, les tableaux  */
/* de joueurs et de PJ, les cartes et les fenetres de dialogue.     */
/*                                                                  */
/********************************************************************/


#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QWorkspace>



#include "ToolBar.h"


#include "Tchat.h"
#include "Image.h"
#include "MinutesEditor.h"
//#include "ClientServeur.h"
#include "audioplayer.h"
#include "userlistwidget.h"
#include "mapwizzarddialog.h"


#include "connection.h"

class PreferencesManager;
class ImprovedWorkspace;
class MapFrame;
class Carte;
class QActionGroup;
class PreferenceDialog;
class ConnectionWizzard;
class CharacterSheetWindow;
class Player;
class MainWindow : public QMainWindow
{
Q_OBJECT

public :
        MainWindow();


        Tchat* trouverTchat(QString idJoueur);
        bool isActiveWindow(QWidget *widget);


public slots :
        void displayTchat(QString id);
        void hideTchat(QString id);
        void displayMinutesEditor(bool display, bool checkAction = false);
        void displayCharacterSheet(bool display, bool checkAction = false);


protected :
        void closeEvent(QCloseEvent *event);

private slots:
    /**
     * @brief is called when user click on start server item menu.
     */
    void startServer();
    /**
     * @brief is called when user click on add connection item menu.
     */
    void addConnection();
    /**
     * @brief is called when user click on connection manager item menu.
     */
    void showConnectionManager();

    /**
     * @brief is called when user click on preference item menu.
     */
    void showPreferenceManager();

    /**
     * @brief is called when user click on usedTabBarAct item menu.
     */
    void onTabBar();

private :
        bool maybeSave();
        void createMenu();
        void connectActions();
        void allowActions();
        void saveAll();

        /**
         * Load informations from the previous rolisteam's execution
         */
        void readSettings();

        /**
         * Save parameters for next executions.
         */
        void writeSettings();

        ImprovedWorkspace* m_workspace;
        QMenu *windowMenu;
        QMenu *tchatSubMenu;
        ToolsBar *m_toolbar;

        MinutesEditor* minutesEditor;
        QList <MapFrame *> listeCarteFenetre;
        QList <Image *> listeImage;
        QList <Tchat *> listeTchat;
        AudioPlayer* m_audioPlayer;

        QMenu *m_fileMenu;
        QMenu* m_newMenu;
        QAction* m_newMapAct;
        QAction* m_newNoteAct;
        QAction* m_newScenarioAct;

        QMenu* m_openMenu;
        QAction* m_openMapAct;
        QAction* m_openMapHiddenAct;
        QAction* m_openPictureAct;
        QAction* m_openScenarioAct;
        QAction* m_openNoteAct;

        QMenu* m_recentlyOpened;

        QAction* m_saveAct;
        QAction* m_saveAsAct;
        QAction* m_saveAllAct;
        QAction* m_saveAllIntoScenarioAct;

        QAction *m_preferencesAct;

        QAction* m_closeAct;
        QAction* m_quitAct;

        //QMenu *m_editMenu;

        QMenu *m_viewMenu;

        QAction* m_usedTabBarAct;
        QMenu* m_organizeMenu;
        QAction* m_cascadeSubWindowsAct;
        QAction* m_tileSubWindowsAct;
        QAction* m_noteEditoAct;
        QAction* m_dataSheetAct;


        //Help menu
        QMenu *m_helpMenu;
        QAction *m_helpAct;
        QAction *m_aproposAct;


        /**
          * pointer to the unique instance of preference manager.
          */
        PreferencesManager* m_options;

        /**
          * pointer to the userlist widget.
          */
        UserListWidget* m_playerListWidget;
        /**
          *
          */
        Player* m_player;

        QMenu* m_currentWindowMenu;



        /**
          * pointer to the network menu
          */
        QMenu* m_networkMenu;
        /**
          * action to start the server in the localhost
          */
        QAction* m_serverAct;
        /**
          * action to display new connection dialog
          */
        QAction* m_newConnectionAct;

        /**
          * action to display the connection manager
          */
        QAction* m_manageConnectionAct;

        QActionGroup* m_connectionActGroup;
        //QList<QAction*> m_registedConnectionList;

        /**
          * List of connections
          */
        ConnectionList m_connectionList;


        /**
          * preference wizzard.
          */
        PreferenceDialog* m_preferenceDialog;

        /**
          * preference wizzard.
          */
        ConnectionWizzard* m_connectDialog;


        /**
          * CharacterSheet viewer
          */
        CharacterSheetWindow* m_characterSheet;

private slots :
        /**
        * @brief Show the map wizzard
        *
        */
        void clickOnMapWizzard();

        /**
        * @brief Show the map wizzard
        *
        */
        void openImage();

        /**
        * \brief Show the about dialog
        *
        */
        void about();


        /// \brief open the Qt assistant with the rolisteam documentation
        void help();




};

#endif
