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
#include <QQmlContext>
#include <QQmlError>
#include <QQmlProperty>
#include <QQuickItem>
#include <QTemporaryFile>
#include <QTimer>
#include <QUrl>

#ifdef WITH_PDF
#include <poppler-qt5.h>
#endif

#include "dialog/aboutrcse.h"
#include "dialog/codeeditordialog.h"
#include "preferences/preferencesdialog.h"
#include "qmlhighlighter.h"

// Controller
#include "controllers/charactercontroller.h"
#include "controllers/editorcontroller.h"
#include "controllers/imagecontroller.h"
#include "controllers/qmlgeneratorcontroller.h"

#include "delegate/pagedelegate.h"

// Undo
#include "undo/addpagecommand.h"
#include "undo/deletefieldcommand.h"
#include "undo/deletepagecommand.h"
#include "undo/setbackgroundimage.h"
#include "undo/setfieldproperties.h"
#include "undo/setpropertyonallcharacters.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_counterZoom(0), m_pdf(nullptr), m_undoStack(new QUndoStack(this))
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

    m_editorCtrl.reset(new EditorController(m_undoStack, m_view));
    m_characterCtrl.reset(new CharacterController(m_undoStack, ui->m_characterView));
    m_qmlCtrl.reset(new QmlGeneratorController(ui->m_codeEdit, ui->treeView));
    m_imageCtrl.reset(new ImageController(ui->m_imageList));

    connect(m_editorCtrl.get(), &EditorController::canvasBackgroundChanged, m_imageCtrl.get(),
            &ImageController::addBackgroundImage);

    connect(m_editorCtrl.get(), &EditorController::pageCountChanged, this, &MainWindow::updatePageSelector);
    connect(m_editorCtrl.get(), &EditorController::pageCountChanged, this,
            [this]() { m_qmlCtrl->setLastPageId(static_cast<unsigned int>(m_editorCtrl->pageCount() - 1)); });

    connect(m_qmlCtrl.get(), &QmlGeneratorController::errors, this, &MainWindow::displayWarningsQML);
    connect(m_qmlCtrl.get(), &QmlGeneratorController::sectionChanged, m_characterCtrl.get(),
            &CharacterController::setRootSection);

    // LOG
    m_logCtrl.reset(new LogController(true, this));
    connect(m_qmlCtrl.get(), &QmlGeneratorController::reportLog, m_logCtrl.get(), &LogController::manageMessage);
    m_logCtrl->setCurrentModes(LogController::Gui);
    QDockWidget* wid= new QDockWidget(tr("Log panel"), this);
    wid->setObjectName(QStringLiteral("logpanel"));
    m_logPanel= new LogPanel(m_logCtrl.get());
    wid->setWidget(m_logPanel);
    addDockWidget(Qt::BottomDockWidgetArea, wid);
    auto showLogPanel= wid->toggleViewAction();

    ui->treeView->setFieldModel(m_qmlCtrl->fieldModel());
    connect(m_editorCtrl.get(), &EditorController::pageCountChanged, this,
            [this]() { ui->treeView->setCurrentPage(static_cast<int>(m_editorCtrl->pageCount())); });
    ui->treeView->setUndoStack(&m_undoStack);

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

    connect(ui->m_backgroundImageAct, &QAction::triggered, this, &MainWindow::openBackgroundImage);
    ui->scrollArea->setWidget(m_view);

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

    auto notifyDataChanged= [this]() { setWindowModified(true); };

    connect(m_editorCtrl.get(), &EditorController::pageAdded, this, [this, notifyDataChanged](Canvas* canvas) {
        canvas->setModel(m_qmlCtrl->fieldModel());
        connect(canvas, &Canvas::pixmapChanged, this, notifyDataChanged);
        connect(canvas, &Canvas::pageIdChanged, this, notifyDataChanged);
    });

    m_editorCtrl->addPage();

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
        SheetProperties sheetProperties(m_qmlCtrl.get(), this);
        sheetProperties.exec();
    });

    connect(ui->m_quitAction, &QAction::triggered, this, &MainWindow::close);

    auto setCurrentTool= [this]() {
        QAction* action= dynamic_cast<QAction*>(sender());
        auto tool= static_cast<Canvas::Tool>(action->data().toInt());
        m_editorCtrl->setCurrentTool(tool);
    };

    connect(ui->m_addCheckBoxAct, &QAction::triggered, this, setCurrentTool);
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
    connect(ui->m_moveAct, &QAction::triggered, this, [this](bool triggered) { m_view->setHandle(triggered); });
    connect(ui->m_exportPdfAct, &QAction::triggered, this, &MainWindow::exportPDF);
    connect(ui->m_moveAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_deleteAct, &QAction::triggered, this, setCurrentTool);
    connect(ui->m_addButtonAct, &QAction::triggered, this, setCurrentTool);

    connect(ui->m_genarateCodeAct, &QAction::triggered, this, &MainWindow::showQML);
    connect(ui->m_codeToViewAct, &QAction::triggered, this, &MainWindow::showQMLFromCode);

    ///////////////////////
    /// @brief action menu
    ///////////////////////
    connect(ui->m_saveAct, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->m_openAct, &QAction::triggered, this, &MainWindow::open);
    connect(ui->m_checkValidityAct, &QAction::triggered, this,
            [this]() { m_characterCtrl->checkCharacter(m_qmlCtrl->fieldModel()->getRootSection()); });
    connect(ui->m_addPage, &QPushButton::clicked, this, &MainWindow::addPage);
    connect(ui->m_removePage, &QPushButton::clicked, this, &MainWindow::removePage);
    connect(ui->m_selectPageCb, QOverload<int>::of(&QComboBox::currentIndexChanged), m_editorCtrl.get(),
            &EditorController::setCurrentPage);
    connect(ui->m_resetIdAct, &QAction::triggered, m_qmlCtrl->fieldModel(), &FieldModel::resetAllId);
    connect(ui->m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);

    ui->m_characterView->setModel(m_characterCtrl->model());
    m_characterCtrl->setRootSection(m_qmlCtrl->fieldModel()->getRootSection());
    ui->m_characterView->setContextMenuPolicy(Qt::CustomContextMenu);

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

    // Help Menu
    connect(ui->m_aboutRcseAct, &QAction::triggered, this, &MainWindow::aboutRcse);
    connect(ui->m_onlineHelpAct, &QAction::triggered, this, &MainWindow::helpOnLine);

    ui->m_addImageBtn->setDefaultAction(ui->m_addImageAct);
    ui->m_removeImgBtn->setDefaultAction(ui->m_deleteImageAct);

    connect(ui->m_addImageAct, &QAction::triggered, this, &MainWindow::openImage);
    connect(ui->m_deleteImageAct, &QAction::triggered, this, [=]() {
        auto index= ui->m_imageList->currentIndex();
        m_imageCtrl->removeImage(index.row());
    });

    readSettings();
    m_logPanel->initSetting();

    clearData();

    connect(m_editorCtrl.get(), &EditorController::dataChanged, this, notifyDataChanged);
    connect(m_characterCtrl.get(), &CharacterController::dataChanged, this, notifyDataChanged);
    connect(m_qmlCtrl.get(), &QmlGeneratorController::dataChanged, this, notifyDataChanged);
    connect(m_imageCtrl.get(), &ImageController::dataChanged, this, notifyDataChanged);
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
void MainWindow::clearData(bool addDefaultCanvas)
{
    m_qmlCtrl->clearData();
    m_editorCtrl->clearData(addDefaultCanvas);
    m_imageCtrl->clearData();
    m_characterCtrl->clear();
    m_undoStack.clear();
    setCurrentFile(QString());
    setWindowModified(false);
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
        m_imageCtrl->clearData();

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
                SetBackgroundCommand* cmd= new SetBackgroundCommand(i, m_editorCtrl.get(), pix, tr("From PDF"));
                m_undoStack.push(cmd);
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

void MainWindow::openBackgroundImage()
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

    m_editorCtrl->loadImageFromUrl(QUrl::fromLocalFile(img));
}

