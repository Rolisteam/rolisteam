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
#include "image.h"
#include "improvedworkspace.h"
#include "map/mapframe.h"
#include "map/map.h"
#include "chat/chatlistwidget.h"
#include "network/clientmanager.h"
#include "charactersheet/charactersheet.h"
#include "data/person.h"
#include "data/player.h"
#include "data/character.h"
#include "data/shortcutvisitor.h"
#include "data/mediacontainer.h"
#include "userlist/playersList.h"
#include "userlist/playersListWidget.h"
#include "preferences/preferencesdialog.h"
#include "services/updatechecker.h"
#include "services/tipchecker.h"
#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"
#include "network/servermanager.h"
#include "widgets/tipofdayviewer.h"
#include "widgets/shortcuteditordialog.h"
#include "widgets/gmtoolbox/gamemastertool.h"
#include "pdfviewer/pdfviewer.h"
#ifdef HAVE_WEBVIEW
#include "webview/webview.h"
#include <QWebEngineSettings>
#endif
//LOG
#include "common/widgets/logpanel.h"

//Undo
#include "undoCmd/addmediacontainer.h"
#include "undoCmd/deletemediacontainercommand.h"

//Text editor
#include "textedit.h"
#include "widgets/aboutrolisteam.h"

//GMToolBox
#include "widgets/gmtoolbox/UnitConvertor/convertor.h"
#include "widgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"
#include "widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.h"

//VMAP
#include "vmap/vmapframe.h"
#include "vmap/vmap.h"
#include "vmap/vmapwizzarddialog.h"

//character sheet
#include "charactersheet/charactersheetwindow.h"
#include "diceparser/qmltypesregister.h"

//session
#include "session/sessionmanager.h"
#ifndef NULL_PLAYER
#include "audio/audioPlayer.h"
#endif

MainWindow::MainWindow()
    : QMainWindow(),
      m_preferencesDialog(nullptr),
      m_clientManager(nullptr),
      m_ui(new Ui::MainWindow),
      m_resetSettings(false),
      m_currentConnectionProfile(nullptr),
      m_profileDefined(false),
      m_currentStory(nullptr),
      m_roomPanelDockWidget(new QDockWidget(this))
{
    setAcceptDrops(true);
    m_profileDefined = false;

    m_ui->setupUi(this);
    m_shownProgress=false;

    registerQmlTypes();
#ifdef HAVE_WEBVIEW
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
#endif

    m_preferences = PreferencesManager::getInstance();

    auto func = [=](QString str){
        auto v = m_preferences->value(str,6).toInt();
        VisualItem::setHighlightWidth(v);
    };
    m_preferences->registerLambda(QStringLiteral("VMAP::highlightPenWidth"),func);
    auto func2 = [=](QString str){
        auto v = m_preferences->value(str,QColor(Qt::red)).value<QColor>();
        VisualItem::setHighlightColor(v);
    };
    m_preferences->registerLambda(QStringLiteral("VMAP::highlightColor"),func2);

    m_downLoadProgressbar = new QProgressBar(this);
    m_downLoadProgressbar->setRange(0,100);

    m_downLoadProgressbar->setVisible(false);
    m_clientManager = nullptr;
    m_vmapToolBar = new VmapToolBar(this);
    addToolBar(Qt::TopToolBarArea,m_vmapToolBar);


    m_ipChecker = new IpChecker(this);
    m_mapAction = new QMap<MediaContainer*,QAction*>();

    m_sessionManager = new SessionManager(this);

    connect(m_sessionManager,SIGNAL(sessionChanged(bool)),this,SLOT(setWindowModified(bool)));
    connect(m_sessionManager,&SessionManager::openResource,this,&MainWindow::openResource);

    /// Create all GM toolbox widget
    m_gmToolBoxList.append(new NameGeneratorWidget(this));
    m_gmToolBoxList.append(new GMTOOL::Convertor(this));
    //m_gmToolBoxList.append(new DiceBookMarkWidget(this));

    for (QWidget* wid : m_gmToolBoxList)
    {
        QDockWidget* widDock = new QDockWidget(this);
        widDock->setAllowedAreas(Qt::AllDockWidgetAreas);
        widDock->setWidget(wid);
        widDock->setWindowTitle(wid->windowTitle());
        widDock->setObjectName(wid->objectName());
        addDockWidget(Qt::RightDockWidgetArea, widDock);

        m_ui->m_gmToolBoxMenu->addAction(widDock->toggleViewAction());
        widDock->setVisible(false);
    }

    //Room List
    m_roomPanel = new ChannelListPanel(this);
    m_roomPanelDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_roomPanelDockWidget->setWidget(m_roomPanel);
    m_roomPanelDockWidget->setWindowTitle(m_roomPanel->windowTitle());
    m_roomPanelDockWidget->setObjectName(m_roomPanel->objectName());
    m_roomPanelDockWidget->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, m_roomPanelDockWidget);

}
MainWindow::~MainWindow()
{
    if(nullptr != m_currentStory)
    {
        delete m_currentStory;
        m_currentStory = nullptr;
    }
}

void MainWindow::setupUi()
{
    // Initialisation de la liste des BipMapWindow, des Image et des Tchat
    m_mediaHash.clear();
    m_version=tr("unknown");
#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    m_version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
#endif
#endif
#endif
    m_dialog = new SelectConnectionProfileDialog(m_version,this);

    //setAnimated(false);
    m_mdiArea = new ImprovedWorkspace(this);
    setCentralWidget(m_mdiArea);
    connect(m_mdiArea, SIGNAL(subWindowActivated ( QMdiSubWindow * )), this, SLOT(activeWindowChanged(QMdiSubWindow *)));

    m_toolBar = new ToolsBar(this);

    m_vToolBar = new VToolsBar(this);
    m_toolBarStack = new QStackedWidget(this);
    m_toolBarStack->setMinimumWidth(10);
    m_toolBarStack->addWidget(m_toolBar);
    m_toolBarStack->addWidget(m_vToolBar);

    QDockWidget* dock = new QDockWidget(this);
    dock->setWidget(m_toolBarStack);
    addDockWidget(Qt::LeftDockWidgetArea,dock);
    dock->setWindowTitle(tr("ToolBox"));
    dock->setObjectName("DockToolBar");
    m_ui->m_menuSubWindows->insertAction(m_ui->m_toolBarAct,dock->toggleViewAction());
    QAction* vmapToolBar  = m_vmapToolBar->toggleViewAction();
    vmapToolBar->setShortcut(Qt::Key_F9);
    m_ui->m_menuSubWindows->insertAction(m_ui->m_toolBarAct,m_vmapToolBar->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_toolBarAct);



    m_chatListWidget = new ChatListWidget(this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::SharePreferencesCategory,m_chatListWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_chatListWidget);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_ui->m_menuSubWindows->insertAction(m_ui->m_chatListAct,m_chatListWidget->toggleViewAction());


    QDockWidget* dock2 = new QDockWidget(this);
    dock2->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    dock2->setWidget(m_sessionManager);
    dock2->setWindowTitle(tr("Resources Explorer"));
    dock2->setObjectName("sessionManager");
    addDockWidget(Qt::RightDockWidgetArea,dock2);
    m_ui->m_menuSubWindows->insertAction(m_ui->m_chatListAct,dock2->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_chatListAct);


    ReceiveEvent::registerNetworkReceiver(NetMsg::MapCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::NPCCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::DrawCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::AdministrationCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::CharacterPlayerCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::MediaCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::SharedNoteCategory,this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::WebPageCategory,this);

    createNotificationZone();
    ///////////////////
    //PlayerList
    ///////////////////
    m_playersListWidget = new PlayersListWidget(this);
    addDockWidget(Qt::RightDockWidgetArea, m_playersListWidget);
    setWindowIcon(QIcon(":/logo.png"));
    m_ui->m_menuSubWindows->insertAction(m_ui->m_characterListAct,m_playersListWidget->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_characterListAct);

    ///////////////////
    //Audio Player
    ///////////////////
#ifndef NULL_PLAYER
    m_audioPlayer = AudioPlayer::getInstance(this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory,m_audioPlayer);
    addDockWidget(Qt::RightDockWidgetArea,m_audioPlayer );
    m_ui->m_menuSubWindows->insertAction(m_ui->m_audioPlayerAct,m_audioPlayer->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_audioPlayerAct);
#endif

    m_preferencesDialog = new PreferencesDialog(this);
    linkActionToMenu();
    if(nullptr!=m_preferencesDialog->getStateModel())
    {
        ReceiveEvent::registerNetworkReceiver(NetMsg::SharePreferencesCategory,m_preferencesDialog->getStateModel());
    }


    // Initialisation des etats de sante des PJ/PNJ (variable declarees dans DessinPerso.cpp)
    m_playerList = PlayersList::instance();

    connect(m_playerList, SIGNAL(playerAdded(Player *)), this, SLOT(notifyAboutAddedPlayer(Player *)));
    connect(m_playerList, SIGNAL(playerDeleted(Player *)), this, SLOT(notifyAboutDeletedPlayer(Player *)));
    connect(m_roomPanel,&ChannelListPanel::CurrentChannelGmIdChanged,m_playerList,&PlayersList::setCurrentGM);
    connect(m_playerList, &PlayersList::characterAdded,this,[=](Character* character){
        if(character->isNpc())
        {
            m_sessionManager->addRessource(character);
        }
    });


    connect(m_dialog,SIGNAL(tryConnection()),this,SLOT(startConnection()));
    connect(m_dialog,&SelectConnectionProfileDialog::rejected,&m_serverThread,&QThread::quit);
    connect(m_ipChecker,&IpChecker::finished,this,[=](QString ip){
        m_connectionAddress = ip;
        if(nullptr!=m_currentConnectionProfile)
        {
            m_logController->manageMessage(tr("Server Ip Address:%1\nPort:%2")
                                           .arg(m_connectionAddress)
                                           .arg(m_currentConnectionProfile->getPort()),
                                           LogController::Features);
        }
    });
}

