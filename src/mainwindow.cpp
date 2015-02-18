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

#include "persons.h"
#include "playersList.h"
#include "playersListWidget.h"
#include "preferencesdialog.h"
#include "updatechecker.h"
#include "WorkspaceAmeliore.h"


#include "textedit.h"


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
//QCursor *G_pointeurDessin;
// Contient le pointeur de souris pour le texte
//QCursor *G_pointeurTexte;
// Contient le pointeur de souris pour deplacer les PJ/PNJ
//QCursor *G_pointeurDeplacer;
// Contient le pointeur de souris pour orienter les PJ/PNJ
//QCursor *G_pointeurOrienter;
// Contient le pointeur de souris pour la pipette (clic droit)
//QCursor *G_pointeurPipette;
// Contient le pointeur de souris pour ajouter un PNJ
//QCursor *G_pointeurAjouter;
// Contient le pointeur de souris pour supprimer en PNJ
//QCursor *G_pointeurSupprimer;
// Contient le pointeur de souris pour changer l'etat des PJ/PNJ
//QCursor *G_pointeurEtat;


// Pointeur vers la fenetre de log utilisateur (utilise seulement dans ce fichier)

MainWindow* MainWindow::m_singleton= NULL;

void MainWindow::notifyUser(QString msg)
{
    if(NULL!=m_singleton)
    {
        m_singleton->notifyUser_p(msg);
    }

}

void MainWindow::notifyUser_p(QString message)
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
    m_notifierDisplay->moveCursor(QTextCursor::End);

    // Affichage de l'heure
    m_notifierDisplay->setTextColor(Qt::darkGreen);
    m_notifierDisplay->append(heure);

    // Affichage du texte
    m_notifierDisplay->setTextColor(couleur);
    m_notifierDisplay->insertPlainText(message);
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
    m_shownProgress=false;
    m_preferences = PreferencesManager::getInstance();
    m_newEmptyMapDialog = new NewEmptyMapDialog(this);
    m_downLoadProgressbar = new QProgressBar();
    m_downLoadProgressbar->setRange(0,100);



    m_downLoadProgressbar->setVisible(false);


    m_mapWizzard = new MapWizzard(this);
    m_networkManager = new ClientServeur;
    m_ipChecker = new IpChecker(this);
    //G_clientServeur = m_networkManager;
    m_mapAction = new QMap<CarteFenetre*,QAction*>();

}
void MainWindow::setupUi()
{
    //m_preferences = Initialisation::getInstance();

    // Initialisation des variables globales
    G_affichageNomPj = true;
    G_affichageNomPnj = true;
    G_affichageNumeroPnj = true;

    // Initialisation de la liste des CarteFenetre, des Image et des Tchat
    m_mapWindowList.clear();
    m_pictureList.clear();


    m_version=tr("unknown");

#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    m_version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
#endif
#endif
#endif





    setAnimated(false);

    m_mdiArea = new WorkspaceAmeliore();



    setCentralWidget(m_mdiArea);
    // Connexion du changement de fenetre active avec la fonction de m.a.j du selecteur de taille des PJ
    connect(m_mdiArea, SIGNAL(subWindowActivated ( QMdiSubWindow * )), this, SLOT(changementFenetreActive(QMdiSubWindow *)));

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
    m_playersListWidget = new PlayersListWidget(this);
    //m_playersList = new PlayersListWidget();
    addDockWidget(Qt::RightDockWidgetArea, m_playersListWidget);



    setWindowIcon(QIcon(":/logo.png"));



#ifndef NULL_PLAYER
    m_audioPlayer = LecteurAudio::getInstance(this);
    addDockWidget(Qt::RightDockWidgetArea,m_audioPlayer );
#endif
    //readSettings();
    m_preferencesDialog = new PreferencesDialog(this);


    creerMenu();

    linkActionToMenu();


    // Creation de l'editeur de notes
    m_noteEditor= new TextEdit(this);

    m_noteEditorSub  = static_cast<QMdiSubWindow*>(m_mdiArea->addWindow(m_noteEditor,m_noteEditorAct));
    if(NULL!=m_noteEditorSub)
    {
        m_noteEditorSub->setWindowTitle(tr("Minutes Editor[*]"));
        m_noteEditorSub->setWindowIcon(QIcon(":/notes.png"));
        m_noteEditorSub->hide();
    }
    connect(m_noteEditor,SIGNAL(closed(bool)),m_noteEditorAct,SLOT(setChecked(bool)));
    connect(m_noteEditor,SIGNAL(closed(bool)),m_noteEditorSub,SLOT(setVisible(bool)));


    // Initialisation des etats de sante des PJ/PNJ (variable declarees dans DessinPerso.cpp)
    AddHealthState(Qt::black, tr("healthy"), G_etatsDeSante);
    AddHealthState(QColor(255, 100, 100),tr("lightly wounded"),G_etatsDeSante);
    AddHealthState(QColor(255, 0, 0),tr("seriously injured"),G_etatsDeSante);
    AddHealthState(Qt::gray,tr("Dead"),G_etatsDeSante);
    AddHealthState(QColor(80, 80, 255),tr("Sleeping"),G_etatsDeSante);
    AddHealthState(QColor(0, 200, 0),tr("Bewitched"),G_etatsDeSante);

    m_playerList = PlayersList::instance();

    connect(m_playerList, SIGNAL(playerAdded(Player *)), this, SLOT(notifyAboutAddedPlayer(Player *)));
    connect(m_playerList, SIGNAL(playerDeleted(Player *)), this, SLOT(notifyAboutDeletedPlayer(Player *)));

    connect(m_networkManager,SIGNAL(connectionStateChanged(bool)),this,SLOT(updateWindowTitle()));
    connect(m_networkManager,SIGNAL(connectionStateChanged(bool)),this,SLOT(networkStateChanged(bool)));

    connect(m_ipChecker,SIGNAL(finished(QString)),this,SLOT(showIp(QString)));
}

MainWindow::~MainWindow()
{
    delete m_dockLogUtil;
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    quitterApplication();
}

ClientServeur* MainWindow::getNetWorkManager()
{
    return m_networkManager;
}

