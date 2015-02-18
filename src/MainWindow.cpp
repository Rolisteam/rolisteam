/*************************************************************************
 *        Copyright (C) 2007 by Romain Campioni                          *
 *        Copyright (C) 2009 by Renaud Guezennec                         *
 *        Copyright (C) 2010 by Berenger Morel                           *
 *        Copyright (C) 2010 by Joseph Boudou                            *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                 *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
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


#include <QtGui>
#include <QDebug>


#include "CarteFenetre.h"
#include "Carte.h"
#include "MainWindow.h"
#include "BarreOutils.h"
#include "ListeUtilisateurs.h"
#include "constantesGlobales.h"
#include "variablesGlobales.h"
#include "NouveauPlanVide.h"
#include "Image.h"
#ifndef NULL_PLAYER
#include "LecteurAudio.h"
#endif
#include "EditeurNotes.h"
#include "WorkspaceAmeliore.h"
#include "preferencesdialog.h"

// Necessaires pour utiliser l'instruction ShellExecute
#ifdef WIN32
#include <windows.h>
#include <shellapi.h>
#endif

/********************************************************************/
/* Variables globales utilisees par tous les elements de                */
/* l'application                                                        */
/********************************************************************/
// Indique si le nom des PJ doit etre affiche ou pas
bool G_affichageNomPj;
// Indique si le nom des PNJ doit etre affiche ou pas
bool G_affichageNomPnj;
// Indique si le numero des PNJ doit etre affiche ou pas
bool G_affichageNumeroPnj;
// Indique si une CarteFenetre est actuellement active
bool G_carteFenetreActive;
// Contient le pointeur de souris pour dessiner
QCursor *G_pointeurDessin;
// Contient le pointeur de souris pour le texte
QCursor *G_pointeurTexte;
// Contient le pointeur de souris pour deplacer les PJ/PNJ
QCursor *G_pointeurDeplacer;
// Contient le pointeur de souris pour orienter les PJ/PNJ
QCursor *G_pointeurOrienter;
// Contient le pointeur de souris pour la pipette (clic droit)
QCursor *G_pointeurPipette;
// Contient le pointeur de souris pour ajouter un PNJ
QCursor *G_pointeurAjouter;
// Contient le pointeur de souris pour supprimer en PNJ
QCursor *G_pointeurSupprimer;
// Contient le pointeur de souris pour changer l'etat des PJ/PNJ
QCursor *G_pointeurEtat;


// Pointeur vers la fenetre de log utilisateur (utilise seulement dans ce fichier)
static QTextEdit *logUtilisateur;


/********************************************************************/
/* Affiche un message dans la fenetre de log utilisateur. Peut etre */
/* appelee par n'importe quel element de l'application                  */
/********************************************************************/
void ecrireLogUtilisateur(QString message)
{
        static bool alternance = false;
        QColor couleur;

        // Changement de la couleur
        alternance = !alternance;
        if (alternance)
                couleur = Qt::darkBlue;
        else
                couleur = Qt::darkRed;

        // Ajout de l'heure
        QString heure = QTime::currentTime().toString("hh:mm:ss") + " - ";

        // On repositionne le curseur a la fin du QTextEdit
        logUtilisateur->moveCursor(QTextCursor::End);

        // Affichage de l'heure
        logUtilisateur->setTextColor(Qt::darkGreen);
        logUtilisateur->append(heure);

        // Affichage du texte
        logUtilisateur->setTextColor(couleur);
        logUtilisateur->insertPlainText(message);
}

/********************************************************************/
/* Constructeur                                                         */
/********************************************************************/
MainWindow::MainWindow()
        : QMainWindow()
{
        // Initialisation des variables globales
        G_affichageNomPj = true;
        G_affichageNomPnj = true;
        G_affichageNumeroPnj = true;
        G_carteFenetreActive = false;

        // Initialisation de la liste des CarteFenetre, des Image et des Tchat
        listeCarteFenetre.clear();
        listeImage.clear();
        listeTchat.clear();

        // Initialisation du pointeur vers la fenetre de parametrage de nouveau plan
        fenetreNouveauPlan = 0;

        // Desactivation des animations
        setAnimated(false);
        // Creation de l'espace de travail
        workspace = new WorkspaceAmeliore();
        // Ajout de l'espace de travail dans la fenetre principale
        setCentralWidget(workspace);
        // Connexion du changement de fenetre active avec la fonction de m.a.j du selecteur de taille des PJ
        connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(changementFenetreActive(QWidget *)));

        // Creation de la barre d'outils
        barreOutils = new BarreOutils(this);
        // Ajout de la barre d'outils a la fenetre principale
        addDockWidget(Qt::LeftDockWidgetArea, barreOutils);

        // Creation du log utilisateur
        dockLogUtil = creerLogUtilisateur();
        // Ajout du log utilisateur a la fenetre principale
        addDockWidget(Qt::RightDockWidgetArea, dockLogUtil);

        // Ajout de la liste d'utilisateurs a la fenetre principale
        m_listeUtilisateurs = new ListeUtilisateurs(this);
        addDockWidget(Qt::RightDockWidgetArea, m_listeUtilisateurs);

#ifndef NULL_PLAYER
        // Creation du lecteur audio
        G_lecteurAudio = LecteurAudio::getInstance(this);
        // Ajout du lecteur audio a la fenetre principale
        addDockWidget(Qt::RightDockWidgetArea, G_lecteurAudio);
#endif

        // Create Preference dialog
        m_preferencesDialog = new PreferencesDialog(this);

        // Creation de la barre de menus et des menus
        creerMenu();
        // Association des actions des menus avec des fonctions
        associerActionsMenus();
        // Autoriser/interdire action en fonction de la nature de l'utilisateur (joueur ou MJ)
        autoriserOuInterdireActions();

        // Creation de l'editeur de notes
        editeurNotes = new EditeurNotes();
        // Ajout de l'editeur de notes au workspace
        workspace->addWindow(editeurNotes);
        // Mise a jour du titre de l'editeur de notes
        editeurNotes->setWindowTitle(tr("Editeur de notes"));
        // Masquage de l'editeur de notes
        editeurNotes->hide();

        // Initialisation des etats de sante des PJ/PNJ (variable declarees dans DessinPerso.cpp)
        AddHealthState(Qt::black, tr("Sain"), G_etatsDeSante);
        AddHealthState(QColor(255, 100, 100),tr("Blessé léger"),G_etatsDeSante);
        AddHealthState(QColor(255, 0, 0),tr("Blessé grave"),G_etatsDeSante);
        AddHealthState(Qt::gray,tr("Mort"),G_etatsDeSante);
        AddHealthState(QColor(80, 80, 255),tr("Endormi"),G_etatsDeSante);
        AddHealthState(QColor(0, 200, 0),tr("Ensorcelé"),G_etatsDeSante);

        // Initialisation des pointeurs de souris
        InitMousePointer(&G_pointeurDessin, ":/resources/icones/pointeur dessin.png", 8, 8);
        InitMousePointer(&G_pointeurTexte, ":/resources/icones/pointeur texte.png", 4, 13); //strange values here


        G_pointeurDeplacer        = new QCursor(QPixmap(":/resources/icones/pointeur deplacer.png"), 0, 0);
        G_pointeurOrienter        = new QCursor(QPixmap(":/resources/icones/pointeur orienter.png"), 10, 12);
        G_pointeurPipette        = new QCursor(QPixmap(":/resources/icones/pointeur pipette.png"), 1, 19);
        G_pointeurAjouter        = new QCursor(QPixmap(":/resources/icones/pointeur ajouter.png"), 6, 0);
        G_pointeurSupprimer        = new QCursor(QPixmap(":/resources/icones/pointeur supprimer.png"), 6, 0);
        G_pointeurEtat                = new QCursor(QPixmap(":/resources/icones/pointeur etat.png"), 0, 0);
}

/********************************************************************/
/* Creation du log utilisateur                                          */
/********************************************************************/
QDockWidget* MainWindow::creerLogUtilisateur()
{
        // Creation du dockWidget contenant la fenetre de log utilisateur
        QDockWidget *dockLogUtil = new QDockWidget(tr("Evènements"), this);
        dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockLogUtil->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

        //Creation du log utilisateur
        logUtilisateur = new QTextEdit(dockLogUtil);
        logUtilisateur->setReadOnly(true);

        // Insertion de la fenetre de log utilisateur dans le dockWidget
        dockLogUtil->setWidget(logUtilisateur);
        // Largeur minimum du log utilisateur
        dockLogUtil->setMinimumWidth(125);

        return dockLogUtil;
}

