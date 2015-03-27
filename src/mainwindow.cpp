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
#include <QStatusBar>
#include <QCommandLineParser>


#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "toolsbar.h"
#include "map/bipmapwindow.h"
#include "map/map.h"
#include "chat/chatlistwidget.h"
#include "network/networkmanager.h"
#include "Image.h"
#include "network/networkmessagewriter.h"

#include "data/persons.h"
#include "userlist/playersList.h"
#include "userlist/playersListWidget.h"
#include "preferences/preferencesdialog.h"
#include "services/updatechecker.h"
#include "improvedworkspace.h"


#include "textedit.h"
#include "variablesGlobales.h"

#ifndef NULL_PLAYER
#include "audioPlayer.h"
#endif

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
    bool result = m_networkManager->configAndConnect();
    m_audioPlayer->updateUi();
    return result;

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
    : QMainWindow(),m_networkManager(NULL),m_localPlayerId(QUuid::createUuid().toString()),m_ui(new Ui::MainWindow)
{


	m_supportedImage=tr("Supported Image formats (*.jpg *.jpeg *.png *.bmp *.svg)");
	m_supportedCharacterSheet=tr("Character Sheets files (*.xml)");
	m_pdfFiles=tr("Pdf File (*.pdf)");
	m_supportedNotes=tr("Supported Text files (*.html *.txt)");
	m_supportedMap=tr("Supported Map files (*.pla )");


    m_ui->setupUi(this);
    m_shownProgress=false;
    m_preferences = PreferencesManager::getInstance();
    m_downLoadProgressbar = new QProgressBar();
	m_downLoadProgressbar->setRange(0,100);

    m_downLoadProgressbar->setVisible(false);

	//m_mapWizzard = new MapWizzard(this);
    m_networkManager = new NetworkManager(m_localPlayerId);
    m_ipChecker = new IpChecker(this);
	//G_NetworkManager = m_networkManager;
    m_mapAction = new QMap<MapFrame*,QAction*>();


}
void MainWindow::setupUi()
{
	// Initialisation de la liste des BipMapWindow, des Image et des Tchat
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

	m_mdiArea = new ImprovedWorkspace();



    setCentralWidget(m_mdiArea);
    // Connexion du changement de fenetre active avec la fonction de m.a.j du selecteur de taille des PJ
    connect(m_mdiArea, SIGNAL(subWindowActivated ( QMdiSubWindow * )), this, SLOT(changementFenetreActive(QMdiSubWindow *)));

    // Creation de la barre d'outils
    m_toolBar = new ToolsBar();
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
    m_audioPlayer = AudioPlayer::getInstance(this);
    m_networkManager->setAudioPlayer(m_audioPlayer);
    addDockWidget(Qt::RightDockWidgetArea,m_audioPlayer );
#endif
    //readSettings();
    m_preferencesDialog = new PreferencesDialog(this);

    linkActionToMenu();


    // Creation de l'editeur de notes
    m_noteEditor= new TextEdit(this);

	m_noteEditorSub  = static_cast<QMdiSubWindow*>(m_mdiArea->addWindow(m_noteEditor,m_ui->m_showMinutesEditorAction));
    if(NULL!=m_noteEditorSub)
    {
        m_noteEditorSub->setWindowTitle(tr("Minutes Editor[*]"));
        m_noteEditorSub->setWindowIcon(QIcon(":/notes.png"));
        m_noteEditorSub->hide();
    }
	connect(m_noteEditor,SIGNAL(closed(bool)),m_ui->m_newNoteAction,SLOT(setChecked(bool)));
    connect(m_noteEditor,SIGNAL(closed(bool)),m_noteEditorSub,SLOT(setVisible(bool)));


    // Initialisation des etats de sante des PJ/PNJ (variable declarees dans DessinPerso.cpp)


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

NetworkManager* MainWindow::getNetWorkManager()
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

void MainWindow::linkActionToMenu()
{
    // file menu
	connect(m_ui->m_newMapAction, SIGNAL(triggered(bool)), this, SLOT(newMap()));
	connect(m_ui->m_openPictureAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
	//connect(m_ui->m_openMapAction, SIGNAL(triggered(bool)), this, SLOT(openMapWizzard()));
	connect(m_ui->m_openMapAction, SIGNAL(triggered(bool)), this, SLOT(openContent()));

	connect(m_ui->m_openStoryAction, SIGNAL(triggered(bool)), this, SLOT(ouvrirScenario()));
	connect(m_ui->m_openNoteAction, SIGNAL(triggered(bool)), this, SLOT(openNote()));
	connect(m_ui->m_closeAction, SIGNAL(triggered(bool)), this, SLOT(closeMapOrImage()));
	connect(m_ui->m_saveAction, SIGNAL(triggered(bool)), this, SLOT(saveMap()));
	connect(m_ui->m_saveScenarioAction, SIGNAL(triggered(bool)), this, SLOT(sauvegarderScenario()));
	//connect(m_ui->m_saveMinuteAct, SIGNAL(triggered(bool)), this, SLOT(saveMinutes()));
	connect(m_ui->m_preferencesAction, SIGNAL(triggered(bool)), m_preferencesDialog, SLOT(show()));

    // close
	connect(m_ui->m_quitAction, SIGNAL(triggered(bool)), this, SLOT(quitterApplication()));

    // network
	connect(m_networkManager,SIGNAL(stopConnectionTry()),this,SLOT(stopReconnection()));
	connect(m_ui->m_disconnectAction,SIGNAL(triggered()),this,SLOT(closeConnection()));
	connect(m_ui->m_connectionAction,SIGNAL(triggered()),this,SLOT(startReconnection()));

    // Windows managing
	connect(m_ui->m_cascadeViewAction, SIGNAL(triggered(bool)), m_mdiArea, SLOT(cascadeSubWindows()));
	connect(m_ui->m_tabViewAction,SIGNAL(triggered(bool)),m_mdiArea,SLOT(setTabbedMode(bool)));
	connect(m_ui->m_tileViewAction, SIGNAL(triggered(bool)), m_mdiArea, SLOT(tileSubWindows()));

    // Help
	connect(m_ui->m_aboutAction, SIGNAL(triggered()), this, SLOT(aboutRolisteam()));
	connect(m_ui->m_onlineHelpAction, SIGNAL(triggered()), this, SLOT(helpOnLine()));


	//Note Editor
	connect(m_ui->m_showMinutesEditorAction, SIGNAL(triggered(bool)), this, SLOT(displayMinutesEditor(bool)));
}

QWidget* MainWindow::addMap(MapFrame *BipMapWindow, QString titre,QSize mapsize,QPoint pos )
{
	QAction *action = m_ui->m_menuSubWindows->addAction(titre);
    action->setCheckable(true);
    action->setChecked(true);

	m_mapWindowList.append(BipMapWindow);

    // Ajout de la carte au workspace
	QWidget* tmp = m_mdiArea->addWindow(BipMapWindow,action);
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

	BipMapWindow->setWindowIcon(QIcon(":/map.png"));
	BipMapWindow->setWindowTitle(titre);

	m_mapAction->insert(BipMapWindow,action);

    // Connexion de l'action avec l'affichage/masquage de la fenetre
    connect(action, SIGNAL(triggered(bool)), tmp, SLOT(setVisible(bool)));
	connect(action, SIGNAL(triggered(bool)), BipMapWindow, SLOT(setVisible(bool)));
	connect(BipMapWindow,SIGNAL(visibleChanged(bool)),action,SLOT(setChecked(bool)));

	Map* map = BipMapWindow->getMap();
	map->setPointeur(m_toolBar->getCurrentTool());
	map->setLocalIsPlayer(m_preferences->value("isPlayer",false).toBool());

	connect(m_toolBar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),map,SLOT(setPointeur(ToolsBar::SelectableTool)));

	connect(m_toolBar,SIGNAL(currentNpcSizeChanged(int)),map,SLOT(setCharacterSize(int)));
	connect(m_toolBar,SIGNAL(currentPenSizeChanged(int)),map,SLOT(setPenSize(int)));
	connect(m_toolBar,SIGNAL(currentTextChanged(QString)),map,SLOT(setCurrentText(QString)));
	connect(m_toolBar,SIGNAL(currentNpcNameChanged(QString)),map,SLOT(setCurrentNpcName(QString)));
	connect(m_toolBar,SIGNAL(currentNpcNumberChanged(int)),map,SLOT(setCurrentNpcNumber(int)));

	connect(map, SIGNAL(changeCurrentColor(QColor)), m_toolBar, SLOT(changeCurrentColor(QColor)));
	connect(map, SIGNAL(incrementeNumeroPnj()), m_toolBar, SLOT(incrementNpcNumber()));
    connect(map, SIGNAL(mettreAJourPnj(int, QString)), m_toolBar, SLOT(updateNpc(int,QString)));

	connect(m_ui->m_showPcNameAction, SIGNAL(triggered(bool)), map, SIGNAL(afficherNomsPj(bool)));
	connect(m_ui->m_showNpcNameAction, SIGNAL(triggered(bool)), map, SIGNAL(afficherNomsPnj(bool)));
	connect(m_ui->m_showNpcNumberAction, SIGNAL(triggered(bool)), map, SIGNAL(afficherNumerosPnj(bool)));

	connect(m_ui->m_showNpcNameAction, SIGNAL(triggered(bool)), map, SLOT(setNpcNameVisible(bool)));
	connect(m_ui->m_showPcNameAction, SIGNAL(triggered(bool)), map, SLOT(setPcNameVisible(bool)));
	connect(m_ui->m_showNpcNumberAction,SIGNAL(triggered(bool)),map,SLOT(setNpcNumberVisible(bool)));

	map->setNpcNameVisible(m_ui->m_showNpcNameAction->isChecked());
	map->setPcNameVisible(m_ui->m_showPcNameAction->isChecked());
	map->setNpcNumberVisible(m_ui->m_showNpcNumberAction->isChecked());
	map->setCurrentNpcNumber(m_toolBar->getCurrentNpcNumber());

    // new PlayersList connection
    connect(BipMapWindow, SIGNAL(activated(Map *)), m_playersListWidget->model(), SLOT(changeMap(Map *)));
    connect(BipMapWindow, SIGNAL(activated(Map *)), m_toolBar, SLOT(changeMap(Map *)));

    // Affichage de la carte
    tmp->setVisible(true);
    return tmp;

}
void  MainWindow::closeConnection()
{
    if(NULL!=m_networkManager)
    {
        m_networkManager->disconnectAndClose();
		m_ui->m_connectionAction->setEnabled(true);
		m_ui->m_disconnectAction->setEnabled(false);
    }
}

void MainWindow::addImage(Image* pictureWindow, QString title)
{
    pictureWindow->setParent(m_mdiArea);
    addImageToMdiArea(pictureWindow,title);
}
void MainWindow::openImage()
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
    Image *imageFenetre = new Image(titre,idImage, m_localPlayerId, img, NULL,m_mdiArea);

    addImageToMdiArea(imageFenetre,titre);

    // Envoie de l'image aux autres utilisateurs
	NetworkMessageWriter message(NetMsg::PictureCategory, NetMsg::AddPictureAction);
    imageFenetre->fill(message);
    message.sendAll();
}

