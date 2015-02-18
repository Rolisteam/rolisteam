/*************************************************************************
 *        Copyright (C) 2007 by Romain Campioni                          *
 *        Copyright (C) 2009 by Renaud Guezennec                         *
 *        Copyright (C) 2010 by Berenger Morel                           *
 *        Copyright (C) 2010 by Joseph Boudou                            *
 *                                                                       *
 *        http://www.rolisteam.org/                                      *
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

#include <QApplication>
#include <QBitmap>
#include <QBuffer>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStringBuilder>
#include <QTime>
#include <QUrl>
#include <QUuid>

#include "mainwindow.h"

#include "BarreOutils.h"
#include "CarteFenetre.h"
#include "Carte.h"
#include "chatlistwidget.h"
#include "ClientServeur.h"
#include "EditeurNotes.h"
#include "Image.h"
#include "networkmessagewriter.h"
#include "NouveauPlanVide.h"
#include "persons.h"
#include "playersList.h"
#include "playersListWidget.h"
#include "preferencesdialog.h"
#include "updatechecker.h"
#include "WorkspaceAmeliore.h"

#include "constantesGlobales.h"
#include "variablesGlobales.h"

#ifndef NULL_PLAYER
#include "LecteurAudio.h"
#endif

// Indique si le nom des PJ doit etre affiche ou pas
bool G_affichageNomPj;
// Indique si le nom des PNJ doit etre affiche ou pas
bool G_affichageNomPnj;
// Indique si le numero des PNJ doit etre affiche ou pas
bool G_affichageNumeroPnj;
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
MainWindow* MainWindow::m_singleton= NULL;

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
bool  MainWindow::showConnectionDialog()
{
    // Get a connection

    return m_networkManager->configAndConnect();

}

MainWindow* MainWindow::getInstance()
{
    if(NULL==m_singleton)
    {
        m_singleton = new MainWindow();
    }
    return m_singleton;
}

MainWindow::MainWindow()
    : QMainWindow(),m_networkManager(NULL)
{
    m_preferences = PreferencesManager::getInstance();
    m_networkManager = new ClientServeur;
    G_clientServeur = m_networkManager;

}
void MainWindow::setupUi()
{
    //m_preferences = Initialisation::getInstance();

    // Initialisation des variables globales
    G_affichageNomPj = true;
    G_affichageNomPnj = true;
    G_affichageNumeroPnj = true;

    // Initialisation de la liste des CarteFenetre, des Image et des Tchat
    listeCarteFenetre.clear();
    listeImage.clear();

    // Initialisation du pointeur vers la fenetre de parametrage de nouveau plan
    fenetreNouveauPlan = 0;
    m_version=tr("unknown");
    #ifdef VERSION_MINOR
        #ifdef VERSION_MAJOR
            #ifdef VERSION_MIDDLE
                m_version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
            #endif
        #endif
    #endif




    // Desactivation des animations
    setAnimated(false);
    // Creation de l'espace de travail
    workspace = new WorkspaceAmeliore();
    // Ajout de l'espace de travail dans la fenetre principale
    setCentralWidget(workspace);
    // Connexion du changement de fenetre active avec la fonction de m.a.j du selecteur de taille des PJ
    connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(changementFenetreActive(QWidget *)));

    // Creation de la barre d'outils
    //barreOutils = new BarreOutils(this);
    m_toolBar = new BarreOutils();
    // Ajout de la barre d'outils a la fenetre principale
    addDockWidget(Qt::LeftDockWidgetArea, m_toolBar);

    // Creation du log utilisateur
    creerLogUtilisateur();
    // Ajout du log utilisateur a la fenetre principale
    addDockWidget(Qt::RightDockWidgetArea, m_dockLogUtil);

    // Add chatListWidget
    m_chatListWidget = new ChatListWidget(this);
    //m_chatListWidget = new ChatListWidget();
    addDockWidget(Qt::RightDockWidgetArea, m_chatListWidget);

    // Ajout de la liste d'utilisateurs a la fenetre principale
    m_playersList = new PlayersListWidget(this);
    //m_playersList = new PlayersListWidget();
    addDockWidget(Qt::RightDockWidgetArea, m_playersList);



    setWindowIcon(QIcon(":/logo.png"));


#ifndef NULL_PLAYER
    // Creation du lecteur audio
    m_audioPlayer = LecteurAudio::getInstance(this);
    /*m_audioDock = new QDockWidget(this);
    m_audioDock->setObjectName("LecteurAudio");
    m_audioDock->setWidget(m_audioPlayer);*/
    // Ajout du lecteur audio a la fenetre principale
    addDockWidget(Qt::RightDockWidgetArea,m_audioPlayer );
