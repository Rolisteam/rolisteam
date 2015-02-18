/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *  http://renaudguezennec.homelinux.org/accueil,3.html                    *
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
#include <QMessageBox>

//former (but renamed) headers
#include "MapFrame.h"

#include "MainWindow.h"
#include "ToolBar.h"

#include "Image.h"
#include "audioplayer.h"
#include "MinutesEditor.h"
#include "improvedworkspace.h"

//new headers from rolisteam 2.0.0 branch
#include "preferencedialog.h"
#include "preferencesmanager.h"
#include "connectionwizzard.h"
#include "charactersheetwindow.h"
#include "pluginmanager.h"

//network module
#include "servermanager.h"
#include "serverdialog.h"
#include "updatechecker.h"


//for the new userlist
#include "player.h"

//session
#include "session.h"
#include "sessionmanager.h"



/////////////////
// MainWindow
/////////////////
MainWindow::MainWindow()
        : QMainWindow()
{
    m_options = PreferencesManager::getInstance();
    m_diceManager=DicePlugInManager::instance();
    //m_toolbar = ToolsBar::getInstance(this);//new ToolsBar(this);

    m_preferenceDialog = new PreferenceDialog(this);
    m_rclient= new RClient();

    readSettings();
    m_session = new Session();
    m_sessionManager = new SessionManager();
    m_sessionManager->setCurrentSession(m_session);

    addDockWidget(Qt::RightDockWidgetArea,m_sessionManager);
    m_preferenceDialog->initValues();

    m_workspace = new ImprovedWorkspace(this/*m_toolbar->currentColor()*/);
    m_workspace->setRClient(m_rclient);
    m_workspace->readSettings();

    // all other allocation must be done after the settings reading.

    m_connectDialog = new ConnectionWizzard(this);
    m_subWindowList = new QMap<QAction*,SubMdiWindows*>;
    m_subWindowActGroup = new QActionGroup(this);
    m_recentFilesActGroup= new QActionGroup(this);

    setWindowTitle(tr("Rolisteam - %1").arg(VERSION));
    setCentralWidget(m_workspace);
  // addDockWidget(Qt::LeftDockWidgetArea, m_toolbar);

   /*
    connect(m_toolbar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),m_workspace,SLOT(currentToolChanged(ToolsBar::SelectableTool)));
    connect(m_toolbar,SIGNAL(currentColorChanged(QColor&)),m_workspace,SLOT(currentColorChanged(QColor&)));
    connect(m_toolbar,SIGNAL(currentModeChanged(int)),m_workspace,SIGNAL(currentModeChanged(int)));
    connect(m_toolbar,SIGNAL(currentPenSizeChanged(int)),m_workspace,SLOT(currentPenSizeChanged(int)));
    connect(m_toolbar,SIGNAL(currentPNCSizeChanged(int)),m_workspace,SLOT(currentNPCSizeChanged(int)));
*/

    m_playerListWidget = new UserListWidget;
    m_playerListWidget->setLocalPlayer(m_player);


    connect(m_playerListWidget,SIGNAL(opentchat()),this,SLOT(openTchat()));

    addDockWidget(Qt::RightDockWidgetArea, m_playerListWidget);

    m_audioPlayer = AudioPlayer::getInstance(this);
    addDockWidget(Qt::RightDockWidgetArea, m_audioPlayer);

    // Read connection list form preferences manager
    QVariant tmp2;
    tmp2.setValue(ConnectionList());
    QVariant tmp = m_options->value("network/connectionsList",tmp2);
    m_connectionList = tmp.value<ConnectionList>();



    createMenu();
    connectActions();


}
MainWindow::~MainWindow()
{
    delete m_preferenceDialog;
    delete m_connectDialog;
    delete m_subWindowList;
    //delete m_toolbar;
    delete m_playerListWidget;
    delete m_recentFilesActGroup;
}