void MainWindow::updateWindowTitle()
{
    setWindowTitle(tr("%1[*] - v%2 - %3 - %4 - %5").arg(m_preferences->value("applicationName","Rolisteam").toString())
                   .arg(m_version)
                   .arg(m_networkManager->isConnected() ? tr("Connected") : tr("Not Connected"))
                   .arg(m_networkManager->isServer() ? tr("Server") : tr("Client")).arg(m_playerList->localPlayer()->isGM() ? tr("GM") : tr("Player")));



}
void MainWindow::receiveData(quint64 readData,quint64 size)
{
    if(size==0)
    {
        m_downLoadProgressbar->setVisible(false);
        if(m_shownProgress)
        {
            statusBar()->removeWidget(m_downLoadProgressbar);
        }
        m_shownProgress=false;
        statusBar()->setVisible(false);
    }
    else if(readData!=size)
    {
        statusBar()->setVisible(true);
               qDebug()<< readData << size;
        statusBar()->addWidget(m_downLoadProgressbar);
        m_downLoadProgressbar->setVisible(true);
        quint64 i = (size-readData)*100/size;

        m_downLoadProgressbar->setValue(i);
        m_shownProgress=true;
    }

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
    m_notifierDisplay = new QTextEdit(m_dockLogUtil);
    m_notifierDisplay->setReadOnly(true);

    // Insertion de la fenetre de log utilisateur dans le dockWidget
    m_dockLogUtil->setWidget(m_notifierDisplay);
    // Largeur minimum du log utilisateur
    m_dockLogUtil->setMinimumWidth(125);


}

void MainWindow::creerMenu()
{
    // Creation de la barre de menus
    m_menuBar = new QMenuBar(this);
    // Ajout de la barre de menus a la fenetre principale
    setMenuBar(m_menuBar);

    // Creation du menu Fichier
    QMenu *menuFichier = new QMenu (tr("File"), m_menuBar);
    m_newMapAct                = menuFichier->addAction(QIcon(":/map.png"),tr("&New empty Map"));


    menuFichier->addSeparator();
    m_openMapAct                 = menuFichier->addAction(QIcon(":/map.png"),tr("Open Map…"));
    //actionOuvrirEtMasquerPlan        = menuFichier->addAction(QIcon(":/map.png"),tr("Open And hide Map"));
    m_openStoryAct         = menuFichier->addAction(QIcon(":/story.png"),tr("Open scenario"));
    m_openImageAct                 = menuFichier->addAction(QIcon(":/picture.png"),tr("Open Picture"));
    m_openMinutesAct = menuFichier->addAction(QIcon(":/notes.png"),tr("Open Minutes"));
    menuFichier->addSeparator();
    m_closeMap                 = menuFichier->addAction(tr("Close Map/Picture"));
    menuFichier->addSeparator();
    m_saveMapAct        = menuFichier->addAction(QIcon(":/map.png"),tr("Save Map"));
    m_saveStoryAct        = menuFichier->addAction(QIcon(":/story.png"),tr("Save scenario"));
    m_saveMinuteAct           = menuFichier->addAction(QIcon(":/notes.png"),tr("Save Minutes"));
    menuFichier->addSeparator();
    m_preferencesAct = menuFichier->addAction(QIcon(":/settings.png"),tr("Preferences"));

    m_preferencesAct->setShortcut(QKeySequence("Ctrl+P"));
    menuFichier->addSeparator();
    m_quitAct = menuFichier->addAction(QIcon(":/exit.png"),tr("Quit"));
    m_quitAct->setShortcut(QKeySequence::Quit);

    // Network action
    QMenu *networkMenu = new QMenu (tr("Network"), m_menuBar);
    m_reconnectAct  = networkMenu->addAction(tr("Reconnection"));
    m_disconnectAct = networkMenu->addAction(tr("Disconnect"));


    // Creation du menu Affichage
    QMenu *menuAffichage = new QMenu (tr("View"), m_menuBar);
    m_showPCAct         = menuAffichage->addAction(tr("Display PC names"));
    m_showNpcNameAct        = menuAffichage->addAction(tr("Display NPC names"));
    m_showNPCNumberAct = menuAffichage->addAction(tr("Display NPC number"));


    // Actions checkables
    m_showPCAct->setCheckable(true);
    m_showNpcNameAct->setCheckable(true);
    m_showNPCNumberAct->setCheckable(true);


    // Choix des actions selectionnees au depart
    m_showPCAct->setChecked(true);
    m_showNpcNameAct->setChecked(true);
    m_showNPCNumberAct->setChecked(true);

    // Creation du menu Fenetre
    m_windowMenu = new QMenu (tr("Sub-Windows"), m_menuBar);

    // Creation du sous-menu Reorganiser
    QMenu* organizeSubMenu        = new QMenu (tr("Reorganize"), m_menuBar);
    m_tabOrdering  = organizeSubMenu->addAction(tr("Tabs"));
    m_tabOrdering->setCheckable(true);
    m_cascadeAction  = organizeSubMenu->addAction(tr("Cascade"));
    m_tuleAction   = organizeSubMenu->addAction(tr("Tile"));
    // Ajout du sous-menu Reorganiser au menu Fenetre
    m_windowMenu->addMenu(organizeSubMenu);
    m_windowMenu->addSeparator();

    // Ajout des actions d'affichage des fenetres d'evenement, utilisateurs et lecteur audio
    m_windowMenu->addAction(m_toolBar->toggleViewAction());
    m_windowMenu->addAction(m_dockLogUtil->toggleViewAction());
    m_windowMenu->addAction(m_chatListWidget->toggleViewAction());
    m_windowMenu->addAction(m_playersListWidget->toggleViewAction());
#ifndef NULL_PLAYER
    m_windowMenu->addAction(m_audioPlayer->toggleViewAction());
#endif
    m_windowMenu->addSeparator();

    // Ajout de l'action d'affichage de l'editeur de notes
    m_noteEditorAct = m_windowMenu->addAction(tr("Minutes Editor"));
    m_noteEditorAct->setCheckable(true);
    m_noteEditorAct->setChecked(false);
    // Connexion de l'action avec l'affichage/masquage de l'editeur de notes
    connect(m_noteEditorAct, SIGNAL(triggered(bool)), this, SLOT(displayMinutesEditor(bool)));

    // Ajout du sous-menu Tchat
    m_windowMenu->addMenu(m_chatListWidget->chatMenu());

    // Creation du menu Aide
    QMenu *menuAide = new QMenu (tr("Help"), m_menuBar);
    m_helpAct = menuAide->addAction(tr("Help about %1").arg(m_preferences->value("Application_Name","rolisteam").toString()));
    m_helpAct->setShortcut(QKeySequence::HelpContents);
    menuAide->addSeparator();
    m_aboutAct = menuAide->addAction(tr("About %1").arg(m_preferences->value("Application_Name","rolisteam").toString()));

    // Ajout des menus a la barre de menus
    m_menuBar->addMenu(menuFichier);
    m_menuBar->addMenu(menuAffichage);
    m_menuBar->addMenu(m_windowMenu);
    m_menuBar->addMenu(networkMenu);
    m_menuBar->addMenu(menuAide);
}

