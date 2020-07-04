/*************************************************************************
 *        Copyright (C) 2007 by Romain Campioni                          *
 *        Copyright (C) 2009 by Renaud Guezennec                         *
 *        Copyright (C) 2010 by Berenger Morel                           *
 *        Copyright (C) 2010 by Joseph Boudou                            *
 *                                                                       *
 *        https://rolisteam.org/                                      *
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
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStatusBar>
#include <QStringBuilder>
#include <QSystemTrayIcon>
#include <QTime>
#include <QUrl>
#include <QUuid>
#include <algorithm>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "charactersheet/charactersheet.h"
//#include "chat/chatlistwidget.h"
#include "controller/contentcontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/playercontroller.h"
#include "data/character.h"
#include "data/mediacontainer.h"
#include "data/person.h"
#include "data/player.h"
#include "data/shortcutvisitor.h"

#include "network/networkmessagewriter.h"
#include "network/receiveevent.h"
#include "network/servermanager.h"
#include "pdfviewer/pdfviewer.h"
#include "preferences/preferencesdialog.h"

#include "userlist/playermodel.h"
#include "userlist/playerspanel.h"
#include "widgets/gmtoolbox/gamemastertool.h"
#include "widgets/keygeneratordialog.h"
#include "widgets/notificationzone.h"
#include "widgets/onlinepicturedialog.h"
#include "widgets/shortcuteditordialog.h"
#include "widgets/tipofdayviewer.h"
#include "widgets/workspace.h"

#include "worker/messagehelper.h"
#include "worker/playermessagehelper.h"

#ifdef HAVE_WEBVIEW
#include "webview/webview.h"
#include <QWebEngineSettings>
#endif
// LOG
#include "common/controller/logcontroller.h"
#include "common/controller/remotelogcontroller.h"
#include "common/widgets/logpanel.h"

// Controller
#include "controller/instantmessagingcontroller.h"
#include "controller/networkcontroller.h"

// Text editor
//#include "noteeditor/src/textedit.h"
#include "widgets/aboutrolisteam.h"

// GMToolBox
#include "widgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.h"
#include "widgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"
#include "widgets/gmtoolbox/NpcMaker/npcmakerwidget.h"
#include "widgets/gmtoolbox/UnitConvertor/convertor.h"

// VMAP
#include "vmap/vmap.h"
#include "vmap/vmapframe.h"
#include "vmap/vmapwizzarddialog.h"

// character sheet
#include "diceparser/qmltypesregister.h"
#include "media/charactersheetwindow.h"

// session
#include "session/sessiondock.h"
#ifndef NULL_PLAYER
#include "audio/audioPlayer.h"
#endif

MainWindow::MainWindow(const QStringList& args)
    : QMainWindow()
    , m_preferencesDialog(nullptr)
    , m_ui(new Ui::MainWindow)
    , m_resetSettings(false)
    , m_currentConnectionProfile(nullptr)
    , m_profileDefined(false)
    , m_roomPanelDockWidget(new QDockWidget(this))
    , m_gameController(new GameController)
    , m_systemTray(new QSystemTrayIcon)
{
    parseCommandLineArguments(args);
    setAcceptDrops(true);
    m_systemTray->setIcon(QIcon(":/resources/images/logo/500-symbole.png"));
    m_systemTray->show();

    // ALLOCATIONS
    m_dialog.reset(new SelectConnectionProfileDialog(m_gameController.get(), this));
    m_sessionDock.reset(new SessionDock(m_gameController->contentController()));
    m_vmapToolBar= new VmapToolBar(m_gameController->contentController()->vmapCtrl(), this);
    m_gmToolBoxList.append({new NameGeneratorWidget(this), new GMTOOL::Convertor(this), new NpcMakerWidget(this)});
    m_roomPanel= new ChannelListPanel(m_gameController->networkController(), this);

    connect(m_gameController.get(), &GameController::updateAvailableChanged, this, &MainWindow::showUpdateNotification);
    connect(m_gameController.get(), &GameController::tipOfDayChanged, this, &MainWindow::showTipChecker);
    connect(m_gameController.get(), &GameController::localPlayerIdChanged, this,
            [this]() { m_roomPanel->setLocalPlayerId(m_gameController->localPlayerId()); });

    m_ui->setupUi(this);

    m_separatorAction= m_ui->m_fileMenu->insertSeparator(m_ui->m_closeAction);
    m_separatorAction->setVisible(false);
    registerQmlTypes();

#ifdef HAVE_WEBVIEW
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
#endif

    m_preferences= m_gameController->preferencesManager();

    auto func= [](QVariant var) {
        auto v= var.toInt();
        if(var.isNull())
            v= 6;
        VisualItem::setHighlightWidth(v);
    };
    m_preferences->registerLambda(QStringLiteral("VMAP::highlightPenWidth"), func);
    auto func2= [](QVariant var) {
        auto v= var.value<QColor>();
        if(!v.isValid())
            v= QColor(Qt::red);
        VisualItem::setHighlightColor(v);
    };
    m_preferences->registerLambda(QStringLiteral("VMAP::highlightColor"), func2);

    addToolBar(Qt::TopToolBarArea, m_vmapToolBar);

    connect(m_gameController->contentController(), &ContentController::sessionChanged, this,
            &MainWindow::setWindowModified);
    connect(m_gameController->networkController(), &NetworkController::connectedChanged, this, [this](bool connected) {
        if(connected)
            postConnection();
        m_dialog->setVisible(!connected);
        updateWindowTitle();
        m_ui->m_changeProfileAct->setEnabled(connected);
        m_ui->m_disconnectAction->setEnabled(connected);
    });
    // connect(m_sessionManager, &SessionManager::openResource, this,
    // &MainWindow::openResource);

    /// Create all GM toolbox widget
    for(auto& gmTool : m_gmToolBoxList)
    {
        QWidget* wid= dynamic_cast<QWidget*>(gmTool);

        if(wid == nullptr)
            continue;

        QDockWidget* widDock= new QDockWidget(this);
        widDock->setAllowedAreas(Qt::AllDockWidgetAreas);
        widDock->setWidget(wid);
        widDock->setWindowTitle(wid->windowTitle());
        widDock->setObjectName(wid->objectName());
        addDockWidget(Qt::RightDockWidgetArea, widDock);

        m_ui->m_gmToolBoxMenu->addAction(widDock->toggleViewAction());
        widDock->setVisible(false);
    }

    // Room List
    m_roomPanelDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_roomPanelDockWidget->setWidget(m_roomPanel);
    m_roomPanelDockWidget->setWindowTitle(m_roomPanel->windowTitle());
    m_roomPanelDockWidget->setObjectName(m_roomPanel->objectName());
    m_roomPanelDockWidget->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, m_roomPanelDockWidget);

    connect(m_ui->m_keyGeneratorAct, &QAction::triggered, this, []() {
        KeyGeneratorDialog dialog;
        dialog.exec();
    });

    setupUi();
    readSettings();
}

MainWindow::~MainWindow()= default;

void MainWindow::setupUi()
{
    if(m_commandLineProfile)
    {
        m_dialog->setArgumentProfile(m_commandLineProfile->m_ip, m_commandLineProfile->m_port,
                                     m_commandLineProfile->m_pass);
    }

    // setAnimated(false);
    m_mdiArea.reset(
        new Workspace(m_gameController->contentController(), m_gameController->instantMessagingController()));
    setCentralWidget(m_mdiArea.get());
    connect(m_mdiArea.get(), &Workspace::subWindowActivated, this, &MainWindow::activeWindowChanged);

    auto vmapController= m_gameController->contentController()->vmapCtrl();
    m_vToolBar= new VToolsBar(vmapController, this);

    QDockWidget* dock= new QDockWidget(this);
    dock->setWidget(m_vToolBar);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    dock->setWindowTitle(tr("ToolBox"));
    dock->setObjectName("DockToolBar");
    auto act= dock->toggleViewAction();

    act->setShortcut(QKeySequence("F8"));
    m_ui->m_menuSubWindows->insertAction(m_ui->m_toolBarAct, act);
    QAction* vmapToolBar= m_vmapToolBar->toggleViewAction();
    vmapToolBar->setShortcut(Qt::Key_F9);
    m_ui->m_menuSubWindows->insertAction(m_ui->m_toolBarAct, m_vmapToolBar->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_toolBarAct);

    /*m_chatListWidget= new ChatListWidget(this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::SharePreferencesCategory, m_chatListWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_chatListWidget);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_ui->m_menuSubWindows->insertAction(m_ui->m_chatListAct, m_chatListWidget->toggleViewAction());*/

    addDockWidget(Qt::RightDockWidgetArea, m_sessionDock.get());
    m_ui->m_menuSubWindows->insertAction(m_ui->m_chatListAct, m_sessionDock->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_chatListAct);

    createNotificationZone();
    ///////////////////
    // PlayerList
    ///////////////////
    m_playersListWidget= new PlayersPanel(m_gameController->playerController(), this);
    /* connect(m_playersListWidget, &PlayersPanel::runDiceForCharacter, this,
             [this](const QString& cmd, const QString& uuid) {
                 m_chatListWidget->rollDiceCmdForCharacter(cmd, uuid, true);
             });*/

    addDockWidget(Qt::RightDockWidgetArea, m_playersListWidget);
    QIcon test(":/logo.png");
    setWindowIcon(QIcon(":/logo.png"));
    m_ui->m_menuSubWindows->insertAction(m_ui->m_characterListAct, m_playersListWidget->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_characterListAct);

    ///////////////////
    // Audio Player
    ///////////////////