void MainWindow::addImageToMdiArea(Image *imageFenetre, QString titre)
{
    //listeImage.append(imageFenetre);

    imageFenetre->setImageTitle(titre);

    // Creation de l'action correspondante
	QAction *action = m_ui->m_menuSubWindows->addAction(titre);
    action->setCheckable(true);
    action->setChecked(true);
    imageFenetre->setInternalAction(action);

    // Ajout de l'image au workspace
    QMdiSubWindow* sub = static_cast<QMdiSubWindow*>(m_mdiArea->addWindow(imageFenetre,action));

    m_pictureList.append(sub);

    // Mise a jour du titre de l'image
    sub->setWindowTitle(titre);
    sub->setWindowIcon(QIcon(":/picture.png"));


    connect(m_toolBar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),imageFenetre,SLOT(setCurrentTool(ToolsBar::SelectableTool)));

    imageFenetre->setCurrentTool(m_toolBar->getCurrentTool());
    connect(action, SIGNAL(triggered(bool)), sub, SLOT(setVisible(bool)));
    connect(action, SIGNAL(triggered(bool)), imageFenetre, SLOT(setVisible(bool)));
    sub->show();
}
void MainWindow::closeMapOrImage()
{

    QMdiSubWindow* subactive = m_mdiArea->currentSubWindow();
    QWidget* active = subactive->widget();
    MapFrame* bipMapWindow = NULL;

    if (NULL!=active)
    {

        Image*  imageFenetre = dynamic_cast<Image*>(active);

        QString mapImageId;
        QString mapImageTitle;
        QAction* associatedAct = NULL;
        mapImageTitle = active->windowTitle();
        bool image=false;
        //it is image
        if(NULL!=imageFenetre)
        {
            m_pictureList.removeOne(subactive);
            associatedAct = imageFenetre->getAssociatedAction();
            mapImageId = imageFenetre->getImageId();
            image = true;
        }
        else//it is a map
        {
            bipMapWindow= dynamic_cast<MapFrame*>(active);
			if(NULL!=bipMapWindow)
            {
				mapImageId = bipMapWindow->getMapId();
				associatedAct = m_mapAction->value(bipMapWindow);
			}
            else// it is undefined
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
            NetworkMessageWriter msg(NetMsg::MapCategory,NetMsg::CloseMap);
            msg.string8(mapImageId);
            msg.sendAll();

			m_mapWindowList.removeAll(bipMapWindow);
            m_playersListWidget->model()->changeMap(NULL);
            m_toolBar->changeMap(NULL);

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
void MainWindow::updateWorkspace()
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
	if (widget != NULL && widget->objectName() == QString("BipMapWindow") && localPlayerIsGM)
    {

		m_ui->m_closeAction->setEnabled(true);
		m_ui->m_saveAction->setEnabled(true);
    }
    else if (widget != NULL && widget->objectName() == QString("Image") &&
             (localPlayerIsGM || static_cast<Image *>(widget)->isImageOwner(m_localPlayerId)))
    {
		m_ui->m_closeAction->setEnabled(true);
		m_ui->m_saveAction->setEnabled(false);
    }
    else
    {
		m_ui->m_closeAction->setEnabled(false);
		m_ui->m_saveAction->setEnabled(false);
    }
}
void MainWindow::majCouleursPersonnelles()
{
    m_toolBar->updatePersonalColor();
}

void MainWindow::newMap()
{
    MapFrame* bipMapWindow = new MapFrame(NULL, m_mdiArea);
    if(!bipMapWindow->createMap())
    {
        delete bipMapWindow;
    }
    else
    {
        addMap(bipMapWindow,bipMapWindow->getTitle());
    }
}

void MainWindow::emettreTousLesPlans(NetworkLink * link)
{
   // qDebug() << "emettreTousLesPlans " << link;
    int tailleListe = m_mapWindowList.size();
    for (int i=0; i<tailleListe; ++i)
    {
		m_mapWindowList[i]->getMap()->setHasPermissionMode(m_playerList->everyPlayerHasFeature("MapPermission"));
		m_mapWindowList[i]->getMap()->emettreCarte(m_mapWindowList[i]->windowTitle(), link);
		m_mapWindowList[i]->getMap()->emettreTousLesPersonnages(link);
    }
}

void MainWindow::emettreToutesLesImages(NetworkLink * link)
{
    NetworkMessageWriter message = NetworkMessageWriter(NetMsg::PictureCategory, NetMsg::AddPictureAction);

    foreach(QMdiSubWindow* sub, m_pictureList)
    {
        Image* img = dynamic_cast<Image*>(sub->widget());
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
    if(NULL!=tmp)
    {

        MapFrame* mapWindow = dynamic_cast<MapFrame*>(tmp->widget());
        m_playersListWidget->model()->changeMap(NULL);
        m_toolBar->changeMap(NULL);
        if(NULL!=mapWindow)
        {
            delete m_mapAction->value(mapWindow);
            m_mapWindowList.removeAll(mapWindow);
            delete mapWindow;
            delete tmp;
        }
    }
}
Map* MainWindow::trouverCarte(QString idCarte)
{
	/// @todo @warning complexity of the method is N, it should be 1. Replace list by map or hash.
    int tailleListe = m_mapWindowList.size();

    bool ok = false;
    int i;
    for (i=0; i<tailleListe && !ok; ++i)
		if ( m_mapWindowList[i]->getMap()->identifiantCarte() == idCarte )
            ok = true;

    // Si la carte vient d'etre trouvee on renvoie son pointeur
    if (ok)
		return m_mapWindowList[i-1]->getMap();
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
            m_pictureList.removeOne(tmp);

            delete imageWindow->getAssociatedAction();
            delete imageWindow;

        }
        delete tmp;
    }
}
bool MainWindow::enleverCarteDeLaListe(QString idCarte)
{
	// Taille de la liste des BipMapWindow
    int tailleListe = m_mapWindowList.size();

    bool ok = false;
    int i;
    for (i=0; i<tailleListe && !ok; ++i)
    {
        if(NULL != m_mapWindowList[i])
        {
			if(NULL!=m_mapWindowList[i]->getMap())
            {
				if ( m_mapWindowList[i]->getMap()->identifiantCarte() == idCarte )
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
    MapFrame* mapWindow = static_cast<MapFrame*>(subWindow->widget());




	if ((NULL==mapWindow) || (mapWindow->objectName() != "BipMapWindow"))
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
	mapWindow->getMap()->saveMap(out);
    file.close();
}


void MainWindow::changementNatureUtilisateur()
{

    m_toolBar->updateUi();
    updateUi();
    updateWindowTitle();
#ifndef NULL_PLAYER
    //removeDockWidget(m_audioPlayer);
    //m_audioPlayer->show();
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
		m_ui->m_showMinutesEditorAction->setChecked(visible);
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

		QMdiSubWindow* mapWindow = NULL;//readMapAndNpc(in);
        if(NULL!=mapWindow)
        {
            mapWindow->setGeometry(posWindow.x(),posWindow.y(),sizeWindow.width(),sizeWindow.height());
        }
    }


    int nbrImages;
    in >>nbrImages;
    // in >>On lit toutes les images presentes dans le fichier
    for (int i=0; i<nbrImages; ++i)
        readImageFromStream(in);


    //reading minutes
    m_noteEditor->readFromBinary(in);

    bool tempbool;
    in >> tempbool;
    m_noteEditor->setVisible(tempbool);
	m_ui->m_showMinutesEditorAction->setChecked(tempbool);

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
		m_mapWindowList[i]->getMap()->saveMap(out, m_mapWindowList[i]->windowTitle());
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
            img->saveImageToFile(out);
        }

    }
}


void MainWindow::readImageFromStream(QDataStream &file)
{
    // Lecture de l'image

    // On recupere le titre

    QString title;
    QByteArray baImage;
    QPoint topleft;
    QSize size;

    file >> title;
    file >>topleft;
    file >> size;
    file >> baImage;


    QImage img;
    if (!img.loadFromData(baImage, "jpg"))
    {
        qWarning()<< tr("Image compression error (readImageFromStream - MainWindow.cpp)");
    }



    // Creation de l'identifiant
    QString idImage = QUuid::createUuid().toString();

    // Creation de la fenetre image
    Image *imageFenetre = new Image(title,idImage, m_localPlayerId, &img, NULL,m_mdiArea);


    addImageToMdiArea(imageFenetre,title);

    connect(m_toolBar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),imageFenetre,SLOT(setCurrentTool(ToolsBar::SelectableTool)));
    imageFenetre->setCurrentTool(m_toolBar->getCurrentTool());

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    if (!img.save(&buffer, "jpg", 70))
    {
        qWarning()<< tr("Image compression error (readImageFromStream - MainWindow.cpp)");
    }

    NetworkMessageWriter msg(NetMsg::PictureCategory,NetMsg::AddPictureAction);
    msg.string16(title);
    msg.string8(idImage);
    msg.string8(m_localPlayerId);
    msg.byteArray32(byteArray);
    msg.sendAll();
}
void MainWindow::aboutRolisteam()
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
		m_ui->m_newMapAction->setEnabled(false);
		m_ui->m_openMapAction->setEnabled(false);

		m_ui->m_openStoryAction->setEnabled(false);
		m_ui->m_closeAction->setEnabled(false);
		m_ui->m_saveAction->setEnabled(false);
		m_ui->m_saveScenarioAction->setEnabled(false);

    }

    if(m_networkManager->isServer())
    {
		m_ui->m_connectionAction->setEnabled(false);
		m_ui->m_disconnectAction->setEnabled(true);
    }
    else
    {
		m_ui->m_connectionAction->setEnabled(false);
		m_ui->m_disconnectAction->setEnabled(true);
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

Map::PermissionMode MainWindow::getPermission(int id)
{
    switch(id)
    {
    case 0:
        return Map::GM_ONLY;
    case 1:
        return Map::PC_MOVE;
    case 2:
        return Map::PC_ALL;
    default:
        return Map::GM_ONLY;
    }

}
void MainWindow::networkStateChanged(bool state)
{
    if(state)
    {
		m_ui->m_connectionAction->setEnabled(false);
		m_ui->m_disconnectAction->setEnabled(true);

        if((m_networkManager->isConnected())&&(m_networkManager->isServer())&&(NULL!=m_ipChecker))
        {
            m_ipChecker->startCheck();
        }
    }
    else
    {
		m_ui->m_connectionAction->setEnabled(true);
		m_ui->m_disconnectAction->setEnabled(false);
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
	m_ui->m_connectionAction->setEnabled(true);
	m_ui->m_disconnectAction->setEnabled(false);
}
void MainWindow::closeAllImagesAndMaps()
{

    foreach(QMdiSubWindow* tmp,m_pictureList)
    {
        if(NULL!=tmp)
        {
                Image* imageWindow = dynamic_cast<Image*>(tmp->widget());

                if(NULL!=imageWindow)
                {
                    removePictureFromId(imageWindow->getImageId());
                }
        }
    }

    foreach(MapFrame* tmp,m_mapWindowList)
    {
        if(NULL!=tmp)
        {
            removeMapFromId(tmp->getMapId());
        }
    }
}

void MainWindow::startReconnection()
{
    if (PreferencesManager::getInstance()->value("isClient",true).toBool())
    {
       closeAllImagesAndMaps();
    }
    if(m_networkManager->startConnection())
    {
		m_ui->m_connectionAction->setEnabled(false);
		m_ui->m_disconnectAction->setEnabled(true);
    }
    else
    {
		m_ui->m_connectionAction->setEnabled(true);
		m_ui->m_disconnectAction->setEnabled(false);
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
        // We want to know if the server refuses local player to be GM
        connect(m_playerList, SIGNAL(localGMRefused()), this, SLOT(changementNatureUtilisateur()));
        // We send a message to del local player when we quit
        connect(this, SIGNAL(closing()), m_playerList, SLOT(sendDelLocalPlayer()));
    }
    else
    {
		  connect(m_networkManager, SIGNAL(linkAdded(NetworkLink *)), this, SLOT(updateSessionToNewClient(NetworkLink*)));
    }
    connect(m_networkManager, SIGNAL(dataReceived(quint64,quint64)), this, SLOT(receiveData(quint64,quint64)));

}
void MainWindow::updateSessionToNewClient(NetworkLink* link)
{
    emettreTousLesPlans(link);
    emettreToutesLesImages(link);
}

void MainWindow::setNetworkManager(NetworkManager* tmp)
{
    m_networkManager = tmp;
    m_networkManager->setParent(this);
}
void MainWindow::readSettings()
{
	QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(m_version));

	if(m_resetSettings)
	{
		settings.clear();
	}

	restoreState(settings.value("windowState").toByteArray());
	bool  maxi = settings.value("Maximized", false).toBool();
	if(!maxi)
	{
		restoreGeometry(settings.value("geometry").toByteArray());
	}

	m_preferences->readSettings(settings);
}
void MainWindow::writeSettings()
{
    QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(m_version));

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
	settings.setValue("Maximized", isMaximized());

    m_preferences->writeSettings(settings);
}
void MainWindow::parseCommandLineArguments(QStringList list)
{

        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addVersionOption();

        QCommandLineOption port(QStringList() << "p"<< "port", tr("Set rolisteam to use <port> for the connection"),"port");
        QCommandLineOption hostname(QStringList() << "s"<< "server", tr("Set rolisteam to connect to <server>."),"server");
        QCommandLineOption role(QStringList() << "r"<< "role", tr("Define the <role>: gm or pc"),"role");
		QCommandLineOption reset(QStringList() << "reset-settings", tr("Erase the settings and use the default parameters"));
        QCommandLineOption user(QStringList() << "u"<<"user", tr("Define the <username>"),"username");


        parser.addOption(port);
        parser.addOption(hostname);
        parser.addOption(role);
		parser.addOption(reset);
        parser.addOption(user);

        parser.process(list);

        bool hasPort = parser.isSet(port);
        bool hasHostname = parser.isSet(hostname);
        bool hasRole = parser.isSet(role);
        bool hasUser = parser.isSet(user);
		m_resetSettings = parser.isSet(reset);

        QString portValue;
        QString hostnameValue;
        QString roleValue;
        QString username;
        if(hasPort)
        {
            portValue = parser.value(port);
        }
        if(hasHostname)
        {
            hostnameValue = parser.value(hostname);
        }
        if(hasRole)
        {
            roleValue = parser.value(role);
        }
        if(hasUser)
        {
            username = parser.value(user);
        }
        if(!(roleValue.isNull()&&hostnameValue.isNull()&&portValue.isNull()&&username.isNull()))
		{
            m_networkManager->setValueConnection(portValue,hostnameValue,username,roleValue);
		}

}
NetWorkReceiver::SendType MainWindow::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type;
    switch(msg->category())
    {
    case NetMsg::PictureCategory:
        qDebug() << "picture";
        processImageMessage(msg);
        type = NetWorkReceiver::AllExceptMe;
        break;
    case NetMsg::MapCategory:
        qDebug() << "MapCategory";
        processMapMessage(msg);
        type = NetWorkReceiver::AllExceptMe;
        break;
    case NetMsg::NPCCategory:
        qDebug() << "NPCCategory";
        processNpcMessage(msg);
        type = NetWorkReceiver::AllExceptMe;
        break;
    case NetMsg::DrawCategory:
        qDebug() << "DrawCategory";
        processPaintingMessage(msg);
        type = NetWorkReceiver::ALL;
        break;
    case NetMsg::CharacterCategory:
        qDebug() << "CharacterCategory";
        processCharacterMessage(msg);
        type = NetWorkReceiver::AllExceptMe;
        break;
    case NetMsg::ConnectionCategory:
        qDebug() << "ConnectionCategory";
        processConnectionMessage(msg);
        type = NetWorkReceiver::NONE;
        break;
    case NetMsg::CharacterPlayerCategory:
        qDebug() << "CharacterPlayerCategory";
        processCharacterPlayerMessage(msg);
        type = NetWorkReceiver::AllExceptMe;
        break;
    }
    return type;//NetWorkReceiver::AllExceptMe;
}
void MainWindow::processConnectionMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::EndConnectionAction)
    {
        notifyUser(tr("End of the connection process"));
        updateWorkspace();
    }
}