void MainWindow::linkActionToMenu()
{
    // file menu
    connect(m_newMapAct, SIGNAL(triggered(bool)), this, SLOT(newMap()));
    connect(m_openImageAct, SIGNAL(triggered(bool)), this, SLOT(ouvrirImage()));
    connect(m_openMapAct, SIGNAL(triggered(bool)), this, SLOT(openMapWizzard()));

    connect(m_openStoryAct, SIGNAL(triggered(bool)), this, SLOT(ouvrirScenario()));
    connect(m_openMinutesAct, SIGNAL(triggered(bool)), this, SLOT(openNote()));
    connect(m_closeMap, SIGNAL(triggered(bool)), this, SLOT(closeMapOrImage()));
    connect(m_saveMapAct, SIGNAL(triggered(bool)), this, SLOT(saveMap()));
    connect(m_saveStoryAct, SIGNAL(triggered(bool)), this, SLOT(sauvegarderScenario()));
    connect(m_saveMinuteAct, SIGNAL(triggered(bool)), this, SLOT(saveMinutes()));
    connect(m_preferencesAct, SIGNAL(triggered(bool)), m_preferencesDialog, SLOT(show()));

    // close
    connect(m_quitAct, SIGNAL(triggered(bool)), this, SLOT(quitterApplication()));

    // network
    connect(m_networkManager,SIGNAL(stopConnectionTry()),this,SLOT(stopReconnection()));
    connect(m_disconnectAct,SIGNAL(triggered()),this,SLOT(closeConnection()));
    connect(m_reconnectAct,SIGNAL(triggered()),this,SLOT(startReconnection()));

    // Windows managing
    connect(m_cascadeAction, SIGNAL(triggered(bool)), m_mdiArea, SLOT(cascadeSubWindows()));

    connect(m_tabOrdering,SIGNAL(triggered(bool)),m_mdiArea,SLOT(setTabbedMode(bool)));

    connect(m_tabOrdering,SIGNAL(triggered(bool)),m_cascadeAction,SLOT(setDisabled(bool)));
    connect(m_tabOrdering,SIGNAL(triggered(bool)),m_tuleAction,SLOT(setDisabled(bool)));
    connect(m_tuleAction, SIGNAL(triggered(bool)), m_mdiArea, SLOT(tileSubWindows()));

    // Display
    connect(m_showPCAct, SIGNAL(triggered(bool)), this, SLOT(afficherNomsPj(bool)));
    connect(m_showNpcNameAct, SIGNAL(triggered(bool)), this, SLOT(afficherNomsPnj(bool)));
    connect(m_showNPCNumberAct, SIGNAL(triggered(bool)), this, SLOT(afficherNumerosPnj(bool)));

    // Help
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(aPropos()));
    connect(m_helpAct, SIGNAL(triggered()), this, SLOT(helpOnLine()));
}

QWidget* MainWindow::ajouterCarte(CarteFenetre *carteFenetre, QString titre,QSize mapsize,QPoint pos )
{
    QAction *action = m_windowMenu->addAction(titre);
    action->setCheckable(true);
    action->setChecked(true);

    m_mapWindowList.append(carteFenetre);

    // Ajout de la carte au workspace
    QWidget* tmp = m_mdiArea->addWindow(carteFenetre,action);
    if(mapsize.isValid())
        tmp->resize(mapsize);
    if(!pos.isNull())
        tmp->move(pos);

    if(titre.isEmpty())
    {
        titre = tr("Unknown Map");
    }

    //tmp->setParent(this);
    tmp->setWindowIcon(QIcon(":/map.png"));
    tmp->setWindowTitle(titre);

    carteFenetre->setWindowIcon(QIcon(":/map.png"));
    carteFenetre->setWindowTitle(titre);

    m_mapAction->insert(carteFenetre,action);

    // Connexion de l'action avec l'affichage/masquage de la fenetre
    connect(action, SIGNAL(triggered(bool)), tmp, SLOT(setVisible(bool)));
    connect(action, SIGNAL(triggered(bool)), carteFenetre, SLOT(setVisible(bool)));
    connect(carteFenetre,SIGNAL(visibleChanged(bool)),action,SLOT(setChecked(bool)));

    Carte *carte = carteFenetre->carte();
    carte->setPointeur(m_toolBar->getCurrentTool());

    connect(m_toolBar,SIGNAL(currentToolChanged(BarreOutils::Tool)),carte,SLOT(setPointeur(BarreOutils::Tool)));
    connect(carte, SIGNAL(changeCurrentColor(QColor)), m_toolBar, SLOT(changeCouleurActuelle(QColor)));
    connect(carte, SIGNAL(incrementeNumeroPnj()), m_toolBar, SLOT(incrementeNumeroPnj()));
    connect(carte, SIGNAL(mettreAJourPnj(int, QString)), m_toolBar, SLOT(mettreAJourPnj(int, QString)));

    connect(m_showPCAct, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNomsPj(bool)));
    connect(m_showNpcNameAct, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNomsPnj(bool)));
    connect(m_showNPCNumberAct, SIGNAL(triggered(bool)), carte, SIGNAL(afficherNumerosPnj(bool)));

    // new PlayersList connection
    connect(carteFenetre, SIGNAL(activated(Carte *)), m_playersListWidget->model(), SLOT(changeMap(Carte *)));
    connect(carteFenetre, SIGNAL(activated(Carte *)), m_toolBar, SLOT(changeMap(Carte *)));

    // Affichage de la carte
    tmp->setVisible(true);
    return tmp;

}
void  MainWindow::closeConnection()
{
    if(NULL!=m_networkManager)
    {
        m_networkManager->disconnectAndClose();
        m_reconnectAct->setEnabled(true);
        m_disconnectAct->setEnabled(false);
    }
}