#ifndef NULL_PLAYER
    m_audioPlayer= AudioPlayer::getInstance(this);
    ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory, m_audioPlayer);
    addDockWidget(Qt::RightDockWidgetArea, m_audioPlayer);
    m_ui->m_menuSubWindows->insertAction(m_ui->m_audioPlayerAct, m_audioPlayer->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_audioPlayerAct);
#endif

    m_preferencesDialog= new PreferencesDialog(m_gameController->preferencesController(), this);
    linkActionToMenu();
    /* if(nullptr != m_preferencesDialog->getStateModel())
     {

     }*/

    // Initialisation des etats de sante des PJ/PNJ (variable declarees dans
    // DessinPerso.cpp)
    /*m_playerModel= PlayerModel::instance();

    connect(m_playerModel, SIGNAL(playerAdded(Player*)), this,
    SLOT(notifyAboutAddedPlayer(Player*))); connect(m_playerModel,
    SIGNAL(playerDeleted(Player*)), this,
    SLOT(notifyAboutDeletedPlayer(Player*)));*/
    // connect(m_roomPanel, &ChannelListPanel::CurrentChannelGmIdChanged,
    // m_playerModel, &PlayerModel::setCurrentGM);
    /* connect(m_playerModel, &PlayerModel::characterAdded, this,
     [this](Character* character) { if(character->isNpc())
         {
             m_sessionManager->addRessource(character);
         }
     });
     connect(m_playerModel, &PlayerModel::eventOccurs, m_gameController.get(),
     &GameController::addFeatureLog);*/

    /*connect(m_dialog, &SelectConnectionProfileDialog::rejected, this, [this]() {
        if(nullptr == m_server)
            return;
        if(m_server->getState() != ServerManager::Listening)
        {
            m_serverThread.quit();
        }
    });*/
}

void MainWindow::closeAllMediaContainer()
{
    /* auto const& values= m_mediaHash.values();
     for(auto& tmp : values)
     {
         if(nullptr != tmp)
         {
             closeMediaContainer(tmp->getMediaId(), true);
         }
     }*/
}
void MainWindow::closeMediaContainer(QString id, bool redo)
{
    /*  if(m_mediaHash.contains(id))
      {
          MediaContainer* mediaCon= m_mediaHash.value(id);
          if(nullptr != mediaCon)
          {
              // auto type= mediaCon->getContentType();*/

    /*  DeleteMediaContainerCommand* cmd= new DeleteMediaContainerCommand(
          mediaCon, m_ui->m_editMenu, m_mdiArea,
      m_currentConnectionProfile->isGM(), m_mediaHash); if(redo)
      {
          // m_undoStack.push(cmd);
      }
      else
      {
          cmd->redo(); // can be undo
          delete cmd;
      }*/

    // m_mediaHash.remove(id);
    /*  if(Core::ContentType::VECTORIALMAP == type)
      {
          m_vmapToolBar->setCurrentMap(nullptr);
      }
      else if(CleverURI::MAP == type)
      {
          // m_playersListWidget->model()->setCurrentMap(nullptr);
      }*/
    //}
    //}
}

void MainWindow::showTipChecker()
{
    auto tip= m_gameController->tipOfDay();
    TipOfDayViewer view(tip.title, tip.content, tip.url, this);
    view.exec();

    m_preferences->registerValue(QStringLiteral("MainWindow::neverDisplayTips"), view.dontshowAgain());
}