#endif
    //readSettings();
    // Create Preference dialog
    m_preferencesDialog = new PreferencesDialog(this);

    // Creation de la barre de menus et des menus
    creerMenu();
    // Association des actions des menus avec des fonctions
    associerActionsMenus();
    // Autoriser/interdire action en fonction de la nature de l'utilisateur (joueur ou MJ)
    //autoriserOuInterdireActions();

    // Creation de l'editeur de notes
    editeurNotes = new EditeurNotes(this);
    // Ajout de l'editeur de notes au workspace
    workspace->addWindow(editeurNotes);
    // Mise a jour du titre de l'editeur de notes
    editeurNotes->setWindowTitle(tr("Minutes Editor"));
    // Masquage de l'editeur de notes
    editeurNotes->hide();

    // Initialisation des etats de sante des PJ/PNJ (variable declarees dans DessinPerso.cpp)
    AddHealthState(Qt::black, tr("healthy"), G_etatsDeSante);
    AddHealthState(QColor(255, 100, 100),tr("lightly wounded"),G_etatsDeSante);
    AddHealthState(QColor(255, 0, 0),tr("seriously injured"),G_etatsDeSante);
    AddHealthState(Qt::gray,tr("Dead"),G_etatsDeSante);
    AddHealthState(QColor(80, 80, 255),tr("Sleeping"),G_etatsDeSante);
    AddHealthState(QColor(0, 200, 0),tr("Bewitched"),G_etatsDeSante);

    // Initialisation des pointeurs de souris
    InitMousePointer(&G_pointeurDessin, ":/resources/icones/pointeur dessin.png", 8, 8);
    InitMousePointer(&G_pointeurTexte, ":/resources/icones/pointeur texte.png", 4, 13); //strange values here

    G_pointeurDeplacer  = new QCursor(QPixmap(":/resources/icones/pointeur deplacer.png"), 0, 0);
    G_pointeurOrienter  = new QCursor(QPixmap(":/resources/icones/pointeur orienter.png"), 10, 12);
    G_pointeurPipette   = new QCursor(QPixmap(":/resources/icones/pointeur pipette.png"), 1, 19);
    G_pointeurAjouter   = new QCursor(QPixmap(":/resources/icones/pointeur ajouter.png"), 6, 0);
    G_pointeurSupprimer = new QCursor(QPixmap(":/resources/icones/pointeur supprimer.png"), 6, 0);
    G_pointeurEtat      = new QCursor(QPixmap(":/resources/icones/pointeur etat.png"), 0, 0);

    PlayersList & g_playersList = PlayersList::instance();
    if (G_client)
    {
        // We want to know if the server refuses local player to be GM
        connect(&g_playersList, SIGNAL(localGMRefused()), this, SLOT(changementNatureUtilisateur()));
        // We send a message to del local player when we quit
        connect(this, SIGNAL(closing()), &g_playersList, SLOT(sendDelLocalPlayer()));
    }
    else
    {
        // send datas on new connection if we are the server
        connect(G_clientServeur, SIGNAL(linkAdded(Liaison *)), this, SLOT(emettreTousLesPlans(Liaison *)));
        connect(G_clientServeur, SIGNAL(linkAdded(Liaison *)), this, SLOT(emettreToutesLesImages(Liaison *)));
    }
    connect(&g_playersList, SIGNAL(playerAdded(Player *)), this, SLOT(notifyAboutAddedPlayer(Player *)));
    connect(&g_playersList, SIGNAL(playerDeleted(Player *)), this, SLOT(notifyAboutDeletedPlayer(Player *)));
}

MainWindow::~MainWindow()
{
    delete m_dockLogUtil;
}
void MainWindow::setNetworkManager(ClientServeur* tmp)
{
    m_networkManager = tmp;
    m_networkManager->setParent(this);
}


void MainWindow::creerLogUtilisateur()
{
        // Creation du dockWidget contenant la fenetre de log utilisateur
        m_dockLogUtil = new QDockWidget(tr("Events"), this);
        //m_dockLogUtil = new QDockWidget(tr("Events"));
        m_dockLogUtil->setObjectName("dockLogUtil");
        m_dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
        m_dockLogUtil->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

        //Creation du log utilisateur
        logUtilisateur = new QTextEdit(m_dockLogUtil);
        logUtilisateur->setReadOnly(true);

        // Insertion de la fenetre de log utilisateur dans le dockWidget
        m_dockLogUtil->setWidget(logUtilisateur);
        // Largeur minimum du log utilisateur
        m_dockLogUtil->setMinimumWidth(125);


}