void MainWindow::ajouterImage(Image* pictureWindow, QString title)
{
    pictureWindow->setParent(m_mdiArea);
    addImageToMdiArea(pictureWindow,title);
}
void MainWindow::ouvrirImage()
{

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

    Qt::WindowFlags flags = msgBox.windowFlags();
    msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

    // On met a jour le chemin vers les images
    int dernierSlash = fichier.lastIndexOf("/");
    m_preferences->registerValue("ImageDirectory",fichier.left(dernierSlash));


    QImage *img = new QImage(fichier);
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



    // Creation de l'identifiant
    QString idImage = QUuid::createUuid().toString();

    // Creation de la fenetre image
    Image *imageFenetre = new Image(this,idImage, G_idJoueurLocal, img, NULL,m_mdiArea);

    addImageToMdiArea(imageFenetre,titre);

    // Envoie de l'image aux autres utilisateurs
    NetworkMessageWriter message = NetworkMessageWriter(NetMsg::PictureCategory, NetMsg::AddPictureAction);
    imageFenetre->fill(message);
    message.sendAll();
}

void MainWindow::addImageToMdiArea(Image *imageFenetre, QString titre)
{
    //listeImage.append(imageFenetre);

    imageFenetre->setImageTitle(titre);

    // Creation de l'action correspondante
    QAction *action = m_windowMenu->addAction(titre);
    action->setCheckable(true);
    action->setChecked(true);
    imageFenetre->setInternalAction(action);

    // Ajout de l'image au workspace
    QMdiSubWindow* sub = static_cast<QMdiSubWindow*>(m_mdiArea->addWindow(imageFenetre,action));

    m_pictureList.append(sub);

    // Mise a jour du titre de l'image
    sub->setWindowTitle(titre);
    sub->setWindowIcon(QIcon(":/picture.png"));


    connect(m_toolBar,SIGNAL(currentToolChanged(BarreOutils::Tool)),imageFenetre,SLOT(setCurrentTool(BarreOutils::Tool)));

    imageFenetre->setCurrentTool(m_toolBar->getCurrentTool());
    connect(action, SIGNAL(triggered(bool)), sub, SLOT(setVisible(bool)));
    connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));
    sub->show();
}

void MainWindow::openMapWizzard()
{


    m_mapWizzard->resetData();
    if(m_mapWizzard->exec()==QMessageBox::Accepted)
    {
        QFileInfo info(m_mapWizzard->getFilepath());

        m_preferences->registerValue("MapDirectory",info.absolutePath());
        openMap(m_mapWizzard->getPermissionMode(),m_mapWizzard->getFilepath(),m_mapWizzard->getTitle(),m_mapWizzard->getHidden());
    }
}

void MainWindow::openMap(Carte::PermissionMode Permission,QString filepath,QString title, bool masquer)
{
    QMessageBox msgBox(this);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle(tr("Loading error"));
    msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));

    Qt::WindowFlags flags = msgBox.windowFlags();
    msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);


    if (filepath.endsWith(".pla"))
    {
        QFile file(filepath);
        if (!file.open(QIODevice::ReadOnly))
        {
            msgBox.setText(tr("Unsupported file format"));
            msgBox.exec();
            return;
        }
        QDataStream in(&file);
        readMapAndNpc(in, masquer, title);
        file.close();
    }
    else
    {
        // Chargement de l'image
        QImage image(filepath);
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
        carte->setPermissionMode(Permission);
        carte->setPointeur(m_toolBar->getCurrentTool());
        // Creation de la CarteFenetre
        CarteFenetre *carteFenetre = new CarteFenetre(carte, m_mdiArea);
        // Ajout de la carte au workspace
        ajouterCarte(carteFenetre, title);

        // Envoie de la carte aux autres utilisateurs
        // On commence par compresser l'image (format jpeg) dans un tableau
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        bool ok = image.save(&buffer, "jpeg", 60);
        if (!ok)
        {
            qWarning() << tr("Compressing image goes wrong (ouvrirPlan - MainWindow.cpp)");
        }

        // Taille des donnees
        quint32 tailleCorps =
                // Taille du titre
                sizeof(quint16) + title.size()*sizeof(QChar) +
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
        quint16 tailleTitre = title.size();
        memcpy(&(donnees[p]), &tailleTitre, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), title.data(), tailleTitre*sizeof(QChar));
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



        quint8 permission = carte->getPermissionMode();
        memcpy(&(donnees[p]), &permission, sizeof(quint8));
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

QMdiSubWindow* MainWindow::readMapAndNpc(QDataStream &in, bool masquer, QString nomFichier)
{
    /*out << titre;
    out << pos();
    out << (quint32)m_currentMode;
    out << m_alphaLayer->size();
    out << taillePj;
    out << baFondOriginal;
    out << baFond;
    out << baAlpha;
*/


    QString titre;
    in >> titre;

    QPoint pos;
    in >>pos;

    Carte::PermissionMode myPermission;
    quint32 permiInt;
    in >> permiInt;
    myPermission = (Carte::PermissionMode) permiInt;

    QSize size;
    in >> size;

    int taillePj;
    in >> taillePj;

    QByteArray baFondOriginal;
    in >> baFondOriginal;

    QByteArray baFond;
    in >> baFond;

    QByteArray baAlpha;
    in>> baAlpha;

    qDebug()<< "read map" <<baAlpha.size()<< baFond.size() << baFondOriginal.size() ;

    bool ok;


    //////////////////
    // Manage Data to create the map.
    //////////////////

    QImage fondOriginal;
    ok = fondOriginal.loadFromData(baFondOriginal, "jpeg");
    if (!ok)
    {
        qWarning("Probleme de decompression du fond original (readMapAndNpc - Mainwindow.cpp)");
    }

    QImage fond;
    ok = fond.loadFromData(baFond, "jpeg");
    if (!ok)
    {
        qWarning("Probleme de decompression du fond (readMapAndNpc - Mainwindow.cpp)");
    }

    QImage alpha;
    ok = alpha.loadFromData(baAlpha, "jpeg");
    if (!ok)
    {
        qWarning("Probleme de decompression de la couche alpha (readMapAndNpc - Mainwindow.cpp)");
    }

    if (masquer)
    {
        QPainter painterAlpha(&alpha);
        QColor color = PreferencesManager::getInstance()->value("Fog_color",QColor(0,0,0)).value<QColor>();
        qDebug()<< "masquer"<< color;
        qDebug() << alpha.size() << size;
        painterAlpha.fillRect(0, 0, alpha.width(), alpha.height(), color);
    }

    QString idCarte = QUuid::createUuid().toString();

    Carte* map = new Carte(idCarte, &fondOriginal, &fond, &alpha);
    map->setPermissionMode(myPermission);
    map->setPointeur(m_toolBar->getCurrentTool());
    CarteFenetre *carteFenetre = new CarteFenetre(map,m_mdiArea);





    QPoint pos2 = carteFenetre->mapFromParent(pos);

    QMdiSubWindow* m_widget=NULL;
    if (nomFichier.isEmpty())
        m_widget=static_cast<QMdiSubWindow*>(ajouterCarte(carteFenetre, titre,size,pos));
    else
        m_widget=static_cast<QMdiSubWindow*>(ajouterCarte(carteFenetre, nomFichier,size,pos));

    // On recupere le nombre de personnages presents dans le message
    quint16 nbrPersonnages;
    in >>  nbrPersonnages;

    for (int i=0; i<nbrPersonnages; ++i)
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

        DessinPerso *pnj = new DessinPerso(map, ident, nomPerso, couleur, diametre, centre, type, numeroDuPnj);

        if (visible || (type == DessinPerso::pnj && PlayersList::instance()->localPlayer()->isGM()))
            pnj->afficherPerso();
        // On m.a.j l'orientation
        pnj->nouvelleOrientation(orientation);
        // Affichage de l'orientation si besoin
        if (orientationAffichee)
            pnj->afficheOuMasqueOrientation();

        pnj->nouvelEtatDeSante(sante, numeroEtat);

        map->afficheOuMasquePnj(pnj);

    }
    map->emettreCarte(carteFenetre->windowTitle());
    map->emettreTousLesPersonnages();

    return m_widget;
}





