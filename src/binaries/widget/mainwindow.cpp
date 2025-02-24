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
#include "dialogs/vmapwizzarddialog.h"
#include <QApplication>
#include <QBitmap>
#include <QBuffer>
#include <QCommandLineParser>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLoggingCategory>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QQmlEngine>
#include <QSettings>
#include <QStatusBar>
#include <QStringBuilder>
#include <QSystemTrayIcon>
#include <QTime>
#include <QUrl>
#include <QUuid>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <algorithm>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common/logcategory.h"
#include "controller/contentcontroller.h"
#include "controller/playercontroller.h"
#include "data/person.h"
#include "data/player.h"
#include "model/historymodel.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets/customs/shortcutvisitor.h"
#include "rwidgets/customs/workspace.h"
#include "rwidgets/dialogs/historyviewerdialog.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "rwidgets/dialogs/keygeneratordialog.h"
#include "rwidgets/dialogs/preferencesdialog.h"
#include "rwidgets/dialogs/shortcuteditordialog.h"
#include "rwidgets/dialogs/tipofdayviewer.h"
#include "rwidgets/docks/notificationzone.h"
#include "rwidgets/docks/playerspanel.h"
#include "rwidgets/gmtoolbox/gamemastertool.h"

// worker
#include "utils/networkdownloader.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"
#include "worker/utilshelper.h"

#include "data/campaignmanager.h"
#include "model/actiononlistmodel.h"
#include "rwidgets/dialogs/campaignproperties.h"

// Controller
#include "controller/applicationcontroller.h"
#include "controller/instantmessagingcontroller.h"
#include "controller/networkcontroller.h"
#include "controller/preferencescontroller.h"
#include "controller/view_controller/imageselectorcontroller.h"

// dialogs
#include "rwidgets/dialogs/aboutrolisteam.h"
#include "rwidgets/dialogs/campaignintegritydialog.h"
#include "rwidgets/dialogs/importdatafromcampaigndialog.h"
#include "rwidgets/dialogs/newfiledialog.h"

// GMToolBox
#include "rwidgets/gmtoolbox/NameGenerator/namegeneratorwidget.h"
#include "rwidgets/gmtoolbox/UnitConvertor/convertor.h"

// session
#include "rwidgets/docks/antagonistboard.h"
#include "rwidgets/docks/campaigndock.h"
#include "utils/iohelper.h"

Q_LOGGING_CATEGORY(WidgetClient, "WidgetClient")

MainWindow::MainWindow(GameController* game, const QStringList& args)
    : QMainWindow()
    , m_gameController(game)
#ifndef NULL_PLAYER
    , m_audioPlayer(new AudioPlayer(m_gameController->audioController(), this))
#endif
    , m_dockLogUtil(new NotificationZone(game->logController(), this))
    , m_systemTray(new QSystemTrayIcon)
    , m_sideTabs(new QTabWidget())
    , m_ui(new Ui::MainWindow)
{
    parseCommandLineArguments(args);
    setAcceptDrops(true);
    m_systemTray->setIcon(QIcon(":/resources/rolisteam/logo/500-symbole.png"));
    m_systemTray->show();

    // ALLOCATIONS
    m_campaignDock.reset(new campaign::CampaignDock(m_gameController->campaignManager()->editor(),
                                                    m_gameController->preferencesController()));
    connect(m_campaignDock.get(), &campaign::CampaignDock::openResource, m_gameController,
            &GameController::openInternalResources);

    connect(m_campaignDock.get(), &campaign::CampaignDock::removeFile, this,
            [this](const QString& path)
            {
                auto campaignManager= m_gameController->campaignManager();
                campaignManager->removeFile(path);
            });

    connect(m_gameController->campaignManager(), &campaign::CampaignManager::createBlankFile, this,
            [this](const QString& path, Core::MediaType mediaType)
            {
                std::map<QString, QVariant> params;
                /// TODO change mediaType to ContentType.
                params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(mediaType)});
                params.insert({Core::keys::KEY_PATH, path});
                m_gameController->newMedia(params);
            });

    /*connect(m_gameController->campaignManager(), &campaign::CampaignManager::createBlankFile, this,
            [this](const QString& path, Core::MediaType mediaType)
            {
                std::map<QString, QVariant> params;
                /// TODO change mediaType to ContentType.
                params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(mediaType)});
                params.insert({Core::keys::KEY_PATH, path});
                m_gameController->newMedia(params);
            });*/

    connect(m_gameController, &GameController::dataLoaded, this,
            [this](const QStringList missingFiles, const QStringList unmanagedFile)
            {
                if(missingFiles.isEmpty() && unmanagedFile.isEmpty())
                    return;
                auto ctrl= new campaign::CampaignIntegrityController(missingFiles, unmanagedFile,
                                                                     m_gameController->campaignManager());
                campaign::CampaignIntegrityDialog dialog(ctrl, this);
                dialog.exec();
                /*if(val == QDialog::Accepted)
                {
                    auto const& missingActions= dialog.missingFileActions();
                    auto const& unmanagedActions= dialog.unmanagedFileActions();
                    auto ctrl= m_gameController->campaignManager();

                    QList<QPair<QString, Core::CampaignAction>> list;
                    std::transform(std::begin(missingActions), std::end(missingActions),
                std::back_inserter(list),
                                   [](const DataInfo& info)
                                   {
                                       return QPair<QString, Core::CampaignAction>(
                                           {info.data, info.indexAction == 0 ?
                Core::CampaignAction::ForgetAction : Core::CampaignAction::CreateAction});
                                   });
                    ctrl->performAction(list);

                    QList<QPair<QString, Core::CampaignAction>> list2;
                    std::transform(std::begin(unmanagedActions), std::end(unmanagedActions),
                std::back_inserter(list2),
                                   [](const DataInfo& info)
                                   {
                                       return QPair<QString, Core::CampaignAction>(
                                           {info.data, info.indexAction == 0 ?
                Core::CampaignAction::ManageAction : Core::CampaignAction::DeleteAction});
                                   });
                    ctrl->performAction(list2);
                }*/
            });

    m_antagonistWidget.reset(new campaign::AntagonistBoard(m_gameController->campaignManager()->editor()));

    m_gmToolBoxList.append({new NameGeneratorWidget(this), new GMTOOL::Convertor(this)});
    m_roomPanel
        = new ChannelListPanel(m_gameController->preferencesManager(), m_gameController->networkController(), this);

    connect(m_gameController, &GameController::localIsGMChanged, this,
            [this]()
            {
                updateUi();
                updateWindowTitle();
            });
    connect(m_gameController, &GameController::updateAvailableChanged, this, &MainWindow::showUpdateNotification);
    connect(m_gameController, &GameController::tipOfDayChanged, this, &MainWindow::showTipChecker);
    connect(m_gameController, &GameController::localPlayerIdChanged, this,
            [this]() { m_roomPanel->setLocalPlayerId(m_gameController->localPlayerId()); });

    m_ui->setupUi(this);

    m_separatorAction= m_ui->m_fileMenu->insertSeparator(m_ui->m_closeAction);
    m_separatorAction->setVisible(false);