void MainWindow::openImage()
{
    QString supportedFormat("Supported files (*.jpg *.png);;All Files (*.*)");
    QString img= QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), supportedFormat);

    if(img.isEmpty())
        return;

    QPixmap map(img);
    m_imageCtrl->addImage(map, img);
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
    QJsonDocument json;
    QJsonObject obj;

    m_qmlCtrl->save(obj);
    obj["pageCount"]= static_cast<int>(m_editorCtrl->pageCount());
    obj["uuid"]= m_imageCtrl->uuid();

    // background
    m_imageCtrl->save(obj);

    m_characterCtrl->save(obj, true);
    json.setObject(obj);
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(json.toJson());
        setCurrentFile(filename);
        return true;
    }
    return false;
}
#include <QUuid>
bool MainWindow::loadFile(const QString& filename)
{
    if(filename.isEmpty())
        return false;

    clearData(false);
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QJsonDocument json= QJsonDocument::fromJson(file.readAll());
    QJsonObject jsonObj= json.object();
    QJsonObject data= jsonObj["data"].toObject();

    QString qml= jsonObj["qml"].toString();

    int pageCount= jsonObj["pageCount"].toInt();
    m_imageCtrl->setUuid(jsonObj["uuid"].toString(QUuid::createUuid().toString(QUuid::WithoutBraces)));

    ui->m_codeEdit->setPlainText(qml);

    QJsonArray images= jsonObj["background"].toArray();

    QJsonArray backGround;
    std::copy_if(images.begin(), images.end(), std::back_inserter(backGround), [](const QJsonValue& val) {
        auto obj= val.toObject();
        return obj["isBg"].toBool();
    });

    QJsonArray regularImage;
    std::copy_if(images.begin(), images.end(), std::back_inserter(regularImage), [](const QJsonValue& val) {
        auto obj= val.toObject();
        return !obj["isBg"].toBool();
    });

    for(auto img : regularImage)
    {
        auto oj= img.toObject();
        QPixmap pix;
        QString str= oj["bin"].toString();
        QString path= oj["filename"].toString();
        QByteArray array= QByteArray::fromBase64(str.toUtf8());

        pix.loadFromData(array);
        m_imageCtrl->addImage(pix, path);
    }

    QPixmap refBgImage;
    for(int i= 0; i < pageCount; ++i)
    {
        QString path;
        QString id;
        QPixmap pix;
        if(i < backGround.size())
        {
            auto oj= backGround[i].toObject();
            QString str= oj["bin"].toString();
            id= oj["key"].toString();
            path= oj["filename"].toString();
            QByteArray array= QByteArray::fromBase64(str.toUtf8());

            pix.loadFromData(array);
            if(refBgImage.isNull())
                refBgImage= pix;
        }
        else
        {
            if(refBgImage.isNull())
                refBgImage= QPixmap(800, 600);
            pix= refBgImage;
        }
        auto idx= m_editorCtrl->addPage();
        if(!backGround.isEmpty())
        {
            m_editorCtrl->setImageBackground(idx, pix, path);
            m_imageCtrl->addBackgroundImage(idx, pix, path, id);
        }
    }

    m_qmlCtrl->load(jsonObj, m_editorCtrl.get());
    m_characterCtrl->setRootSection(m_qmlCtrl->fieldModel()->getRootSection());
    m_characterCtrl->load(jsonObj, false);
    setCurrentFile(filename);
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

void MainWindow::generateQML(QString& qml)
{
    m_qmlCtrl->generateQML(m_imageCtrl.get(), qml);
}

void MainWindow::updatePageSelector()
{
    auto current= m_editorCtrl->currentPage();
    ui->m_selectPageCb->clear();
    QStringList list;
    auto s= m_editorCtrl->pageCount();
    for(unsigned int i= 0; i < s; ++i)
    {
        list << QStringLiteral("Page %1").arg(i + 1);
    }
    ui->m_selectPageCb->addItems(list);
    ui->m_selectPageCb->setCurrentIndex(current);
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

void MainWindow::showQML()
{
    if(m_qmlCtrl->textEdited())
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
    m_qmlCtrl->showQML(ui->m_quickview, m_imageCtrl.get());
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
        m_logCtrl->manageMessage(error.toString(), type);
    }
}

void MainWindow::showQMLFromCode()
{
    m_qmlCtrl->runQmlFromCode(ui->m_quickview, m_imageCtrl.get());
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

void MainWindow::aboutRcse()
{
    QString version("%1.%2.%3");

    AboutRcse dialog(version.arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR), this);
    dialog.exec();
}

void MainWindow::addBackgroundImage()
{
    QString supportedFormat("Supported files (*.jpg *.png);;All Files (*.*)");
    QString img= QFileDialog::getOpenFileName(this, tr("Open Background Image"), QDir::homePath(), supportedFormat);
    if(!img.isEmpty())
        return;

    QPixmap map(img);
    m_imageCtrl->addBackgroundImage(m_editorCtrl->currentPage(), map, img, QString());
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
    DeletePageCommand* cmd= new DeletePageCommand(m_editorCtrl.get(), m_qmlCtrl->fieldModel());
    m_undoStack.push(cmd);
}
