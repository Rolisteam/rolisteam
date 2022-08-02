/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow_rcse.h"

#include "common/logcontroller.h"
#include "common_widgets/logpanel.h"
#include "dialog/aboutrcse.h"
#include "dialog/codeeditordialog.h"
#include "preferences/preferencesdialog.h"
#include "qmlhighlighter.h"
#include "serializerhelper.h"
#include <QBuffer>
#include <QButtonGroup>
#include <QColorDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPagedPaintDevice>
#include <QPrintDialog>
#include <QPrinter>
#include <QQmlContext>
#include <QQmlError>
#include <QQmlProperty>
#include <QQuickItem>
#include <QTemporaryFile>
#include <QTimer>
#include <QTransform>
#include <QUrl>
#include <QUuid>
#include <QtConcurrent>

// Controller
#include "controllers/charactercontroller.h"
#include "controllers/editorcontroller.h"
#include "controllers/imagecontroller.h"
#include "controllers/qmlgeneratorcontroller.h"

#include "data/characterlist.h"
#include "delegate/pagedelegate.h"

#include "canvasfield.h"
#include "itemeditor.h"

// Undo
#include "charactersheet/worker/ioworker.h"
#include "diceparser_qobject/diceroller.h"
#include "diceparser_qobject/qmltypesregister.h"
#include "undo/addpagecommand.h"
#include "undo/deletefieldcommand.h"
#include "undo/deletepagecommand.h"
#include "undo/setbackgroundimage.h"
#include "undo/setfieldproperties.h"
#include "undo/setpropertyonallcharacters.h"
#include "version.h"

#include "charactersheet/charactersheetmodel.h"

#include <common_widgets/busyindicatordialog.h>