void MainWindow::createMenu()
{
    ///////////////
    // File Menu
    ///////////////
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_newMenu = m_fileMenu->addMenu(tr("&New"));
    m_newMapAct = m_newMenu->addAction(tr("&Map"));
    m_newMapAct->setIcon(QIcon(":/resources/icons/map.png"));
    m_noteEditoAct = m_newMenu->addAction(tr("&Note Editor"));
    m_noteEditoAct->setIcon(QIcon(":/resources/icons/notes.png"));
    m_dataSheetAct = m_newMenu->addAction(tr("&CharacterSheet Viewer"));
    m_dataSheetAct->setIcon(QIcon(":/resources/icons/treeview.png"));

    m_newScenarioAct = m_newMenu->addAction(tr("&Scenario"));
    m_newScenarioAct->setIcon(QIcon(":/resources/icons/scenario.png"));

    m_openMenu = m_fileMenu->addMenu(tr("&Open"));
    m_openMapAct= m_openMenu->addAction(tr("&Map"));
    m_openMapAct->setIcon(QIcon(":/resources/icons/map.png"));

    m_openScenarioAct= m_openMenu->addAction(tr("&Scenario"));
    m_openScenarioAct->setIcon(QIcon(":/resources/icons/scenario.png"));

    m_openPictureAct= m_openMenu->addAction(tr("&Picture"));
    m_openPictureAct->setIcon(QIcon(":/resources/icons/image.png"));
    m_openCharacterSheetsAct = m_openMenu->addAction(tr("&CharacterSheet Viewer"));
    m_openCharacterSheetsAct->setIcon(QIcon(":/resources/icons/treeview.png"));

    m_openNoteAct= m_openMenu->addAction(tr("&Note"));
    m_openNoteAct->setIcon(QIcon(":/resources/icons/notes.png"));


    m_recentFilesMenu = m_fileMenu->addMenu(tr("&Recent Files"));
    //m_recentFiles.removeDuplicates();
    /*foreach(CleverURI path,m_recentFiles)
    {

        QAction* act = m_recentFilesActGroup->addAction(path.getUri());
        switch(path.getType())
        {
            case CleverURI::CHARACTERSHEET:
                act->setIcon(QIcon(":/resources/icons/treeview.png"));
                break;
            case CleverURI::MAP:
                act->setIcon(QIcon(":/resources/icons/map.png"));
                break;
            case CleverURI::TEXT:
                    act->setIcon(QIcon(":/resources/icons/notes.png"));
                    break;
            case CleverURI::PICTURE:
                    act->setIcon(QIcon(":/resources/icons/image.png"));
                    break;
        }
        act->setData((int)path.getType());
        m_recentFilesMenu->addAction(act);

    }*/

    m_fileMenu->addSeparator();
    m_saveAct = m_fileMenu->addAction(tr("&Save"));
    m_saveAct->setShortcut(tr("Ctrl+s"));
    m_saveAsAct = m_fileMenu->addAction(tr("Save &As"));
    m_saveAllAct = m_fileMenu->addAction(tr("Save A&ll"));
    m_saveAllIntoScenarioAct= m_fileMenu->addAction(tr("Save &into Scenario"));
    m_fileMenu->addSeparator();
    m_closeAct  = m_fileMenu->addAction(tr("&Close"));
    m_fileMenu->addSeparator();
    m_preferencesAct  = m_fileMenu->addAction(tr("&Preferences"));
    m_preferencesAct->setShortcut(tr("Ctrl+,"));
    m_fileMenu->addSeparator();
    m_quitAct  = m_fileMenu->addAction(tr("&Quit"));
    m_quitAct->setShortcut(tr("Ctrl+q"));

    ///////////////
    // View Menu
    ///////////////
    m_viewMenu = menuBar()->addMenu(tr("&View"));
    m_usedTabBarAct = m_viewMenu->addAction(tr("&Tab Bar Mode"));
    m_usedTabBarAct->setCheckable(true);
    m_usedTabBarAct->setChecked(false);
    m_organizeMenu = m_viewMenu->addMenu(tr("Organize"));
    m_cascadeSubWindowsAct= m_organizeMenu->addAction(tr("&Cascade Windows"));
    m_tileSubWindowsAct= m_organizeMenu->addAction(tr("&Tile Windows"));

    ///////////////
    // Custom Menu
    ///////////////
    m_currentWindowMenu= menuBar()->addMenu(tr("Current Window"));
    m_workspace->setVariantMenu(m_currentWindowMenu);

    //////////////////////////
    //Network Menu and actions
    //////////////////////////
    m_networkMenu = menuBar()->addMenu(tr("&Network"));
    m_serverAct = m_networkMenu->addAction(tr("&Start server..."));

    m_newConnectionAct = m_networkMenu->addAction(tr("&New Connection..."));


    m_connectionActGroup = new QActionGroup(this);
    if(m_connectionList.size() > 0)//(m_connectionList != NULL)&&
    {
        m_connectionMap = new QMap<QAction*,Connection>;
        m_networkMenu->addSeparator();
        foreach(Connection tmp, m_connectionList )
        {
           QAction* p=m_connectionActGroup->addAction(tmp.getName());
           m_networkMenu->addAction(p);
           m_connectionMap->insert(p,tmp);
        }
    }
    connect(m_connectionActGroup,SIGNAL(triggered(QAction*)),this,SLOT(onConnection(QAction*)));
    m_networkMenu->addSeparator();
    m_manageConnectionAct = m_networkMenu->addAction(tr("Manage connections..."));

    ///////////////
    // Help Menu
    ///////////////
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpAct = m_helpMenu->addAction(tr("&Help"));
    m_aproposAct =m_helpMenu->addAction(tr("&About Rolisteam"));

    //Hiding the custom menu because no subwindows displayed.
    menuBar()->removeAction(m_currentWindowMenu->menuAction());
}
void MainWindow::connectActions()
{
    connect(m_newConnectionAct,SIGNAL(triggered()),this,SLOT(addConnection()));
    connect(m_openPictureAct, SIGNAL(triggered(bool)), this, SLOT(askOpenImage()));
    connect(m_newMapAct, SIGNAL(triggered(bool)), this, SLOT(clickOnMapWizzard()));
    connect(m_helpAct, SIGNAL(triggered()), this, SLOT(help()));
    connect(m_aproposAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_quitAct, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(m_preferencesAct,SIGNAL(triggered()),this,SLOT(showPreferenceManager()));

    connect(m_manageConnectionAct,SIGNAL(triggered()),this,SLOT(showConnectionManager()));
    connect(m_serverAct,SIGNAL(triggered()),this,SLOT(startServer()));

    connect(m_dataSheetAct, SIGNAL(triggered()), this, SLOT(addCharacterSheet()));
    connect(m_noteEditoAct, SIGNAL(triggered()), this, SLOT(displayMinutesEditor()));

    connect(m_usedTabBarAct,SIGNAL(toggled(bool)),m_organizeMenu,SLOT(setDisabled(bool)));
    connect(m_usedTabBarAct,SIGNAL(triggered()),this,SLOT(onTabBar()));

    connect(m_subWindowActGroup,SIGNAL(triggered(QAction*)),this,SLOT(hideShowWindow(QAction*)));
    connect(m_recentFilesActGroup,SIGNAL(triggered(QAction*)),this,SLOT(openRecentFile(QAction*)));

    connect( m_cascadeSubWindowsAct,SIGNAL(triggered()),m_workspace,SLOT(cascadeSubWindows()));
    connect( m_tileSubWindowsAct,SIGNAL(triggered()),m_workspace,SLOT(tileSubWindows()));

    connect(m_openCharacterSheetsAct,SIGNAL(triggered()),this,SLOT(AskCharacterSheets()));
    //connect(actionTchatCommun, SIGNAL(triggered(bool)), listeTchat[0], SLOT(setVisible(bool)));
}
void MainWindow::allowActions()
{

}
void MainWindow::openRecentFile(QAction* pathAct)
{

    switch(pathAct->data().toInt())
    {
        case CleverURI::MAP:

            break;
        case CleverURI::CHARACTERSHEET:
            openCharacterSheets(pathAct->text());
            break;
        case CleverURI::PICTURE:
            openImage(pathAct->text());
            break;
        case CleverURI::TCHAT:
        case CleverURI::TEXT:
            break;
    }


}
void MainWindow::addopenedFile(QString& urifile, CleverURI::ContentType type)
{

    /// @todo Manage the list size in the option/preferences system

    m_session->addRessource(urifile,type,m_sessionManager->getCurrentChapter());
    //if(m_recentFiles.indexOf(uri)==-1)//if it's not here, it is added to the list.
     //   m_recentFiles << uri;

}
void MainWindow::AskCharacterSheets()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open Character Sheets"), m_options->value(QString("DataDirectory"),QVariant(".")).toString(),
            tr("Character Sheets files (*.xml)"));
    openCharacterSheets(filepath);
}