void MainWindow::addMediaToMdiArea(MediaContainer* mediac,bool redoable)
{
    if(nullptr != m_currentConnectionProfile)
    {
        AddMediaContainer* addMedia = new AddMediaContainer(mediac,m_sessionManager,m_ui->m_menuSubWindows,this,m_mdiArea,m_currentConnectionProfile->isGM());
        if(!m_mediaHash.contains(mediac->getMediaId()))
        {
            m_mediaHash.insert(mediac->getMediaId(),mediac);
        }
        if(redoable)
        {
            m_undoStack.push(addMedia);
        }
        else
        {
            addMedia->redo();
        }
    }
}
void  MainWindow::closeConnection()
{
    if(nullptr!=m_clientManager)
    {
        m_serverThread.quit();
        cleanUpData();
        m_clientManager->disconnectAndClose();
        m_chatListWidget->cleanChatList();
        m_ui->m_connectionAction->setEnabled(true);
        m_ui->m_disconnectAction->setEnabled(false);
    }
}
void MainWindow::closeAllMediaContainer()
{
    for(MediaContainer* tmp : m_mediaHash.values())
    {
        if(nullptr!=tmp)
        {
            closeMediaContainer(tmp->getMediaId());
        }
    }
}
void MainWindow::closeMediaContainer(QString id)
{
    if(m_mediaHash.contains(id))
    {
        MediaContainer* mediaCon = m_mediaHash.value(id);
        if(nullptr!=mediaCon)
        {
            auto type = mediaCon->getContentType();

            DeleteMediaContainerCommand* cmd = new DeleteMediaContainerCommand(mediaCon,m_sessionManager,m_ui->m_editMenu,this,m_mdiArea,m_currentConnectionProfile->isGM(),m_mediaHash);
            m_undoStack.push(cmd);

            //m_mediaHash.remove(id);
            if(CleverURI::VMAP == type)
            {
                m_vmapToolBar->setCurrentMap(nullptr);
            }
            else if(CleverURI::MAP == type)
            {
                m_playersListWidget->model()->changeMap(nullptr);
            }
        }
    }
}
void MainWindow::closeCurrentSubWindow()
{
    QMdiSubWindow* subactive = m_mdiArea->currentSubWindow();
    MediaContainer* container = dynamic_cast<MediaContainer*>(subactive);
    if(nullptr != container)
    {
        closeMediaContainer(container->getMediaId());
    }
}
void MainWindow::checkUpdate()
{
    if(m_preferences->value(QStringLiteral("MainWindow::MustBeChecked"),true).toBool())
    {
        m_updateChecker = new UpdateChecker(this);
        m_updateChecker->startChecking();
        connect(m_updateChecker,&UpdateChecker::checkFinished,this,&MainWindow::updateMayBeNeeded);
    }
}
void MainWindow::tipChecker()
{
    if(!m_preferences->value(QStringLiteral("MainWindow::neverDisplayTips"),false).toBool())
    {
        TipChecker* tipChecker = new TipChecker(this);
        tipChecker->startChecking();
        connect(tipChecker,&TipChecker::checkFinished,this,[=](){
            auto id = m_preferences->value(QStringLiteral("MainWindow::lastTips"),0).toInt();
            if(tipChecker->hasArticle() && tipChecker->getId()+1 > id)
            {
                TipOfDayViewer view(tipChecker->getArticleTitle(),tipChecker->getArticleContent(),tipChecker->getUrl(),this);
                view.exec();
                m_preferences->registerValue(QStringLiteral("MainWindow::lastTips"),tipChecker->getId());
                m_preferences->registerValue(QStringLiteral("MainWindow::neverDisplayTips"),view.dontshowAgain());
            }
            tipChecker->deleteLater();
        });
    }
}