constexpr int minimalColumnSize= 350;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_mainCtrl(new MainController()), m_counterZoom(0)
{
    m_preferences= PreferencesManager::getInstance();
    setWindowModified(false);
    m_qmlGeneration= true;
    setAcceptDrops(true);
    ui->setupUi(this);

    m_separatorAction= ui->m_fileMenu->insertSeparator(ui->m_recentFileAct1);
    m_separatorAction->setVisible(false);

    m_recentActions= {ui->m_recentFileAct1, ui->m_recentFileAct2, ui->m_recentFileAct3, ui->m_recentFileAct4,
                      ui->m_recentFileAct5};
    for(auto act : m_recentActions)
    {
        connect(act, &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    registerQmlTypes();

    connect(ui->m_codeEdit, &CodeEditor::textChanged, this,
            [this]() { m_mainCtrl->generatorCtrl()->setQmlCode(ui->m_codeEdit->toPlainText()); });
    connect(m_mainCtrl->generatorCtrl(), &QmlGeneratorController::qmlCodeChanged, this,
            [this]() { ui->m_codeEdit->setPlainText(m_mainCtrl->generatorCtrl()->qmlCode()); });

    connect(ui->m_quickview->engine(), &QQmlEngine::warnings, m_mainCtrl->generatorCtrl(),
            &QmlGeneratorController::errors);
    connect(ui->m_quickview, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if(status == QQuickWidget::Error)
            m_mainCtrl->displayQmlError(ui->m_quickview->errors());
    });

    auto headerView= ui->m_imageList->horizontalHeader();
    headerView->setStretchLastSection(true);

    connect(m_mainCtrl->imageCtrl()->model(), &charactersheet::ImageModel::rowsInserted, this, [this]() {
        auto view= ui->m_imageList->horizontalHeader();
        view->resizeSections(QHeaderView::ResizeToContents);
    });

    ui->m_imageList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_imageList, &QTableView::customContextMenuRequested, this, &MainWindow::showContextMenuForImageTab);
    connect(ui->m_characterView, &QTreeView::customContextMenuRequested, this,
            &MainWindow::showContextMenuForCharacterTab);

    setUpActionForImageTab();
    setUpActionForCharacterTab();

    ui->m_view->setController(m_mainCtrl->editCtrl());

    ui->m_characterSelectBox->setModel(m_mainCtrl->characterCtrl()->characters());
    connect(m_mainCtrl->characterCtrl()->characters(), &CharacterList::dataChanged, this,
            [this]() { ui->m_characterSelectBox->setCurrentIndex(0); });

    connect(ui->m_characterSelectBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        m_mainCtrl->generatorCtrl()->setUuidCharacter(ui->m_characterSelectBox->currentData().toString());
    });

    ui->m_characterSelectBox->setCurrentIndex(0);

    connect(m_mainCtrl->editCtrl(), &EditorController::pageCountChanged, this, &MainWindow::updatePageSelector);

    // LOG
    QDockWidget* wid= new QDockWidget(tr("Log panel"), this);
    wid->setObjectName(QStringLiteral("logpanel"));
    m_logPanel= new LogPanel(this);
    m_logPanel->setController(m_mainCtrl->logCtrl());
    wid->setWidget(m_logPanel);
    addDockWidget(Qt::BottomDockWidgetArea, wid);
    auto showLogPanel= wid->toggleViewAction();

    ui->treeView->setController(m_mainCtrl.get());

    ui->m_codeToViewBtn->setDefaultAction(ui->m_codeToViewAct);
    ui->m_generateCodeBtn->setDefaultAction(ui->m_genarateCodeAct);

    //////////////////////////////////////
    // end of QAction for view
    //////////////////////////////////////
    /// TODO better management of scene options
    connect(ui->m_showItemIcon, &QAction::triggered, [=](bool triggered) {
        CanvasField::setShowImageField(triggered);
        QList<QRectF> list;
        list << ui->m_view->sceneRect();
        ui->m_view->updateScene(list);
    });

    ////////////////////
    // undo / redo
    ////////////////////
    QAction* undo= m_mainCtrl->undoStack().createUndoAction(this, tr("&Undo"));
    ui->menuEdition->insertAction(ui->m_genarateCodeAct, undo);

    QAction* redo= m_mainCtrl->undoStack().createRedoAction(this, tr("&Redo"));
    ui->menuEdition->insertAction(ui->m_genarateCodeAct, redo);

    ui->menuEdition->addSeparator();
    ui->menuEdition->addAction(showLogPanel);

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);

    connect(ui->m_backgroundImageAct, &QAction::triggered, this, &MainWindow::openBackgroundImage);
    // ui->scrollArea->setWidget(m_view);

    ui->m_addCheckBoxAct->setData(Canvas::ADDCHECKBOX);
    ui->m_addTextAreaAct->setData(Canvas::ADDTEXTAREA);
    ui->m_addTextInputAct->setData(Canvas::ADDINPUT);
    ui->m_addTextFieldAct->setData(Canvas::ADDTEXTFIELD);
    ui->m_addImageAction->setData(Canvas::ADDIMAGE);
    ui->m_addLabelAction->setData(Canvas::ADDLABEL);
    ui->m_functionButtonAct->setData(Canvas::ADDFUNCBUTTON);
    ui->m_tableFieldAct->setData(Canvas::ADDTABLE);
    ui->m_webPageAct->setData(Canvas::ADDWEBPAGE);
    ui->m_nextPageAct->setData(Canvas::NEXTPAGE);
    ui->m_previousPageAct->setData(Canvas::PREVIOUSPAGE);

    ui->m_moveAct->setData(Canvas::MOVE);
    ui->m_moveAct->setShortcut(QKeySequence(Qt::Key_Escape));
    ui->m_moveAct->setChecked(true);
    ui->m_deleteAct->setData(Canvas::DELETETOOL);
    ui->m_addButtonAct->setData(Canvas::BUTTON);

    ui->m_addTextInput->setDefaultAction(ui->m_addTextInputAct);
    ui->m_addTextArea->setDefaultAction(ui->m_addTextAreaAct);
    ui->m_addTextField->setDefaultAction(ui->m_addTextFieldAct);
    ui->m_addCheckbox->setDefaultAction(ui->m_addCheckBoxAct);
    ui->m_addLabelBtn->setDefaultAction(ui->m_addLabelAction);
    ui->m_imageBtn->setDefaultAction(ui->m_addImageAction);
    ui->m_functionBtn->setDefaultAction(ui->m_functionButtonAct);
    ui->m_tableFieldBtn->setDefaultAction(ui->m_tableFieldAct);
    ui->m_webPageBtn->setDefaultAction(ui->m_webPageAct);
    ui->m_nextPageBtn->setDefaultAction(ui->m_nextPageAct);
    ui->m_previousPageBtn->setDefaultAction(ui->m_previousPageAct);

    auto notifyDataChanged= [this]() { m_mainCtrl->setModified(true); };

    connect(m_mainCtrl->editCtrl(), &EditorController::pageAdded, this, [this, notifyDataChanged](Canvas* canvas) {
        canvas->setModel(m_mainCtrl->generatorCtrl()->fieldModel());

        connect(canvas, &Canvas::pixmapChanged, this, notifyDataChanged);
        connect(canvas, &Canvas::pageIdChanged, this, notifyDataChanged);
    });

    // m_editorCtrl->addPage();

    QButtonGroup* group= new QButtonGroup();
    group->addButton(ui->m_addTextInput);
    group->addButton(ui->m_addTextArea);
    group->addButton(ui->m_addTextField);
    group->addButton(ui->m_addTextInput);
    group->addButton(ui->m_addTextArea);
    group->addButton(ui->m_addCheckbox);
    group->addButton(ui->m_addLabelBtn);
    group->addButton(ui->m_addButtonBtn);
    group->addButton(ui->m_imageBtn);
    group->addButton(ui->m_deleteBtn);
    group->addButton(ui->m_moveBtn);
    group->addButton(ui->m_functionBtn);
    group->addButton(ui->m_tableFieldBtn);
    group->addButton(ui->m_webPageBtn);
    group->addButton(ui->m_nextPageBtn);
    group->addButton(ui->m_previousPageBtn);

    ui->m_moveBtn->setDefaultAction(ui->m_moveAct);
    ui->m_deleteBtn->setDefaultAction(ui->m_deleteAct);
    ui->m_addButtonBtn->setDefaultAction(ui->m_addButtonAct);

    QmlHighlighter* highlighter= new QmlHighlighter(ui->m_codeEdit->document());
    highlighter->setObjectName("HighLighterForQML");

    connect(ui->m_sheetProperties, &QAction::triggered, [=](bool) {
        SheetProperties sheetProperties(m_mainCtrl->generatorCtrl(), this);
        sheetProperties.exec();
    });

    connect(ui->m_quitAction, &QAction::triggered, this, &MainWindow::close);

    auto setCurrentTool= [this]() {
        QAction* action= dynamic_cast<QAction*>(sender());
        auto tool= static_cast<Canvas::Tool>(action->data().toInt());
        m_mainCtrl->editCtrl()->setCurrentTool(tool);
    };

    connect(ui->m_addCheckBoxAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_importFromPdf, &QAction::triggered, this, &MainWindow::openPDF);
    connect(ui->m_addTextAreaAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addLabelAction, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addTextFieldAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addTextInputAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addImageAction, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_functionButtonAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_tableFieldAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_webPageAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_nextPageAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_previousPageAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_moveAct, &QAction::triggered, this, [this](bool triggered) { ui->m_view->setHandle(triggered); });
    connect(ui->m_exportPdfAct, &QAction::triggered, this, &MainWindow::exportPDF);
    connect(ui->m_moveAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_deleteAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addButtonAct, &QAction::triggered, this, setCurrentTool);

    connect(ui->m_genarateCodeAct, &QAction::triggered, this, &MainWindow::generateAndShowQML);
    connect(ui->m_codeToViewAct, &QAction::triggered, this, &MainWindow::showQMLFromCode);

    ///////////////////////
    /// @brief action menu
    ///////////////////////
    connect(ui->m_saveAct, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->m_openAct, &QAction::triggered, this, &MainWindow::open);
    connect(ui->m_checkValidityAct, &QAction::triggered, this, [this]() {
        m_mainCtrl->characterCtrl()->checkCharacter(m_mainCtrl->generatorCtrl()->fieldModel()->getRootSection());
    });
    connect(ui->m_addPage, &QPushButton::clicked, this,
            [this]() { m_mainCtrl->processCommand(new AddPageCommand(m_mainCtrl->editCtrl())); });

    connect(ui->m_removePage, &QPushButton::clicked, this, &MainWindow::removePage);
    connect(ui->m_selectPageView, &QListWidget::currentRowChanged, m_mainCtrl->editCtrl(),
            &EditorController::setCurrentPage);
    connect(ui->m_resetIdAct, &QAction::triggered, m_mainCtrl->generatorCtrl()->fieldModel(), &FieldModel::resetAllId);
    connect(ui->m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);

    ui->m_characterView->setModel(m_mainCtrl->characterCtrl()->model());

    ui->m_characterView->setContextMenuPolicy(Qt::CustomContextMenu);

    auto header= ui->m_characterView->header();
    header->setMinimumWidth(minimalColumnSize);
    // header->setSectionResizeMode(0, QHeaderView::Stretch);
    auto resizeSection= [this](int= 0, int newCount= 1) {
        auto header= ui->m_characterView->header();
        if(!header)
            return;
        auto w= header->width() - minimalColumnSize;
        auto count= std::max(newCount - 1, 1);
        auto idealSize= w / count;
        for(int i= 0; i < newCount; ++i)
        {
            if(i == 0)
                header->resizeSection(i, newCount > 1 ? minimalColumnSize : idealSize);
            else
                header->resizeSection(i, idealSize >= minimalColumnSize ? idealSize : minimalColumnSize);
        }
    };
    connect(header, &QHeaderView::sectionCountChanged, this, resizeSection);

    resizeSection();

    connect(ui->m_scaleSlider, &QSlider::valueChanged, this, [this](int val) {
        qreal scale= val / 100.0;
        QTransform transform(scale, 0, 0, scale, 0, 0);
        ui->m_view->setTransform(transform);
    });

    connect(ui->m_newAct, &QAction::triggered, m_mainCtrl.get(), &MainController::cleanUpData);

    connect(ui->m_openLiberapay, &QAction::triggered, this, [this] {
        if(!QDesktopServices::openUrl(QUrl("https://liberapay.com/Rolisteam/donate")))
        {
            QMessageBox* msgBox= new QMessageBox(
                QMessageBox::Information, tr("Support"),
                tr("The %1 donation page can be found online at :<br> <a "
                   "href=\"https://liberapay.com/Rolisteam/donate\">https://liberapay.com/Rolisteam/donate</a>")
                    .arg(m_preferences->value("Application_Name", "rolisteam").toString()),
                QMessageBox::Ok);
            msgBox->exec();
        }
    });

    // Help Menu
    connect(ui->m_aboutRcseAct, &QAction::triggered, this, [this]() {
        AboutRcse dialog(version::version, this);
        dialog.exec();
    });
    connect(ui->m_onlineHelpAct, &QAction::triggered, this, &MainWindow::helpOnLine);

    ui->m_addImageBtn->setDefaultAction(ui->m_addImageAct);
    ui->m_removeImgBtn->setDefaultAction(ui->m_deleteImageAct);

    connect(ui->m_addImageAct, &QAction::triggered, this, &MainWindow::openImage);
    connect(ui->m_deleteImageAct, &QAction::triggered, this, [this]() {
        auto index= ui->m_imageList->currentIndex();
        m_mainCtrl->imageCtrl()->removeImage(index.row());
    });

    readSettings();
    m_logPanel->initSetting();

    m_mainCtrl->cleanUpData();

    ui->m_view->setHandle(ui->m_moveAct->isChecked());
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSettings()
{
    QSettings settings("rolisteam", QString("rcse/preferences"));
    restoreState(settings.value("windowState").toByteArray());
    settings.value("Maximized", false).toBool();
    // if(!maxi)
    {
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    m_recentFiles= settings.value("recentFile").toStringList();
    m_preferences->readSettings(settings);

    updateRecentFileAction();
}
void MainWindow::writeSettings()
{
    QSettings settings("rolisteam", QString("rcse/preferences"));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("Maximized", isMaximized());
    settings.setValue("recentFile", m_recentFiles);
    m_preferences->writeSettings(settings);
}

QString MainWindow::currentFile() const
{
    return m_currentFile;
}

void MainWindow::setCurrentFile(const QString& filename)
{
    setWindowModified(false);
    if(filename == m_currentFile)
        return;

    m_currentFile= filename;
    auto shortName= tr("Untitled");

    if(!m_currentFile.isEmpty())
    {
        shortName= QFileInfo(m_currentFile).fileName();
        m_recentFiles.removeAll(m_currentFile);
        m_recentFiles.prepend(m_currentFile);
        updateRecentFileAction();
    }
    setWindowTitle(QStringLiteral("%1[*] - %2").arg(shortName).arg("RCSE"));
    emit currentFileChanged();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if((obj == ui->m_view) && (event->type() == QEvent::Wheel))
    {
        return wheelEventForView(dynamic_cast<QWheelEvent*>(event));
    }
    else
        return QMainWindow::eventFilter(obj, event);
}
void MainWindow::closeEvent(QCloseEvent* event)
{
    if(mayBeSaved())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void MainWindow::helpOnLine()
{
    if(!QDesktopServices::openUrl(QUrl(version::documation_site)))
    {
        QMessageBox* msgBox= new QMessageBox(QMessageBox::Information, tr("Help"),
                                             tr("Documentation of Rcse can be found online at :<br> <a "
                                                "href=\"%1\">%1</a>")
                                                 .arg(version::documation_site));
        msgBox->exec();
    }
}
bool MainWindow::mayBeSaved()
{
    if(isWindowModified())
    {
        QMessageBox msgBox(this);

        QString message(tr("The charactersheet has unsaved changes."));
        QString msg= QStringLiteral("RCSE");

        msgBox.setIcon(QMessageBox::Question);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.addButton(QMessageBox::Save);
        msgBox.addButton(QMessageBox::Discard);
        msgBox.setWindowTitle(tr("Quit %1 ").arg(msg));

        msgBox.setText(message);
        int value= msgBox.exec();
        if(QMessageBox::Cancel == value)
        {
            return false;
        }
        else if(QMessageBox::Save == value) // saving
        {
            save();
            return true;
        }
        else if(QMessageBox::Discard == value)
        {
            return true;
        }
    }
    return true;
}

bool MainWindow::wheelEventForView(QWheelEvent* event)
{
    if(nullptr == event)
        return false;

    if(event->modifiers() & Qt::ShiftModifier)
    {
        ui->m_view->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        ui->m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Scale the view / do the zoom
        double scaleFactor= 1.1;

        if((event->angleDelta().x() > 0) && (m_counterZoom < 20))
        {
            ui->m_view->scale(scaleFactor, scaleFactor);
            ++m_counterZoom;
        }
        else if(m_counterZoom > -20)
        {
            --m_counterZoom;
            ui->m_view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        ui->m_view->setResizeAnchor(QGraphicsView::NoAnchor);
        ui->m_view->setTransformationAnchor(QGraphicsView::NoAnchor);
        return true;
    }
    return false;
}

void MainWindow::openPDF()
{
    auto pdf= new PdfManager(this);
    if(pdf->exec())
    {
        BusyIndicatorDialog dialog(tr("Image Generation"), tr("Image generation in progress"),
                                   ":/rcstyle/busy_movie.gif", this);

        auto fvoid= QtConcurrent::run([pdf, this, &dialog]() {
            auto imgs= pdf->images();
            m_mainCtrl->editCtrl()->loadImages(imgs);
            pdf->deleteLater();
            dialog.accept();
            dialog.deleteLater();
        });

        dialog.exec();
    }
}

void MainWindow::openBackgroundImage()
{
    QString supportedFormat("Supported files (*.jpg *.png *.xpm);");

    QString img= QFileDialog::getOpenFileName(this, tr("Open Background Image"), QDir::homePath(), supportedFormat);

    if(img.isEmpty())
        return;

    m_mainCtrl->editCtrl()->loadImageFromUrl(QUrl::fromLocalFile(img));
}

void MainWindow::openImage()
{
    QString supportedFormat("Supported files (*.jpg *.png);;All Files (*.*)");
    QString img= QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), supportedFormat);

    if(img.isEmpty())
        return;

    QPixmap map(img);
    m_mainCtrl->imageCtrl()->addImage(map, img);
}

void MainWindow::showPreferences()
{
    PreferencesDialog dialog;
    if(m_preferences->value("hasCustomPath", false).toBool())
    {
        dialog.setGenerationPath(m_preferences->value("GenerationCustomPath", QDir::homePath()).toString());
    }
    if(QDialog::Accepted == dialog.exec())
    {
        m_preferences->registerValue("hasCustomPath", dialog.hasCustomPath());
        m_preferences->registerValue("GenerationCustomPath", dialog.generationPath());
    }
}

bool MainWindow::saveAs()
{
    auto filename= QFileDialog::getSaveFileName(this, tr("Save CharacterSheet"), QDir::homePath(),
                                                tr("Rolisteam CharacterSheet (*.rcs)"));
    if(filename.isEmpty())
        return false;

    if(!filename.endsWith(".rcs"))
    {
        filename.append(QStringLiteral(".rcs"));
    }
    return saveFile(filename);
}

bool MainWindow::save()
{
    if(m_currentFile.isEmpty())
        return saveAs();
    else
        return saveFile(m_currentFile);
}

bool MainWindow::saveFile(const QString& filename)
{
    if(filename.isEmpty())
        return false;

    // init Json
    IOWorker::saveFile(SerializerHelper::buildData(m_mainCtrl.get()), filename);
    return true;
}
bool MainWindow::loadFile(const QString& filename)
{
    if(filename.isEmpty())
        return false;

    SerializerHelper::fetchMainController(m_mainCtrl.get(), IOWorker::readFileToObject(filename));
    m_mainCtrl->setCurrentFile(filename);
    return true;
}
void MainWindow::open()
{
    if(mayBeSaved())
    {
        auto filename= QFileDialog::getOpenFileName(this, tr("Save CharacterSheet"), QDir::homePath(),
                                                    tr("Rolisteam CharacterSheet (*.rcs)"));
        if(!filename.isEmpty())
        {
            loadFile(filename);
        }
    }
}

void MainWindow::openRecentFile()
{
    if(mayBeSaved())
    {
        QAction* act= qobject_cast<QAction*>(sender());
        if(act)
        {
            loadFile(act->data().toString());
        }
    }
}

void MainWindow::updatePageSelector()
{
    auto current= m_mainCtrl->editCtrl()->currentPage();
    ui->m_selectPageView->clear();
    QStringList list;
    auto s= m_mainCtrl->editCtrl()->pageCount();
    for(unsigned int i= 0; i < s; ++i)
    {
        list << QStringLiteral("Page %1").arg(i + 1);
    }
    ui->m_selectPageView->addItems(list);
    ui->m_selectPageView->setCurrentRow(current);
}

void MainWindow::updateRecentFileAction()
{
    m_recentFiles.erase(
        std::remove_if(m_recentFiles.begin(), m_recentFiles.end(), [](QString const& f) { return !QFile::exists(f); }),
        m_recentFiles.end());

    int i= 0;
    for(auto act : m_recentActions)
    {
        if(i >= m_recentFiles.size())
        {
            act->setVisible(false);
            continue;
        }
        else
        {
            act->setVisible(true);
            act->setText(QStringLiteral("&%1  %2").arg(i + 1).arg(QFileInfo(m_recentFiles[i]).fileName()));
            act->setData(m_recentFiles[i]);
        }
        ++i;
    }
    m_separatorAction->setVisible(!m_recentFiles.empty());
}

void MainWindow::generateAndShowQML()
{
    if(m_mainCtrl->generatorCtrl()->textEdited())
    {
        QMessageBox::StandardButton btn= QMessageBox::question(
            this, tr("Do you want to erase current QML code ?"),
            tr("Generate QML code will override any change you made in the QML.<br/>Do you really want to generate QML "
               "code ?"),
            QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);

        if(btn == QMessageBox::Cancel)
        {
            return;
        }
    }
    m_mainCtrl->generatorCtrl()->generateQML(m_mainCtrl->imageCtrl());
    showQMLFromCode();
}

void MainWindow::showQMLFromCode()
{
    auto data= ui->m_codeEdit->toPlainText();

    // setTextEdited(false);
    auto provider= m_mainCtrl->imageCtrl()->provider();

    QTemporaryFile file;
    if(file.open()) // QIODevice::WriteOnly
    {
        file.write(data.toUtf8());
        file.close();
    }

    ui->m_quickview->engine()->clearComponentCache();
    ui->m_quickview->engine()->addImportPath("qrc:/charactersheet/qml");
    ui->m_quickview->engine()->addImageProvider(QLatin1String("rcs"), provider);

    auto charactersheet
        = m_mainCtrl->characterCtrl()->characterSheetFromUuid(m_mainCtrl->generatorCtrl()->uuidCharacter());
    if(nullptr != charactersheet)
    {
        for(int i= 0; i < charactersheet->getFieldCount(); ++i)
        {
            CharacterSheetItem* field= charactersheet->getFieldAt(i);
            if(nullptr != field)
            {
                ui->m_quickview->engine()->rootContext()->setContextProperty(field->getId(), field);
            }
        }
    }
    else
    {
        QList<CharacterSheetItem*> list= m_mainCtrl->generatorCtrl()->fieldModel()->children();
        for(CharacterSheetItem* item : list)
        {
            ui->m_quickview->engine()->rootContext()->setContextProperty(item->getId(), item);
        }
    }

    ui->m_quickview->engine()->rootContext()->setContextProperty("_character",
                                                                 m_mainCtrl->generatorCtrl()->mockCharacter());

    ui->m_quickview->setSource(QUrl::fromLocalFile(file.fileName()));
    m_mainCtrl->displayQmlError(ui->m_quickview->errors());
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QObject* root= ui->m_quickview->rootObject();
    /*connect(root, SIGNAL(showText(QString)), this, SIGNAL(reportLog(QString)));
    connect(root, SIGNAL(rollDiceCmd(QString, bool)), this, SLOT(rollDice(QString, bool)));
    connect(root, SIGNAL(rollDiceCmd(QString)), this, SLOT(rollDice(QString)));*/
}

/*void MainWindow::saveQML()
{
    QString qmlFile= QFileDialog::getOpenFileName(this, tr("Save CharacterSheet View"), QDir::homePath(),
                                                  tr("CharacterSheet View (*.qml)"));
    if(!qmlFile.isEmpty())
    {
        QString data= ui->m_codeEdit->toPlainText();
        generateQML(data);
        ui->m_codeEdit->setPlainText(data);

        QFile file(qmlFile);
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(data.toLatin1());
            file.close();
        }
    }
}*/

void MainWindow::openQML()
{
    QString qmlFile= QFileDialog::getOpenFileName(this, tr("Save CharacterSheet View"), QDir::homePath(),
                                                  tr("Rolisteam CharacterSheet View (*.qml)"));
    if(!qmlFile.isEmpty())
    {
        QFile file(qmlFile);
        if(file.open(QIODevice::ReadOnly))
        {
            QString qmlContent= file.readAll();
            ui->m_codeEdit->setPlainText(qmlContent);
            showQMLFromCode();
        }
    }
}

void MainWindow::addBackgroundImage()
{
    QString supportedFormat("Supported files (*.jpg *.png);;All Files (*.*)");
    QString img= QFileDialog::getOpenFileName(this, tr("Open Background Image"), QDir::homePath(), supportedFormat);
    if(!img.isEmpty())
        return;

    QPixmap map(img);
    m_mainCtrl->imageCtrl()->addBackgroundImage(m_mainCtrl->editCtrl()->currentPage(), map, img, QString());
}

void MainWindow::exportPDF()
{
    QObject* root= ui->m_quickview->rootObject();
    if(nullptr == root)
        return;

    auto maxPage= QQmlProperty::read(root, "maxPage").toInt();
    auto currentPage= QQmlProperty::read(root, "page").toInt();
    auto sheetW= QQmlProperty::read(root, "width").toReal();
    auto sheetH= QQmlProperty::read(root, "height").toReal();

    ui->m_tabWidget->setCurrentWidget(ui->m_qml);

    QObject* imagebg= root->findChild<QObject*>("imagebg");
    if(nullptr != imagebg)
    {
        sheetW= QQmlProperty::read(imagebg, "width").toReal();
        sheetH= QQmlProperty::read(imagebg, "height").toReal();
    }

    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        QPainter painter;
        if(painter.begin(&printer))
        {
            for(int i= 0; i <= maxPage; ++i)
            {
                root->setProperty("page", i);
                ui->m_quickview->repaint();
                QTimer timer;
                timer.setSingleShot(true);
                QEventLoop loop;
                connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
                timer.start(m_preferences->value("waitingTimeBetweenPage", 300).toInt());
                loop.exec();

                auto image= ui->m_quickview->grabFramebuffer();
                QRectF rect(0, 0, printer.width(), printer.height());
                QRectF source(0.0, 0.0, sheetW, sheetH);
                painter.drawImage(rect, image, source);
                if(i != maxPage)
                    printer.newPage();
            }
            painter.end();
        }
    }
    root->setProperty("page", currentPage);
}