#ifdef HAVE_WEBVIEW
    auto defaultProfile= QWebEngineProfile::defaultProfile();
    QWebEngineSettings* defaultSettings= defaultProfile->settings();
    defaultSettings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    defaultSettings->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
#endif

    m_preferences= m_gameController->preferencesManager();

    auto func= [](QVariant var)
    {
        auto v= var.toInt();
        if(var.isNull())
            v= 6;
        VisualItem::setHighlightWidth(v);
    };
    m_preferences->registerLambda(QStringLiteral("VMAP::highlightPenWidth"), func);
    auto func2= [](QVariant var)
    {
        auto v= var.value<QColor>();
        if(!v.isValid())
            v= QColor(Qt::red);
        VisualItem::setHighlightColor(v);
    };
    m_preferences->registerLambda(QStringLiteral("VMAP::highlightColor"), func2);

    connect(m_ui->m_mediaTitleAct, &QAction::toggled, this,
            [this](bool b)
            { m_ui->m_toolBar->setToolButtonStyle(b ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly); });

    connect(m_gameController->contentController(), &ContentController::sessionChanged, this,
            &MainWindow::setWindowModified);
    connect(m_gameController->contentController(), &ContentController::canPasteChanged, m_ui->m_pasteAct,
            &QAction::setEnabled);

    connect(m_gameController->networkController(), &NetworkController::connectedChanged, this,
            [this](bool connected)
            {
                if(connected)
                    postConnection();

                updateWindowTitle();
                m_ui->m_changeProfileAct->setEnabled(connected);
                m_ui->m_disconnectAction->setEnabled(connected);
            });

    connect(m_gameController->campaign(), &campaign::Campaign::nameChanged, this, &MainWindow::updateWindowTitle);

    // connect(m_sessionManager, &SessionManager::openResource, this,
    // &MainWindow::openResource);
    connect(m_gameController->instantMessagingController(), &InstantMessagingController::unreadChanged, this,
            [this]()
            {
                auto ctrl= m_gameController->instantMessagingController();
                m_ui->m_showChatAct->setIcon(ctrl->unread() ? QIcon::fromTheme("chaticon_with_message") :
                                                              QIcon::fromTheme("chaticon"));
            });

    m_ui->m_pasteAct->setEnabled(m_gameController->contentController()->canPaste());

    connect(m_ui->m_keyGeneratorAct, &QAction::triggered, this,
            []()
            {
                KeyGeneratorDialog dialog;
                dialog.exec();
            });

    setupUi();
    readSettings();
    connect(m_ui->m_cleanHistoryAct, &QAction::triggered, m_gameController->contentController(),
            &ContentController::clearHistory);

    connect(m_gameController->contentController(), &ContentController::historyChanged, this,
            &MainWindow::updateFileHistoryMenu);
    updateFileHistoryMenu();

    qmlRegisterSingletonType<ApplicationController>("Helper", 1, 0, "AppCtrl",
                                                    [this](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject*
                                                    {
                                                        Q_UNUSED(engine);
                                                        Q_UNUSED(scriptEngine);
                                                        return new ApplicationController(m_gameController);
                                                    });

    m_ui->m_showDice3D->setIcon(QIcon(":/dice3d/icons/dice3d.svg"));
    connect(m_ui->m_showDice3D, &QAction::triggered, this,
            [this]()
            {
                auto ctrl= m_gameController->dicePhysicController();
                if(!ctrl)
                    return;
                ctrl->setDisplayed(m_ui->m_showDice3D->isChecked());
            });
    auto ctrl= m_gameController->dicePhysicController();
    connect(ctrl, &Dice3DController::displayedChanged, this,
            [this]()
            {
                auto ctrl= m_gameController->dicePhysicController();
                m_ui->m_showDice3D->setChecked(ctrl->displayed());
            });
    m_ui->m_showDice3D->setChecked(ctrl->displayed());
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

    m_mdiArea.reset(new Workspace(m_ui->m_toolBar, contentCtrl, m_gameController->instantMessagingController(),
                                  m_gameController->dicePhysicController()));
    setCentralWidget(m_mdiArea.get());

    setWindowIcon(QIcon(":/resources/rolisteam/logo/500-symbole.png"));

    ///////////////////
    // Audio Player
    ///////////////////

    m_preferencesDialog= new PreferencesDialog(m_gameController->preferencesController(), this);
    linkActionToMenu();
    createTabs();
}