/********************************************************************/
/* Creation de la barre de menus, des menus, des actions associees  */
/********************************************************************/
void MainWindow::creerMenu()
{
        // Creation de la barre de menus
        QMenuBar *barreMenus = new QMenuBar(this);
        // Ajout de la barre de menus a la fenetre principale
        setMenuBar(barreMenus);

        // Creation du menu Fichier
        QMenu *menuFichier = new QMenu (tr("Fichier"), barreMenus);
        actionNouveauPlan                = menuFichier->addAction(tr("Nouveau plan vide"));
        menuFichier->addSeparator();
        actionOuvrirPlan                 = menuFichier->addAction(tr("Ouvrir plan"));
        actionOuvrirEtMasquerPlan        = menuFichier->addAction(tr("Ouvrir et masquer plan"));
        actionOuvrirScenario         = menuFichier->addAction(tr("Ouvrir scénario"));
        actionOuvrirImage                 = menuFichier->addAction(tr("Ouvrir image"));
        actionOuvrirNotes                = menuFichier->addAction(tr("Ouvrir notes"));
        menuFichier->addSeparator();
        actionFermerPlan                 = menuFichier->addAction(tr("Fermer plan/image"));
        menuFichier->addSeparator();
        actionSauvegarderPlan        = menuFichier->addAction(tr("Sauvegarder plan"));
        actionSauvegarderScenario        = menuFichier->addAction(tr("Sauvegarder scénario"));
        actionSauvegarderNotes           = menuFichier->addAction(tr("Sauvegarder notes"));
        menuFichier->addSeparator();
        actionPreferences                = menuFichier->addAction(tr("Préférences"));
        menuFichier->addSeparator();
        actionQuitter                = menuFichier->addAction(tr("Quitter"));

        // Creation du menu Affichage
        QMenu *menuAffichage = new QMenu (tr("Affichage"), barreMenus);
        actionAfficherNomsPj         = menuAffichage->addAction(tr("Afficher noms PJ"));
        actionAfficherNomsPnj        = menuAffichage->addAction(tr("Afficher noms PNJ"));
        actionAfficherNumerosPnj         = menuAffichage->addAction(tr("Afficher numros PNJ"));
        /*
                // Creation du sous-menu Grille
                QMenu *sousMenuGrille = new QMenu (tr("Grille"), barreMenus);
                actionSansGrille                 = sousMenuGrille->addAction(tr("Aucune"));
                actionCarre                  = sousMenuGrille->addAction(tr("Carrs"));
                actionHexagones                  = sousMenuGrille->addAction(tr("Hexagones"));
                // Ajout du sous-menu Grille au menu Affichage
                menuAffichage->addSeparator();
                menuAffichage->addMenu(sousMenuGrille);
                // Creation du groupe d'actions pour le menu Grille
                QActionGroup *groupeAction = new QActionGroup(barreMenus);
                groupeAction->addAction(actionSansGrille);
                groupeAction->addAction(actionCarre);
                groupeAction->addAction(actionHexagones);
        */

        // Actions checkables
        actionAfficherNomsPj        ->setCheckable(true);
        actionAfficherNomsPnj   ->setCheckable(true);
        actionAfficherNumerosPnj->setCheckable(true);
        /*
                actionSansGrille        ->setCheckable(true);
                actionCarre                 ->setCheckable(true);
                actionHexagones         ->setCheckable(true);
        */

        // Choix des actions selectionnees au depart
        actionAfficherNomsPj        ->setChecked(true);
        actionAfficherNomsPnj   ->setChecked(true);
        actionAfficherNumerosPnj->setChecked(true);
        /*
                actionSansGrille        ->setChecked(true);
        */
        // Creation du menu Fenetre
        menuFenetre = new QMenu (tr("Fenêtre"), barreMenus);

        // Creation du sous-menu Reorganiser
        QMenu *sousMenuReorganise        = new QMenu (tr("Réorganiser"), barreMenus);
        actionCascade                = sousMenuReorganise->addAction(tr("Cascade"));
        actionTuiles                 = sousMenuReorganise->addAction(tr("Tuiles"));
        // Ajout du sous-menu Reorganiser au menu Fenetre
        menuFenetre->addMenu(sousMenuReorganise);
        menuFenetre->addSeparator();

        // Ajout des actions d'affichage des fenetres d'evenement, utilisateurs et lecteur audio
        menuFenetre->addAction(barreOutils->toggleViewAction());
        menuFenetre->addAction(dockLogUtil->toggleViewAction());
        menuFenetre->addAction(G_listeUtilisateurs->toggleViewAction());
#ifndef NULL_PLAYER
        menuFenetre->addAction(G_lecteurAudio->toggleViewAction());
#endif
        menuFenetre->addSeparator();

        // Ajout de l'action d'affichage de l'editeur de notes
        actionEditeurNotes = menuFenetre->addAction(tr("Editeur de notes"));
        actionEditeurNotes->setCheckable(true);
        actionEditeurNotes->setChecked(false);
        // Connexion de l'action avec l'affichage/masquage de l'editeur de notes
        connect(actionEditeurNotes, SIGNAL(triggered(bool)), this, SLOT(afficherEditeurNotes(bool)));

        // Ajout du sous-menu Tchat
        sousMenuTchat = new QMenu (tr("Tchats"), barreMenus);
        menuFenetre->addMenu(sousMenuTchat);

        // Ajout de l'action d'affichage de la fenetre de tchat commun
        actionTchatCommun = sousMenuTchat->addAction(tr("Tchat commun"));
        actionTchatCommun->setCheckable(true);
        actionTchatCommun->setChecked(false);
        menuFenetre->addSeparator();

        // Creation du tchat commun
        listeTchat.append(new Tchat("", actionTchatCommun));
        // Ajout du tchat commun au workspace
        workspace->addWindow(listeTchat[0]);
        // Mise a jour du titre du tchat commun
        listeTchat[0]->setWindowTitle(tr("Tchat commun"));
        // Masquage du tchat commun
        listeTchat[0]->hide();
        // Connexion de l'action avec l'affichage/masquage du tchat commun
        connect(actionTchatCommun, SIGNAL(triggered(bool)), listeTchat[0], SLOT(setVisible(bool)));

        // Creation du menu Aide
        QMenu *menuAide = new QMenu (tr("Aide"), barreMenus);
        actionAideLogiciel = menuAide->addAction(tr("Aide sur") + " " + NOM_APPLICATION);
        menuAide->addSeparator();
        actionAPropos = menuAide->addAction(tr("A propos de") + " " + NOM_APPLICATION);

        // Ajout des menus a la barre de menus
        barreMenus->addMenu(menuFichier);
        barreMenus->addMenu(menuAffichage);
        barreMenus->addMenu(menuFenetre);
        barreMenus->addMenu(menuAide);
}

/********************************************************************/
/* Association des actions des menus avec des fonctions                 */
/********************************************************************/
void MainWindow::associerActionsMenus()
{
        // file menu
        connect(actionNouveauPlan, SIGNAL(triggered(bool)), this, SLOT(nouveauPlan()));
        connect(actionOuvrirImage, SIGNAL(triggered(bool)), this, SLOT(ouvrirImage()));
        connect(actionOuvrirPlan, SIGNAL(triggered(bool)), this, SLOT(ouvrirPlan()));
        connect(actionOuvrirEtMasquerPlan, SIGNAL(triggered(bool)), this, SLOT(ouvrirEtMasquerPlan()));
        connect(actionOuvrirScenario, SIGNAL(triggered(bool)), this, SLOT(ouvrirScenario()));
        connect(actionOuvrirNotes, SIGNAL(triggered(bool)), this, SLOT(ouvrirNotes()));
        connect(actionFermerPlan, SIGNAL(triggered(bool)), this, SLOT(fermerPlanOuImage()));
        connect(actionSauvegarderPlan, SIGNAL(triggered(bool)), this, SLOT(sauvegarderPlan()));
        connect(actionSauvegarderScenario, SIGNAL(triggered(bool)), this, SLOT(sauvegarderScenario()));
        connect(actionSauvegarderNotes, SIGNAL(triggered(bool)), this, SLOT(sauvegarderNotes()));
        connect(actionPreferences, SIGNAL(triggered(bool)), m_preferencesDialog, SLOT(show()));

        // close
        connect(actionQuitter, SIGNAL(triggered(bool)), this, SLOT(quitterApplication()));

        // Windows managing
        connect(actionCascade, SIGNAL(triggered(bool)), workspace, SLOT(cascade()));
        connect(actionTuiles, SIGNAL(triggered(bool)), workspace, SLOT(tile()));

        // Display
        connect(actionAfficherNomsPj, SIGNAL(triggered(bool)), this, SLOT(afficherNomsPj(bool)));
        connect(actionAfficherNomsPnj, SIGNAL(triggered(bool)), this, SLOT(afficherNomsPnj(bool)));
        connect(actionAfficherNumerosPnj, SIGNAL(triggered(bool)), this, SLOT(afficherNumerosPnj(bool)));

        // Help
        connect(actionAPropos, SIGNAL(triggered()), this, SLOT(aPropos()));
        connect(actionAideLogiciel, SIGNAL(triggered()), this, SLOT(aideEnLigne()));
}

/********************************************************************/
/* Autorise ou interdit certains menus selon que l'utilisateur est  */
/* MJ ou joueur                                                         */
/********************************************************************/
void MainWindow::autoriserOuInterdireActions()
{
        // L'utilisateur est un joueur
        if (G_joueur)
        {
                actionNouveauPlan->setEnabled(false);
                actionOuvrirPlan->setEnabled(false);
                actionOuvrirEtMasquerPlan->setEnabled(false);
                actionOuvrirScenario->setEnabled(false);
                actionFermerPlan->setEnabled(false);
                actionSauvegarderPlan->setEnabled(false);
                actionSauvegarderScenario->setEnabled(false);
        }

        // L'utilisateur est un MJ
        else
        {
        }
}

/********************************************************************/
/* Creation d'une nouvelle carte dans le workspace                  */
/********************************************************************/
void MainWindow::ajouterCarte(CarteFenetre *carteFenetre, QString titre,QSize mapsize,QPoint pos )
{
        // Ajout de la CarteFenetre a la liste (permet par la suite de parcourir l'ensemble des cartes)
        listeCarteFenetre.append(carteFenetre);

        // Ajout de la carte au workspace
        QWidget* tmp = workspace->addWindow(carteFenetre);
        qDebug() << mapsize << pos;
        if(mapsize.isValid())
            tmp->resize(mapsize);
        if(!pos.isNull())
            tmp->move(pos);


        // Mise a jour du titre de la CarteFenetre
        carteFenetre->setWindowTitle(titre);

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

        // Association de l'action avec la carte
        carteFenetre->associerAction(action);

        // Connexion de l'action avec l'affichage/masquage de la fenetre
        connect(action, SIGNAL(triggered(bool)), carteFenetre, SLOT(setVisible(bool)));

        // Recuperation de la Carte contenue dans la CarteFenetre
        Carte *carte = (Carte *)(carteFenetre->widget());

        // Connexion des actions de changement d'outil avec les fonctions de changement de pointeur de souris
        connect(barreOutils->actionCrayon,         SIGNAL(triggered(bool)), carte, SLOT(pointeurCrayon()));
        connect(barreOutils->actionLigne,          SIGNAL(triggered(bool)), carte, SLOT(pointeurLigne()));
        connect(barreOutils->actionRectVide,   SIGNAL(triggered(bool)), carte, SLOT(pointeurRectVide()));
        connect(barreOutils->actionRectPlein,  SIGNAL(triggered(bool)), carte, SLOT(pointeurRectPlein()));
        connect(barreOutils->actionElliVide,   SIGNAL(triggered(bool)), carte, SLOT(pointeurElliVide()));
        connect(barreOutils->actionElliPlein,  SIGNAL(triggered(bool)), carte, SLOT(pointeurElliPlein()));
        connect(barreOutils->actionTexte,          SIGNAL(triggered(bool)), carte, SLOT(pointeurTexte()));
        connect(barreOutils->actionMain,                SIGNAL(triggered(bool)), carte, SLOT(pointeurMain()));
        connect(barreOutils->actionAjoutPnj,   SIGNAL(triggered(bool)), carte, SLOT(pointeurAjoutPnj()));
        connect(barreOutils->actionSupprPnj,   SIGNAL(triggered(bool)), carte, SLOT(pointeurSupprPnj()));
        connect(barreOutils->actionDeplacePnj, SIGNAL(triggered(bool)), carte, SLOT(pointeurDeplacePnj()));
        connect(barreOutils->actionEtatPnj,        SIGNAL(triggered(bool)), carte, SLOT(pointeurEtatPnj()));

        // Connexion de la demande de changement de couleur de la carte avec celle de la barre d'outils
        connect(carte, SIGNAL(changeCouleurActuelle(QColor)), barreOutils, SLOT(changeCouleurActuelle(QColor)));
        // Connexion de la demande d'incrementation du numero de PNJ de la carte avec celle de la barre d'outils
        connect(carte, SIGNAL(incrementeNumeroPnj()), barreOutils, SLOT(incrementeNumeroPnj()));
        // Connexion de la demande de changement de diametre des PNJ de la carte avec celle de la barre d'outils
        connect(carte, SIGNAL(mettreAJourPnj(int, QString)), barreOutils, SLOT(mettreAJourPnj(int, QString)));
        // Affichage des noms et numeros des PJ/PNJ
        connect(actionAfficherNomsPj, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNomsPj(bool)));
        connect(actionAfficherNomsPnj, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNomsPnj(bool)));
        connect(actionAfficherNumerosPnj, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNumerosPnj(bool)));
        // Creation d'un DessinPerso a la creation d'un nouveau personnage dans la liste
        connect(G_listeUtilisateurs, SIGNAL(ajouterPj(QString , QString , QColor)), carte, SLOT(ajouterPj(QString , QString , QColor)));
        // Changement de nom d'un PJ
        connect(G_listeUtilisateurs, SIGNAL(renommerPj(QString , QString)), carte, SLOT(renommerPj(QString , QString)));
        // Suppression d'un PJ
        connect(G_listeUtilisateurs, SIGNAL(effacerPj(QString)), carte, SLOT(effacerPerso(QString)));
        // Changement de couleur d'un PJ
        connect(G_listeUtilisateurs, SIGNAL(changerCouleurPerso(QString, QColor)), carte, SLOT(changerCouleurPj(QString, QColor)));

        // Mise a jour du pointeur de souris de la carte
        switch(G_outilCourant)
        {
        case BarreOutils::crayon :
                carte->pointeurCrayon();
                break;
        case BarreOutils::ligne :
                carte->pointeurLigne();
                break;
        case BarreOutils::rectVide :
                carte->pointeurRectVide();
                break;
        case BarreOutils::rectPlein :
                carte->pointeurRectPlein();
                break;
        case BarreOutils::elliVide :
                carte->pointeurElliVide();
                break;
        case BarreOutils::elliPlein :
                carte->pointeurElliPlein();
                break;
        case BarreOutils::texte :
                carte->pointeurTexte();
                break;
        case BarreOutils::main :
                carte->pointeurMain();
                break;
        case BarreOutils::ajoutPnj :
                carte->pointeurAjoutPnj();
                break;
        case BarreOutils::supprPnj :
                carte->pointeurSupprPnj();
                break;
        case BarreOutils::deplacePerso :
                carte->pointeurDeplacePnj();
                break;
        case BarreOutils::etatPerso :
                carte->pointeurEtatPnj();
                break;
        default :
                qWarning("Outil non dfini a la creation d'une fentre (ajouterCarte - MainWindow.cpp)");
                break;
        }

        // Ajout dans la carte des PJ deja presents
        G_listeUtilisateurs->ajouterTousLesPj(carte);
        // Affichage de la carte
        carteFenetre->show();
}

