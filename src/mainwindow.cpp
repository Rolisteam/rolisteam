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
#include "ui_mainwindow.h"

#include "common/controller/logcontroller.h"
#include "common/widgets/logpanel.h"
#include <QBuffer>
#include <QButtonGroup>
#include <QClipboard>
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
#include <QOpenGLWidget>
#include <QPagedPaintDevice>
#include <QPrintDialog>
#include <QPrinter>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>
#include <QQmlProperty>
#include <QQuickItem>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrl>
#include <QUuid>

#ifdef WITH_PDF
#include <poppler-qt5.h>
#endif

#include "aboutrcse.h"
#include "codeeditordialog.h"
#include "preferencesdialog.h"
#include "qmlhighlighter.h"

#include "delegate/pagedelegate.h"

// Undo
#include "undo/addcharactercommand.h"
#include "undo/addpagecommand.h"
#include "undo/deletecharactercommand.h"
#include "undo/deletefieldcommand.h"
#include "undo/deletepagecommand.h"
#include "undo/setbackgroundimage.h"
#include "undo/setfieldproperties.h"
#include "undo/setpropertyonallcharacters.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentPage(0)
    , m_editedTextByHand(false)
    , m_counterZoom(0)
    , m_pdf(nullptr)
    , m_undoStack(new QUndoStack(this))
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
    m_view= new ItemEditor(this);

    m_editorCtrl.reset(new EditorController(m_view));

    connect(m_editorCtrl.get(), &EditorController::pageCountChanged, this, &MainWindow::updatePageSelector);

    // LOG
    m_logManager= new LogController(true, this);
    m_logManager->setCurrentModes(LogController::Gui);
    QDockWidget* wid= new QDockWidget(tr("Log panel"), this);
    wid->setObjectName(QStringLiteral("logpanel"));
    m_logPanel= new LogPanel(m_logManager);
    wid->setWidget(m_logPanel);
    addDockWidget(Qt::BottomDockWidgetArea, wid);
    auto showLogPanel= wid->toggleViewAction();

    m_imageModel= new ImageModel(m_pixList);
    m_model= new FieldModel();
    connect(m_model, SIGNAL(modelChanged()), this, SLOT(modelChanged()));
    ui->treeView->setFieldModel(m_model);
    ui->treeView->setCurrentPage(&m_currentPage);
    // ui->treeView->setCanvasList(&m_canvasList);
    ui->treeView->setUndoStack(&m_undoStack);

    // DeletePageCommand::setPagesModel(AddPageCommand::getPagesModel());
    // connect(AddPageCommand::getPagesModel(), SIGNAL(modelReset()), this, SLOT(pageCountChanged()));

    ui->m_codeToViewBtn->setDefaultAction(ui->m_codeToViewAct);
    ui->m_generateCodeBtn->setDefaultAction(ui->m_genarateCodeAct);

    //////////////////////////////////////
    // end of QAction for view
    //////////////////////////////////////

    connect(ui->m_showItemIcon, &QAction::triggered, [=](bool triggered) {
        CanvasField::setShowImageField(triggered);
        QList<QRectF> list;
        list << m_view->sceneRect();
        m_view->updateScene(list);
    });

    ////////////////////
    // undo / redo
    ////////////////////

    QAction* undo= m_undoStack.createUndoAction(this, tr("&Undo"));
    ui->menuEdition->insertAction(ui->m_genarateCodeAct, undo);

    QAction* redo= m_undoStack.createRedoAction(this, tr("&Redo"));
    ui->menuEdition->insertAction(ui->m_genarateCodeAct, redo);

    ui->menuEdition->addSeparator();
    ui->menuEdition->addAction(showLogPanel);

    undo->setShortcut(QKeySequence::Undo);
    redo->setShortcut(QKeySequence::Redo);

    connect(ui->m_backgroundImageAct, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    ui->scrollArea->setWidget(m_view);

    ui->m_addCheckBoxAct->setData(Canvas::ADDCHECKBOX);
    ui->m_addTextAreaAct->setData(Canvas::ADDTEXTAREA);
    ui->m_addTextInputAct->setData(Canvas::ADDINPUT);
    ui->m_addTextFieldAct->setData(Canvas::ADDTEXTFIELD);
    ui->m_addImageAction->setData(Canvas::ADDIMAGE);
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
    ui->m_imageBtn->setDefaultAction(ui->m_addImageAction);
    ui->m_functionBtn->setDefaultAction(ui->m_functionButtonAct);
    ui->m_tableFieldBtn->setDefaultAction(ui->m_tableFieldAct);
    ui->m_webPageBtn->setDefaultAction(ui->m_webPageAct);
    ui->m_nextPageBtn->setDefaultAction(ui->m_nextPageAct);
    ui->m_previousPageBtn->setDefaultAction(ui->m_previousPageAct);

    connect(m_editorCtrl.get(), &EditorController::pageAdded, this, [this](Canvas* canvas) {
        connect(canvas, &Canvas::pixmapChanged, this, &MainWindow::setImage);
        canvas->setModel(m_model);
        canvas->setUndoStack(&m_undoStack);
        canvas->setImageModel(m_imageModel);
    });

    m_editorCtrl->addPage();

    QButtonGroup* group= new QButtonGroup();
    group->addButton(ui->m_addTextInput);
    group->addButton(ui->m_addTextArea);
    group->addButton(ui->m_addTextField);
    group->addButton(ui->m_addTextInput);
    group->addButton(ui->m_addTextArea);
    group->addButton(ui->m_addCheckbox);
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

    m_sheetProperties= new SheetProperties(this);

    connect(ui->m_sheetProperties, &QAction::triggered, [=](bool) {
        m_sheetProperties->setAdditionalHeadCode(m_additionnalHeadCode);
        m_sheetProperties->setAdditionalBottomCode(m_additionnalBottomCode);
        m_sheetProperties->setAdditionalImport(m_additionnalImport);
        m_sheetProperties->setFixedScale(m_fixedScaleSheet);
        m_sheetProperties->setNoAdaptation(m_flickableSheet);

        if(QDialog::Accepted == m_sheetProperties->exec())
        {
            m_additionnalHeadCode= m_sheetProperties->getAdditionalHeadCode();
            m_additionnalBottomCode= m_sheetProperties->getAdditionalBottomCode();
            m_fixedScaleSheet= m_sheetProperties->getFixedScale();
            m_additionnalImport= m_sheetProperties->getAdditionalImport();
            m_flickableSheet= m_sheetProperties->isNoAdaptation();
        }
    });

    connect(ui->m_quitAction, &QAction::triggered, this, &MainWindow::close);

    auto setCurrentTool= [this]() {
        QAction* action= dynamic_cast<QAction*>(sender());
        auto tool= static_cast<Canvas::Tool>(action->data().toInt());
        m_editorCtrl->setCurrentTool(tool);
    };

    connect(ui->m_addCheckBoxAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addTextAreaAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addTextFieldAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addTextInputAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addImageAction, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_functionButtonAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_tableFieldAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_webPageAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_nextPageAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_previousPageAct, &QAction::triggered, this, setCurrentTool);

    connect(ui->m_moveAct, &QAction::triggered, [=](bool triggered) { m_view->setHandle(triggered); });

    connect(ui->m_exportPdfAct, &QAction::triggered, this, &MainWindow::exportPDF);

    connect(ui->m_moveAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_deleteAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addButtonAct, &QAction::triggered, this, setCurrentTool);

    connect(ui->m_genarateCodeAct, SIGNAL(triggered(bool)), this, SLOT(showQML()));
    connect(ui->m_codeToViewAct, SIGNAL(triggered(bool)), this, SLOT(showQMLFromCode()));

    ///////////////////////
    /// @brief action menu
    ///////////////////////
    connect(ui->m_saveAct, SIGNAL(triggered(bool)), this, SLOT(save()));
    connect(ui->actionSave_As, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(ui->m_openAct, SIGNAL(triggered(bool)), this, SLOT(open()));
    connect(ui->m_checkValidityAct, SIGNAL(triggered(bool)), this, SLOT(checkCharacters()));
    connect(ui->m_addPage, SIGNAL(clicked(bool)), this, SLOT(addPage()));
    connect(ui->m_removePage, SIGNAL(clicked(bool)), this, SLOT(removePage()));
    connect(ui->m_selectPageCb, QOverload<int>::of(&QComboBox::currentIndexChanged), m_editorCtrl.get(),
            &EditorController::currentPageChanged);
    connect(ui->m_resetIdAct, SIGNAL(triggered(bool)), m_model, SLOT(resetAllId()));
    connect(ui->m_preferencesAction, SIGNAL(triggered(bool)), this, SLOT(showPreferences()));

    m_imgProvider= new RolisteamImageProvider();
    m_addCharacter= new QAction(tr("Add character"), this);

    m_characterModel= new CharacterSheetModel();
    connect(m_characterModel, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)), this, SLOT(modelChanged()));
    connect(m_characterModel, SIGNAL(columnsInserted(QModelIndex, int, int)), this, SLOT(columnAdded()));
    ui->m_characterView->setModel(m_characterModel);
    m_characterModel->setRootSection(m_model->getRootSection());
    ui->m_characterView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_characterView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(menuRequested(QPoint)));
    connect(m_addCharacter, &QAction::triggered,
            [&]() { m_undoStack.push(new AddCharacterCommand(m_characterModel)); });

    connect(ui->m_scaleSlider, &QSlider::valueChanged, this, [this](int val) {
        qreal scale= val / 100.0;
        QTransform transform(scale, 0, 0, 0, scale, 0, 0, 0);
        m_view->setTransform(transform);
    });

    connect(ui->m_newAct, &QAction::triggered, this, &MainWindow::clearData);

    connect(ui->m_openLiberapay, &QAction::triggered, [=] {
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

    m_editorCtrl->setCurrentTool(Canvas::MOVE);

    // Character table
    m_deleteCharacter= new QAction(tr("Delete character"), this);
    m_copyCharacter= new QAction(tr("Copy character"), this);
    m_defineAsTabName= new QAction(tr("Character's Name"), this);

    m_applyValueOnAllCharacterLines= new QAction(tr("Apply on all lines"), this);
    m_applyValueOnSelectedCharacterLines= new QAction(tr("Apply on Selection"), this);
    m_applyValueOnAllCharacters= new QAction(tr("Apply on all characters"), this);

    connect(ui->m_codeEdit, SIGNAL(textChanged()), this, SLOT(codeChanged()));

    // Help Menu
    connect(ui->m_aboutRcseAct, SIGNAL(triggered(bool)), this, SLOT(aboutRcse()));
    connect(ui->m_onlineHelpAct, SIGNAL(triggered()), this, SLOT(helpOnLine()));

    ui->m_imageList->setModel(m_imageModel);

    ui->m_imageList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_imageList, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(menuRequestedForImageModel(QPoint)));

    m_imageModel->setImageProvider(m_imgProvider);
    auto* view= ui->m_imageList->horizontalHeader();
    view->setStretchLastSection(true);
    connect(m_imageModel, &ImageModel::rowsInserted, this, [this]() {
        auto* view= ui->m_imageList->horizontalHeader();
        view->resizeSections(QHeaderView::ResizeToContents);
    });

#ifndef Q_OS_OSX
    ui->m_imageList->setAlternatingRowColors(true);
#endif

    ui->m_addImageBtn->setDefaultAction(ui->m_addImageAct);
    ui->m_removeImgBtn->setDefaultAction(ui->m_deleteImageAct);

    connect(ui->m_addImageAct, SIGNAL(triggered(bool)), this, SLOT(addImage()));
    connect(ui->m_deleteImageAct, &QAction::triggered, this, [=]() {
        auto index= ui->m_imageList->currentIndex();
        m_imageModel->removeImageAt(index);
    });

    //////////////////////////////////////////
    ///
    /// contextual action for image model
    ///
    //////////////////////////////////////////
    m_copyPath= new QAction(tr("Copy Path"), ui->m_imageList);
    m_copyPath->setShortcut(QKeySequence("CTRL+c"));
    m_copyPath->setData(1);

    m_replaceImage= new QAction(tr("Change Image"), ui->m_imageList);

    m_copyUrl= new QAction(tr("Copy Url"), ui->m_imageList);
    m_copyUrl->setShortcut(QKeySequence("CTRL+u"));
    m_copyUrl->setData(0);

    ui->m_imageList->addAction(m_copyPath);
    ui->m_imageList->addAction(m_copyUrl);
    connect(m_copyPath, &QAction::triggered, this, &MainWindow::copyPath);
    connect(m_copyUrl, &QAction::triggered, this, &MainWindow::copyPath);
    ui->m_imageList->addAction(m_replaceImage);
    connect(m_replaceImage, &QAction::triggered, this, [this]() {
        auto index= ui->m_imageList->currentIndex();
        auto filepath= QFileDialog::getOpenFileName(this, tr("Load Image"), QDir::homePath(),
                                                    tr("Supported Image Format (*.jpg *.png *.svg *.gif)"));
        if(filepath.isEmpty())
            return;
        m_imageModel->setPathFor(index, filepath);
    });

    readSettings();
    m_logPanel->initSetting();

    // TODO REMOVE THIS LINE
    /*   auto pixMapDebug = new
       QPixmap("/home/renaud/documents/rolisteam/fiche/L5R-4E-Character-Sheet-LoRes-page-001.jpg");
       SetBackgroundCommand* cmd = new SetBackgroundCommand(canvas,pixMapDebug);
       m_undoStack.push(cmd);*/
    clearData();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::checkCharacters()
{
    m_characterModel->checkCharacter(m_model->getRootSection());
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
void MainWindow::clearData(bool addDefaultCanvas)
{
    m_additionnalHeadCode= "";
    m_additionnalImport= "";
    m_fixedScaleSheet= 1.0;
    m_additionnalBottomCode= "";
    m_flickableSheet= false;

    setCurrentUuid(QUuid::createUuid().toString(QUuid::WithoutBraces));
    m_editorCtrl->clearData(addDefaultCanvas);

    m_imageModel->clear();
    m_model->clearModel();
    m_characterModel->clearModel();
    ui->m_codeEdit->clear();
    setCurrentFile(QString());
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if((obj == m_view) && (event->type() == QEvent::Wheel))
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
    if(!QDesktopServices::openUrl(QUrl("http://wiki.rolisteam.org/")))
    {
        QMessageBox* msgBox= new QMessageBox(QMessageBox::Information, tr("Help"),
                                             tr("Documentation of Rcse can be found online at :<br> <a "
                                                "href=\"http://wiki.rolisteam.org\">http://wiki.rolisteam.org/</a>"));
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

void MainWindow::modelChanged()
{
    if((nullptr != m_characterModel) && (nullptr != m_model))
    {
        m_characterModel->setRootSection(m_model->getRootSection());
    }
    setWindowModified(true);
}
bool MainWindow::wheelEventForView(QWheelEvent* event)
{
    if(nullptr == event)
        return false;

    if(event->modifiers() & Qt::ShiftModifier)
    {
        m_view->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Scale the view / do the zoom
        double scaleFactor= 1.1;

        if((event->delta() > 0) && (m_counterZoom < 20))
        {
            m_view->scale(scaleFactor, scaleFactor);
            ++m_counterZoom;
        }
        else if(m_counterZoom > -20)
        {
            --m_counterZoom;
            m_view->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        m_view->setResizeAnchor(QGraphicsView::NoAnchor);
        m_view->setTransformationAnchor(QGraphicsView::NoAnchor);
        return true;
    }
    return false;
}
void MainWindow::openPDF()
{
#ifdef WITH_PDF
    Poppler::Document* document= Poppler::Document::load(m_pdfPath);
    if(nullptr == document || document->isLocked())
    {
        QMessageBox::warning(this, tr("Error! this PDF file can not be read!"),
                             tr("This PDF document can not be read: %1").arg(m_pdfPath), QMessageBox::Ok);
        delete document;
        return;
    }
    if(nullptr != m_pdf)
    {
        qreal res= m_pdf->getDpi();
        m_imageModel->clear();
        QString id= QUuid::createUuid().toString();
        static int lastCanvas= static_cast<int>(m_editorCtrl->pageCount()) - 1;

        QSize previous;
        if(m_pdf->hasResolution())
        {
            previous.setHeight(m_pdf->getHeight());
            previous.setWidth(m_pdf->getWidth());
        }
        for(int i= 0; i < document->numPages(); ++i)
        {
            Poppler::Page* pdfPage= document->page(i); // Document starts at page 0
            if(nullptr == pdfPage)
            {
                QMessageBox::warning(this, tr("Error! This PDF file seems empty!"),
                                     tr("This PDF document has no page."), QMessageBox::Ok);
                return;
            }
            // Generate a QImage of the rendered page

            QImage image= pdfPage->renderToImage(res, res); // xres, yres, x, y, width, height
            if(image.isNull())
            {
                QMessageBox::warning(this, tr("Error! Can not make image!"),
                                     tr("System has failed while making image of the pdf page."), QMessageBox::Ok);
                return;
            }
            QPixmap pix; //= new QPixmap()
            if(!m_pdf->hasResolution())
            {
                m_pdf->setWidth(image.size().width());
                m_pdf->setHeight(image.size().height());
            }
            if(!previous.isValid())
            {
                previous= image.size();
                pix= QPixmap::fromImage(image);
            }
            else if(previous != image.size())
            {
                pix= QPixmap::fromImage(
                    image.scaled(previous.width(), previous.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else
            {
                pix= QPixmap::fromImage(image);
            }

            if(!pix.isNull())
            {
                SetBackgroundCommand* cmd= new SetBackgroundCommand(i, m_editorCtrl.get(), pix);
                m_undoStack.push(cmd);
                // m_editorCtrl->setImageBackground(i, pix);
                /*if(i >= m_canvasList.size())
                {
                    addPage();
                }
                if(i < m_canvasList.size())
                {
                    Canvas* canvas= m_canvasList[i];
                    if(nullptr != canvas)
                    {
                        canvas->setPixmap(pix);

                        QString key= QStringLiteral("%2_background_%1.jpg").arg(lastCanvas + i).arg(id);
                        m_imageModel->insertImage(pix, key, QString("From PDF"), true);
                    }
                }*/
            }
            delete pdfPage;
        }
    }
    delete document;
#endif
}
void MainWindow::managePDFImport()
{
    m_pdf= new PdfManager(this);
    connect(m_pdf, SIGNAL(apply()), this, SLOT(openPDF()));
    connect(m_pdf, SIGNAL(accepted()), this, SLOT(openPDF()));
    openPDF();
    m_pdf->exec();
}

void MainWindow::openImage()
{
#ifdef WITH_PDF
    QString supportedFormat("Supported files (*.jpg *.png *.xpm *.pdf);;All Files (*.*)");
#else
    QString supportedFormat("Supported files (*.jpg *.png);;All Files (*.*)");
#endif
    QString img= QFileDialog::getOpenFileName(this, tr("Open Background Image"), QDir::homePath(), supportedFormat);

    if(img.isEmpty())
        return;

    if(img.endsWith("pdf"))
    {
        // openPDF(img);
        m_pdfPath= img;
        managePDFImport();
        return;
    }

    QPixmap pix(img);
    if(pix.isNull())
        return;

    SetBackgroundCommand* cmd= new SetBackgroundCommand(m_editorCtrl->currentPage(), m_editorCtrl.get(), pix);
    m_undoStack.push(cmd);
    /*QString id= QUuid::createUuid().toString();
    QString key= QStringLiteral("%2_background_%1.jpg").arg(m_currentPage).arg(id);*/
    // m_imageModel->insertImage(pix, key, img, true);
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

void MainWindow::menuRequested(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);

    QModelIndex index= ui->m_characterView->currentIndex();

    menu.addAction(m_addCharacter);
    // menu.addAction(m_copyCharacter);
    menu.addSeparator();
    menu.addAction(m_applyValueOnAllCharacters);
    menu.addAction(m_applyValueOnSelectedCharacterLines);
    //  menu.addAction(m_applyValueOnAllCharacterLines);
    menu.addAction(m_defineAsTabName);
    menu.addSeparator();
    menu.addAction(m_deleteCharacter);
    QAction* act= menu.exec(QCursor::pos());

    if(act == m_deleteCharacter)
    {
        DeleteCharacterCommand* cmd= new DeleteCharacterCommand(index, m_characterModel);
        m_undoStack.push(cmd);
    }
    else if(act == m_defineAsTabName)
    {
        QString name= index.data().toString();
        if(!name.isEmpty())
        {
            CharacterSheet* sheet= m_characterModel->getCharacterSheet(index.column() - 1);
            sheet->setName(name);
        }
    }
    else if(act == m_applyValueOnAllCharacters)
    {
        QString value= index.data().toString();
        QString formula= index.data(Qt::UserRole).toString();
        auto characterItem= m_characterModel->indexToSection(index);
        if((!value.isEmpty()) && (nullptr != characterItem))
        {
            QString key= characterItem->getId();
            SetPropertyOnCharactersCommand* cmd
                = new SetPropertyOnCharactersCommand(key, value, formula, m_characterModel);
            m_undoStack.push(cmd);
        }
    }
    else if(act == m_applyValueOnSelectedCharacterLines)
    {
        applyValueOnCharacterSelection(index, true, false);
    } /// TODO these functions
    else if(act == m_applyValueOnAllCharacterLines)
    {
    }
    else if(act == m_copyCharacter)
    {
    }
}

void MainWindow::applyValueOnCharacterSelection(QModelIndex& index, bool selection, bool allCharacter)
{
    Q_UNUSED(allCharacter);
    if(!index.isValid())
        return;

    if(selection)
    {
        QVariant var= index.data(Qt::DisplayRole);
        QVariant editvar= index.data(Qt::EditRole);
        if(editvar != var)
        {
            var= editvar;
        }
        int col= index.column();
        QModelIndexList list= ui->m_characterView->selectionModel()->selectedIndexes();
        for(QModelIndex modelIndex : list)
        {
            if(modelIndex.column() == col)
            {
                m_characterModel->setData(modelIndex, var, Qt::EditRole);
            }
        }
    }
}

void MainWindow::menuRequestedForImageModel(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);

    QModelIndex index= ui->m_imageList->currentIndex();
    if(index.isValid())
    {
        menu.addAction(m_copyPath);
        menu.addAction(m_copyUrl);
        menu.addSeparator();
        menu.addAction(m_replaceImage);
        menu.addAction(ui->m_deleteImageAct);

        m_copyUrl->setEnabled(index.column() == 0);
        m_copyPath->setEnabled(index.column() == 1);
    }

    menu.exec(QCursor::pos());
}
void MainWindow::copyPath()
{
    auto act= dynamic_cast<QAction*>(sender());
    if(nullptr == act)
        return;

    QModelIndex index= ui->m_imageList->currentIndex();
    if(index.column() == act->data().toInt())
    {
        QString path= index.data().toString();
        QClipboard* clipboard= QGuiApplication::clipboard();
        clipboard->setText(path);
    }
}

void MainWindow::columnAdded()
{
    int col= m_characterModel->columnCount();
    ui->m_characterView->resizeColumnToContents(col - 2);
}
// Todo move to editor controller
void MainWindow::setImage()
{
    /*  int i= 0;
      m_imageModel->clear();
      QSize previous;
      bool issue= false;
      for(auto canvas : m_canvasList)
      {
          QPixmap* pix= canvas->pixmap();
          if(pix != nullptr)
          {
              if(!previous.isValid())
              {
                  previous= pix->size();
              }
              if(previous != pix->size())
              {
                  issue= true;
              }
              m_editorCtrl->setFitInView();
          }
          else if(nullptr == pix)
          {
              pix= new QPixmap();
          }
          QString idList= QStringLiteral("%2_background_%1.jpg").arg(i).arg(m_currentUuid);
          m_imageModel->insertImage(pix, idList, "from canvas", true);
          ++i;
      }
      if(issue)
      {
          QMessageBox::warning(this, tr("Error!"), tr("Background images have to be of the same size"),
      QMessageBox::Ok);
      }*/
}

bool MainWindow::saveAs()
{
    auto filename= QFileDialog::getSaveFileName(this, tr("Save CharacterSheet"), QDir::homePath(),
                                                tr("Rolisteam CharacterSheet (*.rcs)"));
    if(!filename.isEmpty())
    {
        if(!filename.endsWith(".rcs"))
        {
            filename.append(QStringLiteral(".rcs"));
        }
        return saveFile(filename);
    }
    return false;
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
    if(!filename.isEmpty())
    {

        // init Json
        QJsonDocument json;
        QJsonObject obj;

        // Get datamodel
        QJsonObject data;
        m_model->save(data);
        obj["data"]= data;

        // qml file
        QString qmlFile= ui->m_codeEdit->document()->toPlainText();
        if(qmlFile.isEmpty())
        {
            generateQML(qmlFile);
        }
        obj["qml"]= qmlFile;

        obj["additionnalHeadCode"]= m_additionnalHeadCode;
        obj["additionnalImport"]= m_additionnalImport;
        obj["fixedScale"]= m_fixedScaleSheet;
        obj["additionnalBottomCode"]= m_additionnalBottomCode;
        obj["flickable"]= m_flickableSheet;
        obj["pageCount"]= static_cast<int>(m_editorCtrl->pageCount());
        obj["uuid"]= m_currentUuid;

        QJsonArray fonts;
        QStringList list= m_sheetProperties->getFontUri();
        for(QString fontUri : list)
        {
            QFile file(fontUri);
            if(file.open(QIODevice::ReadOnly))
            {
                QJsonObject font;
                font["name"]= fontUri;
                QByteArray array= file.readAll();
                font["data"]= QString(array.toBase64());
                fonts.append(font);
            }
        }
        obj["fonts"]= fonts;

        // background
        QJsonArray images= m_imageModel->save();
        obj["background"]= images;
        m_characterModel->writeModel(obj, true);
        json.setObject(obj);
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly))
        {
            file.write(json.toJson());
            setCurrentFile(filename);
            return true;
        }
        //
    }
    return false;
}
bool MainWindow::loadFile(const QString& filename)
{
    if(!filename.isEmpty())
    {
        clearData(false);
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonDocument json= QJsonDocument::fromJson(file.readAll());
            QJsonObject jsonObj= json.object();
            QJsonObject data= jsonObj["data"].toObject();

            QString qml= jsonObj["qml"].toString();

            m_additionnalHeadCode= jsonObj["additionnalHeadCode"].toString("");
            if(m_additionnalHeadCode.isEmpty())
                m_additionnalHeadCode= jsonObj["additionnalCode"].toString("");
            m_additionnalImport= jsonObj["additionnalImport"].toString("");
            m_fixedScaleSheet= jsonObj["fixedScale"].toDouble(1.0);
            m_additionnalBottomCode= jsonObj["additionnalBottomCode"].toString("");
            m_flickableSheet= jsonObj["flickable"].toBool(false);
            int pageCount= jsonObj["pageCount"].toInt();
            m_currentUuid= jsonObj["uuid"].toString(m_currentUuid);

            const auto fonts= jsonObj["fonts"].toArray();
            for(const auto obj : fonts)
            {
                const auto font= obj.toObject();
                const auto fontData= QByteArray::fromBase64(font["data"].toString("").toLatin1());
                QFontDatabase::addApplicationFontFromData(fontData);
            }

            ui->m_codeEdit->setPlainText(qml);

            QJsonArray images= jsonObj["background"].toArray();
            QList<QJsonObject> objList;
            for(auto obj : images)
            {
                objList.append(obj.toObject());
            }

            std::sort(objList.begin(), objList.end(), [](const QJsonObject& aObj, const QJsonObject& bObj) {
                QRegularExpression exp(".*_background_(\\d+).*");
                QRegularExpressionMatch match= exp.match(aObj["key"].toString());
                int bInt= -1;
                int aInt= -1;
                if(match.hasMatch())
                {
                    aInt= match.captured(1).toInt();
                }
                QRegularExpressionMatch match2= exp.match(bObj["key"].toString());
                if(match2.hasMatch())
                {
                    bInt= match2.captured(1).toInt();
                }
                if((0 != bInt) || (0 != aInt))
                {
                    return bInt > aInt;
                }
                else
                {
                    return bObj["key"].toString() > aObj["key"].toString();
                }
            });
            QPixmap refBgImage;
            for(auto jsonpix : objList)
            {
                QJsonObject oj= jsonpix; // jsonpix.toObject();
                QString str= oj["bin"].toString();
                QString id= oj["key"].toString();
                bool isBg= oj["isBg"].toBool();
                QByteArray array= QByteArray::fromBase64(str.toUtf8());
                QPixmap pix;
                pix.loadFromData(array);
                if(isBg)
                {
                    if(refBgImage.isNull())
                        refBgImage= pix;
                    // Canvas* canvas= m_editorCtrl->addPage();
                    SetBackgroundCommand cmd(m_editorCtrl->pageCount(), m_editorCtrl.get(), pix);
                    cmd.redo();
                }
                m_imageModel->insertImage(new QPixmap(pix), id, tr("from rcs file"), isBg);
            }

            while(pageCount > static_cast<int>(m_editorCtrl->pageCount()))
            {
                Canvas* canvas= m_editorCtrl->addPage();
                QPixmap copy(refBgImage);
                copy.fill();
                canvas->setPixmap(copy);
                m_imageModel->insertImage(
                    new QPixmap(copy), QStringLiteral("%1_background_%2.jpg").arg(m_currentUuid).arg(canvas->pageId()),
                    tr("blank page"), true);
            }

            m_model->load(data, m_editorCtrl.get());
            m_characterModel->setRootSection(m_model->getRootSection());
            m_characterModel->readModel(jsonObj, false);
            setCurrentFile(filename);
            return true;
        }
    }
    return false;
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
    ui->m_selectPageCb->clear();
    QStringList list;
    auto s= m_editorCtrl->pageCount();
    for(unsigned int i= 0; i < s; ++i)
    {
        list << QStringLiteral("Page %1").arg(i + 1);
    }
    ui->m_selectPageCb->addItems(list);
    ui->m_selectPageCb->setCurrentIndex(m_editorCtrl->currentPage());
}

void MainWindow::updateRecentFileAction()
{
    std::remove_if(m_recentFiles.begin(), m_recentFiles.end(), [](QString const& f) { return !QFile::exists(f); });

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

/*void MainWindow::pageCountChanged()
{
    if(m_currentPage >= pageCount())
    {
        ui->m_selectPageCb->setCurrentIndex(pageCount() - 1);
    }
    PageDelegate::setPageNumber(pageCount());
}
int MainWindow::pageCount()
{
    auto model= AddPageCommand::getPagesModel();
    return model->rowCount();
}*/

QString MainWindow::currentUuid() const
{
    return m_currentUuid;
}

void MainWindow::setCurrentUuid(const QString& currentUuid)
{
    m_currentUuid= currentUuid;
}

void MainWindow::codeChanged()
{
    if(!ui->m_codeEdit->toPlainText().isEmpty())
    {
        m_editedTextByHand= true;
        setWindowModified(true);
    }
}

void MainWindow::generateQML(QString& qml)
{
    QTextStream text(&qml);
    QPixmap* pix= nullptr;
    bool allTheSame= true;
    QSize size;

    QList<QPixmap*> imageList;
    for(auto key : m_pixList.keys())
    {
        if(m_imageModel->isBackgroundById(key))
        {
            imageList.append(m_pixList.value(key));
        }
    }

    for(QPixmap* pix2 : imageList)
    {
        if(size != pix2->size())
        {
            if(size.isValid())
                allTheSame= false;
            size= pix2->size();
        }
        pix= pix2;
    }
    qreal ratio= 1;
    qreal ratioBis= 1;
    bool hasImage= false;
    if((allTheSame) && (nullptr != pix) && (!pix->isNull()))
    {
        ratio= static_cast<qreal>(pix->width()) / static_cast<qreal>(pix->height());
        ratioBis= static_cast<qreal>(pix->height()) / static_cast<qreal>(pix->width());
        hasImage= true;
    }

    QString key= m_pixList.key(pix);
    QStringList keyParts= key.split('_');
    if(!keyParts.isEmpty())
    {
        key= keyParts[0];
    }
    text << "import QtQuick 2.4\n";
    text << "import QtQuick.Layouts 1.3\n";
    text << "import QtQuick.Controls 2.3\n";
    text << "import Rolisteam 1.0\n";
    text << "import \"qrc:/resources/qml/\"\n";
    if(!m_additionnalImport.isEmpty())
    {
        text << "   " << m_additionnalImport << "\n";
    }
    text << "\n";
    if(m_flickableSheet)
    {
        text << "Flickable {\n";
        text << "    id:root\n";
        text << "    contentWidth: imagebg.width;\n   contentHeight: imagebg.height;\n";
        text << "    boundsBehavior: Flickable.StopAtBounds;\n";
    }
    else
    {
        text << "Item {\n";
        text << "    id:root\n";
    }
    if(hasImage)
    {
        text << "    property alias realscale: imagebg.realscale\n";
    }
    text << "    focus: true\n";
    text << "    property int page: 0\n";
    text << "    property int maxPage:" << static_cast<int>(m_editorCtrl->pageCount()) - 1 << "\n";
    text << "    onPageChanged: {\n";
    text << "        page=page>maxPage ? maxPage : page<0 ? 0 : page\n";
    text << "    }\n";
    if(!m_additionnalHeadCode.isEmpty())
    {
        text << "   " << m_additionnalHeadCode << "\n";
    }
    text << "    Keys.onLeftPressed: --page\n";
    text << "    Keys.onRightPressed: ++page\n";
    text << "    signal rollDiceCmd(string cmd, bool alias)\n";
    text << "    signal showText(string text)\n";
    text << "    MouseArea {\n";
    text << "         anchors.fill:parent\n";
    text << "         onClicked: root.focus = true\n";
    text << "     }\n";
    if(hasImage)
    {
        text << "    Image {\n";
        text << "        id:imagebg"
             << "\n";
        text << "        objectName:\"imagebg\""
             << "\n";
        text << "        property real iratio :" << ratio << "\n";
        text << "        property real iratiobis :" << ratioBis << "\n";
        if(m_flickableSheet)
        {
            text << "       property real realscale: " << m_fixedScaleSheet << "\n";
            text << "       width: sourceSize.width*realscale"
                 << "\n";
            text << "       height: sourceSize.height*realscale"
                 << "\n";
        }
        else
        {
            text << "       property real realscale: width/" << pix->width() << "\n";
            text << "       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width"
                 << "\n";
            text << "       height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width"
                 << "\n";
        }
        text << "       source: \"image://rcs/" + key + "_background_%1.jpg\".arg(root.page)"
             << "\n";
        m_model->generateQML(text, 1, false);
        text << "\n";
        text << "  }\n";
    }
    else
    {
        if(m_flickableSheet)
        {
            text << "    property real realscale: " << m_fixedScaleSheet << "\n";
        }
        else
        {
            text << "    property real realscale: 1\n";
        }
        m_model->generateQML(text, 1, false);
    }
    if(!m_additionnalBottomCode.isEmpty())
    {
        text << "   " << m_additionnalBottomCode << "\n";
    }
    text << "}\n";
    text.flush();
}

void MainWindow::showQML()
{
    if(m_editedTextByHand)
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
    QString data;
    generateQML(data);
    ui->m_codeEdit->setPlainText(data);
    m_editedTextByHand= false;
    QSharedPointer<QHash<QString, QPixmap>> imgdata= m_imgProvider->getData();

    QTemporaryFile file;
    if(file.open()) // QIODevice::WriteOnly
    {
        file.write(data.toUtf8());
        file.close();
    }

    ui->m_quickview->engine()->clearComponentCache();
    m_imgProvider= new RolisteamImageProvider();
    m_imgProvider->setData(imgdata);
    ui->m_quickview->engine()->addImageProvider(QLatin1String("rcs"), m_imgProvider);
    QList<CharacterSheetItem*> list= m_model->children();
    for(CharacterSheetItem* item : list)
    {
        ui->m_quickview->engine()->rootContext()->setContextProperty(item->getId(), item);
    }
    connect(ui->m_quickview->engine(), &QQmlEngine::warnings, this,
            [this](const QList<QQmlError>& warning) { displayWarningsQML(warning); });

    connect(ui->m_quickview, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if(status == QQuickWidget::Error)
            displayWarningsQML(ui->m_quickview->errors());
    });

    ui->m_quickview->setSource(QUrl::fromLocalFile(file.fileName()));
    displayWarningsQML(ui->m_quickview->errors());
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QObject* root= ui->m_quickview->rootObject();
    connect(root, SIGNAL(rollDiceCmd(QString, bool)), this, SLOT(rollDice(QString, bool)));
    connect(root, SIGNAL(rollDiceCmd(QString)), this, SLOT(rollDice(QString)));
}
void MainWindow::displayWarningsQML(const QList<QQmlError>& list)
{
    for(auto error : list)
    {
        LogController::LogLevel type;
        switch(error.messageType())
        {
        case QtDebugMsg:
            type= LogController::Debug;
            break;
        case QtInfoMsg:
            type= LogController::Info;
            break;
        case QtWarningMsg:
            type= LogController::Warning;
            break;
        case QtCriticalMsg:
            type= LogController::Error;
            break;
        case QtFatalMsg:
            type= LogController::Error;
            break;
        }
        m_logManager->manageMessage(error.toString(), type);
    }
}

void MainWindow::showQMLFromCode()
{
    QString data= ui->m_codeEdit->document()->toPlainText();

    QTemporaryFile file;
    if(file.open()) // QIODevice::WriteOnly
    {
        file.write(data.toUtf8());
        file.close();
    }

    // delete ui->m_quickview;
    ui->m_quickview->engine()->clearComponentCache();
    QSharedPointer<QHash<QString, QPixmap>> imgdata= m_imgProvider->getData();
    m_imgProvider= new RolisteamImageProvider();
    m_imgProvider->setData(imgdata);
    ui->m_quickview->engine()->addImageProvider("rcs", m_imgProvider);

    QList<CharacterSheetItem*> list= m_model->children();
    for(CharacterSheetItem* item : list)
    {
        ui->m_quickview->engine()->rootContext()->setContextProperty(item->getId(), item);
    }
    ui->m_quickview->setSource(QUrl::fromLocalFile(file.fileName()));
    displayWarningsQML(ui->m_quickview->errors());
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QObject* root= ui->m_quickview->rootObject();
    connect(root, SIGNAL(rollDiceCmd(QString)), this, SLOT(rollDice(QString)));
}

void MainWindow::saveQML()
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
}

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

bool MainWindow::qmlGeneration() const
{
    return m_qmlGeneration;
}

void MainWindow::setQmlGeneration(bool qmlGeneration)
{
    m_qmlGeneration= qmlGeneration;
}

void MainWindow::rollDice(QString cmd)
{
    qDebug() << cmd;
}

void MainWindow::rollDice(QString cmd, bool b)
{
    qDebug() << cmd << b;
}

void MainWindow::aboutRcse()
{
    QString version("%1.%2.%3");

    AboutRcse dialog(version.arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR), this);
    dialog.exec();
}
void MainWindow::addImage()
{
    QString supportedFormat("Supported files (*.jpg *.png);;All Files (*.*)");
    QString img= QFileDialog::getOpenFileName(this, tr("Open Background Image"), QDir::homePath(), supportedFormat);
    if(!img.isEmpty())
    {
        QPixmap* pix= new QPixmap(img);
        if(!pix->isNull())
        {
            QString fileName= QFileInfo(img).fileName();
            m_imageModel->insertImage(pix, fileName, img, false);
        }
    }
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

void MainWindow::addPage()
{
    AddPageCommand* cmd= new AddPageCommand(m_editorCtrl.get());
    m_undoStack.push(cmd);
}

void MainWindow::removePage()
{
    DeletePageCommand* cmd= new DeletePageCommand(m_editorCtrl.get(), m_model);
    m_undoStack.push(cmd);
}