void MainWindow::moveEvent(QMoveEvent* event)
{
    QMainWindow::moveEvent(event);
    m_mdiArea->updateDicePanelGeometry();
}

void MainWindow::changeEvent(QEvent* event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
        auto stateEvent= static_cast<QWindowStateChangeEvent*>(event);
        static bool oldState= true;
        if(windowState() & Qt::WindowMinimized)
        {
            auto diceCtrl= m_gameController->dicePhysicController();
            oldState= diceCtrl->displayed();
            if(oldState)
                diceCtrl->setDisplayed(false);
        }
        else if(stateEvent->oldState() & Qt::WindowMinimized)
        {
            auto diceCtrl= m_gameController->dicePhysicController();
            diceCtrl->setDisplayed(oldState);
        }
    }
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
    qDebug() << "closeEvent";
    auto i= mayBeSaved();
    if(QMessageBox::Save == i)
    { // GM
        qDebug() << "inside if closeEvent";
        m_gameController->aboutToClose();
        writeSettings();
        event->accept();
    }
    else if(QMessageBox::Yes == i)
    { // Player
        writeSettings();
        event->accept();
    }
    else if(QMessageBox::Discard == i)
    { // GM
        event->accept();
    }
    else
    { // GM and Player
        qDebug() << "inside else closeEvent";
        event->ignore();
    }
}
void MainWindow::userNatureChange()
{
    updateUi();
    updateWindowTitle();
}