/********************************************************************/
/* Ajout de l'Image passee en parametre sans le workspace           */
/********************************************************************/
void MainWindow::ajouterImage(Image *imageFenetre, QString titre)
{
        imageFenetre->setParent(workspace);
        // Ajout de la CarteFenetre a la liste (permet par la suite de parcourir l'ensemble des cartes)
        listeImage.append(imageFenetre);

        // Ajout de la carte au workspace
        workspace->addWindow(imageFenetre);

        // Mise a jour du titre de la CarteFenetre
        imageFenetre->setWindowTitle(titre);

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

        // Association de l'action avec la carte
        imageFenetre->associerAction(action);

        // Connexion de l'action d'outil main avec la fonction de changement de pointeur de souris en main
        connect(barreOutils->actionMain,                SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurMain()));

        // Connexion des actions de changement d'outil avec la fonction de changement de pointeur de souris normal
        connect(barreOutils->actionCrayon,         SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionLigne,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionRectVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionRectPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionElliVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionElliPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionTexte,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionAjoutPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionSupprPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionDeplacePnj, SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionEtatPnj,        SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));

        // Connexion de l'action avec l'affichage/masquage de l'image
        connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));

        // Mise a jour du pointeur de souris de l'image
        switch(G_outilCourant)
        {
        case BarreOutils::main :
                imageFenetre->pointeurMain();
                break;
        default :
                imageFenetre->pointeurNormal();
                break;
        }

        // Affichage de l'image
        imageFenetre->show();
}

/********************************************************************/
/* Appelle la fonction ouvrirPlan avec le parametre "masquer" egale */
/* a true                                                           */
/********************************************************************/
void MainWindow::ouvrirEtMasquerPlan()
{
        ouvrirPlan(true);
}

/********************************************************************/
/* Demande a l'utilisateur de selectionner un fichier image ou un   */
/* fichier plan, puis ouvre la carte. Le parametre "masquer" n'est  */
/* utilise que lorsque l'utilisateur ouvre une image                */
/********************************************************************/
void MainWindow::ouvrirPlan(bool masquer)
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, masquer?tr("Ouvrir et masquer un plan"):tr("Ouvrir un plan"), G_initialisation.dossierPlans,
                                          tr("Plans (*.pla *.jpg *.jpeg *.png *.bmp)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Erreur de chargement"));
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // On met a jour le chemin vers les plans
        int dernierSlash = fichier.lastIndexOf("/");
        G_initialisation.dossierPlans = fichier.left(dernierSlash);

        // Suppression de l'extension du fichier pour obtenir le titre de la CarteFenetre
        int dernierPoint = fichier.lastIndexOf(".");
        QString titre = fichier.left(dernierPoint);
        titre = titre.right(titre.length()-dernierSlash-1);

        // 2 cas de figure possibles :
        // Le fichier est un plan, on l'ouvre
        if (fichier.right(4) == ".pla")
        {
                // Ouverture du fichier .pla
                QFile file(fichier);
                if (!file.open(QIODevice::ReadOnly))
                {
                        qWarning("Probleme a l'ouverture du fichier .pla (ouvrirPlan - MainWindow.cpp)");
                        return;
                }
                QDataStream in(&file);
                // Lecture de la carte et des PNJ
                lireCarteEtPnj(in, masquer, titre);
                // Fermeture du fichier
                file.close();
        }

        // Le fichier est une image : on cree une nouvelle carte
        else
        {
                // Chargement de l'image
                QImage image(fichier);
                // Verification du chargement de l'image
                if (image.isNull())
                {
                        msgBox.setText(tr("Format de fichier incorrect"));
                        msgBox.exec();
                        return;
                }
                // Creation de l'identifiant
                QString idCarte = QUuid::createUuid().toString();
                // On recupere la taille des PJ
                int tailleDesPj = G_listeUtilisateurs->taillePj();
                // Creation de la carte
                Carte *carte = new Carte(idCarte, &image, tailleDesPj, masquer);
                // Creation de la CarteFenetre
                CarteFenetre *carteFenetre = new CarteFenetre(carte, workspace);
                // Ajout de la carte au workspace
                ajouterCarte(carteFenetre, titre);

                // Envoie de la carte aux autres utilisateurs
                // On commence par compresser l'image (format jpeg) dans un tableau
                QByteArray byteArray;
                QBuffer buffer(&byteArray);
                bool ok = image.save(&buffer, "jpeg", 60);
                if (!ok)
                        qWarning("Probleme de compression de l'image (ouvrirPlan - MainWindow.cpp)");

                // Taille des donnees
                quint32 tailleCorps =
                        // Taille du titre
                        sizeof(quint16) + titre.size()*sizeof(QChar) +
                        // Taille de l'identifiant
                        sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                        // Taille des PJ
                        sizeof(quint8) +
                        // Taille de l'info "masquer ou pas"
                        sizeof(quint8) +
                        // Taille de l'image
                        sizeof(quint32) + byteArray.size();

                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = plan;
                uneEntete->action = chargerPlan;
                uneEntete->tailleDonnees = tailleCorps;

                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout du titre
                quint16 tailleTitre = titre.size();
                memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
                p+=sizeof(quint16);
                memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
                p+=tailleTitre*sizeof(QChar);
                // Ajout de l'identifiant
                quint8 tailleId = idCarte.size();
                memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idCarte.data(), tailleId*sizeof(QChar));
                p+=tailleId*sizeof(QChar);
                // Ajout de la taille des PJ
                memcpy(&(donnees[p]), &tailleDesPj, sizeof(quint8));
                p+=sizeof(quint8);
                // Ajout de l'info "plan masque au chargement?"
                quint8 masquerPlan = masquer;
                memcpy(&(donnees[p]), &masquerPlan, sizeof(quint8));
                p+=sizeof(quint8);
                // Ajout de l'image
                quint32 tailleImage = byteArray.size();
                memcpy(&(donnees[p]), &tailleImage, sizeof(quint32));
                p+=sizeof(quint32);
                memcpy(&(donnees[p]), byteArray.data(), tailleImage);
                p+=tailleImage;

                // Emission de l'image au serveur ou a l'ensemble des clients
                emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;
        }
}