void MainWindow::activeWindowChanged(QMdiSubWindow *subWindow)
{
    if(nullptr!=m_currentConnectionProfile)
    {
        bool localPlayerIsGM = m_currentConnectionProfile->isGM();
        if(nullptr!=subWindow)
        {
            if (subWindow->objectName() == QString("MapFrame") && (localPlayerIsGM))
            {
                m_toolBarStack->setCurrentWidget(m_toolBar);
                m_ui->m_closeAction->setEnabled(true);
                m_ui->m_saveAction->setEnabled(true);
                m_ui->m_saveAsAction->setEnabled(true);
                m_vmapToolBar->setEnabled(false);
                subWindow->setFocus();
            }
            else if(subWindow->objectName() == QString("Image") && (localPlayerIsGM))
            {
                m_playersListWidget->model()->changeMap(nullptr);
                m_ui->m_closeAction->setEnabled(true);
                m_ui->m_saveAction->setEnabled(true);
                m_ui->m_saveAsAction->setEnabled(true);
                m_vmapToolBar->setEnabled(false);
            }
            else if(subWindow->objectName() == QString("VMapFrame") && (localPlayerIsGM))
            {
                m_playersListWidget->model()->changeMap(nullptr);
                m_toolBarStack->setCurrentWidget(m_vToolBar);
                m_vmapToolBar->setEnabled(true);
                m_ui->m_closeAction->setEnabled(true);
                m_ui->m_saveAction->setEnabled(true);
                m_ui->m_saveAsAction->setEnabled(true);

                VMapFrame* frame = dynamic_cast<VMapFrame*>(subWindow);
                m_vmapToolBar->setCurrentMap(frame->getMap());
                m_vToolBar->updateUi(frame->getMap()->getPermissionMode());
            }
            else if(subWindow->objectName() == QString("CharacterSheetViewer") && (localPlayerIsGM))
            {
                m_playersListWidget->model()->changeMap(nullptr);
                m_ui->m_closeAction->setEnabled(true);
                m_ui->m_saveAction->setEnabled(true);
                m_ui->m_saveAsAction->setEnabled(true);
                m_vmapToolBar->setEnabled(false);
            }
        }
        else
        {
            m_ui->m_closeAction->setEnabled(false);
            m_ui->m_saveAction->setEnabled(false);
        }
    }
    else
    {
        m_ui->m_closeAction->setEnabled(false);
        m_ui->m_saveAction->setEnabled(false);
        m_ui->m_saveAsAction->setEnabled(false);
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(mayBeSaved())
    {
        sendGoodBye();
        writeSettings();
        if(m_serverThread.isRunning())
            m_serverThread.quit();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void MainWindow::userNatureChange(bool isGM)
{
    if(nullptr!=m_currentConnectionProfile)
    {
        m_currentConnectionProfile->setGm(isGM);
        updateUi();
        updateWindowTitle();
    }
}
ClientManager* MainWindow::getNetWorkManager()
{
    return m_clientManager;
}
void MainWindow::sendGoodBye()
{
    NetworkMessageWriter message (NetMsg::AdministrationCategory, NetMsg::Goodbye);
    message.sendToServer();
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

void MainWindow::receiveData(quint64 readData,quint64 size)
{
    if(size==0)
    {
        m_downLoadProgressbar->setVisible(false);
        m_shownProgress=false;
    }
    else if(readData!=size)
    {
        m_downLoadProgressbar->setVisible(true);
        int i = static_cast<int>((size-readData)*100/size);

        m_downLoadProgressbar->setValue(i);
        m_shownProgress=true;
    }

}

void MainWindow::createNotificationZone()
{
    m_dockLogUtil = new QDockWidget(tr("Notification Zone"), this);
    m_dockLogUtil->setObjectName("dockLogUtil");
    m_dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_dockLogUtil->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    QWidget* wd = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    wd->setLayout(layout);


    m_notifierDisplay = new LogPanel(m_dockLogUtil);

    layout->addWidget(m_notifierDisplay);
    layout->addWidget(m_downLoadProgressbar);

    m_dockLogUtil->setWidget(wd);
    m_dockLogUtil->setMinimumWidth(80);

    m_ui->m_menuSubWindows->insertAction(m_ui->m_notificationAct,m_dockLogUtil->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_notificationAct);
    addDockWidget(Qt::RightDockWidgetArea, m_dockLogUtil);
}
void MainWindow::createPostSettings()
{

    // Log controller
    auto logDebug = m_preferences->value(QStringLiteral("LogDebug"),false).toBool();
    m_notifierDisplay->initSetting();
    m_logController = new LogController(logDebug,this);
    auto LogResearch = m_preferences->value(QStringLiteral("LogResearch"),false).toBool();
    auto logRoli = m_preferences->value(QStringLiteral("LogRolisteam"),false).toBool();
    m_logController->setSignalInspection(logDebug && (LogResearch || logRoli));
    if((LogResearch || logRoli))
    {
        m_logController->listenObjects(this);
    }
    m_logController->setCurrentModes(LogController::Gui);
    m_logController->setLogLevel(LogController::Info);
    connect(m_logController,&LogController::showMessage,
            m_notifierDisplay,&LogPanel::showMessage);

}

void MainWindow::linkActionToMenu()
{
    // file menu
    const auto& fun = [this](){
        auto act = qobject_cast<QAction*>(sender());
        newDocument(static_cast<CleverURI::ContentType>(act->data().toInt()));
    };
    m_ui->m_newMapAction->setData(static_cast<int>(CleverURI::MAP));
    m_ui->m_addVectorialMap->setData(static_cast<int>(CleverURI::VMAP));
    m_ui->m_newNoteAction->setData(static_cast<int>(CleverURI::TEXT));
    m_ui->m_newSharedNote->setData(static_cast<int>(CleverURI::SHAREDNOTE));
    m_ui->m_newWebViewACt->setData(static_cast<int>(CleverURI::WEBVIEW));


    connect(m_ui->m_newMapAction, &QAction::triggered, this, fun);
    connect(m_ui->m_addVectorialMap, &QAction::triggered, this, fun);
    connect(m_ui->m_newNoteAction, &QAction::triggered, this, fun);
    connect(m_ui->m_newSharedNote, &QAction::triggered, this, fun);
    connect(m_ui->m_newWebViewACt, &QAction::triggered, this, fun);
    connect(m_ui->m_newChatAction, SIGNAL(triggered(bool)), m_chatListWidget, SLOT(createPrivateChat()));

    //open
    connect(m_ui->m_openPictureAction, SIGNAL(triggered(bool)), this, SLOT(openContent()));
    connect(m_ui->m_openOnlinePictureAction, SIGNAL(triggered(bool)), this, SLOT(openContent()));
    connect(m_ui->m_openMapAction, SIGNAL(triggered(bool)), this, SLOT(openContent()));
    connect(m_ui->m_openCharacterSheet,SIGNAL(triggered(bool)),this,SLOT(openContent()));
    connect(m_ui->m_openVectorialMap, SIGNAL(triggered(bool)), this, SLOT(openContent()));
    connect(m_ui->m_openStoryAction, SIGNAL(triggered(bool)), this, SLOT(openStory()));
    connect(m_ui->m_openNoteAction, SIGNAL(triggered(bool)), this, SLOT(openContent()));
    connect(m_ui->m_openShareNote, SIGNAL(triggered(bool)), this, SLOT(openContent()));
    connect(m_ui->m_openPdfAct, SIGNAL(triggered(bool)), this, SLOT(openContent()));

    connect(m_ui->m_shortCutEditorAct, SIGNAL(triggered(bool)),this,SLOT(showShortCutEditor()));

    m_ui->m_openPictureAction->setData(static_cast<int>(CleverURI::PICTURE));
    m_ui->m_openOnlinePictureAction->setData(static_cast<int>(CleverURI::ONLINEPICTURE));
    m_ui->m_openMapAction->setData(static_cast<int>(CleverURI::MAP));
    m_ui->m_openCharacterSheet->setData(static_cast<int>(CleverURI::CHARACTERSHEET));
    m_ui->m_openVectorialMap->setData(static_cast<int>(CleverURI::VMAP));
    m_ui->m_openStoryAction->setData(static_cast<int>(CleverURI::SCENARIO));
    m_ui->m_openNoteAction->setData(static_cast<int>(CleverURI::TEXT));
    m_ui->m_openShareNote->setData(static_cast<int>(CleverURI::SHAREDNOTE));
    #ifdef WITH_PDF
    m_ui->m_openPdfAct->setData(static_cast<int>(CleverURI::PDF));
    #endif
    m_ui->m_recentFileMenu->setVisible(false);
    connect(m_ui->m_closeAction, SIGNAL(triggered(bool)), this, SLOT(closeCurrentSubWindow()));
    connect(m_ui->m_saveAction, SIGNAL(triggered(bool)), this, SLOT(saveCurrentMedia()));
    connect(m_ui->m_saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveCurrentMedia()));
    connect(m_ui->m_saveScenarioAction, &QAction::triggered, this,[=](){
         saveStory(false);
    });
    connect(m_ui->m_saveScenarioAsAction, &QAction::triggered, this,[=](){
         saveStory(true);
    });
    connect(m_ui->m_preferencesAction, SIGNAL(triggered(bool)), m_preferencesDialog, SLOT(show()));

    //Edition
    // Windows managing
    connect(m_ui->m_cascadeViewAction, SIGNAL(triggered(bool)), m_mdiArea, SLOT(cascadeSubWindows()));
    connect(m_ui->m_tabViewAction,SIGNAL(triggered(bool)),m_mdiArea,SLOT(setTabbedMode(bool)));
    connect(m_ui->m_tileViewAction, SIGNAL(triggered(bool)), m_mdiArea, SLOT(tileSubWindows()));

    addAction(m_ui->m_fullScreenAct);
    m_mdiArea->addAction(m_ui->m_fullScreenAct);

    connect(m_ui->m_fullScreenAct,&QAction::triggered,this,[=](bool enable){
        if(enable)
        {
            showFullScreen();
            menuBar()->setVisible(false);
            m_mdiArea->setMouseTracking(true);
            m_vmapToolBar->setMouseTracking(true);
            setMouseTracking(true);
        }
        else
        {
            showNormal();
            menuBar()->setVisible(true);
            m_mdiArea->setMouseTracking(false);
            m_vmapToolBar->setMouseTracking(false);
            setMouseTracking(false);
        }
    });

    auto redo = m_undoStack.createRedoAction(this,tr("&Redo"));
    auto undo = m_undoStack.createUndoAction(this,tr("&Undo"));

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);

    connect(&m_undoStack, &QUndoStack::cleanChanged,this,[this](bool clean){
        setWindowModified(!clean);
    });


    m_ui->m_editMenu->insertAction(m_ui->m_shortCutEditorAct,redo);
    m_ui->m_editMenu->insertAction(redo,undo);
    m_ui->m_editMenu->insertSeparator(m_ui->m_shortCutEditorAct);


    // close
    connect(m_ui->m_quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    // network

    connect(m_ui->m_disconnectAction,SIGNAL(triggered()),this,SLOT(closeConnection()));
    connect(m_ui->m_connectionAction,SIGNAL(triggered()),this,SLOT(startReconnection()));
    connect(m_ui->m_changeProfileAct,SIGNAL(triggered()),this,SLOT(showConnectionDialog()));
    connect(m_ui->m_connectionLinkAct,&QAction::triggered,this,[=](){
        QString str("rolisteam://%1/%2/%3");
        if(m_currentConnectionProfile == nullptr)
            return;
        auto* clipboard = QGuiApplication::clipboard();
        clipboard->setText(str.arg(m_connectionAddress)
                           .arg(m_currentConnectionProfile->getPort())
                           .arg(QString::fromUtf8(m_currentConnectionProfile->getPassword().toBase64())));
    });
    connect(m_ui->m_roomListAct,SIGNAL(triggered(bool)),m_roomPanelDockWidget,SLOT(setVisible(bool)));


    // Help
    connect(m_ui->m_aboutAction,&QAction::triggered, this,[=]()
    {
        AboutRolisteam diag(m_version,this);
        diag.exec();
    });
    connect(m_ui->m_onlineHelpAction, SIGNAL(triggered()), this, SLOT(helpOnLine()));
    connect(m_ui->m_supportRolisteam,&QAction::triggered,[=]{
        if (!QDesktopServices::openUrl(QUrl("https://liberapay.com/Rolisteam/donate")))
        {
            QMessageBox * msgBox = new QMessageBox(
                        QMessageBox::Information,
                        tr("Support"),
                        tr("The %1 donation page can be found online at :<br> <a href=\"https://liberapay.com/Rolisteam/donate\">https://liberapay.com/Rolisteam/donate</a>").arg(m_preferences->value("Application_Name","rolisteam").toString()),
                        QMessageBox::Ok
                        );
            msgBox->exec();
        }
    });
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if(isFullScreen())
    {
        if(event->windowPos().y() == 0.0)
        {
            menuBar()->setVisible(true);
        }
        else if(event->windowPos().y() > 100)
        {
            menuBar()->setVisible(false);
        }
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::prepareMap(MapFrame* mapFrame)
{
    Map* map = mapFrame->getMap();
    if(nullptr==map)
        return;
    map->setPointeur(m_toolBar->getCurrentTool());

    if(nullptr!=m_currentConnectionProfile)
    {
        map->setLocalIsPlayer(!m_currentConnectionProfile->isGM());
    }

    connect(m_toolBar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),map,SLOT(setPointeur(ToolsBar::SelectableTool)));

    connect(m_toolBar,SIGNAL(currentNpcSizeChanged(int)),map,SLOT(setCharacterSize(int)));
    connect(m_toolBar,SIGNAL(currentPenSizeChanged(int)),map,SLOT(setPenSize(int)));
    connect(m_toolBar,SIGNAL(currentTextChanged(QString)),map,SLOT(setCurrentText(QString)));
    connect(m_toolBar,SIGNAL(currentNpcNameChanged(QString)),map,SLOT(setCurrentNpcName(QString)));
    connect(m_toolBar,SIGNAL(currentNpcNumberChanged(int)),map,SLOT(setCurrentNpcNumber(int)));

    connect(map, SIGNAL(changeCurrentColor(QColor)), m_toolBar, SLOT(changeCurrentColor(QColor)));
    connect(map, SIGNAL(increaseNpcNumber()), m_toolBar, SLOT(incrementNpcNumber()));
    connect(map, SIGNAL(updateNPC(int, QString)), m_toolBar, SLOT(updateNpc(int,QString)));

    connect(m_ui->m_showPcNameAction, SIGNAL(triggered(bool)), map, SLOT(setPcNameVisible(bool)));
    connect(m_ui->m_showNpcNameAction, SIGNAL(triggered(bool)), map, SLOT(setNpcNameVisible(bool)));
    connect(m_ui->m_showNpcNumberAction, SIGNAL(triggered(bool)), map, SLOT(setNpcNumberVisible(bool)));

    connect(m_ui->m_showNpcNameAction, SIGNAL(triggered(bool)), map, SLOT(setNpcNameVisible(bool)));
    connect(m_ui->m_showPcNameAction, SIGNAL(triggered(bool)), map, SLOT(setPcNameVisible(bool)));
    connect(m_ui->m_showNpcNumberAction,SIGNAL(triggered(bool)),map,SLOT(setNpcNumberVisible(bool)));

    map->setNpcNameVisible(m_ui->m_showNpcNameAction->isChecked());
    map->setPcNameVisible(m_ui->m_showPcNameAction->isChecked());
    map->setNpcNumberVisible(m_ui->m_showNpcNumberAction->isChecked());
    map->setCurrentNpcNumber(m_toolBar->getCurrentNpcNumber());
    map->setPenSize(m_toolBar->getCurrentPenSize());

    // new PlayersList connection
    connect(mapFrame, SIGNAL(activated(Map *)), m_playersListWidget->model(), SLOT(changeMap(Map *)));
    connect(mapFrame, SIGNAL(activated(Map *)), m_toolBar, SLOT(changeMap(Map *)));
}
void MainWindow::prepareImage(Image* imageFrame)
{
    connect(m_toolBar,SIGNAL(currentToolChanged(ToolsBar::SelectableTool)),imageFrame,SLOT(setCurrentTool(ToolsBar::SelectableTool)));
    imageFrame->setCurrentTool(m_toolBar->getCurrentTool());
}

void MainWindow::updateWorkspace()
{
    QMdiSubWindow* active = m_mdiArea->currentSubWindow();
    if (nullptr!=active)
    {
        activeWindowChanged(active);
    }
}

MediaContainer* MainWindow::newDocument(CleverURI::ContentType type)
{
    MediaContainer* media=nullptr;
    auto uri = new CleverURI(tr("Untitled"),"",type);
    uri->setCurrentMode(CleverURI::Internal);

    auto localIsGM = false;
    if(m_currentConnectionProfile != nullptr)
        localIsGM = m_currentConnectionProfile->isGM();

    switch(type)
    {
        case CleverURI::SHAREDNOTE:
        {
            SharedNoteContainer* note = new SharedNoteContainer(localIsGM);
            media = note;
            note->setOwner(m_playerList->getLocalPlayer());

        }
            break;
        case CleverURI::VMAP:
        {
            MapWizzardDialog mapWizzard(m_mdiArea);
            if(mapWizzard.exec())
            {
                VMap* tempmap = new VMap();
                if(nullptr != tempmap)
                {
                    tempmap->setOption(VisualItem::LocalIsGM,localIsGM);
                }
                mapWizzard.setAllMap(tempmap);
                QString name = tempmap->getMapTitle();
                uri->setName(name);
                VMapFrame* tmp = new VMapFrame(localIsGM,uri,tempmap);
                prepareVMap(tmp);
                media = tmp;
            }
        }
            break;
        case CleverURI::MAP:
        {
            MapFrame* mapFrame = new MapFrame(nullptr, m_mdiArea);
            if(mapFrame->createMap())
            {
                media = mapFrame;
                prepareMap(mapFrame);
                uri->setName(mapFrame->getUriName());

            }
            else
                delete mapFrame;
        }
            break;
        case CleverURI::TEXT:
        {
            media = new NoteContainer(localIsGM);
        }
        break;
        #ifdef HAVE_WEBVIEW
        case CleverURI::WEBVIEW:
        {
            media = new WebView(localIsGM);
            uri->setCurrentMode(CleverURI::Linked);
        }
#endif
            break;
        default:
            break;
    }
    if(nullptr != media)
    {
        media->setCleverUri(uri);
        addMediaToMdiArea(media);
    }
    return media;
}

void MainWindow::sendOffAllMaps(Player* player)
{
    for(auto mediaC : m_mediaHash)
    {
        if(CleverURI::VMAP == mediaC->getContentType())
        {
            //mapi.next();
            VMapFrame* tmp = dynamic_cast<VMapFrame*>(mediaC);
            if(nullptr!=tmp)
            {
                VMap* tempmap = tmp->getMap();
                NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addVmap);
                tempmap->fill(msg);
                tempmap->sendAllItems(msg);
                QStringList idList;
                idList << player->getUuid();
                msg.setRecipientList(idList,NetworkMessage::OneOrMany);
                tmp->fill(msg);
                msg.sendToServer();
            }
        }
        else if(CleverURI::MAP == mediaC->getContentType())
        {
            MapFrame* tmp = dynamic_cast<MapFrame*>(mediaC);
            if(nullptr!=tmp)
            {
                tmp->getMap()->setHasPermissionMode(m_playerList->everyPlayerHasFeature("MapPermission"));
                tmp->getMap()->sendMap(tmp->windowTitle(), player->getUuid());
                tmp->getMap()->sendOffAllCharacters(player->getUuid());
            }
        }
    }
}
void MainWindow::sendOffAllImages(Player* player)
{
    NetworkMessageWriter message = NetworkMessageWriter(NetMsg::MediaCategory, NetMsg::addMedia);
    for(MediaContainer* sub: m_mediaHash.values())
    {
        if(sub->getContentType() == CleverURI::PICTURE)
        {
            message.uint8(sub->getContentType());
            Image* img = dynamic_cast<Image*>(sub);
            if(nullptr!=sub)
            {
                img->fill(message);
                QStringList idList;
                idList << player->getUuid();
                message.setRecipientList(idList,NetworkMessage::OneOrMany);
                message.sendToServer();
            }
        }
    }
}
Map* MainWindow::findMapById(QString idMap)
{
    MediaContainer* media = m_mediaHash.value(idMap);
    if(nullptr!=media)
    {
        MapFrame* mapframe = dynamic_cast<MapFrame*>(media);
        if(nullptr!=mapframe)
        {
            return mapframe->getMap();
        }
    }
    return nullptr;
}
bool MainWindow::mayBeSaved(bool connectionLoss)
{
    QMessageBox msgBox(this);
    QAbstractButton *boutonSauvegarder = msgBox.addButton(QMessageBox::Save);
    QAbstractButton *boutonQuitter = msgBox.addButton(tr("Quit"), QMessageBox::RejectRole);
    Qt::WindowFlags flags = msgBox.windowFlags();
    msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

    QString message;
    QString msg = m_preferences->value("Application_Name","rolisteam").toString();
    if (connectionLoss)
    {
        message = tr("Connection has been lost. %1 will be close").arg(msg);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Connection lost"));
    }
    else
    {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setWindowTitle(tr("Quit %1 ").arg(msg));
    }

    if(nullptr!=PlayersList::instance()->getLocalPlayer())
    {
        if(!PlayersList::instance()->getLocalPlayer()->isGM())
        {
            message += tr("Do you want to save your minutes before to quit %1?").arg(msg);
        }
        else
        {
            message += tr("Do you want to save your scenario before to quit %1?").arg(msg);

        }

        msgBox.setText(message);
        msgBox.exec();
        if (msgBox.clickedButton() == boutonQuitter)
        {
            return true;
        }
        else if (msgBox.clickedButton() == boutonSauvegarder) //saving
        {
            bool ok;
            if (!PlayersList::instance()->getLocalPlayer()->isGM())
                ok = saveMinutes();
            else
                ok = saveStory(false);

            if (ok || connectionLoss)
            {
                return true;
            }
        }
        return false;
    }
    return true;
}
QWidget* MainWindow::registerSubWindow(QWidget * subWindow,QAction* action)
{
    return m_mdiArea->addWindow(subWindow,action);
}

void MainWindow::openStory()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open scenario"), m_preferences->value("SessionDirectory",QDir::homePath()).toString(), tr("Scenarios (*.sce)"));
    readStory(fileName);
}
void MainWindow::readStory(QString fileName)
{
    if (fileName.isNull())
        return;
    QFileInfo info(fileName);
    m_preferences->registerValue("SessionDirectory",info.absolutePath());
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        m_logController->manageMessage("Cannot be read (openStory - MainWindow.cpp)",LogController::Error);
        return;
    }
    m_sessionManager->setSessionName(info.baseName());
    QDataStream in(&file);
    m_sessionManager->loadSession(in);
    file.close();
    m_currentStory = new  CleverURI(getShortNameFromPath(fileName),fileName,CleverURI::SCENARIO);
    updateWindowTitle();
}
QString MainWindow::getShortNameFromPath(QString path)
{
    QFileInfo info(path);
    return info.baseName();
}

bool MainWindow::saveStory(bool saveAs)
{
    if(nullptr == m_currentStory || saveAs)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Scenario as"), m_preferences->value("SessionDirectory",QDir::homePath()).toString(), tr("Scenarios (*.sce)"));
        if (fileName.isNull())
        {
            return false;
        }
        if(!fileName.endsWith(".sce"))
        {
            fileName.append(QStringLiteral(".sce"));
        }
        m_currentStory = new  CleverURI(getShortNameFromPath(fileName),fileName,CleverURI::SCENARIO);
    }
    QFileInfo info(m_currentStory->getUri());

    m_preferences->registerValue("SessionDirectory",info.absolutePath());

    QFile file(m_currentStory->getUri());
    if (!file.open(QIODevice::WriteOnly))
    {
        m_logController->manageMessage(tr("%1 cannot be opened (saveStory - MainWindow.cpp)").arg(m_currentStory->getUri()),LogController::Error);
        return false;
    }

    saveAllMediaContainer();

    QDataStream out(&file);
    m_sessionManager->saveSession(out);
    file.close();
    m_sessionManager->setSessionName(m_currentStory->getData(ResourcesNode::NAME).toString());
    updateWindowTitle();
    m_undoStack.setClean();
    return true;
}
////////////////////////////////////////////////////
// Save data
////////////////////////////////////////////////////
void MainWindow::saveCurrentMedia()
{
    bool saveAs = false;
    if(qobject_cast<QAction*>(sender())==m_ui->m_saveAsAction)
    {
        saveAs=true;
    }
    QMdiSubWindow* active = m_mdiArea->currentSubWindow();
    if(nullptr != active)
    {
        MediaContainer* currentMedia = dynamic_cast<MediaContainer*>(active);
        if(nullptr!=currentMedia)
        {
            auto uri = currentMedia->getCleverUri();
            if(nullptr != uri && !saveAs)
            {
                if(uri->getCurrentMode() == CleverURI::Internal)
                {
                    saveStory(false);
                    return;
                }
            }
            saveMedia(currentMedia,saveAs);
        }
    }
}
void MainWindow::saveMedia(MediaContainer* mediaC, bool saveAs)
{
    if(nullptr!=mediaC)
    {
        CleverURI* cleverURI = mediaC->getCleverUri();
        if(nullptr!=cleverURI)
        {
            QString uri  = cleverURI->getUri();
            if(cleverURI->getCurrentMode() == CleverURI::Linked || saveAs )
            {
                if(uri.isEmpty() || saveAs)
                {
                    QString key = CleverURI::getPreferenceDirectoryKey(cleverURI->getType());
                    QString filter = CleverURI::getFilterForType(cleverURI->getType());
                    QString media = CleverURI::typeToString(cleverURI->getType());
                    QString fileName= QFileDialog::getSaveFileName(this, tr("Save %1").arg(media), m_preferences->value(key,QDir::homePath()).toString(), filter);
                    if(fileName.isEmpty())
                    {
                        return;
                    }
                    QFileInfo info(fileName);
                    m_preferences->registerValue(key,info.absolutePath());
                    cleverURI->setUri(fileName);
                }
                mediaC->saveMedia();
            }
            else if(cleverURI->getCurrentMode() == CleverURI::Internal)
            {
                mediaC->putDataIntoCleverUri();
            }
            setLatestFile(cleverURI);
        }
    }
}