void MainWindow::creerMenu()
{
        // Creation de la barre de menus
        QMenuBar *barreMenus = new QMenuBar(this);
        // Ajout de la barre de menus a la fenetre principale
        setMenuBar(barreMenus);

        // Creation du menu Fichier
        QMenu *menuFichier = new QMenu (tr("File"), barreMenus);
        actionNouveauPlan                = menuFichier->addAction(tr("New empty Map"));
        menuFichier->addSeparator();
        actionOuvrirPlan                 = menuFichier->addAction(tr("Open Map"));
        actionOuvrirEtMasquerPlan        = menuFichier->addAction(tr("Open And hide Map"));
        actionOuvrirScenario         = menuFichier->addAction(tr("Open scenario"));
        actionOuvrirImage                 = menuFichier->addAction(tr("Open Picture"));
        actionOuvrirNotes                = menuFichier->addAction(tr("Open Minutes"));
        menuFichier->addSeparator();
        actionFermerPlan                 = menuFichier->addAction(tr("Close Map/Picture"));
        menuFichier->addSeparator();
        actionSauvegarderPlan        = menuFichier->addAction(tr("Save Map"));
        actionSauvegarderScenario        = menuFichier->addAction(tr("Save scenario"));
        actionSauvegarderNotes           = menuFichier->addAction(tr("Save Minutes"));
        menuFichier->addSeparator();
        actionPreferences                = menuFichier->addAction(tr("Preferences"));
        menuFichier->addSeparator();
        actionQuitter                = menuFichier->addAction(tr("Quit"));

        // Network action
        QMenu *networkMenu = new QMenu (tr("Network"), barreMenus);
        m_reconnectAct  = networkMenu->addAction(tr("Reconnection"));
        m_disconnectAct = networkMenu->addAction(tr("Disconnect"));


        // Creation du menu Affichage
        QMenu *menuAffichage = new QMenu (tr("View"), barreMenus);
        actionAfficherNomsPj         = menuAffichage->addAction(tr("Display PC names"));
        actionAfficherNomsPnj        = menuAffichage->addAction(tr("Display NPC names"));
        actionAfficherNumerosPnj         = menuAffichage->addAction(tr("Display NPC number"));


        // Actions checkables
        actionAfficherNomsPj->setCheckable(true);
        actionAfficherNomsPnj->setCheckable(true);
        actionAfficherNumerosPnj->setCheckable(true);


        // Choix des actions selectionnees au depart
        actionAfficherNomsPj        ->setChecked(true);
        actionAfficherNomsPnj   ->setChecked(true);
        actionAfficherNumerosPnj->setChecked(true);

        // Creation du menu Fenetre
        menuFenetre = new QMenu (tr("Sub-Windows"), barreMenus);

        // Creation du sous-menu Reorganiser
        QMenu *sousMenuReorganise        = new QMenu (tr("Reorganize"), barreMenus);
        actionCascade                = sousMenuReorganise->addAction(tr("Cascade"));
        actionTuiles                 = sousMenuReorganise->addAction(tr("Tile"));
        // Ajout du sous-menu Reorganiser au menu Fenetre
        menuFenetre->addMenu(sousMenuReorganise);
        menuFenetre->addSeparator();

        // Ajout des actions d'affichage des fenetres d'evenement, utilisateurs et lecteur audio
        menuFenetre->addAction(m_toolBar->toggleViewAction());
        menuFenetre->addAction(m_dockLogUtil->toggleViewAction());
        menuFenetre->addAction(m_chatListWidget->toggleViewAction());
        menuFenetre->addAction(m_playersList->toggleViewAction());
#ifndef NULL_PLAYER
        menuFenetre->addAction(m_audioPlayer->toggleViewAction());
#endif
        menuFenetre->addSeparator();

        // Ajout de l'action d'affichage de l'editeur de notes
        actionEditeurNotes = menuFenetre->addAction(tr("Minutes Editor"));
        actionEditeurNotes->setCheckable(true);
        actionEditeurNotes->setChecked(false);
        // Connexion de l'action avec l'affichage/masquage de l'editeur de notes
        connect(actionEditeurNotes, SIGNAL(triggered(bool)), this, SLOT(afficherEditeurNotes(bool)));

        // Ajout du sous-menu Tchat
        menuFenetre->addMenu(m_chatListWidget->chatMenu());

        // Creation du menu Aide
        QMenu *menuAide = new QMenu (tr("Help"), barreMenus);
        actionAideLogiciel = menuAide->addAction(tr("Help about %1").arg(m_preferences->value("Application_Name","rolisteam").toString()));
        menuAide->addSeparator();
        actionAPropos = menuAide->addAction(tr("About %1").arg(m_preferences->value("Application_Name","rolisteam").toString()));

        // Ajout des menus a la barre de menus
        barreMenus->addMenu(menuFichier);
        barreMenus->addMenu(menuAffichage);
        barreMenus->addMenu(menuFenetre);
        barreMenus->addMenu(networkMenu);
        barreMenus->addMenu(menuAide);
}

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

        // network
        connect(m_networkManager,SIGNAL(stopConnectionTry()),this,SLOT(stopReconnection()));
        connect(m_disconnectAct,SIGNAL(triggered()),m_networkManager,SLOT(disconnect()));

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

void MainWindow::autoriserOuInterdireActions()
{
        // L'utilisateur est un joueur
//        Player* tmp = PlayersList::instance().localPlayer();
//        if (NULL==tmp)//!tmp->isGM()
//        {
//                actionNouveauPlan->setEnabled(false);
//                actionOuvrirPlan->setEnabled(false);
//                actionOuvrirEtMasquerPlan->setEnabled(false);
//                actionOuvrirScenario->setEnabled(false);
//                actionFermerPlan->setEnabled(false);
//                actionSauvegarderPlan->setEnabled(false);
//                actionSauvegarderScenario->setEnabled(false);
//        }

//        // L'utilisateur est un MJ
//        else
//        {
//        }
}