void MainWindow::lireCarteEtPnj(QDataStream &in, bool masquer, QString nomFichier)
{

        QPoint topleft;
        in >>topleft;

        QSize mapsize;
        in >> mapsize;



        QString titre;
        in >> titre;



        QTextStream out2(stderr,QIODevice::WriteOnly);
        out2 <<" lire plan " << topleft.x() << "," << topleft.y()  << " size=("<< mapsize.width()<<","<<mapsize.height() << endl;

        ///QString titre(tableauTitre, tailleTitre);
        // On recupere la taille des PJ
        int taillePj;
        in >> taillePj;

        QByteArray baFondOriginal;
        in >> baFondOriginal;

        QByteArray baFond;
        in >> baFond;

        QByteArray baAlpha;
        in>> baAlpha;



        bool ok;
        // Creation de l'image de fond original
        QImage fondOriginal;
        ok = fondOriginal.loadFromData(baFondOriginal, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression du fond original (lireCarteEtPnj - Mainwindow.cpp)");
        // Creation de l'image de fond
        QImage fond;
        ok = fond.loadFromData(baFond, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression du fond (lireCarteEtPnj - Mainwindow.cpp)");
        // Creation de la couche alpha
        QImage alpha;
        ok = alpha.loadFromData(baAlpha, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression de la couche alpha (lireCarteEtPnj - Mainwindow.cpp)");
        // Si necessaire la couche alpha est remplie, de facon a masquer l'image
        if (masquer)
        {
                QPainter painterAlpha(&alpha);
                painterAlpha.fillRect(0, 0, alpha.width(), alpha.height(), G_couleurMasque);
        }

        // Creation de l'identifiant de la carte
        QString idCarte = QUuid::createUuid().toString();
        // Creation de la carte
        Carte *carte = new Carte(idCarte, &fondOriginal, &fond, &alpha, taillePj);
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte,workspace);

        // Ajout de la carte au workspace : si aucun nom de fichier n'est passe en parametre, il s'agit d'une lecture de
        // carte dans le cadre de l'ouverture d'un fichier scenario : on prend alors le titre associe a la carte. Sinon
        // il s'agit d'un fichier plan : on prend alors le nom du fichier

        QPoint pos2 = carteFenetre->mapFromParent(topleft);
        out2 <<" lire plan 2 " << pos2.x() << "," << pos2.y()  << " size=("<< mapsize.width()<<","<<mapsize.height() << endl;

        if (nomFichier.isEmpty())
                G_mainWindow->ajouterCarte(carteFenetre, titre,mapsize,topleft);
        else
                G_mainWindow->ajouterCarte(carteFenetre, nomFichier,mapsize,topleft);


        // On recupere le nombre de personnages presents dans le message
        quint16 nbrPersonnages;
        in >>  nbrPersonnages;

        for (int i=0; i<nbrPersonnages; i++)
        {


            QString nomPerso,ident;
            DessinPerso::typePersonnage type;
            int numeroDuPnj;
            uchar diametre;

            QColor couleur;
            DessinPerso::etatDeSante sante;

            QPoint centre;
            QPoint orientation;
            int numeroEtat;
            bool visible;
            bool orientationAffichee;


            in >> nomPerso;
            in >> ident ;
            int typeint;
            in >> typeint;
            type=(DessinPerso::typePersonnage) typeint;
            in >> numeroDuPnj ;
            in >> diametre;
            in >> couleur ;
            in >> centre ;
            in >> orientation ;
            in >>sante.couleurEtat ;
            in >> sante.nomEtat ;
            in >> numeroEtat ;
            in>> visible;
            in >> orientationAffichee;

            DessinPerso *pnj = new DessinPerso(carte, ident, nomPerso, couleur, diametre, centre, type, numeroDuPnj);

                if (visible || (type == DessinPerso::pnj && !G_joueur))
                        pnj->afficherPerso();
                // On m.a.j l'orientation
                pnj->nouvelleOrientation(orientation);
                // Affichage de l'orientation si besoin
                if (orientationAffichee)
                        pnj->afficheOuMasqueOrientation();

                pnj->nouvelEtatDeSante(sante, numeroEtat);

                carte->afficheOuMasquePnj(pnj);


        }
        carte->emettreCarte(carteFenetre->windowTitle());
        carte->emettreTousLesPersonnages();
}

/********************************************************************/
/* Demande a l'utilisateur de selectionner un fichier image puis        */
/* l'ouvre sous forme d'Image (image non modifiable)                */
/********************************************************************/
void MainWindow::ouvrirImage()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir une image"), G_initialisation.dossierImages,
                                          tr("Images (*.jpg *.jpeg *.png *.bmp)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Erreur de chargement"));
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // On met a jour le chemin vers les images
        int dernierSlash = fichier.lastIndexOf("/");
        G_initialisation.dossierImages = fichier.left(dernierSlash);

        // Chargement de l'image
        QImage *img = new QImage(fichier);
        // Verification du chargement de l'image
        if (img->isNull())
        {
                msgBox.setText(tr("Format de fichier incorrect"));
                msgBox.exec();
                delete img;
                return;
        }

        // Suppression de l'extension du fichier pour obtenir le titre de l'image
        int dernierPoint = fichier.lastIndexOf(".");
        QString titre = fichier.left(dernierPoint);
        titre = titre.right(titre.length()-dernierSlash-1);
        titre+= tr(" (Image)");

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

        // Creation de l'identifiant
        QString idImage = QUuid::createUuid().toString();

        // Creation de la fenetre image
        Image *imageFenetre = new Image(idImage, G_idJoueurLocal, img, action,workspace);

        // Ajout de l'image a la liste (permet par la suite de parcourir l'ensemble des images)
        listeImage.append(imageFenetre);

        // Ajout de l'image au workspace
        workspace->addWindow(imageFenetre);

        // Mise a jour du titre de l'image
        imageFenetre->setWindowTitle(titre);

        // Connexion de l'action d'outil main avec la fonction de changement de pointeur de souris en main
        connect(barreOutils->actionMain,                SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurMain()));

        // Connexion des actions de changement d'outil avec la fonction de changement de pointeur de souris normal
        connect(barreOutils->actionCrayon,         SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionLigne,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionRectVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionRectPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionElliVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionElliPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionTexte,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionAjoutPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionSupprPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionDeplacePnj, SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(barreOutils->actionEtatPnj,        SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));

        // Connexion de l'action avec l'affichage/masquage de la fenetre
        connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));

        // Mise a jour du pointeur de souris de l'image
        switch(G_outilCourant)
        {
        case BarreOutils::main :
                imageFenetre->pointeurMain();
                break;
        default :
                imageFenetre->pointeurNormal();
                break;
        }

        // Affichage de l'image
        imageFenetre->show();

        // Envoie de l'image aux autres utilisateurs

        // On commence par compresser l'image (format jpeg) dans un tableau
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        bool ok = img->save(&buffer, "jpeg", 60);
        if (!ok)
                qWarning("Probleme de compression de l'image (ouvrirImage - MainWindow.cpp)");

        // Taille des donnees
        quint32 tailleCorps =
                // Taille du titre
                sizeof(quint16) + titre.size()*sizeof(QChar) +
                // Taille de l'identifiant de l'image
                sizeof(quint8) + idImage.size()*sizeof(QChar) +
                // Taille de l'identifiant du joueur
                sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
                // Taille de l'image
                sizeof(quint32) + byteArray.size();

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = image;
        uneEntete->action = chargerImage;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout du titre
        quint16 tailleTitre = titre.size();
        memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), titre.data(), tailleTitre*sizeof(QChar));
        p+=tailleTitre*sizeof(QChar);
        // Ajout de l'identifiant de l'image
        quint8 tailleIdImage = idImage.size();
        memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
        p+=tailleIdImage*sizeof(QChar);
        // Ajout de l'identifiant du joueur
        quint8 tailleIdJoueur = G_idJoueurLocal.size();
        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        // Ajout de l'image
        quint32 tailleImage = byteArray.size();
        memcpy(&(donnees[p]), &tailleImage, sizeof(quint32));
        p+=sizeof(quint32);
        memcpy(&(donnees[p]), byteArray.data(), tailleImage);
        p+=tailleImage;

        // Emission de l'image au serveur ou a l'ensemble des clients
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
}