void MainWindow::openCharacterSheets(QString filepath)
{
    if(!filepath.isEmpty())
    {
        CharacterSheetWindow* characterSheet = new CharacterSheetWindow();
        addopenedFile(filepath,CleverURI::CHARACTERSHEET);
        characterSheet->openFile(filepath);
        addToWorkspace(characterSheet);
        characterSheet->setVisible(true);
    }
}
void MainWindow::hideShowWindow(QAction* p)
{
    SubMdiWindows* tmp = (*m_subWindowList)[p];
    tmp->setVisible(!tmp->isVisible());
    p->setChecked(tmp->isVisible());
}

void MainWindow::addToWorkspace(SubMdiWindows* subWindow)
{
    if(m_subWindowList->size()==0)
        m_viewMenu->addSeparator();

    m_workspace->addWidget(subWindow);
    QAction* tmp = m_subWindowActGroup->addAction(subWindow->windowTitle());
    tmp->setCheckable(true);
    tmp->setChecked(true);

    m_subWindowList->insert(tmp,subWindow);
    m_viewMenu->addAction(tmp);
}
void MainWindow::openTchat()
{
    Tchat* tchat=new Tchat(this);

    tchat->setClients(m_playerListWidget->getSelectedPerson());
    addToWorkspace(tchat);
    tchat->setVisible(true);
}
void MainWindow::addCharacterSheet()
{
    CharacterSheetWindow* characterSheet = new CharacterSheetWindow();
    addToWorkspace(characterSheet);
    characterSheet->setVisible(true);

}
void MainWindow::clickOnMapWizzard()
{
    MapWizzardDialog mapWizzard;
    if(mapWizzard.exec())
    {
        Map* tempmap  = new Map();
        mapWizzard.setAllMap(tempmap);
        MapFrame* tmp = new MapFrame(tempmap);
        addToWorkspace(tmp);
        tmp->show();
    }
}
void MainWindow::askOpenImage()
{

    QString filepath = QFileDialog::getOpenFileName(this, tr("Open Image file"), m_options->value(QString("ImageDirectory"),QVariant(".")).toString(),
            tr("Supported Image formats (*.jpg *.jpeg *.png *.bmp *.svg)"));

    openImage(filepath);
}
void MainWindow::openImage(QString filepath)
{
    if(!filepath.isEmpty())
    {
        Image* tmpImage=new Image(filepath,m_workspace);
        addopenedFile(filepath,CleverURI::PICTURE);
        addToWorkspace(tmpImage);
        tmpImage->show();
    }
}