void MainWindow::activeWindowChanged(QMdiSubWindow* subWindow)
{
    /* if((nullptr == m_currentConnectionProfile) || (nullptr == subWindow))
     {
         m_ui->m_closeAction->setEnabled(false);
         m_ui->m_saveAction->setEnabled(false);
         m_ui->m_saveAsAction->setEnabled(false);
         return;
     }
     auto media= dynamic_cast<MediaContainer*>(subWindow);
     bool localPlayerIsGM= m_currentConnectionProfile->isGM();
     if(nullptr == media)
     {
         m_ui->m_closeAction->setEnabled(false);
         m_ui->m_saveAction->setEnabled(false);
         m_ui->m_saveAsAction->setEnabled(false);
         return;
     }
     m_vmapToolBar->setEnabled(false);
     auto owner= media->ownerId();
     auto localIsOwner= (m_gameController->localPlayerId() == owner);
     if(localPlayerIsGM)
         localIsOwner= true;

     m_ui->m_closeAction->setEnabled(localIsOwner);
     m_ui->m_saveAction->setEnabled(localIsOwner);
     m_ui->m_saveAsAction->setEnabled(localIsOwner);

     switch(media->getContainerType())
     {
     case MediaContainer::ContainerType::MapContainer:
     {
         m_toolBarStack->setCurrentWidget(m_toolBar);
         subWindow->setFocus();
     }
     break;
     case MediaContainer::ContainerType::VMapContainer:
     {
         // m_playersListWidget->model()->setCurrentMap(nullptr);
         m_vmapToolBar->setEnabled(true);

         if(localPlayerIsGM)
             m_toolBarStack->setCurrentWidget(m_vToolBar);

         VMapFrame* frame= dynamic_cast<VMapFrame*>(subWindow);
         if(frame)
         {
             auto map= frame->getMap();
             m_vmapToolBar->setCurrentMap(map);
             if(map)
             {
                 auto mode= map->getPermissionMode();
                 switch(mode)
                 {
                 case Core::PC_ALL:
                     m_toolBarStack->setCurrentWidget(m_vToolBar);
                     break;
                 default:
                     break;
                 }
                 // m_vToolBar->setCurrentTool(VToolsBar::HANDLER);
                 m_vToolBar->updateUi(mode);
             }
         }
     }
     break;
     case MediaContainer::ContainerType::NoteContainer:
     case MediaContainer::ContainerType::SharedNoteContainer:
         // m_playersListWidget->model()->setCurrentMap(nullptr);
         m_ui->m_saveAction->setEnabled(localIsOwner);
         m_ui->m_saveAsAction->setEnabled(localIsOwner);
         break;
     default:
         // m_playersListWidget->model()->setCurrentMap(nullptr);
         break;
     }*/
}
void MainWindow::closeEvent(QCloseEvent* event)
{
    if(mayBeSaved())
    {
        m_gameController->aboutToClose();
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void MainWindow::userNatureChange(bool isGM)
{
    if(nullptr != m_currentConnectionProfile)
    {
        m_currentConnectionProfile->setGm(isGM);
        updateUi();
        updateWindowTitle();
    }
}

void MainWindow::createNotificationZone()
{
    m_dockLogUtil= new NotificationZone(m_gameController->logController(), this);
    m_dockLogUtil->setObjectName("dockLogUtil");
    m_dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_dockLogUtil->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable
                               | QDockWidget::DockWidgetFloatable);

    m_ui->m_menuSubWindows->insertAction(m_ui->m_notificationAct, m_dockLogUtil->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_notificationAct);
    addDockWidget(Qt::RightDockWidgetArea, m_dockLogUtil);
}

void MainWindow::linkActionToMenu()
{
    m_ui->m_addVectorialMap->setData(static_cast<int>(Core::ContentType::VECTORIALMAP));
    m_ui->m_newNoteAction->setData(static_cast<int>(Core::ContentType::NOTES));
    m_ui->m_newSharedNote->setData(static_cast<int>(Core::ContentType::SHAREDNOTE));
    m_ui->m_newWebViewACt->setData(static_cast<int>(Core::ContentType::WEBVIEW));

    connect(m_ui->m_addVectorialMap, &QAction::triggered, this, &MainWindow::newVMap);
    connect(m_ui->m_newNoteAction, &QAction::triggered, this, [this]() {
        auto ctrl= m_gameController->contentController();
        if(nullptr == ctrl)
            return;
        std::map<QString, QVariant> params;
        ctrl->newMedia(Core::ContentType::NOTES, params);
    });
    connect(m_ui->m_newSharedNote, &QAction::triggered, this, [this]() {
        auto ctrl= m_gameController->contentController();
        if(nullptr == ctrl)
            return;
        std::map<QString, QVariant> params;
        ctrl->newMedia(Core::ContentType::SHAREDNOTE, params);
    });
    connect(m_ui->m_newWebViewACt, &QAction::triggered, this, [this]() {
        auto ctrl= m_gameController->contentController();
        if(nullptr == ctrl)
            return;
        std::map<QString, QVariant> params;
        ctrl->newMedia(Core::ContentType::WEBVIEW, params);
    });
    // connect(m_ui->m_newChatAction, &QAction::triggered, m_chatListWidget, &ChatListWidget::createPrivateChat);

    // open
    connect(m_ui->m_openPictureAction, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openOnlinePictureAction, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openCharacterSheet, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openVectorialMap, &QAction::triggered, this, &MainWindow::openVMap);
    connect(m_ui->m_openStoryAction, &QAction::triggered, this, &MainWindow::openStory);
    connect(m_ui->m_openNoteAction, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openShareNote, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openPdfAct, &QAction::triggered, this, &MainWindow::openGenericContent);

    connect(m_ui->m_shortCutEditorAct, &QAction::triggered, this, &MainWindow::showShortCutEditor);

    m_ui->m_openPictureAction->setData(static_cast<int>(Core::ContentType::PICTURE));
    m_ui->m_openOnlinePictureAction->setData(static_cast<int>(Core::ContentType::ONLINEPICTURE));
    m_ui->m_openCharacterSheet->setData(static_cast<int>(Core::ContentType::CHARACTERSHEET));
    m_ui->m_openVectorialMap->setData(static_cast<int>(Core::ContentType::VECTORIALMAP));
    // m_ui->m_openStoryAction->setData(static_cast<int>(CleverURI::SCENARIO));
    m_ui->m_openNoteAction->setData(static_cast<int>(Core::ContentType::NOTES));
    m_ui->m_openShareNote->setData(static_cast<int>(Core::ContentType::SHAREDNOTE));
#ifdef WITH_PDF
    m_ui->m_openPdfAct->setData(static_cast<int>(Core::ContentType::PDF));
#endif
    m_ui->m_recentFileMenu->setVisible(false);
    connect(m_ui->m_closeAction, &QAction::triggered, m_mdiArea.get(), &Workspace::closeActiveSub);
    connect(m_ui->m_saveAction, &QAction::triggered, this, &MainWindow::saveCurrentMedia);
    connect(m_ui->m_saveAllAction, &QAction::triggered, this, &MainWindow::saveAllMediaContainer);
    connect(m_ui->m_saveAsAction, &QAction::triggered, this, &MainWindow::saveCurrentMedia);
    connect(m_ui->m_saveScenarioAction, &QAction::triggered, this, [this]() { saveStory(false); });
    connect(m_ui->m_saveScenarioAsAction, &QAction::triggered, this, [this]() { saveStory(true); });
    connect(m_ui->m_preferencesAction, &QAction::triggered, m_preferencesDialog, &PreferencesDialog::show);

    // Edition
    // Windows managing
    connect(m_ui->m_cascadeViewAction, &QAction::triggered, m_mdiArea.get(), &Workspace::cascadeSubWindows);
    connect(m_ui->m_tabViewAction, &QAction::triggered, m_mdiArea.get(), &Workspace::setTabbedMode);
    connect(m_ui->m_tileViewAction, &QAction::triggered, m_mdiArea.get(), &Workspace::tileSubWindows);

    connect(m_ui->m_fullScreenAct, &QAction::triggered, this, [=](bool enable) {
        if(enable)
        {
            showFullScreen();
            m_mdiArea->addAction(m_ui->m_fullScreenAct);
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
            m_mdiArea->removeAction(m_ui->m_fullScreenAct);
        }
    });

    auto undoStack= m_gameController->undoStack();

    auto redo= undoStack->createRedoAction(this, tr("&Redo"));
    auto undo= undoStack->createUndoAction(this, tr("&Undo"));

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);

    connect(undoStack, &QUndoStack::cleanChanged, this, [this](bool clean) { setWindowModified(!clean); });

    m_ui->m_editMenu->insertAction(m_ui->m_shortCutEditorAct, redo);
    m_ui->m_editMenu->insertAction(redo, undo);
    m_ui->m_editMenu->insertSeparator(m_ui->m_shortCutEditorAct);

    // close
    connect(m_ui->m_quitAction, &QAction::triggered, this, &MainWindow::close);

    // network
    connect(m_ui->m_disconnectAction, &QAction::triggered, m_gameController->networkController(),
            &NetworkController::disconnection);
    connect(m_ui->m_changeProfileAct, &QAction::triggered, this, &MainWindow::showConnectionDialog);
    connect(m_ui->m_connectionLinkAct, &QAction::triggered, this, [=]() {
        QString str("rolisteam://%1/%2/%3");
        if(m_currentConnectionProfile == nullptr)
            return;
        auto* clipboard= QGuiApplication::clipboard();
        clipboard->setText(str.arg(m_connectionAddress)
                               .arg(m_currentConnectionProfile->port())
                               .arg(QString::fromUtf8(m_currentConnectionProfile->password().toBase64())));
    });
    connect(m_ui->m_roomListAct, &QAction::triggered, m_roomPanelDockWidget, &QDockWidget::setVisible);
    // Help
    connect(m_ui->m_aboutAction, &QAction::triggered, this, [this]() {
        AboutRolisteam diag(m_gameController->version(), this);
        diag.exec();
    });
    connect(m_ui->m_onlineHelpAction, &QAction::triggered, this, &MainWindow::helpOnLine);

    m_ui->m_supportRolisteam->setData(QStringLiteral("https://liberapay.com/Rolisteam/donate"));
    m_ui->m_patreon->setData(QStringLiteral("https://www.patreon.com/rolisteam"));
    connect(m_ui->m_supportRolisteam, &QAction::triggered, this, &MainWindow::showSupportPage);
    connect(m_ui->m_patreon, &QAction::triggered, this, &MainWindow::showSupportPage);
}

