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
#include "ListeUtilisateurs.h"

#include "Tchat.h"
#include "Image.h"
#include "EditeurNotes.h"
#include "ClientServeur.h"
#include "audioplayer.h"
#include "userlistdockwidget.h"
#include "mapwizzarddialog.h"

class PreferencesManager;
class ImprovedWorkspace;
class MapFrame;
class Carte;
class MainWindow : public QMainWindow
{
Q_OBJECT

public :
        MainWindow();
        /*void affichageDuPj(QString idPerso, bool afficher);
        void majCouleursPersonnelles();
        void ajouterCarte(MapFrame *carteFenetre, QString titre);
        void ajouterImage(Image *imageFenetre, QString titre);
        void emettreTousLesPlans(QString idJoueur);
        void emettreToutesLesImages(QString idJoueur);
        void mettreAJourEspaceTravail();
        void mettreAJourSelecteurTaille(QString idCarte, int taillePj);
        void emettreChangementTaillePj(int nouvelleTaille);
        void ajouterTchat(QString idJoueur, QString nomJoueur);
        void supprimerTchat(QString idJoueur);
        void devientFenetreActive(QMdiSubWindow *widget);
        void cocherActionTchatCommun();
        void changementNatureUtilisateur();*/

        Image* trouverImage(QString idImage);
        Tchat* trouverTchat(QString idJoueur);
        bool estLaFenetreActive(QWidget *widget);
        bool enleverCarteDeLaListe(QString idCarte);
        bool enleverImageDeLaListe(QString idImage);


public slots :
     //   void changerTaillePj(int nouvelleTaille);

        void afficherTchat(QString id);
        void masquerTchat(QString id);
        void afficherEditeurNotes(bool afficher, bool cocherAction = false);



protected :
        void closeEvent(QCloseEvent *event);


private :
        bool maybeSave();
        QDockWidget* creerLogUtilisateur();
        void creerMenu();
        void associerActionsMenus();
        void autoriserOuInterdireActions();
        void saveAll();
       /* void lireCarteEtPnj(QFile &file, bool masquer = false, QString nomFichier = "");
        void lireImage(QFile &file);
        void sauvegarderTousLesPlans(QFile &file);
        void sauvegarderToutesLesImages(QFile &file);
        void sauvegarderTousLesTchats();*/



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
        QAction *actionPreferences;
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

        QAction *actionAideLogiciel;
        QAction *actionAPropos;

       // ClientServeur* m_clientServer;
        /**
          * pointer to the unique instance of preference manager.
          */
        PreferencesManager* m_options;

        /**
          * pointer to the userlist widget.
          */
        UserListDockWidget* m_playerListDockWidget;


        QMenu* m_currentWindowMenu;

private slots :
       /* void afficherNomsPj(bool afficher);
        void afficherNomsPnj(bool afficher);
        void afficherNumerosPnj(bool afficher);*/
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

       /* void ouvrirPlan(bool masquer = false);

        void ouvrirEtMasquerPlan();
        void ouvrirScenario();
        void ouvrirNotes();
        void fermerPlanOuImage();
        void sauvegarderPlan();*/

        /**
        * \brief Show the about dialog
        *
        */
        void about();


        /// \brief open the Qt assistant with the rolisteam documentation
        void aideEnLigne();
        /*bool sauvegarderScenario();
        bool sauvegarderNotes();*/



};

#endif