void MainWindow::closeMapOrImage()
{

    QMdiSubWindow* subactive = m_mdiArea->currentSubWindow();
    QWidget* active = subactive->widget();
    CarteFenetre* carteFenetre = NULL;

    if (NULL!=active)
    {

        Image*  imageFenetre = dynamic_cast<Image*>(active);

        QString mapImageId;
        QString mapImageTitle;
        QAction* associatedAct = NULL;
        mapImageTitle = active->windowTitle();
        bool image=false;
        if(NULL!=imageFenetre)
        {
            associatedAct = imageFenetre->getAssociatedAction();
            mapImageId = imageFenetre->getImageId();
            image = true;
        }
        else
        {
            carteFenetre= dynamic_cast<CarteFenetre*>(active);
            if(NULL!=carteFenetre)
            {
                mapImageId = carteFenetre->getMapId();
                associatedAct = m_mapAction->value(carteFenetre);

            }
            else
            {
                return;
            }
        }

        QMessageBox msgBox(this);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel );
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.move(QPoint(width()/2, height()/2) + QPoint(-100, -50));
        Qt::WindowFlags flags = msgBox.windowFlags();
        msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);


        if (!image)
        {
            msgBox.setWindowTitle(tr("Close Map"));
        }
        else
        {
            msgBox.setWindowTitle(tr("Close Picture"));
        }
        msgBox.setText(tr("Do you want to close %1 %2?\nIt will be closed for everybody").arg(mapImageTitle).arg(image?tr(""):tr("(Map)")));



        msgBox.exec();
        if (msgBox.result() != QMessageBox::Yes)
            return;

        if (!image)
        {
            // Taille des donnees
            quint32 tailleCorps =
                    // Taille de l'identifiant de la carte
                    sizeof(quint8) + mapImageId.size()*sizeof(QChar);

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
            quint8 tailleIdCarte = mapImageId.size();
            memcpy(&(donnees[p]), &tailleIdCarte, sizeof(quint8));
            p+=sizeof(quint8);
            memcpy(&(donnees[p]), mapImageId.data(), tailleIdCarte*sizeof(QChar));
            p+=tailleIdCarte*sizeof(QChar);

            // Emission de la demande de fermeture de la carte au serveur ou a l'ensemble des clients
            emettre(donnees, tailleCorps + sizeof(enteteMessage));
            // Liberation du buffer d'emission
            delete[] donnees;


            m_mapWindowList.removeAll(carteFenetre);
            //((CarteFenetre *)active)->~CarteFenetre();

        }
        else
        {
            NetworkMessageWriter msg(NetMsg::PictureCategory,NetMsg::DelPictureAction);
            msg.string8(mapImageId);
            msg.sendAll();
        }
        if(NULL!=associatedAct)
        {
            delete associatedAct;
        }
        delete active;
        delete subactive;
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

    QMdiSubWindow* active = m_mdiArea->currentSubWindow();


    if (NULL!=active)
    {
        changementFenetreActive(active);
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
void MainWindow::changementFenetreActive(QMdiSubWindow *subWindow)
{
    QWidget* widget=NULL;
    if(NULL!=subWindow)
    {
        widget = subWindow->widget();
    }

    bool localPlayerIsGM = PlayersList::instance()->localPlayer()->isGM();
    if (widget != NULL && widget->objectName() == QString("CarteFenetre") && localPlayerIsGM)
    {
        m_closeMap->setEnabled(true);
        m_saveMapAct->setEnabled(true);
    }
    else if (widget != NULL && widget->objectName() == QString("Image") &&
             (localPlayerIsGM || static_cast<Image *>(widget)->proprietaireImage()))
    {
        m_closeMap->setEnabled(true);
        m_saveMapAct->setEnabled(false);
    }
    else
    {
        m_closeMap->setEnabled(false);
        m_saveMapAct->setEnabled(false);
    }
}
void MainWindow::majCouleursPersonnelles()
{
    m_toolBar->majCouleursPersonnelles();
}

void MainWindow::newMap()
{
    // fenetreNouveauPlan = new NouveauPlanVide(this);


    m_newEmptyMapDialog->resetData();
    if(m_newEmptyMapDialog->exec()==QMessageBox::Accepted)
    {


        QString idMap = QUuid::createUuid().toString();
        buildNewMap(m_newEmptyMapDialog->getTitle(),idMap,m_newEmptyMapDialog->getColor(),m_newEmptyMapDialog->getSize(),m_newEmptyMapDialog->getPermission());


        QString titre = m_newEmptyMapDialog->getTitle();
        // On recupere la couleur du fond
        QColor couleur = m_newEmptyMapDialog->getColor();
        quint16 larg = m_newEmptyMapDialog->getSize().width();
        quint16 haut = m_newEmptyMapDialog->getSize().height();

        // Emission de la demande de creation d'un plan vide
        // Taille des donnees
        quint32 tailleCorps =
                // Taille du nom
                sizeof(quint16) + titre.size()*sizeof(QChar) +
                // Taille de l'identifiant
                sizeof(quint8) + idMap.size()*sizeof(QChar) +
                // Taille de la couleur
                sizeof(QRgb) +
                // Taille des dimensions de la carte
                sizeof(quint16) + sizeof(quint16) +
                // Taille des PJ
                sizeof(quint8) + sizeof(Carte::PermissionMode);

        // Buffer d'emission
        char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

        // Creation de l'entete du message
        enteteMessage *uneEntete;
        uneEntete = (enteteMessage *) donnees;
        uneEntete->categorie = plan;
        uneEntete->action = nouveauPlanVide;
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
        quint8 tailleId = idMap.size();
        memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
        p+=sizeof(quint8);
        memcpy(&(donnees[p]), idMap.data(), tailleId*sizeof(QChar));
        p+=tailleId*sizeof(QChar);
        // Ajout de la couleur
        QRgb rgb = couleur.rgb();
        memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
        p+=sizeof(QRgb);
        // Ajout de la largeur et de la hauteur de la carte
        memcpy(&(donnees[p]), &larg, sizeof(quint16));
        p+=sizeof(quint16);
        memcpy(&(donnees[p]), &haut, sizeof(quint16));
        p+=sizeof(quint16);
        // Ajout de la taille des PJ
        quint8 taillePj = 1;
        memcpy(&(donnees[p]), &taillePj, sizeof(quint8));
        p+=sizeof(quint8);

        quint8 mode = (quint8)m_newEmptyMapDialog->getPermission();
        memcpy(&(donnees[p]), &mode, sizeof(quint8));
        p+=sizeof(quint8);

        // On emet vers les clients ou le serveur
        emettre(donnees, tailleCorps + sizeof(enteteMessage));
        // Liberation du buffer d'emission
        delete[] donnees;
    }

}
void MainWindow::buildNewMap(QString titre, QString idCarte, QColor couleurFond, QSize size,Carte::PermissionMode mode)
{
    QImage image(size, QImage::Format_ARGB32_Premultiplied);
    image.fill(couleurFond.rgb());
    Carte *carte = new Carte(idCarte, &image);
    carte->setPermissionMode(getPermission(mode));
    carte->setPointeur(m_toolBar->getCurrentTool());
    CarteFenetre *carteFenetre = new CarteFenetre(carte, m_mdiArea);
    ajouterCarte(carteFenetre, titre);
}





void MainWindow::emettreTousLesPlans(Liaison * link)
{
    qDebug() << "emettreTousLesPlans " << link;
    int tailleListe = m_mapWindowList.size();
    for (int i=0; i<tailleListe; ++i)
    {
        m_mapWindowList[i]->carte()->setHasPermissionMode(m_playerList->everyPlayerHasFeature("MapPermission"));
        m_mapWindowList[i]->carte()->emettreCarte(m_mapWindowList[i]->windowTitle(), link);
        m_mapWindowList[i]->carte()->emettreTousLesPersonnages(link);
    }
}


void MainWindow::emettreToutesLesImages(Liaison * link)
{
    NetworkMessageWriter message = NetworkMessageWriter(NetMsg::PictureCategory, NetMsg::AddPictureAction);
    foreach(QMdiSubWindow* sub, m_pictureList)
    {
        Image* img = static_cast<Image*>(sub->widget());
        if(NULL!=img)
        {
            img->fill(message);
            message.sendTo(link);
        }

    }
}



void MainWindow::removeMapFromId(QString idCarte)
{
    QMdiSubWindow* tmp = m_mdiArea->getSubWindowFromId(idCarte);
    CarteFenetre* mapWindow = dynamic_cast<CarteFenetre*>(tmp->widget());
    if(NULL!=mapWindow)
    {
        delete m_mapAction->value(mapWindow);
        m_mapWindowList.removeAll(mapWindow);
        delete mapWindow;
        delete tmp;
    }
}
Carte * MainWindow::trouverCarte(QString idCarte)
{
    // Taille de la liste des CarteFenetre
    int tailleListe = m_mapWindowList.size();

    bool ok = false;
    int i;
    for (i=0; i<tailleListe && !ok; ++i)
        if ( m_mapWindowList[i]->carte()->identifiantCarte() == idCarte )
            ok = true;

    // Si la carte vient d'etre trouvee on renvoie son pointeur
    if (ok)
        return m_mapWindowList[i-1]->carte();
    // Sinon on renvoie -1
    else
        return 0;
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
    if (!PlayersList::instance()->localPlayer()->isGM())
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
        m_noteEditor->close();

        /// @todo : make sure custom colors are saved.
        // On quitte l'application
        qApp->quit();
    }

    // Si l'utilisateur a clique sur "Sauvegarder", on applique l'action appropriee a la nature de l'utilisateur
    else if (msgBox.clickedButton() == boutonSauvegarder)
    {
        bool ok;
        // Si l'utilisateur est un joueur, on sauvegarde les notes
        if (!PlayersList::instance()->localPlayer()->isGM())
            ok = saveMinutes();
        // S'il est MJ, on sauvegarde le scenario
        else
            ok = sauvegarderScenario();

        // Puis on quitte l'application si l'utilisateur a sauvegarde ou s'il s'agit d'une perte de connexion
        if (ok || perteConnexion)
        {
            emit closing();
            writeSettings();
            m_noteEditor->close();
            // On sauvegarde le fichier d'initialisation
            /// @todo : make sure custom colors are saved.
            // On quitte l'application
            qApp->quit();
        }
    }
}