void MainWindow::showSupportPage()
{
    auto act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    QString url= act->data().toString();
    if(!QDesktopServices::openUrl(QUrl(url))) //"https://liberapay.com/Rolisteam/donate"
    {
        QMessageBox msgBox(QMessageBox::Information, tr("Support"),
                           tr("The %1 donation page can be found online at :<br> <a "
                              "href=\"%2\">%2</a>")
                               .arg(m_preferences->value("Application_Name", "rolisteam").toString())
                               .arg(url),
                           QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if(isFullScreen())
    {
        if(qFuzzyCompare(event->windowPos().y(), 0.0))
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

void MainWindow::showAsPreferences()
{
    m_preferences->value("FullScreenAtStarting", true).toBool() ? showMaximized() : show();
    showConnectionDialog();
}

void MainWindow::updateWorkspace()
{
    QMdiSubWindow* active= m_mdiArea->currentSubWindow();
    if(nullptr != active)
    {
        activeWindowChanged(active);
    }
}

void MainWindow::newVMap()
{
    MapWizzardDialog mapWizzard(m_mdiArea.get());
    if(mapWizzard.exec())
    {
        auto ctrl= m_gameController->contentController();
        std::map<QString, QVariant> params;
        params.insert({QStringLiteral("title"), mapWizzard.name()});
        params.insert({QStringLiteral("permission"), mapWizzard.permission()});
        params.insert({QStringLiteral("bgcolor"), mapWizzard.backgroundColor()});
        params.insert({QStringLiteral("gridSize"), mapWizzard.gridSize()});
        params.insert({QStringLiteral("gridPattern"), QVariant::fromValue(mapWizzard.pattern())});
        params.insert({QStringLiteral("gridColor"), mapWizzard.gridColor()});
        params.insert({QStringLiteral("visibility"), mapWizzard.visibility()});
        params.insert({QStringLiteral("scale"), mapWizzard.scale()});
        params.insert({QStringLiteral("unit"), mapWizzard.unit()});
        ctrl->newMedia(Core::ContentType::VECTORIALMAP, params);
    }
}

bool MainWindow::mayBeSaved(bool connectionLoss)
{
    QMessageBox msgBox(this);
    QAbstractButton* saveBtn= msgBox.addButton(QMessageBox::Save);
    QAbstractButton* quitBtn= msgBox.addButton(tr("Quit"), QMessageBox::RejectRole);
    Qt::WindowFlags flags= msgBox.windowFlags();
    msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

    QString message;
    QString msg= m_preferences->value("Application_Name", "rolisteam").toString();
    if(connectionLoss)
    {
        message= tr("Connection has been lost. %1 will be close").arg(msg);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Connection lost"));
    }
    else
    {
        msgBox.setIcon(QMessageBox::Information);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.setWindowTitle(tr("Quit %1 ").arg(msg));
    }

    /*if(nullptr != PlayerModel::instance()->getLocalPlayer())
    {
        if(!PlayerModel::instance()->getLocalPlayer()->isGM())
        {
            message+= tr("Do you want to save your minutes before to quit
    %1?").arg(msg);
        }
        else
        {
            message+= tr("Do you want to save your scenario before to quit
    %1?").arg(msg);
        }

        msgBox.setText(message);
        msgBox.exec();
        if(msgBox.clickedButton() == boutonQuitter)
        {
            return true;
        }
        else if(msgBox.clickedButton() == boutonSauvegarder) // saving
        {
            bool ok;
            if(!PlayerModel::instance()->getLocalPlayer()->isGM())
                ok= saveMinutes();
            else
                ok= saveStory(false);

            if(ok || connectionLoss)
            {
                return true;
            }
        }
        return false;
    }*/
    return true;
}
/*QWidget* MainWindow::registerSubWindow(QWidget* subWindow, QAction* action)
{
    return m_mdiArea->addWindow(subWindow, action);
}*/

void MainWindow::openStory()
{
    QString fileName= QFileDialog::getOpenFileName(
        this, tr("Open scenario"), m_preferences->value("SessionDirectory", QDir::homePath()).toString(),
        tr("Scenarios (*.sce)"));
    readStory(fileName);
}
void MainWindow::readStory(QString fileName)
{
    if(fileName.isNull())
        return;
    QFileInfo info(fileName);
    m_preferences->registerValue("SessionDirectory", info.absolutePath());

    auto contentCtrl= m_gameController->contentController();
    contentCtrl->setSessionPath(fileName);
    contentCtrl->loadSession();

    m_recentScenarios.removeAll(fileName);
    m_recentScenarios.prepend(fileName);
    updateRecentScenarioAction();

    updateWindowTitle();
}
QString MainWindow::getShortNameFromPath(QString path)
{
    QFileInfo info(path);
    return info.baseName();
}

bool MainWindow::saveStory(bool saveAs)
{
    auto contentCtrl= m_gameController->contentController();
    if(contentCtrl->sessionPath().isEmpty() || saveAs)
    {
        QString fileName= QFileDialog::getSaveFileName(
            this, tr("Save Scenario as"), m_preferences->value("SessionDirectory", QDir::homePath()).toString(),
            tr("Scenarios (*.sce)"));
        if(fileName.isNull())
        {
            return false;
        }
        if(!fileName.endsWith(".sce"))
        {
            fileName.append(QStringLiteral(".sce"));
        }
        contentCtrl->setSessionPath(fileName);
    }
    QFileInfo info(contentCtrl->sessionPath());
    m_preferences->registerValue("SessionDirectory", info.absolutePath());
    contentCtrl->saveSession();
    updateWindowTitle();
    return true;
}
////////////////////////////////////////////////////
// Save data
////////////////////////////////////////////////////
void MainWindow::saveCurrentMedia()
{
    // auto mediaC= m_mdiArea->currentMediaContainer();

    bool saveAs= false;
    if(qobject_cast<QAction*>(sender()) == m_ui->m_saveAsAction)
    {
        saveAs= true;
    }
    QMdiSubWindow* active= m_mdiArea->currentSubWindow();
    if(nullptr != active)
    {
        MediaContainer* currentMedia= dynamic_cast<MediaContainer*>(active);
        if(nullptr != currentMedia)
        {
            saveMedia(currentMedia, saveAs);
        }
    }
}
void MainWindow::saveMedia(MediaContainer* mediaC, bool saveAs)
{
    if(nullptr == mediaC)
        return;

    /*CleverURI* cleverURI= mediaC->getCleverUri();
    if(nullptr == cleverURI)
        return;

    QString uri= cleverURI->getUri();
    if(cleverURI->getCurrentMode() == CleverURI::Linked || saveAs)
    {
        if(uri.isEmpty() || saveAs)
        {
            QString key=
    CleverURI::getPreferenceDirectoryKey(cleverURI->getType()); QString filter=
    CleverURI::getFilterForType(cleverURI->getType()); QString media=
    CleverURI::typeToString(cleverURI->getType()); QString fileName=
    QFileDialog::getSaveFileName( this, tr("Save %1").arg(media),
    m_preferences->value(key, QDir::homePath()).toString(), filter);
            if(fileName.isEmpty())
            {
                return;
            }
            QFileInfo info(fileName);
            m_preferences->registerValue(key, info.absolutePath());
            cleverURI->setUri(fileName);
        }
        mediaC->saveMedia();
    }
    else if(cleverURI->getCurrentMode() == CleverURI::Internal)
    {
        mediaC->putDataIntoCleverUri();
    }
    setLatestFile(cleverURI);*/
}

bool MainWindow::saveMinutes()
{
    // auto const& values= m_mediaHash.values();
    //  for(auto& edit : values)
    {
        /*  if(Core::ContentType::NOTES == edit->getContentType())
          {
              NoteContainer* note= dynamic_cast<NoteContainer*>(edit);
              if(nullptr != note)
              {
                  note->saveMedia();
              }
          }*/
    }
    return true;
}

void MainWindow::saveAllMediaContainer()
{
    /*  for(auto& media : m_mediaHash)
      {
          saveMedia(media, false);
      }*/
}
void MainWindow::stopReconnection()
{
    m_ui->m_changeProfileAct->setEnabled(true);
    m_ui->m_disconnectAction->setEnabled(false);
}

void MainWindow::setUpNetworkConnection()
{
    connect(m_gameController.get(), &GameController::localIsGMChanged, this, &MainWindow::userNatureChange);
    auto networkCtrl= m_gameController->networkController();
    connect(networkCtrl, &NetworkController::downloadingData, m_dockLogUtil, &NotificationZone::receiveData);
}

void MainWindow::helpOnLine()
{
    if(!QDesktopServices::openUrl(QUrl("http://wiki.rolisteam.org/")))
    {
        QMessageBox* msgBox= new QMessageBox(QMessageBox::Information, tr("Help"),
                                             tr("Documentation of %1 can be found online at :<br> <a "
                                                "href=\"http://wiki.rolisteam.org\">http://wiki.rolisteam.org/</a>")
                                                 .arg(m_preferences->value("Application_Name", "rolisteam").toString()),
                                             QMessageBox::Ok);
        msgBox->exec();
    }
}
void MainWindow::updateUi()
{
    if(nullptr == m_currentConnectionProfile)
    {
        return;
    }
#ifndef NULL_PLAYER
    m_audioPlayer->updateUi(m_currentConnectionProfile->isGM());
#endif
    bool isGM= m_currentConnectionProfile->isGM();
    m_vToolBar->setGM(isGM);
    // m_ui->m_newMapAction->setEnabled(isGM);
    m_ui->m_addVectorialMap->setEnabled(isGM);
    m_ui->m_openMapAction->setEnabled(isGM);
    m_ui->m_openStoryAction->setEnabled(isGM);
    m_ui->m_closeAction->setEnabled(isGM);
    m_ui->m_saveAction->setEnabled(isGM);
    m_ui->m_saveAllAction->setEnabled(isGM);
    m_ui->m_saveScenarioAction->setEnabled(isGM);
    m_ui->m_connectionLinkAct->setVisible(isGM);
    m_ui->m_saveScenarioAsAction->setEnabled(isGM);
    m_vmapToolBar->setVisible(isGM);
    m_vmapToolBar->toggleViewAction()->setVisible(isGM);

    m_ui->m_changeProfileAct->setEnabled(false);
    m_ui->m_disconnectAction->setEnabled(true);

    updateRecentFileActions();
}
void MainWindow::showUpdateNotification()
{
    QMessageBox::information(this, tr("Update Notification"),
                             tr("The %1 version has been released. "
                                "Please take a look at <a "
                                "href=\"http://www.rolisteam.org/download\">Download page</a> for "
                                "more "
                                "information")
                                 .arg(m_gameController->remoteVersion()));
}

void MainWindow::notifyAboutAddedPlayer(Player* player) const
{
    m_gameController->addFeatureLog(tr("%1 just joins the game.").arg(player->name()));
    if(player->getUserVersion().compare(m_gameController->version()) != 0)
    {
        m_gameController->addErrorLog(
            tr("%1 has not the right version: %2.").arg(player->name(), player->getUserVersion()));
    }
}

void MainWindow::notifyAboutDeletedPlayer(Player* player) const
{
    m_gameController->addFeatureLog(tr("%1 just leaves the game.").arg(player->name()));
}

void MainWindow::readSettings()
{
    QSettings settings("rolisteam", QString("rolisteam_%1/preferences").arg(m_gameController->version()));

    if(m_resetSettings)
    {
        settings.clear();
    }

    restoreState(settings.value("windowState").toByteArray());
    bool maxi= settings.value("Maximized", false).toBool();
    if(!maxi)
    {
        restoreGeometry(settings.value("geometry").toByteArray());
    }

    m_maxSizeRecentFile= m_preferences->value("recentfilemax", 5).toInt();

    // Read recent files
    int size= settings.beginReadArray("recentFiles");
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        auto path= settings.value("path").toString();
        auto type= settings.value("type").value<Core::ContentType>();
        m_recentFiles.push_back(FileInfo({path, type}));
    }
    settings.endArray();

    // read recent scenario
    m_recentScenarios= settings.value("recentScenario").toStringList();
    for(int i= 0; i < m_maxSizeRecentFile; ++i)
    {
        auto act= m_ui->m_recentFileMenu->addAction(QStringLiteral("recentFile %1").arg(i));
        m_recentFileActs.append(act);
        act->setVisible(false);
        connect(act, &QAction::triggered, this, &MainWindow::openRecentFile);

        act= new QAction(QStringLiteral("Recent Scenario %1").arg(i + 1), this);
        m_ui->m_fileMenu->insertAction(m_separatorAction, act);
        act->setVisible(false);
        m_recentScenarioActions.push_back(act);
        connect(act, &QAction::triggered, this, &MainWindow::openRecentScenario);
    }
    settings.beginGroup("MapShow");
    m_ui->m_showPcNameAction->setChecked(settings.value("showPcName", m_ui->m_showPcNameAction->isChecked()).toBool());
    m_ui->m_showNpcNameAction->setChecked(
        settings.value("showNpcName", m_ui->m_showNpcNameAction->isChecked()).toBool());
    m_ui->m_showNpcNumberAction->setChecked(
        settings.value("showNpcNumber", m_ui->m_showNpcNumberAction->isChecked()).toBool());
    m_ui->m_showHealtStatusAction->setChecked(
        settings.value("showHealthState", m_ui->m_showHealtStatusAction->isChecked()).toBool());
    m_ui->m_healthBarAct->setChecked(settings.value("showHealthBar", m_ui->m_healthBarAct->isChecked()).toBool());
    m_ui->m_showInitiativeAct->setChecked(
        settings.value("showInitiative", m_ui->m_showInitiativeAct->isChecked()).toBool());
    settings.endGroup();

    updateRecentFileActions();
    updateRecentScenarioAction();
    // m_chatListWidget->readSettings(settings);
    m_audioPlayer->readSettings();
    m_dockLogUtil->initSetting();
}
void MainWindow::writeSettings()
{
    QSettings settings("rolisteam", QString("rolisteam_%1/preferences").arg(m_gameController->version()));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("Maximized", isMaximized());

    settings.beginWriteArray("recentFiles", m_recentFiles.size());
    auto i= 0;
    for(auto p : m_recentFiles)
    {
        settings.setArrayIndex(i);
        settings.setValue("path", p.path);
        settings.setValue("type", QVariant::fromValue(p.type));
        ++i;
    }
    settings.endArray();

    settings.setValue("recentScenario", m_recentScenarios);
    // m_chatListWidget->writeSettings(settings);
    for(auto& gmtool : m_gmToolBoxList)
    {
        gmtool->writeSettings();
    }
    settings.beginGroup("MapShow");
    settings.setValue("showPcName", m_ui->m_showPcNameAction->isChecked());
    settings.setValue("showNpcName", m_ui->m_showNpcNameAction->isChecked());
    settings.setValue("showNpcNumber", m_ui->m_showNpcNumberAction->isChecked());
    settings.setValue("showHealthState", m_ui->m_showHealtStatusAction->isChecked());
    settings.setValue("showHealthBar", m_ui->m_healthBarAct->isChecked());
    settings.setValue("showInitiative", m_ui->m_showInitiativeAct->isChecked());
    settings.endGroup();
}
void MainWindow::parseCommandLineArguments(const QStringList& list)
{
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption port(QStringList() << "p"
                                          << "port",
                            tr("Set rolisteam to use <port> for the connection"), "port");
    QCommandLineOption hostname(QStringList() << "s"
                                              << "server",
                                tr("Set rolisteam to connect to <server>."), "server");
    QCommandLineOption role(QStringList() << "r"
                                          << "role",
                            tr("Define the <role>: gm or pc"), "role");
    QCommandLineOption reset(QStringList() << "reset-settings",
                             tr("Erase the settings and use the default parameters"));
    QCommandLineOption user(QStringList() << "u"
                                          << "user",
                            tr("Define the <username>"), "username");
    QCommandLineOption websecurity(QStringList() << "w"
                                                 << "disable-web-security",
                                   tr("Remove limit to PDF file size"));
    QCommandLineOption translation(QStringList() << "t"
                                                 << "translation",
                                   QObject::tr("path to the translation file: <translationfile>"), "translationfile");
    QCommandLineOption url(QStringList() << "l"
                                         << "link",
                           QObject::tr("Define URL to connect to server: <url>"), "url");

    parser.addOption(port);
    parser.addOption(hostname);
    parser.addOption(role);
    parser.addOption(reset);
    parser.addOption(user);
    parser.addOption(translation);
    parser.addOption(websecurity);
    parser.addOption(url);

    parser.process(list);

    bool hasPort= parser.isSet(port);
    bool hasHostname= parser.isSet(hostname);
    bool hasRole= parser.isSet(role);
    bool hasUser= parser.isSet(user);
    bool hasUrl= parser.isSet(url);
    m_resetSettings= parser.isSet(reset);

    QString portValue;
    QString hostnameValue;
    QString roleValue;
    QString username;
    QString urlString;
    QString passwordValue;
    if(hasPort)
    {
        portValue= parser.value(port);
    }
    if(hasHostname)
    {
        hostnameValue= parser.value(hostname);
    }
    if(hasRole)
    {
        roleValue= parser.value(role);
    }
    if(hasUser)
    {
        username= parser.value(user);
    }
    if(hasUrl)
    {
        urlString= parser.value(url);
        // auto list = urlString.split("/",QString::SkipEmptyParts);
        QRegularExpression ex("^rolisteam://(.*)/([0-9]+)/(.*)$");
        QRegularExpressionMatch match= ex.match(urlString);
        // rolisteam://IP/port/password
        // if(list.size() ==  4)
        if(match.hasMatch())
        {
            hostnameValue= match.captured(1);
            portValue= match.captured(2);
            passwordValue= match.captured(3);
            QByteArray pass= QByteArray::fromBase64(passwordValue.toUtf8());
            m_commandLineProfile.reset(new CommandLineProfile({hostnameValue, portValue.toInt(), pass}));
        }
    }
}


void MainWindow::processWebPageMessage(NetworkMessageReader* msg)
{
#ifdef HAVE_WEBVIEW
    /*  if(msg->action() == NetMsg::UpdateContent)
      {
          QString idMedia= msg->string8();
          if(m_mediaHash.contains(idMedia))
          {
              MediaContainer* mediaContainer= m_mediaHash.value(idMedia);
              WebView* note= dynamic_cast<WebView*>(mediaContainer);
              note->readMessage(*msg);
          }
      }*/
#endif
}

void MainWindow::showConnectionDialog(bool forced)
{
    if((!m_profileDefined) || (forced))
    {
        m_dialog->open();
    }
}

/*
void MainWindow::processAdminstrationMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::EndConnectionAction)
    {
        m_gameController->addInfoLog(tr("End of the connection process"));
        updateWorkspace();
    }
    else if((msg->action() == NetMsg::SetChannelList) || (NetMsg::AdminAuthFail
== msg->action())
            || (NetMsg::AdminAuthSucessed == msg->action()))
    {
        ChannelListPanel* roomPanel=
qobject_cast<ChannelListPanel*>(m_roomPanelDockWidget->widget()); if(nullptr !=
roomPanel)
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
*/
/*void MainWindow::initializedClientManager()
{
    if(nullptr == m_currentConnectionProfile)
        return;

    if(nullptr == m_currentConnectionProfile->getPlayer())
        return;

    m_localPlayerId= m_currentConnectionProfile->getPlayer()->getUuid();
    m_roomPanel->setLocalPlayerId(m_localPlayerId);

    if((nullptr != m_currentConnectionProfile) && (nullptr != m_clientManager))
    {
        if(m_currentConnectionProfile->isServer())
        {
            m_ipChecker->startCheck();
        }
        if(nullptr != m_playerList)
        {
            m_playerList->completeListClean();
            m_playerList->setLocalPlayer(m_currentConnectionProfile->getPlayer());
        }
    }
}*/

void MainWindow::cleanUpData()
{
    m_gameController->clear();

    ChannelListPanel* roomPanel= qobject_cast<ChannelListPanel*>(m_roomPanelDockWidget->widget());
    if(nullptr != roomPanel)
    {
        roomPanel->cleanUp();
    }
}

void MainWindow::postConnection()
{
    if(nullptr != m_preferences)
    {
        m_preferences->registerValue("isClient", true);
    }

    m_ui->m_changeProfileAct->setEnabled(false);
    m_ui->m_disconnectAction->setEnabled(true);
    m_dialog->accept();

    setUpNetworkConnection();
    updateWindowTitle();
    updateUi();

    // if(m_currentConnectionProfile->isGM())
    {
        // m_preferencesDialog->sendOffAllDiceAlias();
        // m_preferencesDialog->sendOffAllState();
    }
}

/*void MainWindow::processMapMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::CloseMap)
    {
        QString idMap= msg->string8();
        closeMediaContainer(idMap, false);
    }
    else
    {
        MapFrame* mapFrame= new MapFrame(nullptr, m_mdiArea);
        if((nullptr != m_currentConnectionProfile)
           && (!mapFrame->processMapMessage(msg,
!m_currentConnectionProfile->isGM())))
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
}*/

/*void MainWindow::processNpcMessage(NetworkMessageReader* msg)
{
    QString idMap= msg->string8();
    if(msg->action() == NetMsg::addNpc)
    {
        Map* map= findMapById(idMap);
        extractCharacter(map, msg);
    }
    else if(msg->action() == NetMsg::delNpc)
    {
        QString idNpc= msg->string8();
        Map* map= findMapById(idMap);
        if(nullptr != map)
        {
            map->eraseCharacter(idNpc);
        }
    }
}

void MainWindow::processCharacterPlayerMessage(NetworkMessageReader* msg)
{
    QString idMap= msg->string8();
    QString idCharacter= msg->string8();
    Map* map= findMapById(idMap);
    if(nullptr == map)
        return;

quint8 param= msg->uint8();
if(msg->action() == NetMsg::ToggleViewPlayerCharacterAction)
{
    map->showPc(idCharacter, param);
}
else if(msg->action() == NetMsg::ChangePlayerCharacterSizeAction)
{
    map->selectCharacter(idCharacter);
    map->changePcSize(param + 11);
}
}
*/
void MainWindow::openGenericContent()
{
    QAction* action= static_cast<QAction*>(sender());
    Core::ContentType type= static_cast<Core::ContentType>(action->data().toInt());
    QString filter= CleverURI::getFilterForType(type);

    QString folder= m_preferences->value(CleverURI::getPreferenceDirectoryKey(type), ".").toString();
    QString title= tr("Open %1").arg(CleverURI::typeToString(type));
    QStringList filepath= QFileDialog::getOpenFileNames(this, title, folder, filter);
    QStringList list= filepath;
    auto contentCtrl= m_gameController->contentController();
    for(auto const& path : list)
    {
        /* auto uri= new CleverURI(getShortNameFromPath(path), path,
                                 m_gameController->playerController()->localPlayer()->uuid(), type);*/
        contentCtrl->openMedia({{"path", path},
                                {"type", QVariant::fromValue(type)},
                                {"name", getShortNameFromPath(path)},
                                {"ownerId", m_gameController->playerController()->localPlayer()->uuid()}});
    }
}

void MainWindow::openOnlineImage()
{
    OnlinePictureDialog dialog;
    if(QDialog::Accepted != dialog.exec())
        return;

    std::map<QString, QVariant> args({{"name", dialog.getTitle()},
                                      {"path", dialog.getPath()},
                                      {"ownerId", m_gameController->playerController()->localPlayer()->uuid()},
                                      {"type", QVariant::fromValue(Core::ContentType::PICTURE)},
                                      {QStringLiteral("data"), dialog.getData()}});

    m_gameController->contentController()->openMedia(args);
}

void MainWindow::openVMap()
{
    /*MapWizzard mapWizzard(true, this);
    mapWizzard.resetData();
    if(mapWizzard.exec() != QMessageBox::Accepted)
        return;

    auto permission= mapWizzard.getPermissionMode();
    auto filepath= mapWizzard.getFilepath();
    auto hidden= mapWizzard.getHidden();

    std::map<QString, QVariant> args({{QStringLiteral("permission"), permission}, {QStringLiteral("hidden"), hidden}});

    auto uri= new CleverURI(mapWizzard.getTitle(), filepath,
                            m_gameController->playerController()->localPlayer()->uuid(),
    Core::ContentType::VECTORIALMAP); m_gameController->contentController()->openMedia(uri, args);

    QFileInfo info(mapWizzard.getFilepath());
    m_preferences->registerValue("MapDirectory", info.absolutePath());*/
}

void MainWindow::openRecentFile()
{
    QAction* action= qobject_cast<QAction*>(sender());
    if(action)
    {
        // CleverURI* uri= new CleverURI();
        //*uri= action->data().value<CleverURI>();
        // ri->setDisplayed(false);
        // openCleverURI(uri, true);
    }
}

void MainWindow::openRecentScenario()
{
    QAction* action= qobject_cast<QAction*>(sender());
    if(action)
    {
        readStory(action->data().toString());
    }
}

void MainWindow::updateRecentScenarioAction()
{
    std::remove_if(m_recentScenarios.begin(), m_recentScenarios.end(),
                   [](QString const& f) { return !QFile::exists(f); });

    int i= 0;
    for(auto act : m_recentScenarioActions)
    {
        if(i >= m_recentScenarios.size())
        {
            act->setVisible(false);
            continue;
        }
        else
        {
            act->setVisible(true);
            act->setText(QStringLiteral("&%1  %2").arg(i + 1).arg(QFileInfo(m_recentScenarios[i]).fileName()));
            act->setData(m_recentScenarios[i]);
        }
        ++i;
    }
    m_separatorAction->setVisible(!m_recentFiles.empty());
}

void MainWindow::updateRecentFileActions()
{
    m_recentFiles.erase(std::remove_if(m_recentFiles.begin(), m_recentFiles.end(),
                                       [](const FileInfo& info) { return !QFileInfo::exists(info.path); }),
                        m_recentFiles.end());

    std::size_t i= 0;
    for(auto& action : m_recentFileActs)
    {
        if(i >= m_recentFiles.size())
        {
            action->setVisible(false);
        }
        else
        {
            QString text= QStringLiteral("&%1 %2").arg(i + 1).arg(QFileInfo(m_recentFiles[i].path).fileName());
            action->setText(text);
            action->setData(static_cast<int>(i));
            action->setIcon(QIcon(CleverURI::typeToIconPath(m_recentFiles[i].type)));
            action->setVisible(true);
        }
        ++i;
    }
    m_ui->m_recentFileMenu->setEnabled(!m_recentFiles.empty());
}

void MainWindow::setLatestFile(CleverURI* fileName)
{
    // no online picture because they are handled in a really different way.
    /* if((nullptr == fileName) || (fileName->contentType() == Core::ContentType::ONLINEPICTURE)
        || (fileName->loadingMode() == CleverURI::Internal) || !QFileInfo::exists(fileName->getUri()))
     {
         return;
     }
     m_recentFiles.removeAll(*fileName);
     m_recentFiles.prepend(*fileName);
     while(m_recentFiles.size() > m_maxSizeRecentFile)
     {
         m_recentFiles.removeLast();
     }
     updateRecentFileActions();*/
}

void MainWindow::openResource(ResourcesNode* node, bool force)
{
    if(node->type() == ResourcesNode::Cleveruri)
    {
        // openCleverURI(dynamic_cast<CleverURI*>(node), force);
    }
    else if(node->type() == ResourcesNode::Person)
    {
        // m_playerModel->addLocalCharacter(dynamic_cast<Character*>(node));
    }
}

/*for(auto const& uri : uriList)
{
    setLatestFile(uri);
}*/
void MainWindow::updateWindowTitle()
{
    auto networkCtrl= m_gameController->networkController();
    auto contentCtrl= m_gameController->contentController();
    auto const connectionStatus= m_gameController->connected() ? tr("Connected") : tr("Not Connected");
    auto const networkStatus= networkCtrl->hosting() ? tr("Server") : tr("Client");
    auto const profileStatus= networkCtrl->isGM() ? tr("GM") : tr("Player");

    setWindowTitle(QStringLiteral("%6[*] - v%2 - %3 - %4 - %5 - %1")
                       .arg(m_preferences->value("applicationName", "Rolisteam").toString(),
                            m_gameController->version(), connectionStatus, networkStatus, profileStatus,
                            contentCtrl->sessionName()));
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    QMainWindow::dragEnterEvent(event);
}
Core::ContentType MainWindow::getContentType(QString str)
{
    /*QImage imag(str);
    if(!imag.isNull())
    {
        return Core::ContentType::PICTURE;
    }
    else if(str.endsWith(".vmap"))
    {
        return Core::ContentType::VECTORIALMAP;
    }
    else if(str.endsWith(".m3u"))
    {
        return CleverURI::SONGLIST;
    }
    else if(str.endsWith(".rcs"))
    {
        return Core::ContentType::CHARACTERSHEET;
    }
    else if(str.endsWith(".md"))
    {
        return Core::ContentType::SHAREDNOTE;
    }
    else if(str.endsWith(".odt") || str.endsWith(".txt"))
    {
        return Core::ContentType::NOTES;
    }
#ifdef WITH_PDF
    else if(str.endsWith(".pdf"))
    {
        return Core::ContentType::PDF;
    }
#endif
    else
    {
        QStringList list
            = m_preferences->value("AudioFileFilter", "*.wav *.mp2 *.mp3 *.ogg *.flac").toString().split(' ');
        int i= 0;
        while(i < list.size())
        {
            QString filter= list.at(i);
            filter.replace("*", "");
            if(str.endsWith(filter))
            {
                return CleverURI::SONG;
            }
            ++i;
        }
    }*/
    return Core::ContentType::NOTES;
}
void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* data= event->mimeData();
    if(!data->hasUrls())
        return;

    QList<QUrl> list= data->urls();
    for(auto url : list)
    {
        Core::ContentType type= getContentType(url.toLocalFile());
        qInfo() << QStringLiteral("MainWindow: dropEvent for %1").arg(CleverURI::typeToString(type));
        CleverURI* uri= new CleverURI(getShortNameFromPath(url.toLocalFile()), url.toLocalFile(), type);
        // openCleverURI(uri, true);
    }
    event->acceptProposedAction();
}

void MainWindow::showShortCutEditor()
{
    ShortcutVisitor visitor;
    visitor.registerWidget(this, "mainwindow", true);

    ShortCutEditorDialog dialog;
    dialog.setModel(visitor.getModel());
    dialog.exec();
}

void MainWindow::openImageAs(const QPixmap& pix, Core::ContentType type)
{
    auto viewer= qobject_cast<MediaContainer*>(sender());
    QString title(tr("Export from %1"));
    QString sourceName= tr("unknown");
    if(nullptr != viewer)
    {
        // sourceName= viewer->getUriName();
    }

    MediaContainer* destination= nullptr;
    if(type == Core::ContentType::VECTORIALMAP)
    {
        /* auto media= newDocument(type, false);
         auto vmapFrame= dynamic_cast<VMapFrame*>(media);*/
        // if(vmapFrame)
        {
            /*auto vmap= vmapFrame->getMap();
            vmap->addImageItem(pix.toImage());
            destination= media;*/
        }
    }
    else if(type == Core::ContentType::PICTURE)
    {
        /* auto img= new Image(m_mdiArea);
         auto imgPix= pix.toImage();
         img->setImage(imgPix);
         destination= img;*/
    }
    // if(destination)
    // destination->setUriName(title.arg(sourceName));

    destination->setRemote(false);
    // destination->setCleverUri(new CleverURI(sourceName, "", type));
    // addMediaToMdiArea(destination, true);
}
void MainWindow::focusInEvent(QFocusEvent* event)
{
    QMainWindow::focusInEvent(event);
    if(m_isOut)
    {
        m_gameController->addSearchLog(QStringLiteral("Rolisteam gets focus."));
        m_isOut= false;
    }
}
void MainWindow::focusOutEvent(QFocusEvent* event)
{
    QMainWindow::focusOutEvent(event);
    if(!isActiveWindow())
    {
        m_gameController->addSearchLog(QStringLiteral("User gives focus to another windows."));
        m_isOut= true;
    }
}