void MainWindow::ajouterCarte(CarteFenetre *carteFenetre, QString titre,QSize mapsize,QPoint pos )
{
        // Ajout de la CarteFenetre a la liste (permet par la suite de parcourir l'ensemble des cartes)
        listeCarteFenetre.append(carteFenetre);

        // Ajout de la carte au workspace
        QWidget* tmp = workspace->addWindow(carteFenetre);
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


        connect(m_toolBar,SIGNAL(currentToolChanged(BarreOutils::Tool)),carte,SLOT(setPointeur(BarreOutils::Tool)));

        // Connexion de la demande de changement de couleur de la carte avec celle de la barre d'outils
        connect(carte, SIGNAL(changeCouleurActuelle(QColor)), m_toolBar, SLOT(changeCouleurActuelle(QColor)));
        // Connexion de la demande d'incrementation du numero de PNJ de la carte avec celle de la barre d'outils
        connect(carte, SIGNAL(incrementeNumeroPnj()), m_toolBar, SLOT(incrementeNumeroPnj()));
        // Connexion de la demande de changement de diametre des PNJ de la carte avec celle de la barre d'outils
        connect(carte, SIGNAL(mettreAJourPnj(int, QString)), m_toolBar, SLOT(mettreAJourPnj(int, QString)));
        // Affichage des noms et numeros des PJ/PNJ
        connect(actionAfficherNomsPj, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNomsPj(bool)));
        connect(actionAfficherNomsPnj, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNomsPnj(bool)));
        connect(actionAfficherNumerosPnj, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNumerosPnj(bool)));

        // Mise a jour du pointeur de souris de la carte
        carte->setPointeur(G_outilCourant);

        
        // new PlayersList connection
        connect(carteFenetre, SIGNAL(activated(Carte *)), m_playersList->model(), SLOT(changeMap(Carte *)));
        connect(carteFenetre, SIGNAL(activated(Carte *)), m_toolBar, SLOT(changeMap(Carte *)));

        // Affichage de la carte
        carteFenetre->show();
        tmp->setWindowIcon(QIcon(":/resources/icones/chatIcone.png"));
        carteFenetre->setWindowIcon(QIcon(":/resources/icones/chatIcone.png"));
}

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
        connect(m_toolBar->actionMain,                SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurMain()));

        // Connexion des actions de changement d'outil avec la fonction de changement de pointeur de souris normal
        connect(m_toolBar->actionCrayon,         SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionLigne,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionRectVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionRectPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionElliVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionElliPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionTexte,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionAjoutPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionSupprPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionDeplacePnj, SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionEtatPnj,        SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));

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

void MainWindow::ouvrirEtMasquerPlan()
{
        ouvrirPlan(true);
}

void MainWindow::ouvrirPlan(bool masquer)
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, masquer?tr("Open and Hide Map"):tr("Open Map"), m_preferences->value("MapDirectory",QDir::homePath()).toString(),
                                          tr("Map (*.pla *.jpg *.jpeg *.png *.bmp)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Loading error"));
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // On met a jour le chemin vers les plans
        int dernierSlash = fichier.lastIndexOf("/");
        m_preferences->registerValue("MapDirectory",fichier.left(dernierSlash));

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
                        msgBox.setText(tr("Unsupported file format"));
                        msgBox.exec();
                        return;
                }
                // Creation de l'identifiant
                QString idCarte = QUuid::createUuid().toString();
                // Creation de la carte
                Carte *carte = new Carte(idCarte, &image, masquer);
                carte->setPermissionMode(NouveauPlanVide::GM_ONLY);
                // Creation de la CarteFenetre
                CarteFenetre *carteFenetre = new CarteFenetre(carte,this, workspace);
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
                        sizeof(quint8) + sizeof(quint8) +
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
                quint8 tailleDesPj = 12;
                memcpy(&(donnees[p]), &tailleDesPj, sizeof(quint8));
                p+=sizeof(quint8);
                // Ajout de l'info "plan masque au chargement?"
                quint8 masquerPlan = masquer;
                memcpy(&(donnees[p]), &masquerPlan, sizeof(quint8));
                p+=sizeof(quint8);


                quint8 mode = carte->getPermissionMode();
                memcpy(&(donnees[p]), &mode, sizeof(quint8));
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
        // On recupere la taille des PJ (ignored)
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
        Carte *carte = new Carte(idCarte, &fondOriginal, &fond, &alpha);
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte,this,workspace);

        // Ajout de la carte au workspace : si aucun nom de fichier n'est passe en parametre, il s'agit d'une lecture de
        // carte dans le cadre de l'ouverture d'un fichier scenario : on prend alors le titre associe a la carte. Sinon
        // il s'agit d'un fichier plan : on prend alors le nom du fichier

        QPoint pos2 = carteFenetre->mapFromParent(topleft);
        out2 <<" lire plan 2 " << pos2.x() << "," << pos2.y()  << " size=("<< mapsize.width()<<","<<mapsize.height() << endl;

        if (nomFichier.isEmpty())
                ajouterCarte(carteFenetre, titre,mapsize,topleft);
        else
                ajouterCarte(carteFenetre, nomFichier,mapsize,topleft);

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

                if (visible || (type == DessinPerso::pnj && PlayersList::instance().localPlayer()->isGM()))
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