bool MainWindow::saveMinutes()
{
    for(MediaContainer* edit : m_mediaHash.values())
    {
        if(CleverURI::TEXT == edit->getContentType())
        {
            NoteContainer* note = dynamic_cast<NoteContainer*>(edit);
            if(nullptr!=note)
            {
                note->saveMedia();
            }
        }
    }
    return true;
}

void MainWindow::saveAllMediaContainer()
{
    //new fashion
    for(auto media : m_mediaHash.values())
    {
        saveMedia(media,false);
    }
}
void MainWindow::stopReconnection()
{
    m_ui->m_connectionAction->setEnabled(true);
    m_ui->m_disconnectAction->setEnabled(false);
}
void MainWindow::startReconnection()
{
    if(nullptr==m_currentConnectionProfile)
    {
        showConnectionDialog(true);
        return;
    }
    if(!m_currentConnectionProfile->isServer())
    {
        closeAllMediaContainer();
    }
    if(m_clientManager->startConnection())
    {
        m_roomPanel->setServerName(m_currentConnectionProfile->getAddress());
        m_playerList->sendOffLocalPlayerInformations();
        m_playerList->sendOffFeatures(m_currentConnectionProfile->getPlayer());
        m_ui->m_connectionAction->setEnabled(false);
        m_ui->m_disconnectAction->setEnabled(true);
    }
    else
    {
        m_ui->m_connectionAction->setEnabled(true);
        m_ui->m_disconnectAction->setEnabled(false);
    }
}