/********************************************************************/
/* Ferme le plan ou l'image actuellement ouvert sur l'ordinateur        */
/* local et chez les autres utilisateurs                                */
/********************************************************************/
void MainWindow::fermerPlanOuImage()
{
        // On recupere la fenetre active (qui est forcement de type CarteFenetre ou Image, sans quoi l'action
        // ne serait pas dispo dans le menu Fichier)
        QWidget *active = workspace->activeWindow();

        // Ne devrait jamais arriver
        if (!active)
        {
                qWarning("Action fermerPlan appelee alors qu'aucun widget n'est actif dans le workspace (fermerPlanOuImage - MainWindow.h)");
                return;
        }

        // On verifie pour le principe qu'il s'agit bien d'une CarteFenetre ou d'une Image
        if (active->objectName() != "CarteFenetre" && active->objectName() != "Image")
        {
                qWarning("Demande de fermeture d'un widget qui n'est ni une CarteFenetre, ni une Image (fermerPlanOuImage - MainWindow.h)");
                return;
        }

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(tr("Fermer"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
        // M.a.j du titre et du message
        if (active->objectName() == "CarteFenetre")
        {
                msgBox.setWindowTitle(tr("Fermer plan"));
                msgBox.setText(tr("Voulez-vous vraiment fermer ce plan?\nCelui-ci sera également fermé chez tous\nles autres utilisateurs"));
        }
        else
        {
                msgBox.setWindowTitle(tr("Fermer image"));
                msgBox.setText(tr("Voulez-vous vraiment fermer cette image?\nCelle-ci sera également fermée chez tous\nles autres utilisateurs"));
        }
        msgBox.exec();

        // Si l'utilisateur n'a pas clique sur "Fermer", on quitte
        if (msgBox.result() != QMessageBox::AcceptRole)
                return;

        // Emission de la demande de fermeture de la carte
        if (active->objectName() == "CarteFenetre")
        {
                // Recuperation de l'identifiant de la carte
                QString idCarte = ((CarteFenetre *)active)->carte()->identifiantCarte();

                // Taille des donnees
                quint32 tailleCorps =
                        // Taille de l'identifiant de la carte
                        sizeof(quint8) + idCarte.size()*sizeof(QChar);

                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = plan;
                uneEntete->action = fermerPlan;
                uneEntete->tailleDonnees = tailleCorps;

                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout de l'identifiant de la carte
                quint8 tailleIdCarte = idCarte.size();
                memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                p+=tailleIdCarte*sizeof(QChar);

                // Emission de la demande de fermeture de la carte au serveur ou a l'ensemble des clients
                emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;

                // Suppression de la CarteFenetre et de l'action associee sur l'ordinateur local
                ((CarteFenetre *)active)->~CarteFenetre();
        }

        // Emission de la demande de fermeture de l'image
        else
        {
                // Recuperation de l'identifiant de la carte
                QString idImage = ((Image *)active)->identifiantImage();

                // Taille des donnees
                quint32 tailleCorps =
                        // Taille de l'identifiant de la carte
                        sizeof(quint8) + idImage.size()*sizeof(QChar);

                // Buffer d'emission
                char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                // Creation de l'entete du message
                enteteMessage *uneEntete;
                uneEntete = (enteteMessage *) donnees;
                uneEntete->categorie = image;
                uneEntete->action = fermerImage;
                uneEntete->tailleDonnees = tailleCorps;

                // Creation du corps du message
                int p = sizeof(enteteMessage);
                // Ajout de l'identifiant de la carte
                quint8 tailleIdImage = idImage.size();
                memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
                p+=sizeof(quint8);
                memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
                p+=tailleIdImage*sizeof(QChar);

                // Emission de la demande de fermeture de l'image au serveur ou a l'ensemble des clients
                emettre(donnees, tailleCorps + sizeof(enteteMessage));
                // Liberation du buffer d'emission
                delete[] donnees;

                // Suppression de l'Image et de l'action associee sur l'ordinateur local
                ((Image *)active)->~Image();
        }
}

/********************************************************************/
/* Met a jour la variable globale indiquant s'il faut afficher le   */
/* nom des nouveaux PJ                                                  */
/********************************************************************/
void MainWindow::afficherNomsPj(bool afficher)
{
        G_affichageNomPj = afficher;
}

/********************************************************************/
/* Met a jour la variable globale indiquant s'il faut afficher le   */
/* nom des nouveaux PNJ                                                 */
/********************************************************************/
void MainWindow::afficherNomsPnj(bool afficher)
{
        G_affichageNomPnj = afficher;
}

/********************************************************************/
/* Met a jour la variable globale indiquant s'il faut afficher le   */
/* numero des nouveaux PNJ                                          */
/********************************************************************/
void MainWindow::afficherNumerosPnj(bool afficher)
{
        G_affichageNumeroPnj = afficher;
}

/********************************************************************/
/* Regarde quelle est la carte active et lui envoie une demande         */
/* de changement de taille pour les PJ qu'elle contient                 */
/********************************************************************/
void MainWindow::changerTaillePj(int nouvelleTaille)
{
        // On recupere le widget actif dans le workspace
        QWidget *widget = workspace->activeWindow();

        // S'il y a un widget actif on verifie qu'il s'agit d'un objet "CarteFenetre"
        if (widget)
                if (widget->objectName() == "CarteFenetre")
                {
                        // On effectue un changement de taille pour les personnages de la carte active
                        ((CarteFenetre *)widget)->carte()->changerTaillePjCarte(nouvelleTaille);
                }
}

/********************************************************************/
/* Emet aux clients ou au serveur une demande de changement de          */
/* taille des PJ                                                        */
/********************************************************************/
void MainWindow::emettreChangementTaillePj(int nouvelleTaille)
{
        // On recupere le widget actif dans le workspace
        QWidget *widget = workspace->activeWindow();

        // S'il y a un widget actif on verifie qu'il s'agit d'un objet "CarteFenetre"
        if (widget)
                if (widget->objectName() == "CarteFenetre")
                {
                        // Emission du changement de taille

                        // Recuperation de l'identifiant de la carte
                        QString idCarte = ((CarteFenetre *)widget)->carte()->identifiantCarte();

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille de l'identifiant de la carte
                                sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                                // Taille de la nouvelle taille des PJ
                                sizeof(quint8);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = persoJoueur;
                        uneEntete->action = changerTaillePersoJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant de la carte
                        quint8 tailleIdCarte = idCarte.size();
                        memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                        p+=tailleIdCarte*sizeof(QChar);
                        // Ajout de la nouvelle taille de PJ
                        quint8 taillePj = nouvelleTaille;
                        memcpy(&(donnees[p]), &taillePj, sizeof(quint8));
                        p+=sizeof(quint8);

                        // Emission de la demande de changement de taille des PJ au serveur ou a l'ensemble des clients
                        emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
}

/********************************************************************/
/* Demande a la carte active de masquer/afficher le personnage dont */
/* l'identifiant est passe en parametre                                 */
/********************************************************************/
void MainWindow::affichageDuPj(QString idPerso, bool afficher)
{
        // On recupere le widget actif dans le workspace
        QWidget *widget = workspace->activeWindow();

        // S'il y a un widget actif on verifie qu'il s'agit d'un objet "CarteFenetre"
        if (widget)
                if (widget->objectName() == "CarteFenetre")
                {
                        // On demande a la carte d'afficher ou masquer le PJ
                        ((CarteFenetre *)widget)->carte()->affichageDuPj(idPerso, afficher);

                        // Emision de la demande d'affichage/masquage du PJ au serveur ou aux clients

                        // Recuperation de l'identifiant de la carte
                        QString idCarte = ((CarteFenetre *)widget)->carte()->identifiantCarte();

                        // Taille des donnees
                        quint32 tailleCorps =
                                // Taille de l'identifiant de la carte
                                sizeof(quint8) + idCarte.size()*sizeof(QChar) +
                                // Taille de l'identifiant du PJ
                                sizeof(quint8) + idPerso.size()*sizeof(QChar) +
                                // Taille de l'info affichier/masquer
                                sizeof(quint8);

                        // Buffer d'emission
                        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

                        // Creation de l'entete du message
                        enteteMessage *uneEntete;
                        uneEntete = (enteteMessage *) donnees;
                        uneEntete->categorie = persoJoueur;
                        uneEntete->action = afficherMasquerPersoJoueur;
                        uneEntete->tailleDonnees = tailleCorps;

                        // Creation du corps du message
                        int p = sizeof(enteteMessage);
                        // Ajout de l'identifiant de la carte
                        quint8 tailleIdCarte = idCarte.size();
                        memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idCarte.data(), tailleIdCarte*sizeof(QChar));
                        p+=tailleIdCarte*sizeof(QChar);
                        // Ajout de l'identifiant du PJ
                        quint8 tailleIdPerso = idPerso.size();
                        memcpy(&(donnees[p]), &tailleIdPerso, sizeof(quint8));
                        p+=sizeof(quint8);
                        memcpy(&(donnees[p]), idPerso.data(), tailleIdPerso*sizeof(QChar));
                        p+=tailleIdPerso*sizeof(QChar);
                        // Ajout de l'info afficher/masquer
                        quint8 afficherMasquer = afficher;
                        memcpy(&(donnees[p]), &afficherMasquer, sizeof(quint8));
                        p+=sizeof(quint8);

                        // Emission de la demande d'affichage/masquage de PJ au serveur ou a l'ensemble des clients
                        emettre(donnees, tailleCorps + sizeof(enteteMessage));
                        // Liberation du buffer d'emission
                        delete[] donnees;
                }
}

/********************************************************************/
/* Met a jour le selecteur de taille de la liste d'utilisateurs si  */
/* la carte dont l'identifiant est passe en parametre se trouve         */
/* etre la carte active                                                 */
/********************************************************************/
void MainWindow::mettreAJourSelecteurTaille(QString idCarte, int taillePj)
{
        // On recupere la fenetre active
        QWidget *active = workspace->activeWindow();

        // S'il y a une fenetre active
        if (active)
                // S'il s'agit d'une CarteFenetre
                if (active->objectName() == "CarteFenetre")
                        // S'il d'agit de la carte demandee
                        if (((CarteFenetre *)active)->carte()->identifiantCarte() == idCarte)
                                // Alors on met a jour le selecteur de taille de la liste d'utilisateurs
                                G_listeUtilisateurs->majTaillePj(taillePj);
}

/********************************************************************/
/* M.a.j l'espace de travail en fonction de la fenetre actuellement */
/* active                                                           */
/********************************************************************/
void MainWindow::mettreAJourEspaceTravail()
{
        // On recupere la fenetre active
        QWidget *active = workspace->activeWindow();

        // S'il y a une fenetre active, on la passe a la fonction changementFenetreActive
        if (active)
                changementFenetreActive(active);
}

/********************************************************************/
/* Met a jour le selecteur de taille des PJ, ainsi que l'etat           */
/* d'affichage des PJ, si la fenetre activee dans le workspace est  */
/* de type CarteFenetre                                                 */
/********************************************************************/
void MainWindow::changementFenetreActive(QWidget *widget)
{
        // Si le widget existe on verifie qu'il s'agit d'un objet "CarteFenetre"
        if (widget)
        {
                if (widget->objectName() == "CarteFenetre")
                {
                        // Il existe une CarteFenetre active (les personnages nouvellement crees peuvent etre coches)
                        G_carteFenetreActive = true;

                        // Si l'utilisateur local est MJ on autorise la fermeture et la sauvegarde du plan
                        if (!G_joueur)
                        {
                                actionFermerPlan->setEnabled(true);
                                actionSauvegarderPlan->setEnabled(true);;
                        }
                        else
                                actionFermerPlan->setEnabled(false);

                        // On demande la taille des PJ dans la carte selectionnee
                        int taille = ((CarteFenetre *)widget)->carte()->tailleDesPj();
                        // Puis on met a jour le selecteur de taille des PJ
                        G_listeUtilisateurs->majTaillePj(taille);

                        // M.a.j de l'etat d'affichage de chaque PJ
                        G_listeUtilisateurs->majAffichagePj(((CarteFenetre *)widget)->carte());
                }

                else if (widget->objectName() == "Tchat")
                {
                        // Aucune CarteFenetre active (les personnages nouvellement crees ne peuvent pas etre coches)
                        G_carteFenetreActive = false;

                        // Si l'utilisateur local est MJ on interdit la fermeture et la sauvegarde des plans
                        if (!G_joueur)
                        {
                                actionFermerPlan->setEnabled(false);
                                actionSauvegarderPlan->setEnabled(false);;
                        }
                        else
                                actionFermerPlan->setEnabled(false);

                        // On decoche tous les PJ
                        G_listeUtilisateurs->casesPjNonSelectionnables();

                        // Le joueur concerne ne doit plus clignoter
                        G_listeUtilisateurs->nePlusFaireClignoter(((Tchat *)widget)->identifiant());
                }

                else if (widget->objectName() == "Image")
                {
                        // Aucune CarteFenetre active (les personnages nouvellement crees ne peuvent pas etre coches)
                        G_carteFenetreActive = false;

                        // Si l'utilisateur local est MJ ou proprietaire de l'image on interdit la sauvegarde des plans et on autorise la fermeture de l'image
                        if (!G_joueur || ((Image *)widget)->proprietaireImage() )
                        {
                                actionFermerPlan->setEnabled(true);
                                actionSauvegarderPlan->setEnabled(false);;
                        }
                        else
                                actionFermerPlan->setEnabled(false);

                        // On decoche tous les PJ
                        G_listeUtilisateurs->casesPjNonSelectionnables();
                }

                else if (widget->objectName() == "EditeurNotes")
                {
                        // Aucune CarteFenetre active (les personnages nouvellement crees ne peuvent pas etre coches)
                        G_carteFenetreActive = false;

                        // On interdit la sauvegarde et la fermeture des plans
                        actionFermerPlan->setEnabled(false);
                        actionSauvegarderPlan->setEnabled(false);;

                        // On decoche tous les PJ
                        G_listeUtilisateurs->casesPjNonSelectionnables();
                }

                else
                {
                        // Aucune CarteFenetre active (les personnages nouvellement crees ne peuvent pas etre coches)
                        G_carteFenetreActive = false;

                        qWarning("Fenetre active autre que CarteFenetre, Tchat, Image ou EditeurNotes (changementFenetreActive - MainWindow.cpp)");
                        // On decoche tous les PJ
                        G_listeUtilisateurs->casesPjNonSelectionnables();
                }
        }        // Fin du if(widget)

        // Aucun widget n'est actif
        else
        {
                // Aucune CarteFenetre active (les personnages nouvellement crees ne peuvent pas etre coches)
                G_carteFenetreActive = false;

                // Si l'utilisateur local est MJ on interdit la fermeture et la sauvegarde des plans
                if (!G_joueur)
                {
                        actionFermerPlan->setEnabled(false);
                        actionSauvegarderPlan->setEnabled(false);;
                }
                else
                        actionFermerPlan->setEnabled(false);

                // On decoche tous les PJ
                G_listeUtilisateurs->casesPjNonSelectionnables();
        }
}

/********************************************************************/
/* Demande une m.a.j des couleurs personnelles de la barre d'outils */
/********************************************************************/
void MainWindow::majCouleursPersonnelles()
{
        barreOutils->majCouleursPersonnelles();
}

/********************************************************************/
/* L'utilisateur vient de demander la creation d'un nouveau plan        */
/* vide                                                                 */
/********************************************************************/
void MainWindow::nouveauPlan()
{
        fenetreNouveauPlan = new NouveauPlanVide(this);
}

/********************************************************************/
/* Cree un nouveau plan vide dont les caracteristiques sont passees */
/* en parametre                                                         */
/********************************************************************/
void MainWindow::creerNouveauPlanVide(QString titre, QString idCarte, QColor couleurFond, quint16 largeur, quint16 hauteur, quint8 taillePj)
{
        if (fenetreNouveauPlan)
        {
                fenetreNouveauPlan->~NouveauPlanVide();
                fenetreNouveauPlan = 0;
        }

        // Creation de l'image
        QImage image(largeur, hauteur, QImage::Format_ARGB32_Premultiplied);
        image.fill(couleurFond.rgb());
        // Creation de la carte
        Carte *carte = new Carte(idCarte, &image, taillePj);
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte, workspace);
        // Ajout de la carte au workspace
        ajouterCarte(carteFenetre, titre);
}

/********************************************************************/
/* Ferme la fenetre de creation d'un nouveau plan vide                  */
/********************************************************************/
void MainWindow::aucunNouveauPlanVide()
{
        fenetreNouveauPlan->~NouveauPlanVide();
        fenetreNouveauPlan = 0;
}

/********************************************************************/
/* Envoie tous les plans deja ouvert au joueur dont l'identifiant   */
/* est passe en parametre (serveur uniquement)                          */
/********************************************************************/
void MainWindow::emettreTousLesPlans(int numeroLiaison)
{
        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
        {
                // On demande a la carte contenue dans la CarteFenetre de s'emettre vers
                // l'utilisateur dont la liaison est passee en parametre
                listeCarteFenetre[i]->carte()->emettreCarte(listeCarteFenetre[i]->windowTitle(), numeroLiaison);
                // On emet egalement vers la liaison l'ensemble des personnages presents sur la carte
                listeCarteFenetre[i]->carte()->emettreTousLesPersonnages(numeroLiaison);
        }
}

/********************************************************************/
/* Envoie toutes les images deja ouvertes au joueur dont                */
/* l'identifiant est passe en parametre (serveur uniquement)        */
/********************************************************************/
void MainWindow::emettreToutesLesImages(int numeroLiaison)
{
        // Taille de la liste des Images
        int tailleListe = listeImage.size();

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
                // On demande a l'Image de s'emettre vers l'utilisateur dont la liaison est passee en parametre
                listeImage[i]->emettreImage(listeImage[i]->windowTitle(), numeroLiaison);
}

/********************************************************************/
/* Renvoie le pointeur vers la Carte dont l'identifiant est passe   */
/* en parametre, ou 0 si la Carte n'est pas trouvee                 */
/********************************************************************/
Carte * MainWindow::trouverCarte(QString idCarte)
{
        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if ( listeCarteFenetre[i]->carte()->identifiantCarte() == idCarte )
                        ok = true;

        // Si la carte vient d'etre trouvee on renvoie son pointeur
        if (ok)
                return listeCarteFenetre[i-1]->carte();
        // Sinon on renvoie -1
        else
                return 0;
}

/********************************************************************/
/* Renvoie le pointeur vers la CarteFenetre dont l'identifiant de   */
/* la Carte associee est passe en parametre, ou 0 si la Carte n'est */
/* pas trouvee                                                          */
/********************************************************************/
CarteFenetre * MainWindow::trouverCarteFenetre(QString idCarte)
{
        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if ( listeCarteFenetre[i]->carte()->identifiantCarte() == idCarte )
                        ok = true;

        // Si la carte vient d'etre trouvee on renvoie le pointeur vers la CarteFenetre qui lui est associee
        if (ok)
                return listeCarteFenetre[i-1];
        // Sinon on renvoie 0
        else
                return 0;
}

/********************************************************************/
/* Affiche le tchat correspondant a l'utilisateur dont l'ID est         */
/* passe en parametre                                                   */
/********************************************************************/
void MainWindow::afficherTchat(QString id)
{
        int i;
        bool trouve = false;
        int tailleListe = listeTchat.size();

        // Recherche du tchat
        for (i=0; i<tailleListe && !trouve; i++)
                if (listeTchat[i]->identifiant() == id)
                        trouve = true;

        // Ne devrait jamais arriver
        if (!trouve)
        {
                qWarning("Tchat introuvable et impossible a afficher (afficherTchat - MainWindow.cpp)");
                return;
        }

        // Affichage du tchat
        listeTchat[i-1]->show();
        // Mise a jour de l'action du sous-menu Tchats
        listeTchat[i-1]->majAction();
}

/********************************************************************/
/* Masque le tchat correspondant a l'utilisateur dont l'ID est          */
/* passe en parametre                                                   */
/********************************************************************/
void MainWindow::masquerTchat(QString id)
{
        int i;
        bool trouve = false;
        int tailleListe = listeTchat.size();

        // Recherche du tchat
        for (i=0; i<tailleListe && !trouve; i++)
                if (listeTchat[i]->identifiant() == id)
                        trouve = true;

        // Ne devrait jamais arriver
        if (!trouve)
        {
                qWarning("Tchat introuvable et impossible a masquer (masquerTchat - MainWindow.cpp)");
                return;
        }

        // Masquage du tchat
        listeTchat[i-1]->hide();
        // Mise a jour de l'action du sous-menu Tchats
        listeTchat[i-1]->majAction();
}

/********************************************************************/
/* Ajoute un tchat                                                  */
/********************************************************************/
void MainWindow::ajouterTchat(QString idJoueur, QString nomJoueur)
{

        // Creation de l'action correspondante
        QAction *action = sousMenuTchat->addAction(tr("Tchat avec ") + nomJoueur);
        action->setCheckable(true);
        action->setChecked(false);
        // Creation du Tchat
        Tchat *tchat = new Tchat(idJoueur, action);
        // Connexion de l'action avec l'affichage/masquage de la fenetre
        connect(action, SIGNAL(triggered(bool)), tchat, SLOT(setVisible(bool)));
        // Ajout du tchat a la liste
        listeTchat.append(tchat);
        // Ajout du tchat au workspace
        workspace->addWindow(tchat);
        // Mise a jour du titre du tchat
        tchat->setWindowTitle(tr("Tchat avec ") + nomJoueur);
        // Masquage du tchat
        tchat->hide();
}

/********************************************************************/
/* Supprime le tchat associe au joueurs dont l'ID est passe en          */
/* parametre                                                        */
/********************************************************************/
void MainWindow::supprimerTchat(QString idJoueur)
{
        int i;
        bool trouve = false;
        int tailleListe = listeTchat.size();

        // Recherche du tchat
        for (i=0; i<tailleListe && !trouve; i++)
                if (listeTchat[i]->identifiant() == idJoueur)
                        trouve = true;

        // Ne devrait jamais arriver
        if (!trouve)
        {
                qWarning("Tchat introuvable et impossible a supprimer (supprimerTchat - MainWindow.cpp)");
                return;
        }

        // Suppression du Tchat
        listeTchat[i-1]->~Tchat();
        // Suppresion de l'element dans la liste
        listeTchat.removeAt(i-1);
}

/********************************************************************/
/* Renvoie le pointeur vers le Tchat dont l'identifiant est passe   */
/* en parametre, ou 0 si le Tchat n'est pas trouve                  */
/********************************************************************/
Tchat * MainWindow::trouverTchat(QString idJoueur)
{
        // Taille de la liste des Tchat
        int tailleListe = listeTchat.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if (listeTchat[i]->identifiant() == idJoueur)
                        ok = true;

        // Si le Tchat vient d'etre trouve on renvoie son pointeur
        if (ok)
                return listeTchat[i-1];
        // Sinon on renvoie 0
        else
                return 0;
}

/********************************************************************/
/* Renvoie true si la fenetre passee en parametre est la fenetre        */
/* active (fenetre de 1er plan) et qu'elle est affichee. Sinon          */
/* renvoie false                                                        */
/********************************************************************/
bool MainWindow::estLaFenetreActive(QWidget *widget)
{
        return widget == workspace->activeWindow() && widget->isVisible();
}

/********************************************************************/
/* Change la fenetre active du workspace devient celle passee en        */
/* parametre                                                        */
/********************************************************************/
void MainWindow::devientFenetreActive(QWidget *widget)
{
        workspace->setActiveWindow(widget);
        changementFenetreActive(widget);
}

/********************************************************************/
/* Coche l'action Tchat commun du sous-menu Tchats                  */
/********************************************************************/
void MainWindow::cocherActionTchatCommun()
{
        actionTchatCommun->setChecked(true);
}

/********************************************************************/
/* Demande a l'utilisateur s'il desire reellement quitter           */
/* l'application, et si oui, ferme le programme. Si le parametre        */
/* perteConnexion = true alors la fermeture est due a une perte de  */
/* connexion avec le serveur                                        */
/********************************************************************/
void MainWindow::quitterApplication(bool perteConnexion)
{
        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        QAbstractButton *boutonSauvegarder         = msgBox.addButton(tr("Sauvegarder"), QMessageBox::YesRole);
        QAbstractButton *boutonQuitter                 = msgBox.addButton(tr("Quitter"), QMessageBox::AcceptRole);
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // Creation du message
        QString message;

        // S'il s'agit d'une perte de connexion
        if (perteConnexion)
        {
                message = tr("La connexion avec le serveur a été perdue, ") + NOM_APPLICATION + tr(" va être fermé\n");
                // Icone de la fenetre
                msgBox.setIcon(QMessageBox::Critical);
                // M.a.j du titre et du message
                msgBox.setWindowTitle(tr("Perte de connexion"));
        }
        else
        {
                // Icone de la fenetre
                msgBox.setIcon(QMessageBox::Information);
                // Ajout d'un bouton
                msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
                // M.a.j du titre et du message
                msgBox.setWindowTitle(tr("Quitter ") + NOM_APPLICATION);
        }

        // Si l'utilisateur est un joueur
        if (G_joueur)
                message += tr("Voulez-vous sauvegarder vos notes avant de quitter l'application?");

        // Si l'utilisateut est un MJ
        else
                message += tr("Voulez-vous sauvegarder le scénario avant de quitter l'application?");

        //M.a.j du message de la boite de dialogue
        msgBox.setText(message);
        // Ouverture de la boite d'alerte
        msgBox.exec();

        // Si l'utilisateur a clique sur "Quitter", on quitte l'application
        if (msgBox.clickedButton() == boutonQuitter)
        {
                // On sauvegarde les tchats
                sauvegarderTousLesTchats();
                // On sauvegarde le fichier d'initialisation
                sauvegarderFichierInitialisation();
                // On quitte l'application
                qApp->quit();
        }

        // Si l'utilisateur a clique sur "Sauvegarder", on applique l'action appropriee a la nature de l'utilisateur
        else if (msgBox.clickedButton() == boutonSauvegarder)
        {
                bool ok;
                // Si l'utilisateur est un joueur, on sauvegarde les notes
                if (G_joueur)
                        ok = sauvegarderNotes();
                // S'il est MJ, on sauvegarde le scenario
                else
                        ok = sauvegarderScenario();

                // Puis on quitte l'application si l'utilisateur a sauvegarde ou s'il s'agit d'une perte de connexion
                if (ok || perteConnexion)
                {
                        // On sauvegarde les tchats
                        sauvegarderTousLesTchats();
                        // On sauvegarde le fichier d'initialisation
                        sauvegarderFichierInitialisation();
                        // On quitte l'application
                        qApp->quit();
                }
        }
}

/********************************************************************/
/* L'utilisateur vient de cliquer sur l'icone de fermeture de la        */
/* fenetre principale, on lui demande de confirmer la fermture          */
/********************************************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
        // Arret de la procedure de fermeture
        event->ignore();
        // Demande de confirmation de la fermture a l'utilisateur
        quitterApplication();
}

/********************************************************************/
/* Renvoie le pointeur vers l'image dont l'identifiant est passe        */
/* en parametre, ou 0 si l'image n'est pas trouvee                  */
/********************************************************************/
Image *MainWindow::trouverImage(QString idImage)
{
        // Taille de la liste des Images
        int tailleListe = listeImage.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if (listeImage[i]->identifiantImage() == idImage)
                        ok = true;

        // Si l'Image vient d'etre trouve on renvoie son pointeur
        if (ok)
                return listeImage[i-1];
        // Sinon on renvoie 0
        else
                return 0;
}

/********************************************************************/
/* Enleve de la liste la carte dont l'ID est passe en parametre.        */
/* Renvoie true en cas de reussite, false sinon                         */
/********************************************************************/
bool MainWindow::enleverCarteDeLaListe(QString idCarte)
{
        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if ( listeCarteFenetre[i]->carte()->identifiantCarte() == idCarte )
                        ok = true;

        // Si la carte vient d'etre trouvee on supprime l'element
        if (ok)
        {
                listeCarteFenetre.removeAt(i-1);
                return true;
        }
        // Sinon on renvoie false
        else
                return false;
}

/********************************************************************/
/* Enleve de la liste l'image dont l'ID est passe en parametre.         */
/* Renvoie true en cas de reussite, false sinon                         */
/********************************************************************/
bool MainWindow::enleverImageDeLaListe(QString idImage)
{
        // Taille de la liste des Images
        int tailleListe = listeImage.size();

        bool ok = false;
        int i;
        for (i=0; i<tailleListe && !ok; i++)
                if (listeImage[i]->identifiantImage() == idImage)
                        ok = true;

        // Si l'image vient d'etre trouvee on supprime l'element
        if (ok)
        {
                listeImage.removeAt(i-1);
                return true;
        }
        // Sinon on renvoie false
        else
                return false;
}

/********************************************************************/
/* Sauvegarde la plan actuellement ouvert ainsi que la position des */
/* PNJ et PJ                                                        */
/********************************************************************/
void MainWindow::sauvegarderPlan()
{
        // On recupere la fenetre active
        QWidget *active = workspace->activeWindow();

        // On verifie pour le principe qu'il s'agit bien d'une CarteFenetre
        if (active->objectName() != "CarteFenetre")
        {
                qWarning("Demande de sauvegarde d'un plan qui n'est pas une CarteFenetre (sauvegarderPlan - MainWindow.h)");
                return;
        }

        // Ouverture du selecteur de fichiers
        QString fichier = QFileDialog::getSaveFileName(this, tr("Sauvegarder un plan"), G_initialisation.dossierPlans, tr("Plans (*.pla)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

#ifdef MACOS
        // On rajoute l'extension
        fichier += ".pla";
#endif

        // On met a jour le chemin vers les plans
        int dernierSlash = fichier.lastIndexOf("/");
        G_initialisation.dossierPlans = fichier.left(dernierSlash);

        // Creation du fichier
        QFile file(fichier);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (sauvegarderPlan - MainWindow.cpp)");
                return;
        }
        QDataStream out(&file);
        // On demande a la carte de se sauvegarder dans le fichier
        ((CarteFenetre *)active)->carte()->sauvegarderCarte(out);
        // Fermeture du fichier
        file.close();
}

/********************************************************************/
/* L'utilisateur local a demande a etre MJ lors de la connexion au  */
/* serveur mais un MJ etait deja connecte : l'utilisateur local         */
/* devient donc simple joueur. On met l'espace de travail a jour en */
/* consequence                                                          */
/********************************************************************/
void MainWindow::changementNatureUtilisateur()
{
        // M.a.j des menus du mainWindow
        autoriserOuInterdireActions();
        // M.a.j de la barre d'outils
        barreOutils->autoriserOuInterdireCouleurs();
      
#ifndef NULL_PLAYER
    // M.a.j du lecteur audio (pour que le changement de taille se passe correctement, on enleve puis on remet le dockWidget)
        removeDockWidget(G_lecteurAudio);
        //G_lecteurAudio->autoriserOuIntedireCommandes();
        addDockWidget(Qt::RightDockWidgetArea, G_lecteurAudio);
        G_lecteurAudio->show();
#endif
}

/********************************************************************/
/* Affiche ou masque l'editeur de notes. Le 2eme parametre doit         */
/* etre a true si la fonction est appelee a partir de l'objet           */
/* listeUtilisateurs ou de l'editeur de notes lui-meme : cela           */
/* permet de cocher/decocher l'action d'affichage/masquage          */
/********************************************************************/
void MainWindow::afficherEditeurNotes(bool afficher, bool cocherAction)
{
        // Affichage de l'editeur de notes
        editeurNotes->setVisible(afficher);

        // Si la fonction a pas ete appelee par la listeUtilisateurs ou par l'editeur lui-meme, on coche/decoche l'action associee
        if (cocherAction)
                actionEditeurNotes->setChecked(afficher);

        // Sinon on coche/decoche la case de l'editeur de notes dans la listeUtilisateurs
        else
        {
                if (afficher)
                        G_listeUtilisateurs->cocherCaseTchat(G_idJoueurLocal);
                else
                        G_listeUtilisateurs->decocherCaseTchat(G_idJoueurLocal);
        }
}

/********************************************************************/
/* Demande a l'utilisateur de selectionner un fichier de notes a        */
/* ouvrir                                                           */
/********************************************************************/
void MainWindow::ouvrirNotes()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir notes"), G_initialisation.dossierNotes, tr("Documents HTML (*.htm *.html)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // On met a jour le chemin vers les notes
        int dernierSlash = fichier.lastIndexOf("/");
        G_initialisation.dossierNotes = fichier.left(dernierSlash);

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en lecture seule
        if (!file.open(QIODevice::ReadOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (ouvrirNotes - MainWindow.cpp)");
                return;
        }
        QDataStream out(&file);
        // On demande a l'editeur de notes de charger le fichier
        editeurNotes->ouvrirNotes(out);
        // Fermeture du fichier
        file.close();
}

/********************************************************************/
/* Demande a l'utilisateur d'entrer un nom de fichier dans lequel   */
/* sauvegarder les notes                                                */
/********************************************************************/
bool MainWindow::sauvegarderNotes()
{
        // Ouverture du selecteur de fichiers
        QString fichier = QFileDialog::getSaveFileName(this, tr("Sauvegarder notes"), G_initialisation.dossierNotes, tr("Documents HTML (*.htm *.html)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return false;

#ifdef MACOS
        // On rajoute l'extension
        fichier += ".htm";
#endif

        // On met a jour le chemin vers les notes
        int dernierSlash = fichier.lastIndexOf("/");
        G_initialisation.dossierNotes = fichier.left(dernierSlash);

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (sauvegarderNotes - MainWindow.cpp)");
                return false;
        }
        QDataStream in(&file);
        // On demande a l'editeur de notes de les sauvegarder dans le fichier
        editeurNotes->sauvegarderNotes(in);
        // Fermeture du fichier

        file.close();

        return true;
}

/********************************************************************/
/* Demande a l'utilisateur de selectionner un scenario a ouvrir         */
/********************************************************************/
void MainWindow::ouvrirScenario()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Ouvrir scénario"), G_initialisation.dossierScenarii, tr("Scénarios (*.sce)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // On met a jour le chemin vers les scenarii
        int dernierSlash = fichier.lastIndexOf("/");
        G_initialisation.dossierScenarii = fichier.left(dernierSlash);

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en lecture seule
        if (!file.open(QIODevice::ReadOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (ouvrirScenario - MainWindow.cpp)");
                return;
        }
        QDataStream in(&file);


        int nbrCartes;
        in >> nbrCartes;

        //file.read((char *)&nbrCartes, sizeof(quint16));
        // On lit toutes les cartes presentes dans le fichier
        for (int i=0; i<nbrCartes; i++)
                lireCarteEtPnj(in);

        // On lit le nbr d'images a suivre
        int nbrImages;
        //file.read((char *)&nbrImages, sizeof(quint16));
        in >>nbrImages;
        // in >>On lit toutes les images presentes dans le fichier
        for (int i=0; i<nbrImages; i++)
                lireImage(in);

        // Enfin on lit les notes
        editeurNotes->ouvrirNotes(in);

        // Fermeture du fichier
        file.close();
}

/********************************************************************/
/* Demande a l'utilisateur de choisir un nom de fichier pour        */
/* sauvegarder le scenario en cours. Un scenario comprend toutes        */
/* les cartes, les images (qui deviennent proprietes du MJ), et les */
/* notes                                                                */
/********************************************************************/
bool MainWindow::sauvegarderScenario()
{
        // Ouverture du selecteur de fichiers
        QString filename = QFileDialog::getSaveFileName(this, tr("Sauvegarder scénario"), G_initialisation.dossierScenarii, tr("Scénarios (*.sce)"));


        if (filename.isNull())
                return false;

        if(!filename.endsWith(".sce"))
                filename += ".sce";


        // On met a jour le chemin vers les scenarii
        int dernierSlash = filename.lastIndexOf("/");
        G_initialisation.dossierScenarii = filename.left(dernierSlash);

        // Creation du descripteur de fichier
        QFile file(filename);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("Probleme a l'ouverture du fichier (sauvegarderScenario - MainWindow.cpp)");
                return false;
        }

        QDataStream out(&file);

        // On commence par sauvegarder toutes les cartes
        sauvegarderTousLesPlans(out);
        // Puis toutes les images
        sauvegarderToutesLesImages(out);
        // Et enfin les notes
        editeurNotes->sauvegarderNotes(out);
        // Fermeture du fichier
        file.close();

        return true;
}
void MainWindow::sauvegarderTousLesPlans(QDataStream &out)
{
    out << listeCarteFenetre.size();
    for (int i=0; i<listeCarteFenetre.size(); i++)
    {
        QTextStream out2(stderr,QIODevice::WriteOnly);
        out2 <<" save tous les plans " << listeCarteFenetre[i]->pos().x() << "," << listeCarteFenetre[i]->pos().y()  << " size=("<< listeCarteFenetre[i]->size().width()<<","<<listeCarteFenetre[i]->size().height() << endl;
        QPoint pos2 = listeCarteFenetre[i]->mapFromParent(listeCarteFenetre[i]->pos());
        out2 <<" save tous les plans " << pos2.x() << "," << pos2.y()  << " size=("<< listeCarteFenetre[i]->size().width()<<","<<listeCarteFenetre[i]->size().height() << endl;

        out << pos2;
        out << listeCarteFenetre[i]->size();
        //QWidgetList list = workspace->windowList();
        //list.indexOf()
            listeCarteFenetre[i]->carte()->sauvegarderCarte(out, listeCarteFenetre[i]->windowTitle());
    }
}

void MainWindow::sauvegarderToutesLesImages(QDataStream &out)
{
   out << listeImage.size();
   for (int i=0; i<listeImage.size(); i++)
           listeImage[i]->sauvegarderImage(out, listeImage[i]->windowTitle());
}


void MainWindow::lireImage(QDataStream &file)
{
        // Lecture de l'image

        // On recupere le titre

        QString titre;
        QByteArray baImage;
        QPoint topleft;
        QSize size;

        file >> titre;
        file >>topleft;
        file >> size;

        QTextStream out(stderr,QIODevice::WriteOnly);
        out << "lire image " << topleft.x() << "," << topleft.y()  << " size=("<< size.width()<<","<<size.height() << endl;

        file >> baImage;

        bool ok;
        // Creation de l'image
        QImage img;
        ok = img.loadFromData(baImage, "jpeg");
        if (!ok)
                qWarning("Probleme de decompression de l'image (lireImage - Mainwindow.cpp)");

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

        // Creation de l'identifiant
        QString idImage = QUuid::createUuid().toString();

        // Creation de la fenetre image
        Image *imageFenetre = new Image(idImage, G_idJoueurLocal, &img, action,workspace);

        // Ajout de l'image a la liste (permet par la suite de parcourir l'ensemble des images)
        listeImage.append(imageFenetre);

        // Ajout de l'image au workspace
        QWidget* tmp = workspace->addWindow(imageFenetre);
        tmp->move(topleft);
        tmp->resize(size);

        // Mise a jour du titre de l'image
        imageFenetre->setWindowTitle(titre);

        // Connexion de l'action avec l'affichage/masquage de la fenetre
        connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));

        // Affichage de l'image
        imageFenetre->show();


        // Envoie de l'image aux autres utilisateurs

        // On commence par compresser l'image (format jpeg) dans un tableau
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        ok = img.save(&buffer, "jpeg", 60);
        if (!ok)
                qWarning("Probleme de compression de l'image (lireImage - MainWindow.cpp)");

        // Taille des donnees
        quint32 tailleCorps =
                // Taille du titre
                sizeof(quint16) + titre.size()*sizeof(QChar) +
                // Taille de l'identifiant de l'image
                sizeof(quint8) + idImage.size()*sizeof(QChar) +
                // Taille de l'identifiant du joueur
                sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
                // Taille de l'image
                sizeof(quint32) + byteArray.size();

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = image;
        uneEntete->action = chargerImage;
        uneEntete->tailleDonnees = tailleCorps;

        // Creation du corps du message
        int p = sizeof(enteteMessage);
        // Ajout du titre
        quint16 tailleDuTitre = titre.size();
        memcpy(&(donnees[p]), &tailleDuTitre, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), titre.data(), tailleDuTitre*sizeof(QChar));
        p+=tailleDuTitre*sizeof(QChar);
        // Ajout de l'identifiant de l'image
        quint8 tailleIdImage = idImage.size();
        memcpy(&(donnees[p]), &tailleIdImage, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idImage.data(), tailleIdImage*sizeof(QChar));
        p+=tailleIdImage*sizeof(QChar);
        // Ajout de l'identifiant du joueur
        quint8 tailleIdJoueur = G_idJoueurLocal.size();
        memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueur*sizeof(QChar));
        p+=tailleIdJoueur*sizeof(QChar);
        // Ajout de l'image
        quint32 tailleDeImage = byteArray.size();
        memcpy(&(donnees[p]), &tailleDeImage, sizeof(quint32));
        p+=sizeof(quint32);
        memcpy(&(donnees[p]), byteArray.data(), tailleDeImage);
        p+=tailleDeImage;

        // Emission de l'image au serveur ou a l'ensemble des clients
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
}