void MainWindow::removePictureFromId(QString idImage)
{
    QMdiSubWindow* tmp = m_mdiArea->getSubWindowFromId(idImage);
    if(NULL!=tmp)
    {
        Image* imageWindow = dynamic_cast<Image*>(tmp->widget());

        if(NULL!=imageWindow)
        {

            delete imageWindow->getAssociatedAction();
            delete imageWindow;

        }
        delete tmp;
    }
}
bool MainWindow::enleverCarteDeLaListe(QString idCarte)
{
    // Taille de la liste des CarteFenetre
    int tailleListe = m_mapWindowList.size();

    bool ok = false;
    int i;
    for (i=0; i<tailleListe && !ok; ++i)
    {
        if(NULL != m_mapWindowList[i])
        {
            if(NULL!=m_mapWindowList[i]->carte())
            {
                if ( m_mapWindowList[i]->carte()->identifiantCarte() == idCarte )
                {
                    ok = true;
                }
            }

        }
    }


    // Si la carte vient d'etre trouvee on supprime l'element
    if (ok)
    {
        m_mapWindowList.removeAt(i-1);
        return true;
    }
    // Sinon on renvoie false
    else
        return false;
}

QWidget* MainWindow::registerSubWindow(QWidget * subWindow,QAction* action)
{
    return m_mdiArea->addWindow(subWindow,action);
}