void MainWindow::setUpNetworkConnection()
{
    if(m_currentConnectionProfile!=nullptr)
    {
        connect(m_playerList, SIGNAL(localGMRefused(bool)), this, SLOT(userNatureChange(bool)));
    }
    connect(m_clientManager, SIGNAL(dataReceived(quint64,quint64)), this, SLOT(receiveData(quint64,quint64)));
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
    if(nullptr==m_currentConnectionProfile)
    {
        return;
    }
    m_toolBar->updateUi(m_currentConnectionProfile->isGM());
#ifndef NULL_PLAYER
    m_audioPlayer->updateUi(m_currentConnectionProfile->isGM());
#endif
    if(nullptr!=m_preferencesDialog)
    {
        m_preferencesDialog->updateUi(m_currentConnectionProfile->isGM());
    }
    if(nullptr!=m_playersListWidget)
    {
        m_playersListWidget->updateUi(m_currentConnectionProfile->isGM());
    }

    bool isGM = m_currentConnectionProfile->isGM();
    m_vToolBar->setGM(isGM);
    m_ui->m_newMapAction->setEnabled(isGM);
    m_ui->m_addVectorialMap->setEnabled(isGM);
    m_ui->m_openMapAction->setEnabled(isGM);
    m_ui->m_openStoryAction->setEnabled(isGM);
    m_ui->m_closeAction->setEnabled(isGM);
    m_ui->m_saveAction->setEnabled(isGM);
    m_ui->m_saveScenarioAction->setEnabled(isGM);
    m_ui->m_connectionLinkAct->setVisible(isGM);
    m_ui->m_saveScenarioAsAction->setEnabled(isGM);
    m_vmapToolBar->setVisible(isGM);
    m_vmapToolBar->toggleViewAction()->setVisible(isGM);

    m_ui->m_connectionAction->setEnabled(false);
    m_ui->m_disconnectAction->setEnabled(true);

    updateRecentFileActions();
}
void MainWindow::updateMayBeNeeded()
{
    if(m_updateChecker->mustBeUpdated())
    {
        QMessageBox::information(this,tr("Update Notification"),
                                 tr("The %1 version has been released. "
                                    "Please take a look at <a href=\"http://www.rolisteam.org/download\">Download page</a> for more information")
                                 .arg(m_updateChecker->getLatestVersion()));
    }
    else
    {
        tipChecker();
    }
    m_updateChecker->deleteLater();
}

void MainWindow::networkStateChanged(ClientManager::ConnectionState state)
{
    switch(state)
    {
    case ClientManager::CONNECTED: /// @brief Action to be done after socket connection.
        m_ui->m_connectionAction->setEnabled(false);
        m_ui->m_disconnectAction->setEnabled(true);
        break;
    case ClientManager::DISCONNECTED:
        m_ui->m_connectionAction->setEnabled(true);
        m_ui->m_disconnectAction->setEnabled(false);
        m_dialog->open();
        break;
    case ClientManager::AUTHENTIFIED:
        m_roomPanel->sendOffLoginAdmin(m_currentConnectionProfile->getPassword());
        m_dialog->accept();
        break;
    case ClientManager::CONNECTING:
        m_chatListWidget->addPublicChat();
        break;
    }
}

void MainWindow::notifyAboutAddedPlayer(Player * player) const
{
    m_logController->manageMessage(tr("%1 just joins the game.").arg(player->name()),LogController::Features);
    if(player->getUserVersion().compare(m_version)!=0)
    {
        m_logController->manageMessage(tr("%1 has not the right version: %2.").arg(player->name())
                   .arg(player->getUserVersion()),LogController::Error);
    }
}

void MainWindow::notifyAboutDeletedPlayer(Player * player) const
{
    m_logController->manageMessage(tr("%1 just leaves the game.").arg(player->name()),LogController::Features);
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

    /**
      * management of recentFileActs
      * */
    m_maxSizeRecentFile = m_preferences->value("recentfilemax",5).toInt();
    for (int i = 0; i < m_maxSizeRecentFile; ++i)
    {
        m_recentFileActs.insert(i,new QAction(m_ui->m_recentFileMenu));
        m_recentFileActs[i]->setVisible(false);
        connect(m_recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));

        m_ui->m_recentFileMenu->addAction(m_recentFileActs[i]);
    }
    updateRecentFileActions();
    m_preferencesDialog->initializePostSettings();
    m_chatListWidget->readSettings(settings);

    createPostSettings();
}
void MainWindow::writeSettings()
{
    QSettings settings("rolisteam",QString("rolisteam_%1/preferences").arg(m_version));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("Maximized", isMaximized());
    m_preferences->writeSettings(settings);
    m_chatListWidget->writeSettings(settings);
    for(auto gmtool : m_gmToolBoxList)
    {
        gmtool->writeSettings();
    }
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
    QCommandLineOption websecurity(QStringList() << "w" <<"disable-web-security", tr("Remove limit to PDF file size"));
    QCommandLineOption translation(QStringList() << "t"<<"translation", QObject::tr("path to the translation file: <translationfile>"),"translationfile");
    QCommandLineOption url(QStringList() << "l"<<"link", QObject::tr("Define URL to connect to server: <url>"),"url");

    parser.addOption(port);
    parser.addOption(hostname);
    parser.addOption(role);
    parser.addOption(reset);
    parser.addOption(user);
    parser.addOption(translation);
    parser.addOption(websecurity);
    parser.addOption(url);

    parser.process(list);

    bool hasPort = parser.isSet(port);
    bool hasHostname = parser.isSet(hostname);
    bool hasRole = parser.isSet(role);
    bool hasUser = parser.isSet(user);
    bool hasUrl = parser.isSet(url);
    m_resetSettings = parser.isSet(reset);

    QString portValue;
    QString hostnameValue;
    QString roleValue;
    QString username;
    QString urlString;
    QString passwordValue;
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
    if(hasUrl)
    {
        urlString = parser.value(url);
        auto list = urlString.split("/",QString::SkipEmptyParts);
        //rolisteam://IP/port/password
        if(list.size() ==  4)
        {
            hostnameValue = list[1];
            portValue = list[2];
            passwordValue = list[3];
            QByteArray pass = QByteArray::fromBase64(passwordValue.toUtf8());
            m_dialog->setArgumentProfile(hostnameValue,portValue.toInt(),pass);
        }
    }

  /*  if(!(roleValue.isNull()&&hostnameValue.isNull()&&portValue.isNull()&&username.isNull()))
    {

    }*/
}
NetWorkReceiver::SendType MainWindow::processMessage(NetworkMessageReader* msg)
{
    if(nullptr==msg)
        return NetWorkReceiver::NONE;

    NetWorkReceiver::SendType type = NetWorkReceiver::NONE;
    switch(msg->category())
    {
    case NetMsg::MapCategory:
        processMapMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    case NetMsg::NPCCategory:
        processNpcMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    case NetMsg::DrawCategory:
        processPaintingMessage(msg);
        type = NetWorkReceiver::ALL;
        break;
    case NetMsg::CharacterCategory:
        processCharacterMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    case NetMsg::AdministrationCategory:
        processAdminstrationMessage(msg);
        type = NetWorkReceiver::NONE;
        break;
    case NetMsg::VMapCategory:
        type = processVMapMessage(msg);
        break;
    case NetMsg::CharacterPlayerCategory:
        processCharacterPlayerMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    case NetMsg::MediaCategory:
        processMediaMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    case NetMsg::SharedNoteCategory:
        processSharedNoteMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    case NetMsg::WebPageCategory:
        processWebPageMessage(msg);
        type = NetWorkReceiver::AllExceptSender;
        break;
    default:
        qWarning("Unexpected message - MainWindow::ProcessMessage");
    }
    return type;//NetWorkReceiver::AllExceptMe;
}
void MainWindow::processMediaMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::addMedia)
    {
        auto type = static_cast<CleverURI::ContentType>(msg->uint8());
        switch(type)
        {
        case CleverURI::MAP:
        {
            MapFrame* mapf = new MapFrame();
            mapf->readMessage(*msg);
            prepareMap(mapf);
            addMediaToMdiArea(mapf,false);
            mapf->setVisible(true);
        }
            break;
        case CleverURI::VMAP:
        {
            VMapFrame* mapFrame = new VMapFrame(false);
            mapFrame->readMessage(*msg);// create the vmap
            prepareVMap(mapFrame);
            addMediaToMdiArea(mapFrame,false);
        }
            break;
        case CleverURI::CHAT:
            break;
        case CleverURI::ONLINEPICTURE:
        case CleverURI::PICTURE:
        {
            Image* image = new Image(m_mdiArea);
            image->readMessage(*msg);
            prepareImage(image);
            addMediaToMdiArea(image,false);
            image->setVisible(true);
        }
            break;
#ifdef HAVE_WEBVIEW

        case CleverURI::WEBVIEW:
        {
            auto webv  =  new WebView(false,m_mdiArea);
            webv->setMediaId(msg->string8());
            webv->readMessage(*msg);
            addMediaToMdiArea(webv,false);
        }
            break;
#endif
#ifdef WITH_PDF
       case CleverURI::PDF:
        {
            auto pdf = new PdfViewer(m_mdiArea);
            pdf->readMessage(*msg);
            addMediaToMdiArea(pdf,false);
            pdf->setVisible(true);
        }
       break;
#endif
        case CleverURI::CHARACTERSHEET:
            break;
        case CleverURI::SHAREDNOTE:
            break;
        case CleverURI::TEXT:
        case CleverURI::SCENARIO:
        case CleverURI::SONG:
        case CleverURI::SONGLIST:
        case NONE:
            break;

        }
    }
    else if(msg->action() == NetMsg::closeMedia)
    {
        closeMediaContainer(msg->string8());
    }
}
void MainWindow::processWebPageMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::UpdateContent)
    {
        QString idMedia = msg->string8();
        if(m_mediaHash.keys().contains(idMedia))
        {
            MediaContainer* mediaContainer = m_mediaHash.value(idMedia);
            WebView* note = dynamic_cast<WebView*>(mediaContainer);
            note->readMessage(*msg);
        }
    }
}
void MainWindow::processSharedNoteMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::updatePermissionOneUser)
    {
        QString idMedia = msg->string8();
        if(m_mediaHash.keys().contains(idMedia))
        {
            MediaContainer* mediaContainer = m_mediaHash.value(idMedia);
            SharedNoteContainer* note = dynamic_cast<SharedNoteContainer*>(mediaContainer);
            note->readMessage(*msg);
        }
    }
    else if(msg->action() == NetMsg::updateText)
    {
        QString idMedia = msg->string8();
        if(m_mediaHash.keys().contains(idMedia))
        {
            MediaContainer* mediaContainer = m_mediaHash.value(idMedia);
            SharedNoteContainer* note = dynamic_cast<SharedNoteContainer*>(mediaContainer);
            QString updateCmd = msg->string32();
            note->runUpdateCmd(updateCmd);
        }
    }
    else if(msg->action() == NetMsg::updateTextAndPermission)
    {
        QString idMedia = msg->string8();

        if(m_mediaHash.keys().contains(idMedia))
        {
            MediaContainer* mediaContainer = m_mediaHash.value(idMedia);
            SharedNoteContainer* note = dynamic_cast<SharedNoteContainer*>(mediaContainer);
            note->readMessage(*msg);
        }
        else
        {
            SharedNoteContainer* note = new SharedNoteContainer(false);
            note->readMessage(*msg);
            note->setMediaId(idMedia);
            m_sessionManager->addRessource(note->getCleverUri());
            addMediaToMdiArea(note);
        }
    }
}

