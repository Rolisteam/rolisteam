/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

//Qt headers
#include <QtGui>
#include <QDebug>
#include <QActionGroup>


//former (but renamed) headers
#include "MapFrame.h"

#include "MainWindow.h"
#include "ToolBar.h"

#include "Image.h"
#include "audioplayer.h"
#include "EditeurNotes.h"
#include "improvedworkspace.h"

//new headers from rolisteam 2.0.0 branch
#include "preferencedialog.h"
#include "preferencesmanager.h"
#include "connectionwizzard.h"
#include "charactersheetwindow.h"



MainWindow::MainWindow()
        : QMainWindow()
{
        m_options = PreferencesManager::getInstance();
        readSettings();

        /// all other allocation must be done after the settings reading.
        m_preferenceDialog = new PreferenceDialog(this);
        m_connectDialog = new ConnectionWizzard(this);

        listeCarteFenetre.clear();
        listeImage.clear();
        listeTchat.clear();

        m_toolbar = new ToolsBar(this);
        setAnimated(false);
        m_workspace = new ImprovedWorkspace(m_toolbar->currentColor());
        setCentralWidget(m_workspace);
        addDockWidget(Qt::LeftDockWidgetArea, m_toolbar);


        connect(m_toolbar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),m_workspace,SLOT(currentToolChanged(ToolsBar::SelectableTool)));
        connect(m_toolbar,SIGNAL(currentColorChanged(QColor&)),m_workspace,SLOT(currentColorChanged(QColor&)));
        connect(m_toolbar,SIGNAL(currentModeChanged(int)),m_workspace,SIGNAL(currentModeChanged(int)));
        connect(m_toolbar,SIGNAL(currentPenSizeChanged(int)),m_workspace,SLOT(currentPenSizeChanged(int)));
        connect(m_toolbar,SIGNAL(currentPNCSizeChanged(int)),m_workspace,SLOT(currentNPCSizeChanged(int)));

        dockLogUtil = creerLogUtilisateur();
        addDockWidget(Qt::RightDockWidgetArea, dockLogUtil);

        m_playerListDockWidget = new UserListDockWidget;
        addDockWidget(Qt::RightDockWidgetArea, m_playerListDockWidget);

        m_audioPlayer = AudioPlayer::getInstance(this);
        addDockWidget(Qt::RightDockWidgetArea, m_audioPlayer);


        createMenu();
        associerActionsMenus();
        autoriserOuInterdireActions();


        editeurNotes = new EditeurNotes();
        m_workspace->addWidget(editeurNotes);
        editeurNotes->setWindowTitle(tr("Minutes editor"));
        editeurNotes->hide();


        m_characterSheet = new CharacterSheetWindow();
        m_workspace->addWidget(m_characterSheet);
        m_characterSheet->setVisible(false);



}


QDockWidget* MainWindow::creerLogUtilisateur()
{
        QDockWidget *dockLogUtil = new QDockWidget(tr("Events"), this);
        dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockLogUtil->setFeatures(QDockWidget::AllDockWidgetFeatures);
        dockLogUtil->setMinimumWidth(125);
        return dockLogUtil;
}