void MainWindow::saveMap()
{

    QWidget *active = m_mdiArea->activeWindow();
    QMdiSubWindow* subWindow = static_cast<QMdiSubWindow*>(active);
    CarteFenetre* mapWindow = static_cast<CarteFenetre*>(subWindow->widget());




    if ((NULL==mapWindow) || (mapWindow->objectName() != "CarteFenetre"))
    {
        qWarning("Not a map (sauvegarderPlan - MainWindow.h)");
        return;
    }


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Map"), m_preferences->value("MapDirectory",QDir::homePath()).toString(), tr("Map (*.pla)"));


    if (fileName.isNull())
        return;

    if (!fileName.endsWith(".pla"))
    {
        fileName += ".pla";
    }



    int dernierSlash = fileName.lastIndexOf("/");
    m_preferences->registerValue("MapDirectory",fileName.left(dernierSlash));


    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("could not open file for writting (sauvegarderPlan - MainWindow.cpp)");
        return;
    }
    QDataStream out(&file);
    mapWindow->carte()->saveMap(out);
    file.close();
}


void MainWindow::changementNatureUtilisateur()
{

    m_toolBar->updateUi();
    updateUi();
#ifndef NULL_PLAYER
    removeDockWidget(m_audioPlayer);
    m_audioPlayer->show();
#endif
}


void MainWindow::displayMinutesEditor(bool visible, bool isCheck)
{

    //m_noteEditor->setVisible(visible);
    if(NULL!=m_noteEditorSub)
    {
        m_noteEditorSub->setVisible(visible);
        m_noteEditor->setVisible(visible);
    }
    if (isCheck)
    {
        m_noteEditorAct->setChecked(visible);
    }

}
void MainWindow::openNote()
{
    // open file name.
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Minutes"), m_preferences->value("MinutesDirectory",QDir::homePath()).toString(), m_noteEditor->getFilter());

    if (!filename.isEmpty())  {
        QFileInfo fi(filename);
        m_preferences->registerValue("MinutesDirectory",fi.absolutePath() +"/");
        m_noteEditor->load(filename);
        displayMinutesEditor(true, true);
    }

}
bool MainWindow::saveMinutes()
{    
    return m_noteEditor->fileSave();
}
void MainWindow::ouvrirScenario()
{
    QString fichier = QFileDialog::getOpenFileName(this, tr("Open scenario"), m_preferences->value("StoryDirectory",QDir::homePath()).toString(), tr("Scenarios (*.sce)"));

    if (fichier.isNull())
        return;

    int dernierSlash = fichier.lastIndexOf("/");
    m_preferences->registerValue("StoryDirectory",fichier.left(dernierSlash));


    QFile file(fichier);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Cannot be read (ouvrirScenario - MainWindow.cpp)");
        return;
    }
    QDataStream in(&file);


    int nbrCartes;
    in >> nbrCartes;


    for (int i=0; i<nbrCartes; ++i)
    {
        QPoint posWindow;
        in >> posWindow;

        QSize sizeWindow;
        in >> sizeWindow;

        QMdiSubWindow* mapWindow = readMapAndNpc(in);
        if(NULL!=mapWindow)
        {
            mapWindow->setGeometry(posWindow.x(),posWindow.y(),sizeWindow.width(),sizeWindow.height());
        }
    }


    int nbrImages;
    in >>nbrImages;
    // in >>On lit toutes les images presentes dans le fichier
    for (int i=0; i<nbrImages; ++i)
        lireImage(in);


    //reading minutes
    m_noteEditor->readFromBinary(in);

    bool tempbool;
    in >> tempbool;
    m_noteEditor->setVisible(tempbool);
    m_noteEditorAct->setChecked(tempbool);

    // closing file
    file.close();
}
bool MainWindow::sauvegarderScenario()
{
    // open file
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
    saveAllMap(out);
    saveAllImages(out);

    m_noteEditor->saveFileAsBinary(out);
    out << m_noteEditor->isVisible();
    // closing file
    file.close();

    return true;
}
void MainWindow::saveAllMap(QDataStream &out)
{
    out << m_mapWindowList.size();
    for (int i=0; i<m_mapWindowList.size(); ++i)
    {
        QPoint pos2 = m_mapWindowList[i]->mapFromParent(m_mapWindowList[i]->pos());
        out << pos2;
        out << m_mapWindowList[i]->size();
        m_mapWindowList[i]->carte()->saveMap(out, m_mapWindowList[i]->windowTitle());
    }
}