void MainWindow::processAdminstrationMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::EndConnectionAction)
    {
        m_logController->manageMessage(tr("End of the connection process"),LogController::Info);
        updateWorkspace();
    }
    else if((msg->action() == NetMsg::SetChannelList) || (NetMsg::AdminAuthFail == msg->action()) ||  (NetMsg::AdminAuthSucessed == msg->action()))
    {
        ChannelListPanel* roomPanel = qobject_cast<ChannelListPanel*>(m_roomPanelDockWidget->widget());
        if(nullptr != roomPanel)
        {
            roomPanel->processMessage(msg);
        }
    }
    else if(NetMsg::AuthentificationFail == msg->action())
    {
        m_dialog->errorOccurs(tr("Error: Wrong password!"));
        closeConnection();
    }

}

void  MainWindow::showConnectionDialog(bool forced)
{
    if((!m_profileDefined)||(forced))
    {
        m_dialog->open();
    }
}

void MainWindow::startConnection()
{
    m_chatListWidget->cleanChatList();
    QSettings settings("rolisteam","rolisteam");
    if(nullptr!=m_dialog)
    {
        m_dialog->writeSettings(settings);
        m_currentConnectionProfile = m_dialog->getSelectedProfile();

        if(nullptr != m_currentConnectionProfile)
        {
            if(m_currentConnectionProfile->isServer())
            {
                if(!m_serverThread.isRunning() || m_server == nullptr)
                {
                    m_server = new ServerManager();
                    m_server->initServerManager();
                    connect(&m_serverThread,SIGNAL(started()),m_server,SLOT(startListening()));
                    connect(&m_serverThread,SIGNAL(finished()),m_server,SLOT(deleteLater()));
                    connect(m_server,&ServerManager::sendLog,m_logController,&LogController::manageMessage);
                    connect(m_server,&ServerManager::sendLog,this,[=](QString str, LogController::LogLevel level){
                        if(LogController::Error == level)
                            m_dialog->errorOccurs(str);
                    });
                    connect(m_server,SIGNAL(listening()),this,SLOT(initializedClientManager()),Qt::QueuedConnection);
                    m_server->moveToThread(&m_serverThread);
                }
                m_server->insertField("port",m_currentConnectionProfile->getPort());
                m_server->insertField("ThreadCount",1);
                m_server->insertField("ChannelCount",1);
                m_server->insertField("LogLevel",3);
                m_server->insertField("ServerPassword",m_currentConnectionProfile->getPassword());
                m_server->insertField("TimeToRetry",5000);
                m_server->insertField("AdminPassword",m_currentConnectionProfile->getPassword());
                m_serverThread.start();
            }
            else
            {
                initializedClientManager();
            }
        }

    }
}
void MainWindow::initializedClientManager()
{
    if(nullptr == m_currentConnectionProfile)
        return;

    if(nullptr == m_currentConnectionProfile->getPlayer())
        return;

    m_localPlayerId = m_currentConnectionProfile->getPlayer()->getUuid();

    if(nullptr == m_clientManager)
    {
        m_clientManager = new ClientManager(m_currentConnectionProfile);

        connect(m_clientManager,&ClientManager::notifyUser,this,[=](QString str){
            m_logController->manageMessage(str,LogController::Features);
        });
        connect(m_clientManager,SIGNAL(stopConnectionTry()),this,SLOT(stopReconnection()));
        connect(m_clientManager,&ClientManager::errorOccur,m_dialog,&SelectConnectionProfileDialog::errorOccurs);
        connect(m_clientManager,SIGNAL(connectionStateChanged(ClientManager::ConnectionState)),this,SLOT(updateWindowTitle()));
        connect(m_clientManager,SIGNAL(connectionStateChanged(ClientManager::ConnectionState)),this,SLOT(networkStateChanged(ClientManager::ConnectionState)));
        connect(m_clientManager,SIGNAL(isAuthentified()),this,SLOT(postConnection()));
        connect(m_clientManager,SIGNAL(clearData()),this,SLOT(cleanUpData()));
        connect(m_clientManager,&ClientManager::gameMasterStatusChanged,this,&MainWindow::userNatureChange);
        //connect(m_clientManager,&ClientManager::moveToAnotherChannel,this,&MainWindow::);

    }
    else
    {
        m_clientManager->setConnectionProfile(m_currentConnectionProfile);
        m_clientManager->reset();
    }
    if((nullptr!=m_currentConnectionProfile)&&(nullptr!=m_clientManager))
    {
        if(m_currentConnectionProfile->isServer())
        {
            m_ipChecker->startCheck();
        }
        if(nullptr!=m_playerList)
        {
            m_playerList->completeListClean();
            m_playerList->setLocalPlayer(m_currentConnectionProfile->getPlayer());
        }
    }
}
void MainWindow::cleanUpData()
{
    m_playerList->cleanListButLocal();
    closeAllMediaContainer();
    m_undoStack.clear();
    ChannelListPanel* roomPanel = qobject_cast<ChannelListPanel*>(m_roomPanelDockWidget->widget());
    if(nullptr != roomPanel)
    {
        roomPanel->cleanUp();
    }
    if(nullptr !=m_dialog)
        m_dialog->disconnection();
}

void MainWindow::postConnection()
{
    if(m_currentConnectionProfile == nullptr)
    {
        return;
    }

    PlayersList*  playerList = PlayersList::instance();
    if(!m_currentConnectionProfile->isGM())
    {
        playerList->addLocalCharacter(m_currentConnectionProfile->getCharacter());
    }
    playerList->sendOffLocalPlayerInformations();
    playerList->sendOffFeatures(m_currentConnectionProfile->getPlayer());

    m_roomPanel->setLocalPlayerId(m_localPlayerId);

    if(nullptr!=m_preferences)
    {
        m_preferences->registerValue("isClient",true);
    }

    m_ui->m_connectionAction->setEnabled(false);
    m_ui->m_disconnectAction->setEnabled(true);
    m_dialog->accept();

    setUpNetworkConnection();
    updateWindowTitle();
    checkUpdate();
    updateUi();


    if(m_currentConnectionProfile->isGM())
    {
        m_preferencesDialog->sendOffAllDiceAlias();
        m_preferencesDialog->sendOffAllState();
    }
}