void MainWindow::createMenu()
{

        QMenuBar *menuBar = new QMenuBar(this);

        setMenuBar(menuBar);

        // Creation du menu Fichier
        QMenu *menuFichier = new QMenu (tr("File"), menuBar);
        newMapAction            = menuFichier->addAction(tr("&New empty map"));
        menuFichier->addSeparator();
        actionOuvrirPlan             = menuFichier->addAction(tr("Open Map"));
        actionOuvrirEtMasquerPlan    = menuFichier->addAction(tr("Open and mask Map"));
        actionOuvrirScenario         = menuFichier->addAction(tr("Open scenario"));
        OpenImageAction	         = menuFichier->addAction(tr("Open Picture"));
        actionOuvrirNotes            = menuFichier->addAction(tr("Open Text"));
        menuFichier->addSeparator();
        actionFermerPlan             = menuFichier->addAction(tr("Close Map/Image"));
        menuFichier->addSeparator();
        actionSauvegarderPlan        = menuFichier->addAction(tr("Save Map"));
        actionSauvegarderScenario    = menuFichier->addAction(tr("Save scenario"));
        actionSauvegarderNotes       = menuFichier->addAction(tr("Save text"));

        menuFichier->addSeparator();
        m_preferencesAct = menuFichier->addAction(tr("Preferences"));
        m_preferencesAct->setShortcut(tr("Ctrl+,"));
        connect(m_preferencesAct,SIGNAL(triggered()),this,SLOT(showPreferenceManager()));

        menuFichier->addSeparator();
        actionQuitter= menuFichier->addAction(tr("Quit"));
        actionQuitter->setShortcut(tr("Ctrl+q"));

        // Creation du menu Affichage
        QMenu *menuAffichage = new QMenu (tr("View"), menuBar);
        actionAfficherNomsPj         = menuAffichage->addAction(tr("Show PC's names"));
        actionAfficherNomsPnj        = menuAffichage->addAction(tr("Show NPC's names"));
        actionAfficherNumerosPnj     = menuAffichage->addAction(tr("Show NPC's numbers"));
/*
        // Creation du sous-menu Grille
        QMenu *sousMenuGrille = new QMenu (tr("Grille"), barreMenus);
        actionSansGrille             = sousMenuGrille->addAction(tr("Aucune"));
        actionCarre                  = sousMenuGrille->addAction(tr("Carrs"));
        actionHexagones              = sousMenuGrille->addAction(tr("Hexagones"));
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
        actionAfficherNomsPj->setCheckable(true);
        actionAfficherNomsPnj->setCheckable(true);
        actionAfficherNumerosPnj->setCheckable(true);
/*
        actionSansGrille        ->setCheckable(true);
        actionCarre             ->setCheckable(true);
        actionHexagones         ->setCheckable(true);
*/

        // Choix des actions selectionnees au depart
        actionAfficherNomsPj->setChecked(true);
        actionAfficherNomsPnj->setChecked(true);
        actionAfficherNumerosPnj->setChecked(true);
/*
        actionSansGrille        ->setChecked(true);
*/

        menuFenetre = new QMenu (tr("Windows"), menuBar);


        QMenu *sousMenuReorganise    = new QMenu (tr("Organize"), menuBar);
        actionCascade = sousMenuReorganise->addAction(tr("Cascade"));
        actionTuiles = sousMenuReorganise->addAction(tr("Tuiles"));

        menuFenetre->addMenu(sousMenuReorganise);
        menuFenetre->addSeparator();


        menuFenetre->addAction(dockLogUtil->toggleViewAction());

        menuFenetre->addAction(m_audioPlayer->toggleViewAction());
        menuFenetre->addSeparator();


        actionEditeurNotes = menuFenetre->addAction(tr("Minutes Editor"));
        actionEditeurNotes->setCheckable(true);
        actionEditeurNotes->setChecked(false);
        connect(actionEditeurNotes, SIGNAL(triggered(bool)), this, SLOT(displayMinutesEditor(bool)));

        m_showDataSheet = menuFenetre->addAction(tr("CharacterSheet Viewer"));
        m_showDataSheet->setCheckable(true);
        m_showDataSheet->setChecked(false);
        connect(m_showDataSheet, SIGNAL(triggered(bool)), this, SLOT(displayCharacterSheet(bool)));




        sousMenuTchat = new QMenu (tr("Tchats"), menuBar);
        menuFenetre->addMenu(sousMenuTchat);

        // Ajout de l'action d'affichage de la fenetre de tchat commun
        actionTchatCommun = sousMenuTchat->addAction(tr("common Tchat"));
        actionTchatCommun->setCheckable(true);
        actionTchatCommun->setChecked(false);
        menuFenetre->addSeparator();


        listeTchat.append(new Tchat("", actionTchatCommun,NULL));
        m_workspace->addWidget(listeTchat[0]);
        listeTchat[0]->setWindowTitle(tr("Tchat commun"));
        listeTchat[0]->hide();

        connect(actionTchatCommun, SIGNAL(triggered(bool)), listeTchat[0], SLOT(setVisible(bool)));

        //////////////////////////
        //Network Menu and actions
        //////////////////////////
        m_networkMenu = new QMenu (tr("Network"), menuBar);
        m_serverAct = m_networkMenu->addAction(tr("Start server..."));
        connect(m_serverAct,SIGNAL(triggered()),this,SLOT(startServer()));

        m_newConnectionAct = m_networkMenu->addAction(tr("New Connection..."));
        connect(m_newConnectionAct,SIGNAL(triggered()),this,SLOT(addConnection()));


        QVariant tmp2;
        tmp2.setValue(ConnectionList());
        QVariant tmp = m_options->value("network/connectionsList",tmp2);
        m_connectionList = tmp.value<ConnectionList>();

        m_connectionActGroup = new QActionGroup(this);
        qDebug() << m_connectionList.size();
        if(m_connectionList.size() > 0)//(m_connectionList != NULL)&&
        {
            m_networkMenu->addSeparator();
            foreach(Connection tmp, m_connectionList )
            {
               m_networkMenu->addAction(m_connectionActGroup->addAction(tmp.getName()));
            }

        }



        m_networkMenu->addSeparator();
        m_manageConnectionAct = m_networkMenu->addAction(tr("Manage connections..."));
        connect(m_manageConnectionAct,SIGNAL(triggered()),this,SLOT(showConnectionManager()));


        /////////////////////////
        //Help menu and actions
        /////////////////////////
        QMenu *helpMenu = new QMenu (tr("Help"), menuBar);

        actionHelp = helpMenu->addAction(tr("Help of %1").arg(m_options->value("APPLICATION_NAME","rolisteam").toString()));
         actionHelp->setShortcut(tr("F1"));
        helpMenu->addSeparator();
        actionAPropos = helpMenu->addAction(tr("About %1").arg(m_options->value("APPLICATION_NAME","rolisteam").toString()));
        m_currentWindowMenu= new QMenu(tr("Current Window"),menuBar);
        m_workspace->setVariantMenu(m_currentWindowMenu);

        menuBar->addMenu(menuFichier);
        menuBar->addMenu(menuAffichage);
        menuBar->addMenu(m_currentWindowMenu);
        menuBar->addMenu(menuFenetre);
        menuBar->addMenu(m_networkMenu);
        menuBar->addMenu(helpMenu);
        menuBar->removeAction(m_currentWindowMenu->menuAction());

}