/********************************************************************/
/* Sauvegarde l'ensemble des tchats sous forme de fichiers HTML         */
/********************************************************************/
void MainWindow::sauvegarderTousLesTchats()
{
        // Taille de la liste des tchats
        int tailleListe = listeTchat.size();

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
        {
                // Creation du chemin complet du fichier
                QString chemin(G_initialisation.dossierTchats + "/" + listeTchat[i]->windowTitle() + ".htm");
                // Creation du descripteur de fichier
                QFile file(chemin);
                // Ouverture du fichier en ecriture seule
                if (!file.open(QIODevice::WriteOnly))
                {
                        qWarning("Probleme a l'ouverture du fichier (sauvegarderTousLesTchats - MainWindow.cpp)");
                        return;
                }
                // On demande au tchat de se sauvegarder dans le fichier
                listeTchat[i]->sauvegarderTchat(file);
                // Fermeture du fichier
                file.close();
        }
}

/********************************************************************/
/* Sauvegarde le fichier d'initialisation                           */
/********************************************************************/
void MainWindow::sauvegarderFichierInitialisation()
{
    // Don't really write anything to the filesystem, but store new values in G_initialisation.

    // ...les couleurs personnelles
    for (int i=0; i<16; i++)
            G_initialisation.couleurPersonnelle[i] = barreOutils->donnerCouleurPersonnelle(i);

#ifndef NULL_PLAYER
    // ...le volume du lecteur audio
    G_initialisation.niveauVolume = G_lecteurAudio->volume();
#endif
}


