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
#ifdef HAVE_WEBVIEW
#include <QWebEngineSettings>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "charactersheet/charactersheet.h"
#include "controller/contentcontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/playercontroller.h"
#include "core/model/playermodel.h"
#include "data/character.h"
#include "data/person.h"
#include "data/player.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets/customs/shortcutvisitor.h"
#include "rwidgets/customs/workspace.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "rwidgets/dialogs/keygeneratordialog.h"
#include "rwidgets/dialogs/preferencesdialog.h"
#include "rwidgets/dialogs/shortcuteditordialog.h"
#include "rwidgets/dialogs/tipofdayviewer.h"
#include "rwidgets/docks/notificationzone.h"
#include "rwidgets/docks/playerspanel.h"
#include "rwidgets/gmtoolbox/gamemastertool.h"
#include "rwidgets/mediacontainers/mediacontainer.h"
#include "worker/iohelper.h"
#include "worker/networkdownloader.h"
#include "worker/playermessagehelper.h"

#include "data/campaignmanager.h"
#include "rwidgets/dialogs/campaignproperties.h"

// LOG
#include "common/controller/logcontroller.h"
#include "common/controller/remotelogcontroller.h"
#include "common/widgets/logpanel.h"

// Controller
#include "controller/instantmessagingcontroller.h"
#include "controller/networkcontroller.h"
#include "core/controller/view_controller/imageselectorcontroller.h"

// Text editor
#include "rwidgets/dialogs/aboutrolisteam.h"

// GMToolBox
#include "diceparser/qmltypesregister.h"
#include "rwidgets/gmtoolbox/DiceBookMark/dicebookmarkwidget.h"
#include "rwidgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"
#include "rwidgets/gmtoolbox/UnitConvertor/convertor.h"

// session
#include "rwidgets/docks/antagonistboard.h"
#include "rwidgets/docks/campaigndock.h"

MainWindow::MainWindow(GameController* game, const QStringList& args)
    : QMainWindow()
    , m_gameController(game)
#ifndef NULL_PLAYER
    , m_audioPlayer(new AudioPlayer(this))
#endif
    , m_dockLogUtil(new NotificationZone(game->logController(), this))
    , m_systemTray(new QSystemTrayIcon)
    , m_ui(new Ui::MainWindow)
    , m_roomPanelDockWidget(new QDockWidget(this))
{
    parseCommandLineArguments(args);
    setAcceptDrops(true);
    m_systemTray->setIcon(QIcon::fromTheme("500-symbole"));
    m_systemTray->show();

    // ALLOCATIONS
    m_campaignDock.reset(new campaign::CampaignDock(m_gameController->campaign()));

    m_antagonistWidget.reset(new campaign::AntagonistBoard(m_gameController->campaignManager()->editor(), this));

    m_gmToolBoxList.append({new NameGeneratorWidget(this), new GMTOOL::Convertor(this)});
    m_roomPanel= new ChannelListPanel(m_gameController->networkController(), this);

    connect(m_gameController, &GameController::updateAvailableChanged, this, &MainWindow::showUpdateNotification);
    connect(m_gameController, &GameController::tipOfDayChanged, this, &MainWindow::showTipChecker);
    connect(m_gameController, &GameController::localPlayerIdChanged, this,
            [this]() { m_roomPanel->setLocalPlayerId(m_gameController->localPlayerId()); });

    m_ui->setupUi(this);

    m_separatorAction= m_ui->m_fileMenu->insertSeparator(m_ui->m_closeAction);
    m_separatorAction->setVisible(false);
    // registerQmlTypes();

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

    connect(m_ui->m_mediaTitleAct, &QAction::toggled, this, [this](bool b) {
        m_ui->m_toolBar->setToolButtonStyle(b ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly);
    });

    connect(m_gameController->contentController(), &ContentController::sessionChanged, this,
            &MainWindow::setWindowModified);
    connect(m_gameController->contentController(), &ContentController::canPasteChanged, m_ui->m_pasteAct,
            &QAction::setEnabled);
    connect(m_gameController->networkController(), &NetworkController::connectedChanged, this, [this](bool connected) {
        if(connected)
            postConnection();
        // m_dialog->setVisible(!connected);
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

    m_ui->m_pasteAct->setEnabled(m_gameController->contentController()->canPaste());

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
    if(true)
        QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":resources/rolistheme");
    else
        QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":resources/rolistheme-dark");

    readSettings();
}