void MainWindow::createTabs()
{
    QDockWidget* dock= new QDockWidget(this);
    dock->setObjectName(tr("SidePanels"));
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(m_sideTabs.get());
    m_ui->m_menuSubWindows->addSeparator();
    auto sideTabsAct= dock->toggleViewAction();
    sideTabsAct->setText(tr("Side Panels"));
    m_ui->m_menuSubWindows->addAction(sideTabsAct);
    m_ui->m_menuSubWindows->addSeparator();

    connect(m_ui->m_roomListAct, &QAction::triggered, dock, &QDockWidget::setVisible);

    m_sideTabs->setIconSize(QSize(32, 32));

    auto playersListWidget
        = new PlayersPanel(m_gameController->playerController(), m_gameController->preferencesManager(), this);
    m_sideTabs->addTab(playersListWidget, QIcon::fromTheme("01_players"), tr("List of Players"));
    m_sideTabs->addTab(m_campaignDock.get(), QIcon::fromTheme("02_campaign2"), tr("All campaign documents"));
    m_sideTabs->addTab(m_antagonistWidget.get(), QIcon::fromTheme("03_antagonist"), tr("List of antagonists"));
    m_sideTabs->addTab(m_audioPlayer.get(), QIcon::fromTheme("04_music"), tr("Music Player"));
    m_sideTabs->addTab(m_roomPanel, QIcon::fromTheme("05_rooms"), tr("Server Panel"));
    m_sideTabs->addTab(m_dockLogUtil.get(), QIcon::fromTheme("06_logger"), tr("Notification Zone"));

    m_audioPlayer->setVisible(true);
    QWidget* wid= new QWidget(this);
    auto vBox= new QVBoxLayout();
    wid->setLayout(vBox);

    for(auto tool : m_gmToolBoxList)
    {
        auto wid= dynamic_cast<QWidget*>(tool);
        if(!wid)
            continue;
        vBox->addWidget(wid);
    }

    m_sideTabs->addTab(wid, QIcon::fromTheme("07_tools"), tr("GM Tools"));

    for(int i= 0; i < m_sideTabs->count(); ++i)
    {
        auto act= new QAction(m_sideTabs->tabIcon(i), m_sideTabs->tabText(i), this);
        m_sideTabs->setTabToolTip(i, m_sideTabs->tabText(i));
        m_sideTabs->setTabText(i, {});
        act->setData(i);
        act->setCheckable(true);
        act->setChecked(m_sideTabs->isTabVisible(i));
        m_ui->m_menuSubWindows->addAction(act);
        connect(act, &QAction::toggled, this,
                [i, this]() { m_sideTabs->setTabVisible(i, !m_sideTabs->isTabVisible(i)); });
    }

    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::linkActionToMenu()
{
    connect(m_ui->m_addVectorialMap, &QAction::triggered, this, &MainWindow::newVMap);
    m_ui->m_newNoteAction->setData(QVariant::fromValue(Core::ContentType::NOTES));
    m_ui->m_newSharedNote->setData(QVariant::fromValue(Core::ContentType::SHAREDNOTE));
    m_ui->m_openWebViewAct->setData(QVariant::fromValue(Core::ContentType::WEBVIEW));
    m_ui->m_newMindmap->setData(QVariant::fromValue(Core::ContentType::MINDMAP));

    auto callNewMedia= [this]()
    {
        auto act= qobject_cast<QAction*>(sender());
        std::map<QString, QVariant> params;
        if(!m_gameController->localIsGM())
        {
            NewFileDialog dial(act->data().value<Core::ContentType>());
            if(QDialog::Rejected == dial.exec())
                return;
            params.insert({Core::keys::KEY_TYPE, static_cast<int>(dial.type())});
            params.insert({Core::keys::KEY_URL, dial.url()});
            params.insert({Core::keys::KEY_NAME, dial.name()});
        }
        else
        {
            params.insert({Core::keys::KEY_TYPE, act->data()});
        }

        m_gameController->newMedia(params);
    };

    connect(m_ui->m_newNoteAction, &QAction::triggered, this, callNewMedia);
    connect(m_ui->m_newSharedNote, &QAction::triggered, this, callNewMedia);
    connect(m_ui->m_newMindmap, &QAction::triggered, this, callNewMedia);

    // open
    connect(m_ui->m_openPictureAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(m_ui->m_openCharacterSheet, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openWebViewAct, &QAction::triggered, this,
            [this]()
            {
                auto str= QInputDialog::getText(this, tr("Open Webpage"), tr("Webpage url:"));
                if(str.isEmpty())
                    return;
                m_gameController->openPageWeb(str);
            });
    connect(m_ui->m_openVectorialMap, &QAction::triggered, this,
            [this]()
            {
                auto str= QFileDialog::getOpenFileName(this, tr("Open Map"), tr("Map:"));
                if(str.isEmpty())
                    return;
                std::map<QString, QVariant> params;
                params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::VECTORIALMAP)});
                QUrl url= QUrl::fromUserInput(str);
                params.insert({Core::keys::KEY_NAME, url.fileName()});
                params.insert({Core::keys::KEY_PATH, str});
                m_gameController->newMedia(params);
            });
    connect(m_ui->m_openCampaignAction, &QAction::triggered, this,
            [this]()
            {
                m_gameController->setCampaignRoot(QFileDialog::getExistingDirectory(
                    this, tr("Open Campaign"), m_gameController->campaign()->rootDirectory()));
            });
    connect(m_ui->m_openNoteAction, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openShareNote, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openPdfAct, &QAction::triggered, this, &MainWindow::openGenericContent);
    connect(m_ui->m_openMindmap, &QAction::triggered, this, &MainWindow::openGenericContent);

    connect(m_ui->m_importFromCampaign, &QAction::triggered, this,
            [this]()
            {
                ImportDataFromCampaignDialog dialog(m_gameController->campaignRoot(), this);

                if(dialog.exec() == QDialog::Accepted)
                {
                    /// TODO call controller to import data.
                    auto src= dialog.source();
                    auto cats= dialog.categoryList();
                    auto manager= m_gameController->campaignManager();
                    manager->importDataFrom(src, cats);
                }
            });

    connect(m_ui->m_shortCutEditorAct, &QAction::triggered, this, &MainWindow::showShortCutEditor);

    m_ui->m_openPictureAction->setData(static_cast<int>(Core::ContentType::PICTURE));
    // m_ui->m_openOnlinePictureAction->setData(static_cast<int>(Core::ContentType::ONLINEPICTURE));
    m_ui->m_openCharacterSheet->setData(static_cast<int>(Core::ContentType::CHARACTERSHEET));
    m_ui->m_openVectorialMap->setData(static_cast<int>(Core::ContentType::VECTORIALMAP));
    m_ui->m_openNoteAction->setData(static_cast<int>(Core::ContentType::NOTES));
    m_ui->m_openShareNote->setData(static_cast<int>(Core::ContentType::SHAREDNOTE));
    m_ui->m_openMindmap->setData(static_cast<int>(Core::ContentType::MINDMAP));
    m_ui->m_openPdfAct->setData(static_cast<int>(Core::ContentType::PDF));

    // m_ui->m_recentFileMenu->setVisible(false);
    connect(m_ui->m_closeAction, &QAction::triggered, m_mdiArea.get(), &Workspace::closeActiveSub);
    connect(m_ui->m_saveCampaignAction, &QAction::triggered, m_gameController, &GameController::save);
    connect(m_ui->m_saveCampaignAsAct, &QAction::triggered, this,
            [this]()
            {
                QString fileName= QFileDialog::getExistingDirectory(this, tr("Open Campaign"),
                                                                    m_gameController->campaign()->rootDirectory());
                if(fileName.isEmpty())
                    return;
                m_gameController->saveAs(fileName);
            });
    connect(m_ui->m_preferencesAction, &QAction::triggered, m_preferencesDialog,
            [this]() { m_preferencesDialog->show(); });

    // Campaign
    auto func= [this]()
    {
        auto s= qobject_cast<QAction*>(sender());
        if(nullptr == s)
            return;
        auto tab= s->data().value<CampaignProperties::Tab>();
        CampaignProperties dialog(m_gameController->campaign(), m_gameController->preferencesController()->themeModel(),
                                  this);
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

    connect(m_ui->m_fullScreenAct, &QAction::triggered, this,
            [=](bool enable)
            {
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
            &NetworkController::stopConnection);
    connect(m_ui->m_connectionLinkAct, &QAction::triggered, this,
            [this]()
            {
                QString str("rolisteam://%1/%2/%3");
                auto networkCtrl= m_gameController->networkController();

                auto* clipboard= QGuiApplication::clipboard();
                clipboard->setText(str.arg(networkCtrl->host())
                                       .arg(networkCtrl->port())
                                       .arg(QString::fromUtf8(networkCtrl->serverPassword().toBase64())));
            });

    //  Help
    connect(m_ui->m_aboutAction, &QAction::triggered, this,
            [this]()
            {
                AboutRolisteam diag(m_gameController->version(), this);
                diag.exec();
            });
    connect(m_ui->m_onlineHelpAction, &QAction::triggered, this,
            [this]()
            {
                if(!QDesktopServices::openUrl(QUrl(version::documation_site)))
                {
                    QMessageBox* msgBox
                        = new QMessageBox(QMessageBox::Information, tr("Help"),
                                          tr("Documentation of %1 can be found online at :<br> <a "
                                             "href=\"%2\">%2</a>")
                                              .arg(m_preferences->value("Application_Name", "rolisteam").toString())
                                              .arg(version::documation_site),
                                          QMessageBox::Ok);
                    msgBox->exec();
                }
            });

    m_ui->m_supportRolisteam->setData(version::liberapay_site);
    m_ui->m_patreon->setData(version::patreon_site);
    connect(m_ui->m_supportRolisteam, &QAction::triggered, this, &MainWindow::showSupportPage);
    connect(m_ui->m_patreon, &QAction::triggered, this, &MainWindow::showSupportPage);

    connect(m_ui->m_manageHistoryAct, &QAction::triggered, this,
            [this]()
            {
                m_ignoreUpdate= true;
                auto ctrl= m_gameController->contentController();
                HistoryViewerDialog dialog(ctrl->historyModel(), this);
                dialog.exec();
                m_ignoreUpdate= false;
                updateFileHistoryMenu();
            });
}

void MainWindow::showSupportPage()
{
    auto act= qobject_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    QString url= act->data().toString();
    if(!QDesktopServices::openUrl(QUrl(url)))
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
    MapWizzardDialog mapWizzard(m_preferences, m_mdiArea.get());
    if(mapWizzard.exec())
    {
        std::map<QString, QVariant> params;
        params.insert({Core::keys::KEY_NAME, mapWizzard.name()});
        params.insert({Core::keys::KEY_PERMISSION, mapWizzard.permission()});
        params.insert({Core::keys::KEY_BGCOLOR, mapWizzard.backgroundColor()});
        params.insert({Core::keys::KEY_GRIDSIZE, mapWizzard.gridSize()});
        params.insert({Core::keys::KEY_GRIDPATTERN, QVariant::fromValue(mapWizzard.pattern())});
        params.insert({Core::keys::KEY_GRIDCOLOR, mapWizzard.gridColor()});
        params.insert({Core::keys::KEY_VISIBILITY, mapWizzard.visibility()});
        params.insert({Core::keys::KEY_SCALE, mapWizzard.scale()});
        params.insert({Core::keys::KEY_UNIT, mapWizzard.unit()});
        params.insert({Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::VECTORIALMAP)});
        m_gameController->newMedia(params);
    }
}