void MainWindow::ouvrirImage()
{
        // Ouverture du selecteur de fichier
    QString fichier = QFileDialog::getOpenFileName(this, tr("Open Picture"), m_preferences->value("ImageDirectory",QDir::homePath()).toString(),
                                          tr("Picture (*.jpg *.jpeg *.png *.bmp)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Loading error"));
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // On met a jour le chemin vers les images
        int dernierSlash = fichier.lastIndexOf("/");
        m_preferences->registerValue("ImageDirectory",fichier.left(dernierSlash));

        // Chargement de l'image
        QImage *img = new QImage(fichier);
        // Verification du chargement de l'image
        if (img->isNull())
        {
                msgBox.setText(tr("Unsupported file format"));
                msgBox.exec();
                delete img;
                return;
        }

        // Suppression de l'extension du fichier pour obtenir le titre de l'image
        int dernierPoint = fichier.lastIndexOf(".");
        QString titre = fichier.left(dernierPoint);
        titre = titre.right(titre.length()-dernierSlash-1);
        titre+= tr(" (Picture)");

        // Creation de l'action correspondante
        QAction *action = menuFenetre->addAction(titre);
        action->setCheckable(true);
        action->setChecked(true);

        // Creation de l'identifiant
        QString idImage = QUuid::createUuid().toString();

        // Creation de la fenetre image
        Image *imageFenetre = new Image(this,idImage, G_idJoueurLocal, img, action,workspace);

        // Ajout de l'image a la liste (permet par la suite de parcourir l'ensemble des images)
        listeImage.append(imageFenetre);

        // Ajout de l'image au workspace
        workspace->addWindow(imageFenetre);

        // Mise a jour du titre de l'image
        imageFenetre->setWindowTitle(titre);

        // Connexion de l'action d'outil main avec la fonction de changement de pointeur de souris en main
        connect(m_toolBar->actionMain,                SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurMain()));

        // Connexion des actions de changement d'outil avec la fonction de changement de pointeur de souris normal
        connect(m_toolBar->actionCrayon,         SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionLigne,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionRectVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionRectPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionElliVide,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionElliPlein,  SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionTexte,          SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionAjoutPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionSupprPnj,   SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionDeplacePnj, SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));
        connect(m_toolBar->actionEtatPnj,        SIGNAL(triggered(bool)), imageFenetre, SLOT(pointeurNormal()));

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
        NetworkMessageWriter message = NetworkMessageWriter(NetMsg::PictureCategory, NetMsg::AddPictureAction);
        imageFenetre->fill(message);
        message.sendAll();
}


void MainWindow::fermerPlanOuImage()
{
        // On recupere la fenetre active (qui est forcement de type CarteFenetre ou Image, sans quoi l'action
        // ne serait pas dispo dans le menu Fichier)
        QWidget *active = workspace->activeWindow();

        // Ne devrait jamais arriver
        if (!active)
        {
                qWarning("Close map action called when no widget is active in the workspace (fermerPlanOuImage - MainWindow.h)");
                return;
        }

        // On verifie pour le principe qu'il s'agit bien d'une CarteFenetre ou d'une Image
        if (active->objectName() != "CarteFenetre" && active->objectName() != "Image")
        {
                qWarning("not expected type of windows (fermerPlanOuImage - MainWindow.h)");
                return;
        }

        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
        // M.a.j du titre et du message
        if (active->objectName() == "CarteFenetre")
        {
                msgBox.setWindowTitle(tr("Close Map"));
                msgBox.setText(tr("Do you want to close this map?\nIt will be closed for everybody"));
        }
        else
        {
                msgBox.setWindowTitle(tr("Close Picture"));
                msgBox.setText(tr("Do you want to close this picture?\nIt will be closed for everybody"));
        }
        msgBox.exec();

        // Si l'utilisateur n'a pas clique sur "Fermer", on quitte
        if (msgBox.result() != QMessageBox::YesRole)
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

void MainWindow::afficherNomsPj(bool afficher)
{
        G_affichageNomPj = afficher;
}

void MainWindow::afficherNomsPnj(bool afficher)
{
        G_affichageNomPnj = afficher;
}
void MainWindow::afficherNumerosPnj(bool afficher)
{
        G_affichageNumeroPnj = afficher;
}

void MainWindow::mettreAJourEspaceTravail()
{
        // On recupere la fenetre active
        QWidget *active = workspace->activeWindow();

        // S'il y a une fenetre active, on la passe a la fonction changementFenetreActive
        if (active)
                changementFenetreActive(active);
}

void MainWindow::changementFenetreActive(QWidget *widget)
{
    bool localPlayerIsGM = PlayersList::instance().localPlayer()->isGM();
    if (widget != NULL && widget->objectName() == QString("CarteFenetre") && localPlayerIsGM)
    {
        actionFermerPlan->setEnabled(true);
        actionSauvegarderPlan->setEnabled(true);
    }
    else if (widget != NULL && widget->objectName() == QString("Image") &&
            (localPlayerIsGM || static_cast<Image *>(widget)->proprietaireImage()))
    {
        actionFermerPlan->setEnabled(true);
        actionSauvegarderPlan->setEnabled(false);
    }
    else
    {
        actionFermerPlan->setEnabled(false);
        actionSauvegarderPlan->setEnabled(false);
    }
}
void MainWindow::majCouleursPersonnelles()
{
        m_toolBar->majCouleursPersonnelles();
}

void MainWindow::nouveauPlan()
{
        fenetreNouveauPlan = new NouveauPlanVide(this);
}
void MainWindow::creerNouveauPlanVide(QString titre, QString idCarte, QColor couleurFond, quint16 largeur, quint16 hauteur,quint8 mode)
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
        Carte *carte = new Carte(idCarte, &image);
        carte->setPermissionMode(getPermission(mode));
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte,this, workspace);
        // Ajout de la carte au workspace
        ajouterCarte(carteFenetre, titre);
}