MainWindow::~MainWindow()= default;

void MainWindow::setupUi()
{
    connect(m_ui->m_showChatAct, &QAction::triggered, m_gameController->instantMessagingController(),
            &InstantMessagingController::setVisible);
    connect(m_gameController->instantMessagingController(), &InstantMessagingController::visibleChanged,
            m_ui->m_showChatAct, &QAction::setChecked);

    auto contentCtrl= m_gameController->contentController();

    connect(m_ui->m_pasteAct, &QAction::triggered, contentCtrl, &ContentController::pasteData);

    m_mdiArea.reset(new Workspace(m_ui->m_toolBar, contentCtrl, m_gameController->instantMessagingController()));
    setCentralWidget(m_mdiArea.get());

    addDockWidget(Qt::RightDockWidgetArea, m_campaignDock.get());
    m_ui->m_menuSubWindows->insertAction(m_ui->m_audioPlayerAct, m_campaignDock->toggleViewAction());

    createNotificationZone();
    ///////////////////
    // PlayerList
    ///////////////////
    auto playersListWidget= new PlayersPanel(m_gameController->playerController(), this);

    addDockWidget(Qt::RightDockWidgetArea, playersListWidget);
    auto dock= new QDockWidget();
    dock->setObjectName("AntagonistTable");
    dock->setWindowTitle(tr("Antagonist Table"));
    dock->setWidget(m_antagonistWidget.get());
    addDockWidget(Qt::RightDockWidgetArea, dock);
    setWindowIcon(QIcon::fromTheme("logo"));
    m_ui->m_menuSubWindows->insertAction(m_ui->m_characterListAct, playersListWidget->toggleViewAction());
    m_ui->m_menuSubWindows->insertAction(m_ui->m_characterListAct, dock->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_characterListAct);

    ///////////////////
    // Audio Player
    ///////////////////
#ifndef NULL_PLAYER
    ReceiveEvent::registerNetworkReceiver(NetMsg::MusicCategory, m_audioPlayer.get());
    addDockWidget(Qt::RightDockWidgetArea, m_audioPlayer.get());
    m_ui->m_menuSubWindows->insertAction(m_ui->m_audioPlayerAct, m_audioPlayer->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_audioPlayerAct);
#endif

    m_preferencesDialog= new PreferencesDialog(m_gameController->preferencesController(), this);
    linkActionToMenu();
}

void MainWindow::closeAllMediaContainer()
{
    auto content= m_gameController->contentController();
    content->clear();
}

void MainWindow::showTipChecker()
{
    auto tip= m_gameController->tipOfDay();
    TipOfDayViewer view(tip.title, tip.content, tip.url, this);
    view.exec();

    m_preferences->registerValue(QStringLiteral("MainWindow::neverDisplayTips"), view.dontshowAgain());
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
void MainWindow::userNatureChange()
{
    updateUi();
    updateWindowTitle();
}

void MainWindow::createNotificationZone()
{
    m_dockLogUtil->setObjectName("dockLogUtil");
    m_dockLogUtil->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_dockLogUtil->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable
                               | QDockWidget::DockWidgetFloatable);

    m_ui->m_menuSubWindows->insertAction(m_ui->m_notificationAct, m_dockLogUtil->toggleViewAction());
    m_ui->m_menuSubWindows->removeAction(m_ui->m_notificationAct);
    addDockWidget(Qt::RightDockWidgetArea, m_dockLogUtil.get());
}