void MainWindow::associerActionsMenus()
{

        connect(newMapAction, SIGNAL(triggered(bool)), this, SLOT(clickOnMapWizzard()));
        connect(OpenImageAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));

        // Help
        connect(actionQuitter, SIGNAL(triggered(bool)), this, SLOT(close()));
        connect(actionAPropos, SIGNAL(triggered()), this, SLOT(about()));
        connect(actionHelp, SIGNAL(triggered()), this, SLOT(aideEnLigne()));
}


void MainWindow::autoriserOuInterdireActions()
{

}

void MainWindow::changementFenetreActive(QMdiSubWindow *widget)
{

}
void MainWindow::displayCharacterSheet(bool display, bool checkAction)
{

    m_characterSheet->setVisible(display);

}


void MainWindow::clickOnMapWizzard()
{

    MapWizzardDialog mapWizzard;
    //QTextStream out(stderr,QIODevice::WriteOnly);
    if(mapWizzard.exec())
    {


        Map* tempmap  = new Map();
        mapWizzard.setAllMap(tempmap);
        MapFrame* tmp = new MapFrame(tempmap);
        m_workspace->addWidget(tmp);
        tmp->show();

    }
}
void MainWindow::openImage()
{

        QString filepath = QFileDialog::getOpenFileName(this, tr("Open Image file"), m_options->value(QString("ImageDirectory"),QVariant(".")).toString(),
                tr("Supported Image formats (*.jpg *.jpeg *.png *.bmp)"));


        Image* tmpImage=new Image(filepath,m_workspace);

        m_workspace->addWidget(tmpImage);
        tmpImage->show();
}