void MainWindow::aucunNouveauPlanVide()
{
        delete fenetreNouveauPlan;
        fenetreNouveauPlan = 0;
}


void MainWindow::emettreTousLesPlans(Liaison * link)
{
        // Taille de la liste des CarteFenetre
        int tailleListe = listeCarteFenetre.size();

        // Parcours de la liste
        for (int i=0; i<tailleListe; i++)
        {
                // On demande a la carte contenue dans la CarteFenetre de s'emettre vers
                // l'utilisateur dont la liaison est passee en parametre
                listeCarteFenetre[i]->carte()->emettreCarte(listeCarteFenetre[i]->windowTitle(), link);
                // On emet egalement vers la liaison l'ensemble des personnages presents sur la carte
                listeCarteFenetre[i]->carte()->emettreTousLesPersonnages(link);
        }
}


void MainWindow::emettreToutesLesImages(Liaison * link)
{
        int tailleListe = listeImage.size();
        NetworkMessageWriter message = NetworkMessageWriter(NetMsg::PictureCategory, NetMsg::AddPictureAction);
        for (int i=0; i < tailleListe; i++)
        {
                listeImage[i]->fill(message);
                message.sendTo(link);
        }
}

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

bool MainWindow::estLaFenetreActive(QWidget *widget)
{
        return widget == workspace->activeWindow() && widget->isVisible();
}
void MainWindow::quitterApplication(bool perteConnexion)
{
        // Creation de la boite d'alerte
        QMessageBox msgBox(this);
        QAbstractButton *boutonSauvegarder         = msgBox.addButton(QMessageBox::Save);
        QAbstractButton *boutonQuitter                 = msgBox.addButton(tr("Quit"), QMessageBox::RejectRole);
        //msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        // On supprime l'icone de la barre de titre
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

        // Creation du message
        QString message;
        QString msg = m_preferences->value("Application_Name","rolisteam").toString();

        // S'il s'agit d'une perte de connexion
        if (perteConnexion)
        {
            message = tr("Connection has been lost. %1 will be close").arg(msg);
                // Icone de la fenetre
                msgBox.setIcon(QMessageBox::Critical);
                // M.a.j du titre et du message
                msgBox.setWindowTitle(tr("Connection lost"));
        }
        else
        {
                // Icone de la fenetre
                msgBox.setIcon(QMessageBox::Information);
                // Ajout d'un bouton
                msgBox.addButton(QMessageBox::Cancel);
                // M.a.j du titre et du message
                msgBox.setWindowTitle(tr("Quit %1 ").arg(msg));
        }

        // Si l'utilisateur est un joueur
        if (!PlayersList::instance().localPlayer()->isGM())
        {
                message += tr("Do you want to save your minutes before to quit %1?").arg(msg);
        }
        // Si l'utilisateut est un MJ
        else
        {
                message += tr("Do you want to save your scenario before to quit %1?").arg(msg);

        }

        //M.a.j du message de la boite de dialogue
        msgBox.setText(message);
        // Ouverture de la boite d'alerte
        msgBox.exec();

        // Si l'utilisateur a clique sur "Quitter", on quitte l'application
        if (msgBox.clickedButton() == boutonQuitter)
        {
            emit closing();
            writeSettings();

        /// @todo : make sure custom colors are saved.
            // On quitte l'application
            qApp->quit();
        }

        // Si l'utilisateur a clique sur "Sauvegarder", on applique l'action appropriee a la nature de l'utilisateur
        else if (msgBox.clickedButton() == boutonSauvegarder)
        {
                bool ok;
                // Si l'utilisateur est un joueur, on sauvegarde les notes
                if (!PlayersList::instance().localPlayer()->isGM())
                        ok = sauvegarderNotes();
                // S'il est MJ, on sauvegarde le scenario
                else
                        ok = sauvegarderScenario();

                // Puis on quitte l'application si l'utilisateur a sauvegarde ou s'il s'agit d'une perte de connexion
                if (ok || perteConnexion)
                {
                    emit closing();
                    writeSettings();
                    // On sauvegarde le fichier d'initialisation
                    /// @todo : make sure custom colors are saved.
                    // On quitte l'application
                    qApp->quit();
                }
        }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
        event->ignore();
        quitterApplication();
}
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

void MainWindow::registerSubWindow(QWidget * subWindow)
{

    workspace->addWindow(subWindow);
}