int MainWindow::mayBeSaved()
{
    QMessageBox msgBox(this);
    Qt::WindowFlags flags= msgBox.windowFlags();
    msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
    msgBox.setModal(true);

    QString msg= m_preferences->value("Application_Name", "rolisteam").toString();
    auto isGM= m_gameController->localIsGM();
    msgBox.setText(isGM ? tr("Do you want to save the campaign ?") : tr("Do you really want to quit Rolisteam ?"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.addButton(0 ? QMessageBox::Save : QMessageBox::Yes);
    if(isGM)
        msgBox.addButton(QMessageBox::Discard);
    msgBox.addButton(isGM ? QMessageBox::Cancel : QMessageBox::No);
    msgBox.setWindowTitle(tr("Quit %1 ").arg(msg));

    return msgBox.exec();
}

void MainWindow::openCampaign()
{
    QString fileName
        = QFileDialog::getExistingDirectory(this, tr("Open Campaign"), m_gameController->campaign()->rootDirectory());

    m_gameController->setCampaignRoot(fileName);
}

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

void MainWindow::updateUi()
{
    auto isGM= m_gameController->localIsGM();
    m_ui->m_addVectorialMap->setEnabled(isGM);
    m_ui->m_openVectorialMap->setEnabled(isGM);
    m_ui->m_openCharacterSheet->setEnabled(isGM);

    m_ui->m_openCampaignAction->setEnabled(isGM);
    m_ui->m_closeAction->setEnabled(isGM);
    m_ui->m_saveCampaignAction->setEnabled(isGM);
    m_ui->m_saveCampaignAsAct->setEnabled(isGM);
    m_ui->m_connectionLinkAct->setVisible(m_gameController->networkController()->hosting());
    m_ui->m_changeProfileAct->setEnabled(false);
    m_ui->m_disconnectAction->setEnabled(true);

    auto dice3d= m_gameController->dicePhysicController();
    dice3d->setReady(true);

    if(isGM)
    {
        auto act= m_ui->m_historyMenu->menuAction();
        m_ui->menubar->removeAction(act);
    }
    else
    {
        auto act= m_ui->m_campaignMenu->menuAction();
        m_ui->menubar->removeAction(act);

        static QList<int> gmOnlyTab{1, 2, 6};
        auto acts= m_ui->m_menuSubWindows->actions();
        for(auto idx : gmOnlyTab)
        {
            auto it= std::find_if(std::begin(acts), std::end(acts),
                                  [idx](QAction* act) { return act->data().toInt() == idx; });
            if(it != std::end(acts))
                (*it)->setVisible(false);
            m_sideTabs->setTabVisible(idx, false);
        }

        QSettings settings(Core::settings::KEY_ROLISTEAM,
                           QString(Core::settings::KEY_PREF_DIR).arg(m_gameController->version()));
        auto bytes= settings.value(Core::settings::KEY_PLAYER_DICE3D, QByteArray()).toByteArray();

        if(!bytes.isEmpty())
            ModelHelper::fetchDice3d(dice3d, bytes);
    }
}
void MainWindow::showUpdateNotification()
{
    QMessageBox::information(this, tr("Update Notification"),
                             tr("The %1 version has been released. "
                                "Please take a look at <a "
                                "href=\"https://www.rolisteam.org/download\">Download page</a> for "
                                "more "
                                "information")
                                 .arg(m_gameController->remoteVersion()));
}

void MainWindow::notifyAboutAddedPlayer(Player* player) const
{
    m_gameController->addFeatureLog(tr("%1 just joins the game.").arg(player->name()), logger::network);
    if(player->userVersion().compare(m_gameController->version()) != 0)
    {
        m_gameController->addErrorLog(
            tr("%1 has not the right version: %2.").arg(player->name(), player->userVersion()), logger::network);
    }
}

void MainWindow::notifyAboutDeletedPlayer(Player* player) const
{
    m_gameController->addFeatureLog(tr("%1 just leaves the game.").arg(player->name()), logger::network);
}

void MainWindow::readSettings()
{
    QSettings settings(Core::settings::KEY_ROLISTEAM,
                       QString(Core::settings::KEY_PREF_DIR).arg(m_gameController->version()));

    restoreState(settings.value(Core::settings::KEY_WINDOW_STATE).toByteArray());
    bool maxi= settings.value(Core::settings::KEY_MAXIMIZED, false).toBool();
    m_ui->m_mediaTitleAct->setChecked(settings.value(Core::settings::KEY_MEDIA_TITLE, false).toBool());
    if(!maxi)
    {
        restoreGeometry(settings.value(Core::settings::KEY_GEOMETRY).toByteArray());
    }

    m_antagonistWidget->setFullMode(settings.value(Core::settings::KEY_ANTA_FULLMODE, false).toBool());
    m_antagonistWidget->setMinimalMode(settings.value(Core::settings::KEY_ANTA_MINIMODE, true).toBool());

    m_sideTabs->setCurrentIndex(settings.value(Core::settings::KEY_CURRENT_TAB, 0).toInt());

    auto size= settings.beginReadArray(Core::settings::KEY_ANTA_ARRAY);
    auto& acts= m_antagonistWidget->columnsActions();
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        auto& act= acts[i];
        act->setChecked(settings.value(Core::settings::KEY_ANTA_COL, true).toBool());
    }

    // read recent scenario
    auto ctrl= m_gameController->contentController();
    SettingsHelper::readHistoryModel(ctrl->historyModel());

    m_dockLogUtil->initSetting();
    m_audioPlayer->updateState();
}
void MainWindow::writeSettings()
{
    QSettings settings(Core::settings::KEY_ROLISTEAM,
                       QString(Core::settings::KEY_PREF_DIR).arg(m_gameController->version()));
    settings.setValue(Core::settings::KEY_GEOMETRY, saveGeometry());
    settings.setValue(Core::settings::KEY_WINDOW_STATE, saveState());
    settings.setValue(Core::settings::KEY_MAXIMIZED, isMaximized());
    settings.setValue(Core::settings::KEY_MEDIA_TITLE, m_ui->m_mediaTitleAct->isChecked());

    settings.setValue(Core::settings::KEY_ANTA_MINIMODE, m_antagonistWidget->minimalMode());
    settings.setValue(Core::settings::KEY_ANTA_FULLMODE, m_antagonistWidget->fullMode());
    settings.setValue(Core::settings::KEY_CURRENT_TAB, m_sideTabs->currentIndex());

    settings.beginWriteArray(Core::settings::KEY_ANTA_ARRAY);
    auto& acts= m_antagonistWidget->columnsActions();
    int i= 0;
    for(auto const& act : acts)
    {
        settings.setArrayIndex(i);
        i++;
        settings.setValue(Core::settings::KEY_ANTA_COL, act->isChecked());
    }
    settings.endArray();

    auto ctrl= m_gameController->contentController();
    if(!m_gameController->localIsGM())
    {
        SettingsHelper::writeHistoryModel(ctrl->historyModel());
        settings.setValue(Core::settings::KEY_PLAYER_DICE3D,
                          ModelHelper::buildDice3dData(m_gameController->dicePhysicController()));
    }
    for(auto& gmtool : m_gmToolBoxList)
    {
        gmtool->writeSettings();
    }
}
void MainWindow::parseCommandLineArguments(const QStringList& list)
{
    if(list.isEmpty())
        return;
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

    /*ChannelListPanel* roomPanel= qobject_cast<ChannelListPanel*>(m_roomPanelDockWidget->widget());
    if(nullptr != roomPanel)
    {
        roomPanel->cleanUp();
    }*/
}

void MainWindow::postConnection()
{
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

    QString folder= m_preferences->value(Core::preferences::KEY_OPEN_DIRECTORY, QDir::homePath()).toString();
    QString title= tr("Open %1").arg(helper::utils::typeToString(type));
    QStringList filepath= QFileDialog::getOpenFileNames(this, title, folder, helper::utils::filterForType(type, false));
    QStringList list= filepath;

    for(auto const& path : list)
    {
        m_gameController->openMedia(
            {{Core::keys::KEY_URL, QUrl::fromUserInput(path)},
             {Core::keys::KEY_TYPE, QVariant::fromValue(type)},
             {Core::keys::KEY_NAME, utils::IOHelper::shortNameFromPath(path)},
             {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
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

void MainWindow::updateFileHistoryMenu()
{
    if(m_ignoreUpdate)
        return;
    if(m_gameController->localIsGM())
        return;

    auto const& ctrl= m_gameController->contentController();
    auto const& model= ctrl->historyModel();
    auto const& data= model->data();

    m_ui->m_historyMenu->clear();
    m_ui->m_historyMenu->addAction(m_ui->m_manageHistoryAct);
    m_ui->m_historyMenu->addAction(m_ui->m_cleanHistoryAct);
    m_ui->m_historyMenu->addSeparator();
    std::for_each(std::begin(data), std::end(data),
                  [this](const history::LinkInfo& info)
                  {
                      auto action= new QAction(info.displayName, this);
                      action->setData(info.id);
                      action->setIcon(QIcon(helper::utils::typeToIconPath(info.type)));
                      m_ui->m_historyMenu->addAction(action);
                      connect(action, &QAction::triggered, this, &MainWindow::openFileFromHistory);
                  });

    SettingsHelper::writeHistoryModel(ctrl->historyModel());
}

void MainWindow::openFileFromHistory()
{
    auto act= qobject_cast<QAction*>(sender());
    auto id= act->data().toString();
    auto const& ctrl= m_gameController->contentController();
    auto const& model= ctrl->historyModel();
    auto info= model->idToPath(id);
    if(info.url.isEmpty())
        return;

    std::map<QString, QVariant> map{{Core::keys::KEY_URL, info.url},
                                    {Core::keys::KEY_PATH, info.url},
                                    {Core::keys::KEY_NAME, info.displayName},
                                    {Core::keys::KEY_TYPE, QVariant::fromValue(info.type)}};
    if(info.type == Core::ContentType::PICTURE)
    {
        ImageSelectorController* ctrl= new ImageSelectorController(false, ImageSelectorController::All);
        connect(ctrl, &ImageSelectorController::imageDataChanged, this,
                [this, ctrl, map]()
                {
                    std::map<QString, QVariant> map2(map);
                    auto data= ctrl->finalImageData();
                    map2.insert({Core::keys::KEY_DATA, data});
                    m_gameController->openMedia(map2);
                    ctrl->deleteLater();
                });
        ctrl->downloadImageFrom(info.url);
    }
    else
    {
        m_gameController->openMedia(map);
    }
}

void MainWindow::updateWindowTitle()
{
    auto networkCtrl= m_gameController->networkController();

    auto const connectionStatus= m_gameController->connected() ? tr("Connected") : tr("Not Connected");
    auto const networkStatus= networkCtrl->hosting() ? tr("Server") : tr("Client");

    if(networkCtrl->isGM())
    {
        auto camp= m_gameController->campaign();
        setWindowTitle(QStringLiteral("%6[*] - v%2 - %3 - %4 - %5 - %1")
                           .arg(m_preferences->value("applicationName", "Rolisteam").toString(),
                                m_gameController->version(), connectionStatus, networkStatus, tr("GM"), camp->name()));
    }
    else
    {
        setWindowTitle(QStringLiteral("%1 - v%2[*] - %3 - %4 - %5")
                           .arg(m_preferences->value("applicationName", "Rolisteam").toString(),
                                m_gameController->version(), connectionStatus, networkStatus, tr("Player")));
    }

    setWindowIcon(QIcon(":/resources/rolisteam/logo/500-symbole.png"));
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if(event->mimeData()->hasUrls() || event->mimeData()->hasImage())
    {
        event->acceptProposedAction();
    }
    QMainWindow::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* data= event->mimeData();
    if(!data->hasUrls() && !data->hasImage())
        return;

    qCDebug(WidgetClient) << "data has url and image" << data->hasImage() << data->hasUrls();

    if(data->hasImage())
    {
        auto contentCtrl= m_gameController->contentController();
        auto img= qvariant_cast<QImage>(data->imageData());
        qDebug() << img.isNull() << img;

        auto name= tr("Unknown");
        if(data->hasText())
        {
            name= data->text();
        }

        contentCtrl->openMedia(
            {{Core::keys::KEY_TYPE, QVariant::fromValue(Core::ContentType::PICTURE)},
             {Core::keys::KEY_DATA, IOHelper::imageToData(img)},
             {Core::keys::KEY_NAME, name},
             {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
    }
    else if(data->hasUrls())
    {
        QList<QUrl> list= data->urls();
        auto contentCtrl= m_gameController->contentController();
        for(const auto& url : list)
        {
            auto path= url.toLocalFile();
            if(!path.isEmpty()) // local file
            {

                Core::ContentType type= helper::utils::extensionToContentType(path);
                if(type == Core::ContentType::UNKNOWN)
                    continue;
                qCInfo(WidgetClient)
                    << QStringLiteral("MainWindow: dropEvent for %1").arg(helper::utils::typeToString(type));
                contentCtrl->openMedia(
                    {{Core::keys::KEY_URL, url},
                     {Core::keys::KEY_TYPE, QVariant::fromValue(type)},
                     {Core::keys::KEY_NAME, utils::IOHelper::shortNameFromPath(path)},
                     {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
            }
            else // remote
            {
                auto urltext= url.toString(QUrl::None);
                Core::ContentType type= helper::utils::extensionToContentType(urltext);
                if(type != Core::ContentType::PICTURE)
                    continue;

#ifdef HAVE_QT_NETWORK
                auto downloader= new NetworkDownloader(url);
                connect(
                    downloader, &NetworkDownloader::finished, this,
                    [this, contentCtrl, type, url, urltext, downloader](const QByteArray& data)
                    {
                        contentCtrl->openMedia(
                            {{Core::keys::KEY_URL, urltext},
                             {Core::keys::KEY_PATH, urltext},
                             {Core::keys::KEY_TYPE, QVariant::fromValue(type)},
                             {Core::keys::KEY_DATA, data},
                             {Core::keys::KEY_NAME, utils::IOHelper::shortNameFromPath(url.fileName())},
                             {Core::keys::KEY_OWNERID, m_gameController->playerController()->localPlayer()->uuid()}});
                        downloader->deleteLater();
                    });
                downloader->download();
#endif
            }
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

void MainWindow::focusInEvent(QFocusEvent* event)
{
    QMainWindow::focusInEvent(event);
    if(m_isOut)
    {
        m_gameController->addSearchLog(QStringLiteral("Rolisteam gets focus."), logger::usability);
        m_isOut= false;
    }
}
void MainWindow::focusOutEvent(QFocusEvent* event)
{
    QMainWindow::focusOutEvent(event);
    if(!isActiveWindow())
    {
        m_gameController->addSearchLog(QStringLiteral("User gives focus to another windows."), logger::usability);
        m_isOut= true;
    }
}