void MainWindow::aPropos()
{
        QMessageBox::about(this, tr("About Rolisteam"),
                                           tr("<h1>Rolisteam v1.0.2</h1>"
                                                  "<p>Rolisteam makes easy the management of any role playing games. It allows players to communicate to each others, share maps or picture. Rolisteam also provides many features for: permission management, sharing background music and dices throw. Rolisteam is written in Qt4. Its dependencies are : Qt4 and Phonon.</p>"
                                                  "<p>Rolisteam may contain some files from the FMOD library. This point prevents commercial use of the software.</p> "
                                                  "<p>You may modify and redistribute the program under the terms of the GPL (version 2 or later).  A copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam.  Rolisteam is copyrighted by its contributors.  See the 'COPYRIGHT' file for the complete list of contributors.  We provide no warranty for this program.</p>"
                                                  "<p><h3>URL:</h3>  <a href=\"http://code.google.com/p/rolisteam/\">http://code.google.com/p/rolisteam/</a></p> "
                                                  "<p><h3>BugTracker:</h3> <a href=\"http://code.google.com/p/rolisteam/issues/list\">http://code.google.com/p/rolisteam/issues/list</a></p> "
                                                  "<p><h3>Current developers :</h3> "
                                                  "<ul>"
                                                  "<li><a href=\"http://renaudguezennec.homelinux.org/accueil,3.html\">Renaud Guezennec</a></li>"
                                                  "</ul></p> "
                                                  "<p><h3>Retired developers :</h3>"
                                                  "<ul>"
                                                  "<li><a href=\"mailto:rolistik@free.fr\">Romain Campioni<a/> (rolistik)  </li>"
                                                  "</ul></p>"));
}