void MainWindow::processMapMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::CloseMap)
    {
        QString idMap = msg->string8();
        removeMapFromId(idMap);
    }
    else
    {
        MapFrame* bipMapWindow = new MapFrame(NULL, m_mdiArea);
        if(!bipMapWindow->processMapMessage(msg))
        {
            delete bipMapWindow;
        }
        else
        {
            addMap(bipMapWindow,bipMapWindow->getTitle());
        }
    }
}

void MainWindow::processImageMessage(NetworkMessageReader* msg)
{
		if(msg->action() == NetMsg::AddPictureAction)
		{
			QString title = msg->string16();
			QString idImage = msg->string8();
			QString idPlayer = msg->string8();
			QByteArray dataImage = msg->byteArray32();

			QImage *img = new QImage;
			if (!img->loadFromData(dataImage, "jpg"))
			{
				qWarning("Cannot read received image (receptionMessageImage - NetworkLink.cpp)");
			}
            Image *image = new Image(title,idImage, idPlayer, img);
			addImage(image, title);


		}
		else if(msg->action() == NetMsg::DelPictureAction)
		{
			QString idImage = msg->string8();
			removePictureFromId(idImage);
		}
}
void MainWindow::processNpcMessage(NetworkMessageReader* msg)
{

        QString idMap = msg->string8();
        if(msg->action() == NetMsg::addNpc)
        {
            Map* map = trouverCarte(idMap);
            extractCharacter(map,msg);

        }
        else if(msg->action() == NetMsg::delNpc)
        {
            QString idNpc = msg->string8();
            Map* map = trouverCarte(idMap);
            if(NULL!=map)
            {
                map->eraseCharacter(idNpc);
            }
        }
}
void MainWindow::processPaintingMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::penPainting)
    {
        QString idPlayer = msg->string8();
        QString idMap = msg->string8();

        quint32 pointNumber = msg->uint32();

        QList<QPoint> pointList;
        QPoint point;
        for (int i=0; i<pointNumber; i++)
        {
            quint16 pointX = msg->uint16();
            quint16 pointY = msg->uint16();
            point = QPoint(pointX, pointY);
            pointList.append(point);
        }
        quint16 zoneX = msg->uint16();
        quint16 zoneY = msg->uint16();
        quint16 zoneW = msg->uint16();
        quint16 zoneH = msg->uint16();

        QRect zoneToRefresh(zoneX,zoneY,zoneW,zoneH);

        quint8 diameter = msg->uint8();
        quint8 colorType = msg->uint8();
        QColor color(msg->rgb());

        Map* map = trouverCarte(idMap);

        if(NULL!=map)
        {
            SelectedColor selectedColor;
            selectedColor.color = color;
            selectedColor.type = (ColorKind)colorType;
			map->paintPenLine(&pointList,zoneToRefresh,diameter,selectedColor,idPlayer==m_localPlayerId);
        }
    }
    else if(msg->action() == NetMsg::textPainting)
    {
        QString idMap = msg->string8();
        QString text = msg->string16();
        quint16 posx = msg->uint16();
        quint16 posy = msg->uint16();
        QPoint pos(posx,posy);

        quint16 zoneX = msg->uint16();
        quint16 zoneY = msg->uint16();
        quint16 zoneW = msg->uint16();
        quint16 zoneH = msg->uint16();

        QRect zoneToRefresh(zoneX,zoneY,zoneW,zoneH);
        quint8 colorType = msg->uint8();
        QColor color(msg->rgb());

        Map* map = trouverCarte(idMap);

        if(NULL!=map)
        {
            SelectedColor selectedColor;
            selectedColor.color = color;
            selectedColor.type = (ColorKind)colorType;

			map->paintText(text,pos,zoneToRefresh,selectedColor);

        }
    }
    else if(msg->action() == NetMsg::handPainting)
    {

    }
    else
    {
        QString idMap = msg->string8();
        quint16 posx = msg->uint16();
        quint16 posy = msg->uint16();
        QPoint startPos(posx,posy);

        quint16 endposx = msg->uint16();
        quint16 endposy = msg->uint16();
        QPoint endPos(endposx,endposy);


        quint16 zoneX = msg->uint16();
        quint16 zoneY = msg->uint16();
        quint16 zoneW = msg->uint16();
        quint16 zoneH = msg->uint16();

        QRect zoneToRefresh(zoneX,zoneY,zoneW,zoneH);

        quint8 diameter = msg->uint8();
        quint8 colorType = msg->uint8();
        QColor color(msg->rgb());
        Map* map = trouverCarte(idMap);

        if(NULL!=map)
        {
            SelectedColor selectedColor;
            selectedColor.color = color;
            selectedColor.type = (ColorKind)colorType;

			map->paintOther(msg->action(),startPos,endPos,zoneToRefresh,diameter,selectedColor);
        }


    }
}
void MainWindow::extractCharacter(Map* map,NetworkMessageReader* msg)
{
    if(NULL!=map)
    {
        QString npcName = msg->string16();
        QString npcId = msg->string8();
        quint8 npcType = msg->uint8();
        quint8 npcNumber = msg->uint8();
        quint8 npcDiameter = msg->uint8();
        QColor npcColor(msg->rgb());
        quint16 npcXpos = msg->uint16();
        quint16 npcYpos = msg->uint16();

        qint16 npcXorient = msg->int16();
        qint16 npcYorient = msg->int16();

        QColor npcState(msg->rgb());
        QString npcStateName = msg->string16();
        quint16 npcStateNum = msg->uint16();

        quint8 npcVisible = msg->uint8();
        quint8 npcVisibleOrient = msg->uint8();

         QPoint orientation(npcXorient, npcYorient);

        QPoint npcPos(npcXpos, npcYpos);

		bool showNumber=(npcType == DessinPerso::pnj)?m_ui->m_showNpcNumberAction->isChecked():false;
		bool showName=(npcType == DessinPerso::pnj)? m_ui->m_showNpcNameAction->isChecked():m_ui->m_showPcNameAction->isChecked();

        DessinPerso* npc = new DessinPerso(map, npcId, npcName, npcColor, npcDiameter, npcPos, (DessinPerso::typePersonnage)npcType,showNumber,showName, npcNumber);

		if((npcVisible)||(npcType == DessinPerso::pnj && !m_preferences->value("isPlayer",false).toBool()))
        {
            npc->afficherPerso();
        }
        npc->nouvelleOrientation(orientation);
        if(npcVisibleOrient)
        {
            npc->afficheOuMasqueOrientation();
        }

        DessinPerso::etatDeSante health;
        health.couleurEtat = npcState;
        health.nomEtat = npcStateName;
        npc->nouvelEtatDeSante(health, npcStateNum);
        map->afficheOuMasquePnj(npc);

    }
}