void MainWindow::displayTchat(QString id)
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


void MainWindow::hideTchat(QString id)
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


bool MainWindow::estLaFenetreActive(QWidget *widget)
{
        return widget == m_workspace->activeSubWindow() && widget->isVisible();
}



bool MainWindow::maybeSave()
{



        if(isWindowModified())
        {
            int ret = QMessageBox::warning(this, tr("Rolisteam"),
                                                tr("one or more documents have been modified.\n"
                                                   "Do you want to save your changes?"),
                                                QMessageBox::SaveAll | QMessageBox::Discard
                                                | QMessageBox::Cancel,
                                                QMessageBox::SaveAll);
            switch(ret)
            {
            case QMessageBox::SaveAll:
                saveAll();
            case QMessageBox::Discard:
                return true;
                break;
            default:
                return false;
            }
        }
        else
            return true;

}
void MainWindow::saveAll()
{
/**
 @TODO : save all documents
 */
}
void MainWindow::startServer()
{
    qDebug() << "Start server here";
}

void MainWindow::addConnection()
{
    qDebug() << "add connection here";
    m_connectDialog->addNewConnection();
    m_connectDialog->setVisible(true);
}

void MainWindow::showConnectionManager()
{
    qDebug() << "show connection manager here";
    m_connectDialog->setVisible(true);
}
void MainWindow::showPreferenceManager()
{
    m_preferenceDialog->setVisible(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
          writeSettings();
          event->accept();
          QApplication::exit();

    } else {
          event->ignore();
    }
}


Image *MainWindow::trouverImage(QString idImage)
{
    return NULL;
}


bool MainWindow::enleverImageDeLaListe(QString idImage)
{

    return false;
}


void MainWindow::displayMinutesEditor(bool afficher, bool cocherAction)
{
        // Affichage de l'editeur de notes
        editeurNotes->setVisible(afficher);

        // Si la fonction a pas ete appelee par la listeUtilisateurs ou par l'editeur lui-meme, on coche/decoche l'action associee
        if (cocherAction)
                actionEditeurNotes->setChecked(afficher);

        // Sinon on coche/decoche la case de l'editeur de notes dans la listeUtilisateurs
        else
        {

        }
}





void MainWindow::about()
{
QMessageBox::about(this, tr("About Rolisteam"),
                 tr("<h1>Rolisteam v1.0</h1>"
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

void MainWindow::readSettings()
{
        QSettings settings("RolisteamTeam", "Rolisteam");
        QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
        QSize size = settings.value("size", QSize(600, 400)).toSize();
        resize(size);
        move(pos);
        m_options->readSettings();

}
void MainWindow::writeSettings()
{
      QSettings settings("RolisteamTeam", "Rolisteam");
      settings.setValue("pos", pos());
      settings.setValue("size", size());
      m_options->writeSettings();

}


void MainWindow::aideEnLigne()
{

     QProcess *process = new QProcess;
     QStringList args;
    QString processName="assistant";
       args << QLatin1String("-collectionFile")
#ifdef Q_WS_X11
             << QLatin1String("/usr/share/doc/rolisteam-doc/rolisteam.qhc");
#elif defined Q_WS_WIN32
             << QLatin1String((qApp->applicationDirPath()+"/../resourcesdoc/rolisteam-doc/rolisteam.qhc").toLatin1());
#elif defined Q_WS_MAC
			 << QLatin1String((QCoreApplication::applicationDirPath()+"/../Resources/doc/rolisteam.qhc").toLatin1());
            processName="/Developer/Applications/Qt/Assistant/Contents/MacOS/Assistant";
#endif
            process->start(processName, args);
            if (!process->waitForStarted())
                return;


 }