void MainWindow::processMapMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::CloseMap)
    {
        QString idMap = msg->string8();
        closeMediaContainer(idMap);
    }
    else
    {
        MapFrame* mapFrame = new MapFrame(nullptr, m_mdiArea);
        if((nullptr!=m_currentConnectionProfile)&&(!mapFrame->processMapMessage(msg,!m_currentConnectionProfile->isGM())))
        {
            delete mapFrame;
        }
        else
        {
            prepareMap(mapFrame);
            addMediaToMdiArea(mapFrame);
            mapFrame->setVisible(true);
        }
    }
}
void MainWindow::processNpcMessage(NetworkMessageReader* msg)
{
    QString idMap = msg->string8();
    if(msg->action() == NetMsg::addNpc)
    {
        Map* map = findMapById(idMap);
        extractCharacter(map,msg);
    }
    else if(msg->action() == NetMsg::delNpc)
    {
        QString idNpc = msg->string8();
        Map* map = findMapById(idMap);
        if(nullptr!=map)
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
        for (quint32 i=0; i<pointNumber; i++)
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

        Map* map = findMapById(idMap);

        if(nullptr!=map)
        {
            SelectedColor selectedColor;
            selectedColor.color = color;
            selectedColor.type = static_cast<ColorKind>(colorType);
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

        Map* map = findMapById(idMap);

        if(nullptr!=map)
        {
            SelectedColor selectedColor;
            selectedColor.color = color;
            selectedColor.type = static_cast<ColorKind>(colorType);

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
        Map* map = findMapById(idMap);

        if(nullptr!=map)
        {
            SelectedColor selectedColor;
            selectedColor.color = color;
            selectedColor.type = static_cast<ColorKind>(colorType);

            map->paintOther(msg->action(),startPos,endPos,zoneToRefresh,diameter,selectedColor);
        }


    }
}
void MainWindow::extractCharacter(Map* map,NetworkMessageReader* msg)
{
    if(nullptr!=map)
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

        bool showNumber=(npcType == CharacterToken::pnj)?m_ui->m_showNpcNumberAction->isChecked():false;
        bool showName=(npcType == CharacterToken::pnj)? m_ui->m_showNpcNameAction->isChecked():m_ui->m_showPcNameAction->isChecked();

        CharacterToken* npc = new CharacterToken(map, npcId, npcName, npcColor, npcDiameter, npcPos, static_cast<CharacterToken::typePersonnage>(npcType),showNumber,showName, npcNumber);

        if((npcVisible)||(npcType == CharacterToken::pnj && (nullptr!=m_currentConnectionProfile) && m_currentConnectionProfile->isGM()))
        {
            npc->showCharacter();
        }
        npc->newOrientation(orientation);
        if(npcVisibleOrient)
        {
            npc->showOrHideOrientation();
        }

        CharacterToken::StateOfHealth health;
        health.stateColor = npcState;
        health.stateName = npcStateName;
        npc->newHealtState(health, npcStateNum);
        map->showHideNPC(npc);

    }
}

void MainWindow::processCharacterMessage(NetworkMessageReader* msg)
{
    if(NetMsg::addCharacterList == msg->action())
    {
        QString idMap = msg->string8();
        quint16 characterNumber = msg->uint16();
        Map* map = findMapById(idMap);

        if(nullptr!=map)
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
        for (quint32 i=0; i<pointNumber; i++)
        {
            quint16 posX = msg->uint16();
            quint16 posY = msg->uint16();
            point = QPoint(posX, posY);
            moveList.append(point);
        }
        Map* map=findMapById(idMap);
        if(nullptr!=map)
        {
            map->startCharacterMove(idCharacter,moveList);
        }
    }
    else if(NetMsg::changeCharacterState == msg->action())
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint16 stateNumber = msg->uint16();
        Map* map=findMapById(idMap);
        if(nullptr!=map)
        {
            CharacterToken* character = map->findCharacter(idCharacter);
            if(nullptr!=character)
            {
                character->changeHealtStatus(stateNumber);
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
        Map* map=findMapById(idMap);
        if(nullptr!=map)
        {
            CharacterToken* character = map->findCharacter(idCharacter);
            if(nullptr!=character)
            {
                character->newOrientation(orientation);
            }
        }
    }
    else if(NetMsg::showCharecterOrientation == msg->action())
    {
        QString idMap = msg->string8();
        QString idCharacter = msg->string8();
        quint8 showOrientation = msg->uint8();
        Map* map=findMapById(idMap);
        if(nullptr!=map)
        {
            CharacterToken* character = map->findCharacter(idCharacter);
            if(nullptr!=character)
            {
                character->showOrientation(showOrientation);
            }
        }
    }
    else if(NetMsg::addCharacterSheet == msg->action())
    {
        CharacterSheetWindow* sheetWindow = new CharacterSheetWindow();
        prepareCharacterSheetWindow(sheetWindow);
        QString idmedia = msg->string8();
        sheetWindow->setMediaId(idmedia);
        sheetWindow->readMessage(*msg);
        addMediaToMdiArea(sheetWindow, false);
    }
    else if(NetMsg::updateFieldCharacterSheet == msg->action())
    {
        QString idCharacterSheetW = msg->string8();
        CharacterSheetWindow* sheet = findCharacterSheetWindowById(idCharacterSheetW);
        if(nullptr!=sheet)
        {
            sheet->processUpdateFieldMessage(msg);
        }

    }
}
CharacterSheetWindow*  MainWindow::findCharacterSheetWindowById(QString id)
{
    if(m_mediaHash.contains(id))
    {
        MediaContainer* media = m_mediaHash.value(id);
        return dynamic_cast<CharacterSheetWindow*>(media);
    }
    else
    {
        return nullptr;
    }
}

void MainWindow::processCharacterPlayerMessage(NetworkMessageReader* msg)
{
    QString idMap = msg->string8();
    QString idCharacter = msg->string8();
    Map* map=findMapById(idMap);
    if(nullptr == map)
        return;

    quint8 param = msg->uint8();
    if(msg->action() == NetMsg::ToggleViewPlayerCharacterAction)
    {
        map->showPc(idCharacter,param);
    }
    else if(msg->action() == NetMsg::ChangePlayerCharacterSizeAction)
    {
        map->selectCharacter(idCharacter);
        map->changePcSize(param + 11);
    }
}
void MainWindow::prepareVMap(VMapFrame* tmp)
{
    if(nullptr==tmp)
        return;

    VMap* map = tmp->getMap();

    if(nullptr==map)
        return;
    if(nullptr!=m_currentConnectionProfile)
    {
        map->setOption(VisualItem::LocalIsGM,m_currentConnectionProfile->isGM());
    }
    map->setLocalId(m_localPlayerId);
    tmp->setUndoStack(&m_undoStack);

    //Toolbar to Map
    connect(m_vToolBar,SIGNAL(currentToolChanged(VToolsBar::SelectableTool)),tmp,SLOT(currentToolChanged(VToolsBar::SelectableTool)));
    connect(tmp,SIGNAL(defineCurrentTool(VToolsBar::SelectableTool)),m_vToolBar,SLOT(setCurrentTool(VToolsBar::SelectableTool)));
    connect(map,SIGNAL(colorPipette(QColor)),m_vToolBar,SLOT(setCurrentColor(QColor)));
    connect(m_vToolBar,SIGNAL(currentColorChanged(QColor&)),tmp,SLOT(currentColorChanged(QColor&)));
    connect(m_vToolBar,SIGNAL(currentModeChanged(int)),tmp,SLOT(setEditingMode(int)));
    connect(m_vToolBar,SIGNAL(currentPenSizeChanged(int)),tmp,SLOT(currentPenSizeChanged(int)));
    connect(m_vToolBar,SIGNAL(currentNpcNameChanged(QString)),tmp,SLOT(setCurrentNpcNameChanged(QString)));
    connect(m_vToolBar,SIGNAL(currentNpcNumberChanged(int)),tmp,SLOT(setCurrentNpcNumberChanged(int)));
    connect(m_vToolBar,SIGNAL(opacityChanged(qreal)),map,SLOT(setCurrentItemOpacity(qreal)));
    connect(map,SIGNAL(currentItemOpacity(qreal)),m_vToolBar,SLOT(setCurrentOpacity(qreal)));
    connect(m_vToolBar,SIGNAL(currentEditionModeChanged(VToolsBar::EditionMode)),map,SLOT(setEditionMode(VToolsBar::EditionMode)));

    //map to toolbar
    connect(map,SIGNAL(npcAdded()),m_vToolBar,SLOT(increaseNpcNumber()));

    // menu to Map
    connect(m_ui->m_showNpcNameAction, &QAction::triggered,[=](bool b){
        map->setOption(VisualItem::ShowNpcName,b);
    });
    connect(m_ui->m_showNpcNumberAction, &QAction::triggered,[=](bool b){
        map->setOption(VisualItem::ShowNpcNumber,b);
    });
    connect(m_ui->m_showPcNameAction, &QAction::triggered,[=](bool b){
        map->setOption(VisualItem::ShowPcName,b);
    });
    connect(m_ui->m_showHealtStatusAction, &QAction::triggered,[=](bool b){
        map->setOption(VisualItem::ShowHealthStatus,b);
    });

    connect(m_ui->m_healthBarAct,&QAction::triggered,[=](bool b){
        map->setOption(VisualItem::ShowHealthBar,b);
    });
    map->setOption(VisualItem::ShowNpcName,m_ui->m_showNpcNameAction->isChecked());
    map->setOption(VisualItem::ShowNpcNumber,m_ui->m_showNpcNumberAction->isChecked());
    map->setOption(VisualItem::ShowPcName,m_ui->m_showPcNameAction->isChecked());
    map->setOption(VisualItem::ShowHealthStatus,m_ui->m_showHealtStatusAction->isChecked());
    map->setCurrentNpcNumber(m_toolBar->getCurrentNpcNumber());
    tmp->currentPenSizeChanged(m_vToolBar->getCurrentPenSize());

    m_vToolBar->setCurrentTool(VToolsBar::HANDLER);
    tmp->currentToolChanged(m_vToolBar->getCurrentTool());

}
NetWorkReceiver::SendType MainWindow::processVMapMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type = NetWorkReceiver::NONE;
    switch(msg->action())
    {
    case NetMsg::loadVmap:
        break;
    case NetMsg::closeVmap:
    {
        QString vmapId = msg->string8();
        closeMediaContainer(vmapId);
    }
        break;
    case NetMsg::addVmap:
    case NetMsg::DelPoint:
        break;
    case NetMsg::AddItem:
    case NetMsg::DelItem:
    case NetMsg::MoveItem:
    case NetMsg::GeometryItemChanged:
    case NetMsg::OpacityItemChanged:
    case NetMsg::LayerItemChanged:
    case NetMsg::MovePoint:
    case NetMsg::vmapChanges:
    case NetMsg::GeometryViewChanged:
    case NetMsg::SetParentItem:
    case NetMsg::RectGeometryItem:
    case NetMsg::RotationItem:
    case NetMsg::CharacterStateChanged:
    case NetMsg::CharacterChanged:
    case NetMsg::VisionChanged:
    case NetMsg::ColorChanged:
    case NetMsg::ZValueItem:
    {
        QString vmapId = msg->string8();
        MediaContainer* tmp = m_mediaHash.value(vmapId);
        if(nullptr!=tmp)
        {
            VMapFrame* mapF = dynamic_cast<VMapFrame*>(tmp);
            if(nullptr!=mapF)
            {
                type = mapF->processMessage(msg);
            }
        }
    }
        break;
    default:
        qWarning("Unexpected Action - MainWindow::processVMapMessage");
        break;
    }

    return type;
}
CleverURI* MainWindow::contentToPath(CleverURI::ContentType type,bool save)
{
    QString filter = CleverURI::getFilterForType(type);
    QString folder;
    QString title;
    switch(type)
    {
    case CleverURI::PICTURE:
        title = tr("Open Picture");
        folder = m_preferences->value(QString("PicturesDirectory"),".").toString();
        break;
    case CleverURI::MAP:
        title = tr("Open Map");
        folder = m_preferences->value(QString("MapsDirectory"),".").toString();
        break;
    case CleverURI::TEXT:
        title = tr("Open Minutes");
        folder = m_preferences->value(QString("MinutesDirectory"),".").toString();
        break;
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

        return new CleverURI(getShortNameFromPath(filepath),filepath,type);
    }
    return nullptr;
}
void MainWindow::openContent()
{
    QAction* action=static_cast<QAction*>(sender());
    CleverURI::ContentType type = static_cast<CleverURI::ContentType>(action->data().toInt());
    openContentFromType(type);
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        CleverURI* uri = new CleverURI();
        *uri = action->data().value<CleverURI>();
        openCleverURI(uri,true);
    }
}

void MainWindow::updateRecentFileActions()
{
    QVariant var = QVariant::fromValue(CleverUriList());
    CleverUriList files = m_preferences->value("recentFileList",var).value<CleverUriList >();
    int numRecentFiles = qMin(files.size(), m_maxSizeRecentFile);
    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = QStringLiteral("&%1 %2").arg(i + 1).arg(files[i].name());
        m_recentFileActs[i]->setText(text);
        QVariant var;
        var.setValue(files[i]);
        m_recentFileActs[i]->setData(var);
        m_recentFileActs[i]->setIcon(QIcon(files[i].getIcon()));
        m_recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < m_recentFileActs.size() ; ++j)
    {
        m_recentFileActs[j]->setVisible(false);
    }
    m_ui->m_recentFileMenu->setEnabled(numRecentFiles > 0);
}