void MainWindow::addPage() {}

void MainWindow::removePage()
{
    m_mainCtrl->processCommand(
        new DeletePageCommand(m_mainCtrl->editCtrl(), m_mainCtrl->generatorCtrl()->fieldModel()));
}

void MainWindow::showContextMenuForImageTab()
{
    QMenu menu;

    menu.addAction(m_copyPath);
    menu.addAction(m_copyUrl);
    menu.addSeparator();
    menu.addAction(m_replaceImage);
    menu.addAction(m_removeImage);
    menu.addAction(m_reloadImageFromFile);

    menu.exec(QCursor::pos());
}

void MainWindow::showContextMenuForCharacterTab()
{
    QMenu menu;
    menu.addAction(m_addCharacter);
    // menu.addAction(m_copyCharacter);
    menu.addSeparator();
    menu.addAction(m_applyValueOnAllCharacters);
    menu.addAction(m_applyValueOnSelectedCharacterLines);
    //  menu.addAction(m_applyValueOnAllCharacterLines);
    menu.addAction(m_defineAsTabName);
    menu.addSeparator();
    menu.addAction(m_deleteCharacter);
    menu.exec(QCursor::pos());
}

void MainWindow::setUpActionForImageTab()
{
    ui->m_imageList->setModel(m_mainCtrl->imageCtrl()->model());
#ifndef Q_OS_OSX
    ui->m_imageList->setAlternatingRowColors(true);
#endif
    m_copyPath= new QAction(tr("Copy Path"), this);
    m_copyPath->setShortcut(QKeySequence("CTRL+c"));

    m_copyUrl= new QAction(tr("Copy Url"), this);
    m_copyUrl->setShortcut(QKeySequence("CTRL+u"));

    m_replaceImage= new QAction(tr("Replace Image"), this);
    m_removeImage= new QAction(tr("Remove Image"), this);
    m_reloadImageFromFile= new QAction(tr("Reload image from file"), this);

    connect(m_copyPath, &QAction::triggered, this,
            [this]() { m_mainCtrl->imageCtrl()->copyPath(ui->m_imageList->currentIndex()); });
    connect(m_copyUrl, &QAction::triggered, this,
            [this]() { m_mainCtrl->imageCtrl()->copyUrl(ui->m_imageList->currentIndex()); });
    connect(m_replaceImage, &QAction::triggered, this, [this]() {
        auto filepath= QFileDialog::getOpenFileName(this, tr("Load Image"), QDir::homePath(),
                                                    tr("Supported Image Format (*.jpg *.png *.svg *.gif)"));
        if(filepath.isEmpty())
            return;
        m_mainCtrl->imageCtrl()->replaceImage(ui->m_imageList->currentIndex(), filepath);
    });
    connect(m_removeImage, &QAction::triggered, this,
            [this]() { m_mainCtrl->imageCtrl()->removeImage(ui->m_imageList->currentIndex().row()); });
    connect(m_reloadImageFromFile, &QAction::triggered, this,
            [this]() { m_mainCtrl->imageCtrl()->reloadImage(ui->m_imageList->currentIndex()); });
}