void MainWindow::processCharacterMessage(NetworkMessageReader* msg)
{
    if(NetMsg::addCharacterList == msg->action())
    {
        QString idMap = msg->string8();
        quint16 characterNumber = msg->uint16();
        Map* map = trouverCarte(idMap);

        if(NULL!=map)
        {
            for(int i=0;i<characterNumber;++i)
            {
                extractCharacter(map,msg);
            }
        }
    }
    else if(NetMsg::moveCharacter == msg->action())
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint32 pointNumber = msg->uint32();


        QList<QPoint> moveList;
        QPoint point;
        for (int i=0; i<pointNumber; i++)
        {
            quint16 posX = msg->uint16();
            quint16 posY = msg->uint16();
            point = QPoint(posX, posY);
            moveList.append(point);
        }
        Map* map=trouverCarte(idMap);
        if(NULL!=map)
        {
            map->lancerDeplacementPersonnage(idCharacter,moveList);
        }
    }
    else if(NetMsg::changeCharacterState == msg->action())
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint16 stateNumber = msg->uint16();
        Map* map=trouverCarte(idMap);
        if(NULL!=map)
        {
            DessinPerso* character = map->trouverPersonnage(idCharacter);
            if(NULL!=character)
            {
                character->changerEtatDeSante(stateNumber);
            }
        }
    }
    else if(NetMsg::changeCharacterOrientation == msg->action())
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        qint16 posX = msg->int16();
        qint16 posY = msg->int16();
        QPoint orientation(posX, posY);
        Map* map=trouverCarte(idMap);
        if(NULL!=map)
        {
            DessinPerso* character = map->trouverPersonnage(idCharacter);
            if(NULL!=character)
            {
                character->nouvelleOrientation(orientation);
            }
        }
    }
    else if(NetMsg::showCharecterOrientation == msg->action())
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint8 showOrientation = msg->uint8();
        Map* map=trouverCarte(idMap);
        if(NULL!=map)
        {
            DessinPerso* character = map->trouverPersonnage(idCharacter);
            if(NULL!=character)
            {
                character->afficherOrientation(showOrientation);
            }
        }
    }
}
void MainWindow::processCharacterPlayerMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::ToggleViewPlayerCharacterAction)
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint8 display = msg->uint8();
        Map* map=trouverCarte(idMap);
        if(NULL!=map)
        {
			map->showPc(idCharacter,display);
        }
    }
    else if(msg->action() == NetMsg::ChangePlayerCharacterSizeAction)
    {
        /// @warning overweird
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint8 size = msg->uint8();
        Map* map=trouverCarte(idMap);
        if(NULL!=map)
        {
            map->selectCharacter(idCharacter);
			map->changePjSize(size + 11);
        }
    }
}
CleverURI* MainWindow::contentToPath(CleverURI::ContentType type,bool save)
{
	QString filter;
    QString folder;
    QString title;
    switch(type)
    {
        case CleverURI::PICTURE:
            filter = m_supportedImage;
            title = tr("Open Picture");
			folder = m_preferences->value(QString("PicturesDirectory"),".").toString();
            break;
        case CleverURI::MAP:
            filter = m_supportedMap;
            title = tr("Open Map");
			folder = m_preferences->value(QString("MapsDirectory"),".").toString();
            break;
        case CleverURI::TEXT:
            filter = m_supportedNotes;
            title = tr("Open Minutes");
			folder = m_preferences->value(QString("MinutesDirectory"),".").toString();
			break;
	   /* case CleverURI::CHARACTERSHEET:
            filter = m_supportedCharacterSheet;
            title = tr("Open Character Sheets");
            folder = m_options->value(QString("DataDirectory"),".").toString();
			break;
            case CleverURI::PDF:
            filter = m_pdfFiles;
            title = tr("Open Pdf file");
            folder = m_options->value(QString("DataDirectory"),".").toString();
			break;*/
    default:
            break;
    }
    if(!filter.isNull())
    {
        QString filepath;
        if(save)
            filepath= QFileDialog ::getSaveFileName(this,title,folder,filter);
        else
            filepath= QFileDialog::getOpenFileName(this,title,folder,filter);

        return new CleverURI(filepath,type);
	}
    return NULL;
}
void MainWindow::openContent()
{
    QAction* action=static_cast<QAction*>(sender());
    CleverURI::ContentType type;
	if(action == m_ui->m_openMapAction)
    {
        type=CleverURI::MAP;
    }
	else if(action == m_ui->m_openPictureAction)
    {
        type=CleverURI::PICTURE;

    }
	else if(action == m_ui->m_openStoryAction)
    {
        type = CleverURI::SCENARIO;
    }
	/*else if(action== m_openCharacterSheetsAct)
    {
        type = CleverURI::CHARACTERSHEET;
	}*/
	else if(action == m_ui->m_openNoteAction)
    {
        type = CleverURI::TEXT;
    }
#ifdef WITH_PDF
    else if(action == m_openPDFAct)
    {
        type = CleverURI::PDF;
    }
#endif
    else
    {
        return;
    }
	//CleverURI* path = contentToPath(type,false);

	openCleverURI(type);
}
void MainWindow::openCleverURI(CleverURI::ContentType type)
{
	MediaContainer* tmp=NULL;
	switch(type)
	{
	case CleverURI::MAP:
        tmp = new MapFrame();
		break;
	case CleverURI::PICTURE:
		//tmp = new Image(m_workspace);
		break;
	case CleverURI::SCENARIO:
		break;
	case CleverURI::TEXT:
		//tmp = new MinutesEditor(uri);
		break;
/*	case CleverURI::CHARACTERSHEET:
		tmp = new CharacterSheetWindow();
		break;
	case CleverURI::PDF:
		tmp = new PDFViewer();
		break;
	case CleverURI::TCHAT:
	case CleverURI::SONG:
		break;*/
	default:
		break;
	}
	if(tmp!=NULL)
	{
		tmp->openMedia();
		if(tmp->readFileFromUri())
		{
            addMap((MapFrame*)tmp,tmp->getTitle());
            ((MapFrame*)tmp)->setVisible(true);
		}
		//addToWorkspace(tmp);
	}
}