void MainWindow::linkActionToMenu()
{
    connect(m_ui->m_addVectorialMap, &QAction::triggered, this, &MainWindow::newVMap);
    m_ui->m_newNoteAction->setData(QVariant::fromValue(Core::ContentType::NOTES));
    m_ui->m_newNoteAction->setData(QVariant::fromValue(Core::ContentType::SHAREDNOTE));
    m_ui->m_newNoteAction->setData(QVariant::fromValue(Core::ContentType::WEBVIEW));
    m_ui->m_newNoteAction->setData(QVariant::fromValue(Core::ContentType::MINDMAP));

    auto callNewMedia= [this]() {
        auto act= qobject_cast<QAction*>(sender());
        std::map<QString, QVariant> params;
        params.insert({Core::keys::KEY_TYPE, act->data()});
        m_gameController->newMedia(params);
    };

    connect(m_ui->m_newNoteAction, &QAction::triggered, this, callNewMedia);
    connect(m_ui->m_newSharedNote, &QAction::triggered, this, callNewMedia);
    connect(m_ui->m_newWebViewACt, &QAction::triggered, this, callNewMedia);
    connect(m_ui->m_newMindmap, &QAction::triggered, this, callNewMedia);

    // open
    connect(m_ui->m_openPictureAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(m_ui->m_openCharacterSheet, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openVectorialMap, &QAction::triggered, this, []() {
        // TODO open vmap
    });
    connect(m_ui->m_openCampaignAction, &QAction::triggered, this, &MainWindow::openCampaign);
    connect(m_ui->m_openNoteAction, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openShareNote, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openPdfAct, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openMindmap, &QAction::triggered, this, &MainWindow::openGenericContent);

    connect(m_ui->m_shortCutEditorAct, &QAction::triggered, this, &MainWindow::showShortCutEditor);

    m_ui->m_openPictureAction->setData(static_cast<int>(Core::ContentType::PICTURE));
    m_ui->m_openOnlinePictureAction->setData(static_cast<int>(Core::ContentType::ONLINEPICTURE));
    m_ui->m_openCharacterSheet->setData(static_cast<int>(Core::ContentType::CHARACTERSHEET));
    m_ui->m_openVectorialMap->setData(static_cast<int>(Core::ContentType::VECTORIALMAP));
    m_ui->m_openNoteAction->setData(static_cast<int>(Core::ContentType::NOTES));
    m_ui->m_openShareNote->setData(static_cast<int>(Core::ContentType::SHAREDNOTE));
    m_ui->m_openMindmap->setData(static_cast<int>(Core::ContentType::MINDMAP));
#ifdef WITH_PDF
    m_ui->m_openPdfAct->setData(static_cast<int>(Core::ContentType::PDF));
#else
    m_ui->m_openPdfAct->setVisible(false);
#endif
    m_ui->m_recentFileMenu->setVisible(false);
    connect(m_ui->m_closeAction, &QAction::triggered, m_mdiArea.get(), &Workspace::closeActiveSub);
    connect(m_ui->m_saveCampaignAction, &QAction::triggered, m_gameController, &GameController::save);
    connect(m_ui->m_saveCampaignAsAct, &QAction::triggered, this, [this]() {
        QString fileName= QFileDialog::getExistingDirectory(this, tr("Open Campaign"),
                                                            m_gameController->campaign()->rootDirectory());
        if(fileName.isEmpty())
            return;
        m_gameController->saveAs(fileName);
    });
    connect(m_ui->m_preferencesAction, &QAction::triggered, m_preferencesDialog, &PreferencesDialog::show);

    // Campaign
    auto func= [this]() {
        auto s= qobject_cast<QAction*>(sender());
        if(nullptr == s)
            return;
        auto tab= s->data().value<CampaignProperties::Tab>();
        CampaignProperties dialog(m_gameController->campaign(), this);
        dialog.setCurrentTab(tab);
        dialog.exec();
    };
    m_ui->m_diceAliasAct->setData(QVariant::fromValue(CampaignProperties::Tab::Dice));
    m_ui->m_statesAct->setData(QVariant::fromValue(CampaignProperties::Tab::States));
    m_ui->m_campaignPropertiesAct->setData(QVariant::fromValue(CampaignProperties::Tab::Properties));

    connect(m_ui->m_diceAliasAct, &QAction::triggered, this, func);
    connect(m_ui->m_statesAct, &QAction::triggered, this, func);
    connect(m_ui->m_campaignPropertiesAct, &QAction::triggered, this, func);

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
            setMouseTracking(true);
        }
        else
        {
            showNormal();
            menuBar()->setVisible(true);
            m_mdiArea->setMouseTracking(false);
            setMouseTracking(false);
            m_mdiArea->removeAction(m_ui->m_fullScreenAct);
        }
    });

    auto undoStack= m_gameController->undoStack();

    auto redo= undoStack->createRedoAction(this, tr("&Redo"));
    auto undo= undoStack->createUndoAction(this, tr("&Undo"));

    redo->setIcon(QIcon::fromTheme("redo"));
    undo->setIcon(QIcon::fromTheme("undo"));

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);
    auto acts= m_ui->m_toolBar->actions();
    auto firstAct= acts.first();

    m_ui->m_toolBar->insertAction(firstAct, undo);
    m_ui->m_toolBar->insertAction(firstAct, redo);

    connect(undoStack, &QUndoStack::cleanChanged, this, [this](bool clean) { setWindowModified(!clean); });

    m_ui->m_editMenu->insertAction(m_ui->m_shortCutEditorAct, redo);
    m_ui->m_editMenu->insertAction(redo, undo);
    m_ui->m_editMenu->insertSeparator(m_ui->m_shortCutEditorAct);

    // close
    connect(m_ui->m_quitAction, &QAction::triggered, this, &MainWindow::close);

    // network
    connect(m_ui->m_disconnectAction, &QAction::triggered, m_gameController->networkController(),
            &NetworkController::disconnection);
    connect(m_ui->m_connectionLinkAct, &QAction::triggered, this, [this]() {
        QString str("rolisteam://%1/%2/%3");
        auto networkCtrl= m_gameController->networkController();

        auto* clipboard= QGuiApplication::clipboard();
        clipboard->setText(str.arg(networkCtrl->host())
                               .arg(networkCtrl->port())
                               .arg(QString::fromUtf8(networkCtrl->serverPassword().toBase64())));
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
                               .arg(m_preferences->value("Application_Name", "rolisteam").toString(), url),
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

void MainWindow::makeVisible(bool value)
{
    if(value)
        m_preferences->value("FullScreenAtStarting", true).toBool() ? showMaximized() : show();
    else
        QMainWindow::setVisible(value);
}

void MainWindow::newVMap()
{
    MapWizzardDialog mapWizzard(m_mdiArea.get());
    if(mapWizzard.exec())
    {
        std::map<QString, QVariant> params;
        params.insert({QStringLiteral("name"), mapWizzard.name()});
        params.insert({QStringLiteral("permission"), mapWizzard.permission()});
        params.insert({QStringLiteral("bgcolor"), mapWizzard.backgroundColor()});
        params.insert({QStringLiteral("gridSize"), mapWizzard.gridSize()});
        params.insert({QStringLiteral("gridPattern"), QVariant::fromValue(mapWizzard.pattern())});
        params.insert({QStringLiteral("gridColor"), mapWizzard.gridColor()});
        params.insert({QStringLiteral("visibility"), mapWizzard.visibility()});
        params.insert({QStringLiteral("scale"), mapWizzard.scale()});
        params.insert({QStringLiteral("unit"), mapWizzard.unit()});
        params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::VECTORIALMAP)});
        m_gameController->newMedia(params);
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

void MainWindow::openCampaign()
{
    QString fileName
        = QFileDialog::getExistingDirectory(this, tr("Open Campaign"), m_gameController->campaign()->rootDirectory());

    m_gameController->setCampaignRoot(fileName);
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
/*void MainWindow::saveCurrentMedia()
{

    auto content= m_gameController->contentController();
    auto mediaId= content->currentMediaId();
    auto media= content->media(mediaId);

    if(!media)
        return;

    QString dest= media->path();
    QUrl url(dest);
    if(qobject_cast<QAction*>(sender()) == m_ui->m_saveAsAction || dest.isEmpty() || !url.isLocalFile())
    {
        auto type= media->contentType();
        auto filter= CleverURI::getFilterForType(type);
        auto key= m_preferences->value(CleverURI::getPreferenceDirectoryKey(type), QDir::homePath()).toString();
        QFileDialog::getSaveFileName(this, tr("Save %1").arg(media->name()), key, filter);
    }
}*/

void MainWindow::stopReconnection()
{
    m_ui->m_changeProfileAct->setEnabled(true);
    m_ui->m_disconnectAction->setEnabled(false);
}

void MainWindow::setUpNetworkConnection()
{
    connect(m_gameController, &GameController::localIsGMChanged, this, &MainWindow::userNatureChange);
    auto networkCtrl= m_gameController->networkController();
    connect(networkCtrl, &NetworkController::downloadingData, m_dockLogUtil.get(), &NotificationZone::receiveData);
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
    auto isGM= m_gameController->localIsGM();

#ifndef NULL_PLAYER
    m_audioPlayer->updateUi(isGM);
#endif
    m_ui->m_addVectorialMap->setEnabled(isGM);
    m_ui->m_openCampaignAction->setEnabled(isGM);
    m_ui->m_closeAction->setEnabled(isGM);
    m_ui->m_saveCampaignAction->setEnabled(isGM);
    m_ui->m_saveCampaignAsAct->setEnabled(isGM);
    m_ui->m_connectionLinkAct->setVisible(isGM);
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

    restoreState(settings.value("windowState").toByteArray());
    bool maxi= settings.value("Maximized", false).toBool();
    m_ui->m_mediaTitleAct->setChecked(settings.value("show_media_title_in_tool_bar", false).toBool());
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
    settings.setValue("show_media_title_in_tool_bar", m_ui->m_mediaTitleAct->isChecked());

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
    bool hasResetSetting= parser.isSet(reset);

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
    if(hasResetSetting)
    {
        QSettings settings("rolisteam", QString("rolisteam_%1/preferences").arg(m_gameController->version()));
        settings.clear();
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

    setUpNetworkConnection();
    updateWindowTitle();
    updateUi();
}

void MainWindow::openGenericContent()
{
    QAction* action= static_cast<QAction*>(sender());
    Core::ContentType type= static_cast<Core::ContentType>(action->data().toInt());
    QString filter= CleverURI::getFilterForType(type);

    QString folder= m_preferences->value(CleverURI::getPreferenceDirectoryKey(type), ".").toString();
    QString title= tr("Open %1").arg(CleverURI::typeToString(type));
    QStringList filepath= QFileDialog::getOpenFileNames(this, title, folder, filter);
    QStringList list= filepath;

    for(auto const& path : list)
    {
        m_gameController->openMedia(
            {{Core::keys::KEY_PATH, path},
             {Core::keys::KEY_TYPE, QVariant::fromValue(type)},
             {Core::keys::KEY_NAME, IOHelper::shortNameFromPath(path)},
             {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
        // campaignManager->importFile(QUrl::fromLocalFile(path), type);
    }
}

void MainWindow::openImage()
{
    ImageSelectorController ctrl(false, ImageSelectorController::All);
    ImageSelectorDialog dialog(&ctrl, this);
    if(QDialog::Accepted != dialog.exec())
        return;

    std::map<QString, QVariant> args(
        {{Core::keys::KEY_NAME, ctrl.title()},
         {Core::keys::KEY_URL, ctrl.address()},
         {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()},
         {Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::PICTURE)},
         {Core::keys::KEY_DATA, ctrl.finalImageData()}});

    m_gameController->openMedia(args);
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
        // readStory(action->data().toString());
    }
}

void MainWindow::updateRecentScenarioAction()
{
    m_recentScenarios.erase(std::remove_if(m_recentScenarios.begin(), m_recentScenarios.end(),
                                           [](QString const& f) { return !QFile::exists(f); }),
                            m_recentScenarios.end());

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
            action->setIcon(QIcon::fromTheme(CleverURI::typeToIconPath(m_recentFiles[i].type)));
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

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* data= event->mimeData();
    if(!data->hasUrls())
        return;

    // qDebug() << data->formats();
    QList<QUrl> list= data->urls();
    auto contentCtrl= m_gameController->contentController();
    for(const auto& url : list)
    {
        auto path= url.toLocalFile();
        if(!path.isEmpty()) // local file
        {

            Core::ContentType type= CleverURI::extensionToContentType(path);
            if(type == Core::ContentType::UNKNOWN)
                continue;
            qInfo() << QStringLiteral("MainWindow: dropEvent for %1").arg(CleverURI::typeToString(type));
            contentCtrl->openMedia(
                {{Core::keys::KEY_PATH, path},
                 {Core::keys::KEY_TYPE, QVariant::fromValue(type)},
                 {Core::keys::KEY_NAME, IOHelper::shortNameFromPath(path)},
                 {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
        }
        else // remote
        {
            auto urltext= url.toString(QUrl::None);
            Core::ContentType type= CleverURI::extensionToContentType(urltext);
            if(type != Core::ContentType::PICTURE)
                continue;

#ifdef HAVE_QT_NETWORK
            auto downloader= new NetworkDownloader(url);
            connect(downloader, &NetworkDownloader::finished, this,
                    [this, contentCtrl, type, url, urltext, downloader](const QByteArray& data) {
                        contentCtrl->openMedia(
                            {{Core::keys::KEY_PATH, urltext},
                             {Core::keys::KEY_TYPE, QVariant::fromValue(type)},
                             {Core::keys::KEY_DATA, data},
                             {Core::keys::KEY_NAME, IOHelper::shortNameFromPath(url.fileName())},
                             {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
                        downloader->deleteLater();
                    });
            downloader->download();
#endif
        }
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

    // MediaContainer* destination= nullptr;
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

    // destination->setRemote(false);
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
