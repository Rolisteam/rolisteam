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
//#include "ListeUtilisateurs.h"

#include "Tchat.h"
#include "Image.h"
#include "EditeurNotes.h"
//#include "ClientServeur.h"
#include "audioplayer.h"
#include "userlistdockwidget.h"
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
class MainWindow : public QMainWindow
{
Q_OBJECT

public :
        MainWindow();


        Image* trouverImage(QString idImage);
        Tchat* trouverTchat(QString idJoueur);
        bool estLaFenetreActive(QWidget *widget);
        bool enleverCarteDeLaListe(QString idCarte);
        bool enleverImageDeLaListe(QString idImage);


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

private :
        bool maybeSave();
        QDockWidget* creerLogUtilisateur();
        void createMenu();
        void associerActionsMenus();
        void autoriserOuInterdireActions();
        void saveAll();



        /**
         * Load informations from the previous rolisteam's execution
         */
        void readSettings();

        /**
         * Save parameters for next executions.
         */
        void writeSettings();

        QDockWidget *dockLogUtil;
        ImprovedWorkspace* m_workspace;
        QMenu *menuFenetre;
        QMenu *sousMenuTchat;
        ToolsBar *m_toolbar;

        EditeurNotes *editeurNotes;
        QList <MapFrame *> listeCarteFenetre;
        QList <Image *> listeImage;
        QList <Tchat *> listeTchat;
        AudioPlayer* m_audioPlayer;

        QAction *newMapAction;
        QAction *OpenImageAction;
        QAction *actionOuvrirPlan;
        QAction *actionOuvrirEtMasquerPlan;
        QAction *actionOuvrirScenario;
        QAction *actionOuvrirNotes;
        QAction *actionFermerPlan;
        QAction *actionSauvegarderPlan;
        QAction *actionSauvegarderScenario;
        QAction *actionSauvegarderNotes;
        QAction *m_preferencesAct;
        QAction *actionQuitter;

        QAction *actionAfficherNomsPj;
        QAction *actionAfficherNomsPnj;
        QAction *actionAfficherNumerosPnj;
        QAction *actionSansGrille;
        QAction *actionCarre;
        QAction *actionHexagones;

        QAction *actionCascade;
        QAction *actionTuiles;
        QAction *actionEditeurNotes;
        QAction *actionTchatCommun;
        QAction* m_showDataSheet;

        QAction *actionHelp;
        QAction *actionAPropos;



        /**
          * pointer to the unique instance of preference manager.
          */
        PreferencesManager* m_options;

        /**
          * pointer to the userlist widget.
          */
        UserListDockWidget* m_playerListDockWidget;


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
        void changementFenetreActive(QMdiSubWindow *widget);

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
        void aideEnLigne();




};

#endif