void MainWindow::aideEnLigne()
{

        QProcess *process = new QProcess;
        QStringList args;


#ifdef Q_WS_X11
        args << QLatin1String("-collectionFile")
        << QLatin1String("/usr/share/doc/rolisteam-doc/rolisteam.qhc");
        /* << QLatin1String("-register")
         << QLatin1String("/usr/share/doc/rolisteam-doc/rolisteam-manual.qch");*/
        process->start(QLatin1String("assistant"), args);

#elif defined Q_WS_WIN32
        args << QLatin1String("-collectionFile")
        << QLatin1String((qApp->applicationDirPath()+"/../resourcesdoc/rolisteam-doc/rolisteam.qhc").toLatin1());
        process->start(QLatin1String("assistant"), args);
#elif defined Q_WS_MAC
        QString a = QCoreApplication::applicationDirPath()+"/../Resources/doc/rolisteam.qhc";
        args << QLatin1String("-collectionFile")
        << QLatin1String(a.toLatin1());
        process->start(QLatin1String("/Developer/Applications/Qt/Assistant/Contents/MacOS/Assistant"), args);



#endif
        process->start(QLatin1String("assistant"), args);
        if (!process->waitForStarted())
                return;


}


void MainWindow::AddHealthState(const QColor &color, const QString &label, QList<DessinPerso::etatDeSante> &listHealthState)
{
        DessinPerso::etatDeSante state;
        state.couleurEtat = color;
        state.nomEtat = label;
        listHealthState.append(state);
}

void MainWindow::InitMousePointer(QCursor **pointer, const QString &iconFileName, const int hotX, const int hotY)
// the pointer of pointer is to avoid deep modification of the rolistik's legacy
{
     QBitmap bitmap(iconFileName);
     #ifndef Q_WS_X11
        QBitmap mask(32,32);
        mask.fill(Qt::color0);
        (*pointer) = new QCursor(bitmap,mask, hotX, hotY);
    #else
        (*pointer) = new QCursor(bitmap, hotX, hotY);
    #endif

}