void MainWindow::saveAllImages(QDataStream &out)
{
    out << m_pictureList.size();
    foreach(QMdiSubWindow* sub, m_pictureList)
    {
        Image* img = static_cast<Image*>(sub->widget());
        if(NULL!=img)
        {
            img->sauvegarderImage(out,sub->windowTitle());
        }

    }
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



    file >> baImage;

    bool ok;
    // Creation de l'image
    QImage img;
    ok = img.loadFromData(baImage, "jpg");
    if (!ok)
        qWarning("Probleme de decompression de l'image (lireImage - Mainwindow.cpp)");



    // Creation de l'identifiant
    QString idImage = QUuid::createUuid().toString();

    // Creation de la fenetre image
    Image *imageFenetre = new Image(this,idImage, G_idJoueurLocal, &img, NULL,m_mdiArea);


    addImageToMdiArea(imageFenetre,titre);



    /* tmp->move(topleft);
    tmp->resize(size);*/



    // Connexion de l'action avec l'affichage/masquage de la fenetre
    //connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));
    connect(m_toolBar,SIGNAL(currentToolChanged(BarreOutils::Tool)),imageFenetre,SLOT(setCurrentTool(BarreOutils::Tool)));
    imageFenetre->setCurrentTool(m_toolBar->getCurrentTool());

    // Affichage de l'image
    //imageFenetre->show();



    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    ok = img.save(&buffer, "jpg", 70);
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



void MainWindow::aPropos()
{
    QMessageBox::about(this, tr("About Rolisteam"),
                       tr("<h1>Rolisteam v%1</h1>"
                          "<p>Rolisteam makes easy the management of any role playing games. It allows players to communicate to each others and to share maps and pictures."
                          "Rolisteam also provides many features for : permission management, background music and dice roll. Rolisteam is written in Qt4."
                          "Its dependencies are : Qt4 and Phonon.</p>").arg(m_version) %
                       tr("<p>You may modify and redistribute the program under the terms of the GPL (version 2 or later)."
                          "A copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam."
                          "Rolisteam is copyrighted by its contributors.  See the 'COPYRIGHT' file for the complete list of contributors.  We provide no warranty for this program.</p>") %
                       tr("<p><h3>Web Sites :</h3>"
                          "<ul>"
                          "<li><a href=\"http://www.rolisteam.org/\">Official Rolisteam Site</a></li> "
                          "<li><a href=\"http://code.google.com/p/rolisteam/issues/list\">Bug Tracker</a></li> "
                          "</ul></p>"
                          "<p><h3>Current developers :</h3>"
                          "<ul>"
                          "<li><a href=\"http://www.rolisteam.org/contact\">Renaud Guezennec</a></li>"
                          "<li><a href=\"mailto:joseph.boudou@matabio.net\">Joseph Boudou<a/></li>"
                          "</ul></p> "
                          "<p><h3>Retired developers :</h3>"
                          "<ul>"
                          "<li><a href=\"mailto:rolistik@free.fr\">Romain Campioni<a/> (rolistik)  </li>"
                          "</ul></p>"));
}


void MainWindow::helpOnLine()
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


void MainWindow::updateUi()
{
    /// @todo hide diametrePNj for players.
    m_toolBar->updateUi();
#ifndef NULL_PLAYER
    m_audioPlayer->updateUi();
#endif
    if(!PlayersList::instance()->localPlayer()->isGM())
    {
        m_newMapAct->setEnabled(false);
        m_openMapAct->setEnabled(false);

        m_openStoryAct->setEnabled(false);
        m_closeMap->setEnabled(false);
        m_saveMapAct->setEnabled(false);
        m_saveStoryAct->setEnabled(false);

    }

    if(m_networkManager->isServer())
    {
        m_reconnectAct->setEnabled(false);
        m_disconnectAct->setEnabled(true);
    }
    else
    {
        m_reconnectAct->setEnabled(false);
        m_disconnectAct->setEnabled(true);
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
    QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(m_version));

    move(settings.value("pos", QPoint(200, 200)).toPoint());
    resize(settings.value("size", QSize(600, 400)).toSize());
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    m_preferences->readSettings(settings);
}




Carte::PermissionMode MainWindow::getPermission(int id)
{
    switch(id)
    {
    case 0:
        return Carte::GM_ONLY;
    case 1:
        return Carte::PC_MOVE;
    case 2:
        return Carte::PC_ALL;
    default:
        return Carte::GM_ONLY;
    }

}
void MainWindow::networkStateChanged(bool state)
{
    if(state)
    {
        m_reconnectAct->setEnabled(false);
        m_disconnectAct->setEnabled(true);

        if((m_networkManager->isConnected())&&(m_networkManager->isServer())&&(NULL!=m_ipChecker))
        {
            m_ipChecker->startCheck();
        }
    }
    else
    {
        m_reconnectAct->setEnabled(true);
        m_disconnectAct->setEnabled(false);
    }
}

void MainWindow::notifyAboutAddedPlayer(Player * player) const
{
    notifyUser(tr("%1 just joins the game.").arg(player->name()));
}

void MainWindow::notifyAboutDeletedPlayer(Player * player) const
{
    notifyUser(tr("%1 just leaves the game.").arg(player->name()));
}
void MainWindow::stopReconnection()
{
    m_reconnectAct->setEnabled(true);
    m_disconnectAct->setEnabled(false);
}
void MainWindow::startReconnection()
{
    if(m_networkManager->startConnection())
    {
        m_reconnectAct->setEnabled(false);
        m_disconnectAct->setEnabled(true);
    }
    else
    {
        m_reconnectAct->setEnabled(true);
        m_disconnectAct->setEnabled(false);
    }
}
void MainWindow::showIp(QString ip)
{
    notifyUser(tr("Server Ip Address:%1\nPort:%2").arg(ip).arg(m_networkManager->getPort()));
}
void MainWindow::setUpNetworkConnection()
{
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {

        qDebug() << "user is client";
        // We want to know if the server refuses local player to be GM
        connect(m_playerList, SIGNAL(localGMRefused()), this, SLOT(changementNatureUtilisateur()));
        // We send a message to del local player when we quit
        connect(this, SIGNAL(closing()), m_playerList, SLOT(sendDelLocalPlayer()));
    }
    else
    {
        qDebug() << "user is server";
        // send datas on new connection if we are the server
        // send datas on new connection if we are the server
//        connect(m_networkManager, SIGNAL(linkAdded(Liaison *)), this, SLOT(emettreTousLesPlans(Liaison *)));
//        connect(m_networkManager, SIGNAL(linkAdded(Liaison *)), this, SLOT(emettreToutesLesImages(Liaison *)));
          connect(m_networkManager, SIGNAL(linkAdded(Liaison *)), this, SLOT(updateSessionToNewClient(Liaison*)));
    }
    connect(m_networkManager, SIGNAL(dataReceived(quint64,quint64)), this, SLOT(receiveData(quint64,quint64)));

}
void MainWindow::updateSessionToNewClient(Liaison* link)
{
   // m_playersListWidget->throwUserInfoToNewClient(link);
    emettreTousLesPlans(link);
    emettreToutesLesImages(link);
}

void MainWindow::setNetworkManager(ClientServeur* tmp)
{
    m_networkManager = tmp;
    m_networkManager->setParent(this);
}
void MainWindow::writeSettings()
{
    QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(m_version));
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    m_preferences->writeSettings(settings);
}