void  MainWindow::onTabBar()
{
    if(m_usedTabBarAct->isChecked())
    {
        m_workspace->setViewMode(QMdiArea::TabbedView);
    }
    else
    {
        m_workspace->setViewMode(QMdiArea::SubWindowView);
    }
}

bool MainWindow::isActiveWindow(QWidget *widget)
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
 @todo : save all documents
 */
}
void MainWindow::startServer()
{
    ServerDialog tmpdialog;
    if(tmpdialog.exec())
    {
        tmpdialog.writePrefences();
        /** @todo: add parameters to use password  */
        ServerManager* tmp = new ServerManager(tmpdialog.getPort(),this);
        tmp->start();


         /** @todo: Make the client connection to the server*/
        m_currentConnection.setAddress("localhost");
        m_currentConnection.setName("localhost");
        m_currentConnection.setPort(tmpdialog.getPort());//m_connectionMap->value(p);
        m_rclient = new RClient(this);
        connect(m_rclient,SIGNAL(stateChanged(RClient::State)),this,SLOT(tcpStateConnectionChanged(RClient::State)));
        m_rclient->startConnection(m_currentConnection);
    }


}
void MainWindow::addConnection()
{
    m_connectDialog->addNewConnection();
    m_connectDialog->setVisible(true);
}
void MainWindow::showConnectionManager()
{
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
void MainWindow::displayMinutesEditor()
{
    MinutesEditor* minutesEditor = new MinutesEditor();
    addToWorkspace(minutesEditor);
    minutesEditor->setVisible(true);
}
void MainWindow::checkUpdate()
{
    if(m_options->value("mainwindow/network/checkupdate",true).toBool())
    {
        qDebug() << "checkupdate 2";
        m_updateChecker = new UpdateChecker();
        m_updateChecker->startChecking();
        connect(m_updateChecker,SIGNAL(checkFinished()),this,SLOT(updateMayBeNeeded()));
    }
}
void MainWindow::updateMayBeNeeded()
{
    if(m_updateChecker->mustBeUpdated())
    {
        QMessageBox::information(this,tr("Update Monitor"),tr("The %1 version has been released. Please take a look at <a href=\"http://www.rolisteam.org/download/\">Download</a> for more information").arg(m_updateChecker->getLatestVersion()));
    }
    m_updateChecker->deleteLater();
}
void MainWindow::about()
{
QMessageBox::about(this, tr("About Rolisteam"),
                 tr("<h1>Rolisteam v2.0.0</h1>"
"<p>Rolisteam makes easy the management of any role playing games. It allows players to communicate to each others, share maps or picture. Rolisteam also provides many features for: permission management, sharing background music and dices throw. Rolisteam is written in Qt4. Its dependencies are : Qt4 and Phonon.</p>"
"<p>Rolisteam may contain some files from the FMOD library. This point prevents commercial use of the software.</p> "
"<p>You may modify and redistribute the program under the terms of the GPL (version 2 or later).  A copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam.  Rolisteam is copyrighted by its contributors.  See the 'COPYRIGHT' file for the complete list of contributors.  We provide no warranty for this program.</p>"
"<p><h3>URL:</h3>  <a href=\"http://www.rolisteam.org\">www.rolisteam.org</a></p> "
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
    QSettings settings("rolisteam","rolisteam");
    qRegisterMetaTypeStreamOperators<Player>("Player");

    qRegisterMetaType<CleverURI>("CleverURI");
    qRegisterMetaType<CleverUriList>("CleverUriList");
    qRegisterMetaTypeStreamOperators<CleverURI>("CleverURI");
    qRegisterMetaTypeStreamOperators<CleverUriList>("CleverUriList");



    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(600, 400)).toSize();
    /**
      *  @warning empty avatar uri.
      */
    m_player= new Player(tr("Player Unknown"),QColor(Qt::black),"");
    QVariant variant;
    variant.setValue(*m_player);
    *m_player = settings.value("player", variant).value<Player>();
    resize(size);
    move(pos);




    QVariant tmp3;
    tmp3.setValue(CleverUriList());
    QVariant tmp4= settings.value("session", tmp3);
    //m_recentFiles=tmp4.value<CleverUriList>();




    m_options->readSettings();

    m_diceManager->readSettings();
    m_preferenceDialog->readSettings();
}
void MainWindow::writeSettings()
{
  QSettings settings("rolisteam");
  settings.setValue("pos", pos());
  settings.setValue("size", size());

  QVariant variant;
  variant.setValue(*m_player);
  settings.setValue("player", variant);
  variant.setValue(*m_session);
  settings.setValue("session",variant);



  m_options->writeSettings();
  m_workspace->writeSettings();
  m_diceManager->writeSettings();
  m_preferenceDialog->writeSettings();
}
void MainWindow::tcpStateConnectionChanged(RClient::State s)
{
    switch(s)
    {
        case RClient::DISCONNECTED:
            m_connectionActGroup->setEnabled(true);
        break;
        case RClient::CONNECTED:
            m_connectionActGroup->setEnabled(false);
        break;
        case RClient::ERROR:
            m_connectionActGroup->setEnabled(true);
        break;
    }
}

void MainWindow::onConnection(QAction* p)
{
    if(m_connectionMap->contains(p))
    {
         m_currentConnection = m_connectionMap->value(p);
         m_rclient->startConnection(m_currentConnection);
    }
}

void MainWindow::help()
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
                            tr("Aide"),
                            tr("L'aide de rolisteam se trouve sur le web :<br>\
                                <a href=\"http://wiki.rolisteam.org\">http://wiki.rolisteam.org/</a>"),
                            QMessageBox::Ok
                            );
                    msgBox->exec();
                }
            }


 }