void MainWindow::sauvegarderPlan()
{
        // On recupere la fenetre active
        QWidget *active = workspace->activeWindow();

        // On verifie pour le principe qu'il s'agit bien d'une CarteFenetre
        if (active->objectName() != "CarteFenetre")
        {
                qWarning("Not a map (sauvegarderPlan - MainWindow.h)");
                return;
        }

        // Ouverture du selecteur de fichiers
        QString fichier = QFileDialog::getSaveFileName(this, tr("Save Map"), m_preferences->value("MapDirectory",QDir::homePath()).toString(), tr("Map (*.pla)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

#ifdef MACOS
        // On rajoute l'extension
        fichier += ".pla";
#endif

        // On met a jour le chemin vers les plans
        int dernierSlash = fichier.lastIndexOf("/");
        m_preferences->registerValue("MapDirectory",fichier.left(dernierSlash));

        // Creation du fichier
        QFile file(fichier);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("could not open file for writting (sauvegarderPlan - MainWindow.cpp)");
                return;
        }
        QDataStream out(&file);
        // On demande a la carte de se sauvegarder dans le fichier
        ((CarteFenetre *)active)->carte()->sauvegarderCarte(out);
        // Fermeture du fichier
        file.close();
}


void MainWindow::changementNatureUtilisateur()
{
        // M.a.j des menus du mainWindow
        autoriserOuInterdireActions();
        // M.a.j de la barre d'outils
        //m_toolBar->updateUi();
        updateUi();
#ifndef NULL_PLAYER
        removeDockWidget(m_audioPlayer);
        //addDockWidget(Qt::RightDockWidgetArea, m_audioDock);
        m_audioPlayer->show();
#endif
}


void MainWindow::afficherEditeurNotes(bool afficher, bool cocherAction)
{
        // Affichage de l'editeur de notes
        editeurNotes->setVisible(afficher);

        // Si la fonction a pas ete appelee par la listeUtilisateurs ou par l'editeur lui-meme, on coche/decoche l'action associee
        if (cocherAction)
        {
                actionEditeurNotes->setChecked(afficher);
        }
        else
        {// Sinon on coche/decoche la case de l'editeur de notes dans la listeUtilisateurs
            /// @todo
        }
}
void MainWindow::ouvrirNotes()
{
        // Ouverture du selecteur de fichier
    QString fichier = QFileDialog::getOpenFileName(this, tr("Open Minutes"), m_preferences->value("MinutesDirectory",QDir::homePath()).toString(), tr("Html Documents (*.htm *.html)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // On met a jour le chemin vers les notes
        int dernierSlash = fichier.lastIndexOf("/");
        m_preferences->registerValue("MinutesDirectory",fichier.left(dernierSlash));

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
        afficherEditeurNotes(true, true);
}
bool MainWindow::sauvegarderNotes()
{
        // Ouverture du selecteur de fichiers
        QString fichier = QFileDialog::getSaveFileName(this, tr("Save Minutes"), m_preferences->value("MinutesDirectory",QDir::homePath()).toString(), tr("HTML Documents (*.htm *.html)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return false;

#ifdef MACOS
        // On rajoute l'extension
        fichier += ".htm";
#endif

        // On met a jour le chemin vers les notes
        int dernierSlash = fichier.lastIndexOf("/");
        m_preferences->registerValue("MinutesDirectory",fichier.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("cannot be open (sauvegarderNotes - MainWindow.cpp)");
                return false;
        }
        QDataStream in(&file);
        // On demande a l'editeur de notes de les sauvegarder dans le fichier
        editeurNotes->sauvegarderNotes(in);
        // Fermeture du fichier

        file.close();

        return true;
}
void MainWindow::ouvrirScenario()
{
        // Ouverture du selecteur de fichier
        QString fichier = QFileDialog::getOpenFileName(this, tr("Open scenario"), m_preferences->value("StoryDirectory",QDir::homePath()).toString(), tr("Scenarios (*.sce)"));

        // Si l'utilisateur a clique sur "Annuler", on quitte la fonction
        if (fichier.isNull())
                return;

        // On met a jour le chemin vers les scenarii
        int dernierSlash = fichier.lastIndexOf("/");
        m_preferences->registerValue("StoryDirectory",fichier.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(fichier);
        // Ouverture du fichier en lecture seule
        if (!file.open(QIODevice::ReadOnly))
        {
                qWarning("Cannot be read (ouvrirScenario - MainWindow.cpp)");
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
bool MainWindow::sauvegarderScenario()
{
        // Ouverture du selecteur de fichiers
        QString filename = QFileDialog::getSaveFileName(this, tr("Save Scenarios"), m_preferences->value("StoryDirectory",QDir::homePath()).toString(), tr("Scenarios (*.sce)"));


        if (filename.isNull())
        {
                return false;
        }

        if(!filename.endsWith(".sce"))
        {
                filename += ".sce";
        }

        int dernierSlash = filename.lastIndexOf("/");
        m_preferences->registerValue("StoryDirectory",filename.left(dernierSlash));

        // Creation du descripteur de fichier
        QFile file(filename);
        // Ouverture du fichier en ecriture seule
        if (!file.open(QIODevice::WriteOnly))
        {
                qWarning("cannot be open (sauvegarderScenario - MainWindow.cpp)");
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
        //out2 <<" save tous les plans " << listeCarteFenetre[i]->pos().x() << "," << listeCarteFenetre[i]->pos().y()  << " size=("<< listeCarteFenetre[i]->size().width()<<","<<listeCarteFenetre[i]->size().height() << endl;
        QPoint pos2 = listeCarteFenetre[i]->mapFromParent(listeCarteFenetre[i]->pos());
       // out2 <<" save tous les plans " << pos2.x() << "," << pos2.y()  << " size=("<< listeCarteFenetre[i]->size().width()<<","<<listeCarteFenetre[i]->size().height() << endl;

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
        Image *imageFenetre = new Image(this,idImage, G_idJoueurLocal, &img, action,workspace);

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
//void MainWindow::sauvegarderFichierInitialisation()
//{
//    // Don't really write anything to the filesystem, but store new values in G_initialisation.

//    // ...les couleurs personnelles
//    for (int i=0; i<16; i++)
//            m_preferences->setCustomColorAt(i,barreOutils->donnerCouleurPersonnelle(i));

//#ifndef NULL_PLAYER
//    // ...le volume du lecteur audio
//    m_preferences->setVolumeLevel(m_audioPlayer->volume());
//#endif
//}


void MainWindow::aPropos()
{
        QMessageBox::about(this, tr("About Rolisteam"),
                            tr("<h1>Rolisteam v%1</h1>"
                            "<p>Rolisteam makes easy the management of any role playing games. It allows players to communicate to each others and to share maps and pictures. Rolisteam also provides many features for : permission management, background music and dice roll. Rolisteam is written in Qt4. Its dependencies are : Qt4 and Phonon.</p>").arg(m_version) %
                            tr("<p>You may modify and redistribute the program under the terms of the GPL (version 2 or later).  A copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam.  Rolisteam is copyrighted by its contributors.  See the 'COPYRIGHT' file for the complete list of contributors.  We provide no warranty for this program.</p>") %
                            tr("<p><h3>Web Sites :</h3>"
                            "<ul>"
                            "<li><a href=\"http://www.rolisteam.org/\">Official Rolisteam Site</a></li> "
                            "<li><a href=\"http://code.google.com/p/rolisteam/issues/list\">Bug Tracker</a></li> "
                            "</ul></p>"
                            "<p><h3>Current developers :</h3>"
                            "<ul>"
                            "<li><a href=\"http://renaudguezennec.homelinux.org/accueil,3.html\">Renaud Guezennec</a></li>"
                            "<li><a href=\"mailto:joseph.boudou@matabio.net\">Joseph Boudou<a/></li>"
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
            if (!process->waitForStarted(2000))
            {
                if (!QDesktopServices::openUrl(QUrl("http://wiki.rolisteam.org/")))
                {
                    QMessageBox * msgBox = new QMessageBox(
                            QMessageBox::Information,
                            tr("Help"),
                                tr("Documentation of %1 can be found online at :<br> <a href=\"http://wiki.rolisteam.org\">http://wiki.rolisteam.org/</a>").arg(m_preferences->value("Application_Name","rolisteam").toString()),
                            QMessageBox::Ok
                            );
                    msgBox->exec();
                }
            }
}

void MainWindow::checkUpdate()
{
    if(m_preferences->value("MainWindow_MustBeChecked",true).toBool())
    {
        m_updateChecker = new UpdateChecker();
        m_updateChecker->startChecking();
        connect(m_updateChecker,SIGNAL(checkFinished()),this,SLOT(updateMayBeNeeded()));
    }

}
void MainWindow::updateUi()
{
/// @todo hide diametrePNj for players.
    /*
        diametrePnj->setVisible(false);
    }*/
    m_toolBar->updateUi();
    #ifndef NULL_PLAYER
        m_audioPlayer->updateUi();
    #endif
    if(!PlayersList::instance().localPlayer()->isGM())
    {
        actionNouveauPlan->setEnabled(false);
        actionOuvrirPlan->setEnabled(false);
        actionOuvrirEtMasquerPlan->setEnabled(false);
        actionOuvrirScenario->setEnabled(false);
        actionFermerPlan->setEnabled(false);
        actionSauvegarderPlan->setEnabled(false);
        actionSauvegarderScenario->setEnabled(false);
    }

}
void MainWindow::updateMayBeNeeded()
{
    if(m_updateChecker->mustBeUpdated())
    {
        QMessageBox::information(this,tr("Update Monitor"),tr("The %1 version has been released. Please take a look at <a href=\"http://www.rolisteam.org/download\">Download page</a> for more information").arg(m_updateChecker->getLatestVersion()));
    }
    m_updateChecker->deleteLater();
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
void MainWindow::readSettings()
{
    QSettings settings("rolisteam","rolisteam1/preferences");

    move(settings.value("pos", QPoint(200, 200)).toPoint());
    resize(settings.value("size", QSize(600, 400)).toSize());
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_preferences->readSettings(settings);
}

void MainWindow::writeSettings()
{
    QSettings settings("rolisteam","rolisteam1/preferences");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    m_preferences->writeSettings(settings);
}

void MainWindow::notifyAboutAddedPlayer(Player * player) const
{
    ecrireLogUtilisateur(tr("%1 just joins the game.").arg(player->name()));
}

void MainWindow::notifyAboutDeletedPlayer(Player * player) const
{
    ecrireLogUtilisateur(tr("%1 just leaves the game.").arg(player->name()));
}
NouveauPlanVide::PermissionMode MainWindow::getPermission(int id)
{
    switch(id)
    {
       case 0:
        return NouveauPlanVide::GM_ONLY;
       case 1:
        return NouveauPlanVide::PC_MOVE;
       case 2:
        return NouveauPlanVide::PC_ALL;
       default:
        return NouveauPlanVide::GM_ONLY;
    }

}
void MainWindow::stopReconnection()
{
    m_reconnectAct->setEnabled(true);
    m_disconnectAct->setEnabled(false);
}