void MainWindow::setUpActionForCharacterTab()
{
    m_addCharacter= new QAction(tr("Add character"), this);
    m_deleteCharacter= new QAction(tr("Delete character"), this);
    // m_copyCharacter= new QAction(tr("Copy character"), this);
    m_defineAsTabName= new QAction(tr("Character's Name"), this);

    m_applyValueOnSelectedCharacterLines= new QAction(tr("Apply on Selection"), this);
    m_applyValueOnAllCharacters= new QAction(tr("Apply on all characters"), this);

    connect(m_addCharacter, &QAction::triggered, m_mainCtrl->characterCtrl(),
            &CharacterController::sendAddCharacterCommand);
    connect(m_deleteCharacter, &QAction::triggered, m_mainCtrl->characterCtrl(),
            [this]() { m_mainCtrl->characterCtrl()->sendRemoveCharacterCommand(ui->m_characterView->currentIndex()); });

    connect(m_applyValueOnAllCharacters, &QAction::triggered, this,
            [this]() { m_mainCtrl->characterCtrl()->applyOnAllCharacter(ui->m_characterView->currentIndex()); });

    connect(m_applyValueOnSelectedCharacterLines, &QAction::triggered, this, [this]() {
        m_mainCtrl->characterCtrl()->applyOnSelection(ui->m_characterView->currentIndex(),
                                                      ui->m_characterView->selectionModel()->selectedIndexes());
    });

    connect(m_defineAsTabName, &QAction::triggered, this, [this]() {
        auto index= ui->m_characterView->currentIndex();
        auto name= index.data().toString();
        if(name.isEmpty())
            return;
        CharacterSheet* sheet= m_mainCtrl->characterCtrl()->model()->getCharacterSheet(index.column() - 1);
        sheet->setName(name);
        // emit dataChanged();
    });
    connect(m_mainCtrl->characterCtrl()->model(), &CharacterSheetModel::columnsInserted, this, [this]() {
        auto count= m_mainCtrl->characterCtrl()->model()->columnCount();
        if(count < 2)
            return;
        auto w= ui->m_characterView->geometry().width() / count;
        for(int i= 0; i < count; ++i)
        {
            ui->m_characterView->setColumnWidth(i, w);
        }
    });
}