void MainWindow::setLatestFile(CleverURI* fileName)
{
    // no online picture because they are handled in a really different way.
    if((nullptr==fileName) ||
       (fileName->getType()==CleverURI::ONLINEPICTURE) ||
       (fileName->getCurrentMode() == CleverURI::Internal))
    {
        return;
    }
    QVariant var = QVariant::fromValue(CleverUriList());
    CleverUriList files = m_preferences->value("recentFileList",var).value<CleverUriList>();
    files.removeAll(*fileName);
    files.prepend(*fileName);
    while (files.size() > m_maxSizeRecentFile)
    {
        files.removeLast();
    }
    QVariant var3;
    var3.setValue(files);
    m_preferences->registerValue("recentFileList", var3,true);
    updateRecentFileActions();
}
void MainWindow::prepareCharacterSheetWindow(CharacterSheetWindow* window)
{
    if(nullptr!=m_currentConnectionProfile)
    {
        window->setLocalIsGM(m_currentConnectionProfile->isGM());
    }
    connect(window,SIGNAL(addWidgetToMdiArea(QWidget*,QString )),m_mdiArea,SLOT(addWidgetToMdi(QWidget*,QString)));
    connect(window,SIGNAL(rollDiceCmd(QString,QString,bool)),m_chatListWidget,SLOT(rollDiceCmd(QString,QString,bool)));
    connect(window,&CharacterSheetWindow::errorOccurs,this,[=](QString msg){
      m_logController->manageMessage(msg,LogController::Error);
    });
    connect(m_playerList,SIGNAL(playerDeleted(Player*)),window,SLOT(removeConnection(Player*)));
}

void MainWindow::openResource(ResourcesNode* node, bool force)
{
    if(node->getResourcesType() == ResourcesNode::Cleveruri)
    {
        openCleverURI(dynamic_cast<CleverURI*>(node),force);
    }
    else if(node->getResourcesType() == ResourcesNode::Person)
    {
        m_playerList->addLocalCharacter(dynamic_cast<Character*>(node));
    }
}

void MainWindow::openCleverURI(CleverURI* uri,bool force)
{
    if(nullptr==uri)
    {
        return;
    }
    if((uri->getState() == CleverURI::Hidden)&&(!force))
    {
        if(m_mdiArea->showCleverUri(uri))
            return;
    }
    else if(uri->getState() == CleverURI::Displayed)
        return;

    auto localIsGM = false;
    if(m_currentConnectionProfile != nullptr)
        localIsGM = m_currentConnectionProfile->isGM();

    MediaContainer* tmp=nullptr;
    switch(uri->getType())
    {
    case CleverURI::MAP:
        tmp = new MapFrame();
        break;
    case CleverURI::VMAP:
    {
        VMapFrame* mapFrame = new VMapFrame(localIsGM);
        VMap* map = mapFrame->getMap();
        if((nullptr != map)&&(nullptr != m_currentConnectionProfile))
        {
            map->setOption(VisualItem::LocalIsGM,m_currentConnectionProfile->isGM());
        }
        tmp =mapFrame;
    }
        break;
    case CleverURI::PICTURE:
    case CleverURI::ONLINEPICTURE:
        tmp = new Image(m_mdiArea);
        break;
    case CleverURI::TEXT:
        tmp = new NoteContainer(localIsGM);
        break;
#ifdef WITH_PDF
    case CleverURI::PDF:
    {
        auto pdfV = new PdfViewer();
        connect(pdfV,&PdfViewer::openImageAs,this,&MainWindow::openImageAs);
        tmp = pdfV;
    }
        break;
#endif
    case CleverURI::SHAREDNOTE:
    {
        SharedNoteContainer* tmpShared = new SharedNoteContainer(localIsGM);
        tmpShared->setOwner(m_playerList->getLocalPlayer());
        tmp = tmpShared;
    }
        break;
    case CleverURI::WEBVIEW:
    {
        WebView* tmpWeb = new WebView(localIsGM);
        tmp = tmpWeb;
    }
        break;
    case CleverURI::SCENARIO:
        readStory(uri->getUri());
        break;
    case CleverURI::CHARACTERSHEET:
    {
        CharacterSheetWindow* csW = new CharacterSheetWindow();
        prepareCharacterSheetWindow(csW);
        tmp = csW;
    }
        break;
    case CleverURI::SONGLIST:
    {
#ifndef nullptr_PLAYER
        m_audioPlayer->openSongList(uri->getUri());
#endif
    }
        break;
    case CleverURI::SONG:
    {
#ifndef nullptr_PLAYER
        m_audioPlayer->openSong(uri->getUri());
#endif
    }
        break;
    default:
        break;
    }
    if(tmp!=nullptr)
    {
        tmp->setLocalPlayerId(m_localPlayerId);
        tmp->setCleverUri(uri);
        if(tmp->readFileFromUri())
        {
            if(uri->getType()==CleverURI::MAP)
            {
                prepareMap(static_cast<MapFrame*>(tmp));
            }
            else if(uri->getType()==CleverURI::VMAP)
            {
                prepareVMap(static_cast<VMapFrame*>(tmp));
            }
            else if((uri->getType()==CleverURI::PICTURE)||((uri->getType()==CleverURI::ONLINEPICTURE)))
            {
                prepareImage(static_cast<Image*>(tmp));
            }
            addMediaToMdiArea(tmp);
        }
        else
        {
            uri->setDisplayed(false);
        }
    }
}
void MainWindow::openContentFromType(CleverURI::ContentType type)
{
    QString filter = CleverURI::getFilterForType(type);
    if(!filter.isEmpty())
    {
        QString folder = m_preferences->value(CleverURI::getPreferenceDirectoryKey(type),".").toString();
        QString title = tr("Open %1").arg(CleverURI::typeToString(type));
        QStringList filepath = QFileDialog::getOpenFileNames(this,title,folder,filter);
        QStringList list = filepath;
        QStringList::Iterator it = list.begin();
        while(it != list.end())
        {
            openCleverURI(new CleverURI(getShortNameFromPath(*it),*it,type));
            ++it;
        }
    }
    else
    {
        MediaContainer* tmp=nullptr;
        switch(type)
        {
        case CleverURI::MAP:
            tmp = new MapFrame();
            break;
        case CleverURI::PICTURE:
        case CleverURI::ONLINEPICTURE:
            tmp = new Image(m_mdiArea);
            break;
        case CleverURI::VMAP:
            tmp = new VMapFrame(true);
            break;
        default:
            break;
        }

        if(tmp!=nullptr)
        {
            tmp->setCleverUriType(type);
            if(tmp->openMedia())
            {
                if(tmp->readFileFromUri())
                {
                    if(type==CleverURI::MAP)
                    {
                        prepareMap(static_cast<MapFrame*>(tmp));
                    }
                    else if(type == CleverURI::VMAP)
                    {
                        prepareVMap(static_cast<VMapFrame*>(tmp));
                    }
                    else if((type==CleverURI::PICTURE)||(type==CleverURI::ONLINEPICTURE))
                    {
                        prepareImage(static_cast<Image*>(tmp));
                    }
                    addMediaToMdiArea(tmp);
                    tmp->setVisible(true);
                }
            }
        }
    }
}
void MainWindow::updateWindowTitle()
{
    if(nullptr != m_currentConnectionProfile)
    {
        setWindowTitle(QStringLiteral("%6[*] - v%2 - %3 - %4 - %5 - %1").arg(m_preferences->value("applicationName","Rolisteam").toString())
                       .arg(m_version)
                       .arg(m_clientManager->isConnected() ? tr("Connected") : tr("Not Connected"))
                       .arg(m_currentConnectionProfile->isServer() ? tr("Server") : tr("Client")).arg(m_currentConnectionProfile->isGM() ? tr("GM") : tr("Player"))
                       .arg(m_sessionManager->getSessionName()));
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    QMainWindow::dragEnterEvent(event);
}
CleverURI::ContentType MainWindow::getContentType(QString str)
{
    QImage imag(str);
    if(str.endsWith(".pla"))
    {
        return CleverURI::MAP;
    }
    else if(!imag.isNull())
    {
        return CleverURI::PICTURE;
    }
    else if(str.endsWith(".vmap"))
    {
        return CleverURI::VMAP;
    }
    else if(str.endsWith(".m3u"))
    {
        return CleverURI::SONGLIST;
    }
    else if(str.endsWith(".sce"))
    {
        return CleverURI::SCENARIO;
    }
    else if(str.endsWith(".rcs"))
    {
        return CleverURI::CHARACTERSHEET;
    }
    #ifdef WITH_PDF
    else if(str.endsWith(".pdf"))
    {
        return CleverURI::PDF;
    }
#endif
    else
    {
        QStringList list = m_preferences->value("AudioFileFilter","*.wav *.mp2 *.mp3 *.ogg *.flac").toString().split(' ');
        int i=0;
        while(i<list.size())
        {
            QString filter = list.at(i);
            filter.replace("*","");
            if(str.endsWith(filter))
            {
                return CleverURI::SONG;
            }
            ++i;
        }
    }
    return CleverURI::NONE;
}
void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* data = event->mimeData();
    if(data->hasUrls())
    {
        QList<QUrl> list = data->urls();
        for(int i = 0; i< list.size();++i)
        {
            CleverURI::ContentType type= getContentType(list.at(i).toLocalFile());
            CleverURI* uri = new CleverURI(getShortNameFromPath(list.at(i).toLocalFile()),list.at(i).toLocalFile(),type);
            openCleverURI(uri,true);
        }
        event->acceptProposedAction();
    }
}

void MainWindow::showShortCutEditor()
{
    ShortcutVisitor visitor;
    visitor.registerWidget(this,"mainwindow",true);

    ShortCutEditorDialog dialog;
    dialog.setModel(visitor.getModel());
    dialog.exec();
}

void MainWindow::openImageAs(const QPixmap pix, CleverURI::ContentType type)
{
    auto viewer = qobject_cast<MediaContainer*>(sender());
    QString title(tr("Export from %1"));
    QString sourceName = tr("unknown");
    if(nullptr != viewer)
    {
        sourceName = viewer->getUriName();
    }

    MediaContainer* destination = nullptr;
    if(type == CleverURI::VMAP)
    {
        auto media = newDocument(type);
        auto vmapFrame = dynamic_cast<VMapFrame*>(media);
        auto vmap = vmapFrame->getMap();
        vmap->addImageItem(pix.toImage());
        destination = media;
    }
    else if(type == CleverURI::MAP)
    {
        //auto mapframe = dynamic_cast<MapFrame*>(media);
        auto mapframe = new MapFrame();
        mapframe->setUriName(title);
        auto img = new QImage(pix.toImage());
        auto map = new Map(m_localPlayerId,mapframe->getMediaId(),img,false);
        mapframe->setMap(map);
        addMediaToMdiArea(mapframe);
        destination = mapframe;
    }
    else if(type == CleverURI::PICTURE)
    {
        auto img = new Image(m_mdiArea);
        auto imgPix = pix.toImage();
        img->setImage(imgPix);
        addMediaToMdiArea(img);
        destination = img;
    }
    destination->setUriName(title.arg(sourceName));
}
