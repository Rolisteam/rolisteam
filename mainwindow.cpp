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

#include <QColorDialog>
#include <QDebug>
#include <QMimeData>
#include <QUrl>
#include <QOpenGLWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QBuffer>
#include <QJsonDocument>
#include <QTemporaryFile>
#include <QQmlError>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlComponent>
#include <QJsonArray>
#include <QButtonGroup>
#include <QUuid>

#ifdef WITH_PDF
#include <poppler-qt5.h>
#endif

#include "borderlisteditor.h"
#include "alignmentdelegate.h"
#include "typedelegate.h"
#include "qmlhighlighter.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_currentPage(0),
    m_editedTextByHand(false),
    m_counterZoom(0),
    m_pdf(nullptr)
{
    m_title = QStringLiteral("%1[*] - %2");
    setWindowTitle(m_title.arg("Unknown").arg("RCSE"));
    setWindowModified(false);
    m_qmlGeneration =true;
    setAcceptDrops(true);
    ui->setupUi(this);

    Canvas* canvas = new Canvas();
    canvas->setCurrentPage(m_currentPage);

    m_canvasList.append(canvas);
    m_model = new FieldModel();
    connect(m_model,SIGNAL(modelChanged()),this,SLOT(modelChanged()));
    ui->treeView->setModel(m_model);

    AlignmentDelegate* delegate = new AlignmentDelegate();
    ui->treeView->setItemDelegateForColumn(10,delegate);

    TypeDelegate* typeDelegate = new TypeDelegate();
    ui->treeView->setItemDelegateForColumn(4,typeDelegate);

    canvas->setModel(m_model);
    ui->treeView->setItemDelegateForColumn(CharacterSheetItem::BORDER,new BorderListEditor);
    m_view = new QGraphicsView(this);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_fitInView = new QAction(tr("Fit the view"),m_view);
    m_fitInView->setCheckable(true);
    connect(m_fitInView,SIGNAL(triggered(bool)),this,SLOT(setFitInView()));
    m_view->installEventFilter(this);
    m_view->setAcceptDrops(true);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //m_view->setViewport(new QOpenGLWidget());
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );

    connect(ui->m_backgroundImageAct,SIGNAL(triggered(bool)),this,SLOT(openImage()));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(menuRequestedFromView(QPoint)));

    m_view->setScene(canvas);
    ui->scrollArea->setWidget(m_view);

    //ui->m_splitter->setStretchFactor(0,1);

    ui->m_addCheckBoxAct->setData(Canvas::ADDCHECKBOX);
    ui->m_addTextAreaAct->setData(Canvas::ADDTEXTAREA);
    ui->m_addTextInputAct->setData(Canvas::ADDINPUT);
    ui->m_addTextFieldAct->setData(Canvas::ADDTEXTFIELD);
    ui->m_addImageAction->setData(Canvas::ADDIMAGE);

    ui->m_moveAct->setData(Canvas::MOVE);
    ui->m_deleteAct->setData(Canvas::DELETETOOL);
    ui->m_addButtonAct->setData(Canvas::BUTTON);

    ui->m_addTextInput->setDefaultAction(ui->m_addTextInputAct);
    ui->m_addTextArea->setDefaultAction(ui->m_addTextAreaAct);
    ui->m_addTextField->setDefaultAction(ui->m_addTextFieldAct);
    ui->m_addCheckbox->setDefaultAction(ui->m_addCheckBoxAct);
    ui->m_imageBtn->setDefaultAction(ui->m_addImageAction);

    QButtonGroup* group = new QButtonGroup();
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

    ui->m_moveBtn->setDefaultAction(ui->m_moveAct);
    ui->m_deleteBtn->setDefaultAction(ui->m_deleteAct);
    ui->m_addButtonBtn->setDefaultAction(ui->m_addButtonAct);

    QmlHighlighter* highlighter = new QmlHighlighter(ui->m_codeEdit->document());
    highlighter->setObjectName("HighLighterForQML");



    connect(ui->m_quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(ui->m_addCheckBoxAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_addTextAreaAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_addTextFieldAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_addTextInputAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_addImageAction,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));


    connect(ui->m_moveAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_deleteAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_addButtonAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));

    connect(ui->actionQML_View,SIGNAL(triggered(bool)),this,SLOT(showQML()));
    connect(ui->actionCode_To_QML,SIGNAL(triggered(bool)),this,SLOT(showQMLFromCode()));

    connect(ui->m_saveAct,SIGNAL(triggered(bool)),this,SLOT(save()));
    connect(ui->actionSave_As,SIGNAL(triggered(bool)),this,SLOT(saveAs()));
    connect(ui->m_openAct,SIGNAL(triggered(bool)),this,SLOT(open()));

    connect(ui->m_addPage,SIGNAL(clicked(bool)),this,SLOT(addPage()));
    connect(ui->m_removePage,SIGNAL(clicked(bool)),this,SLOT(removePage()));
    connect(ui->m_selectPageCb,SIGNAL(currentIndexChanged(int)),this,SLOT(currentPageChanged(int)));

    m_imgProvider = new RolisteamImageProvider();

    connect(canvas,SIGNAL(imageChanged()),this,SLOT(setImage()));

    m_addCharacter = new QAction(tr("Add character"),this);


    m_characterModel = new CharacterSheetModel();
    connect(m_characterModel,SIGNAL(dataCharacterChange()),this,SLOT(modelChanged()));
    connect(m_characterModel,SIGNAL(columnsInserted(QModelIndex,int,int)),this,SLOT(columnAdded()));
    ui->m_characterView->setModel(m_characterModel);
    m_characterModel->setRootSection(m_model->getRootSection());
    ui->m_characterView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(menuRequestedForFieldModel(QPoint)));
    connect(ui->m_characterView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(menuRequested(QPoint)));
    connect(m_addCharacter,SIGNAL(triggered(bool)),m_characterModel,SLOT(addCharacterSheet()));

    canvas->setCurrentTool(Canvas::NONE);

    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(editColor(QModelIndex)));


    m_delItem = new QAction(tr("Delete Item"),this);
    m_applyValueOnSelection = new QAction(tr("Apply on Selection"),this);
    m_applyValueOnAllLines = new QAction(tr("Apply on all lines"),this);

    connect(ui->m_codeEdit,SIGNAL(textChanged()),this,SLOT(codeChanged()));



}
MainWindow::~MainWindow()
{
    delete ui;
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if((obj==m_view)&&(event->type() == QEvent::Wheel))
    {
        return wheelEventForView(dynamic_cast<QWheelEvent*>(event));

    }
    else
        return QMainWindow::eventFilter(obj,event);
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(mayBeSaved())
    {
        // writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
bool MainWindow::mayBeSaved()
{
    if(isWindowModified())
    {
        QMessageBox msgBox(this);

        QString message(tr("The charactersheet has unsaved changes."));
        QString msg =QStringLiteral("RCSE");

        msgBox.setIcon(QMessageBox::Question);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.addButton(QMessageBox::Save);
        msgBox.addButton(QMessageBox::Discard);
        msgBox.setWindowTitle(tr("Quit %1 ").arg(msg));


        msgBox.setText(message);
        int value = msgBox.exec();
        if (QMessageBox::Cancel == value)
        {
            return false;
        }
        else if (QMessageBox::Save == value) //saving
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
    setWindowModified(true);
}
bool MainWindow::wheelEventForView(QWheelEvent *event)
{
    if(NULL==event)
        return false;

    if(event->modifiers() & Qt::ShiftModifier)
    {
        m_view->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
        m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Scale the view / do the zoom
        double scaleFactor = 1.1;

        if((event->delta() > 0)&&(m_counterZoom<20))
        {
            m_view->scale(scaleFactor, scaleFactor);
            ++m_counterZoom;
        }
        else if(m_counterZoom>-20)
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
    Poppler::Document* document = Poppler::Document::load(m_pdfPath);
    if (nullptr==document || document->isLocked()) {
        QMessageBox::warning(this,tr("Error! this PDF file can not be read!"),tr("This PDF document can not be read: %1").arg(m_pdfPath),QMessageBox::Ok);
        delete document;
        return;
    }
    if(nullptr!=m_pdf)
    {
        qreal res = m_pdf->getDpi();
        m_pixList.clear();
        QString id = QUuid::createUuid().toString();
        static int lastCanvas=m_canvasList.size()-1;

        QSize previous;
        if(m_pdf->hasResolution())
        {
            previous.setHeight(m_pdf->getHeight());
            previous.setWidth(m_pdf->getWidth());
        }
        for(int i = 0; i<document->numPages();++i)
        {
            Poppler::Page* pdfPage = document->page(i);  // Document starts at page 0
            if (nullptr == pdfPage)
            {
              QMessageBox::warning(this,tr("Error! This PDF file seems empty!"),tr("This PDF document has no page."),QMessageBox::Ok);
              return;
            }
            // Generate a QImage of the rendered page

            QImage image = pdfPage->renderToImage(res,res);//xres, yres, x, y, width, height
            if (image.isNull())
            {
              QMessageBox::warning(this,tr("Error! Can not make image!"),tr("System has failed while making image of the pdf page."),QMessageBox::Ok);
              return;
            }
            QPixmap* pix = new QPixmap();
            if(!m_pdf->hasResolution())
            {
                m_pdf->setWidth(image.size().width());
                m_pdf->setHeight(image.size().height());
            }
            if(!previous.isValid())
            {
                previous = image.size();
                *pix=QPixmap::fromImage(image);
            }
            else if(previous != image.size())
            {
                *pix=QPixmap::fromImage(image.scaled(previous.width(),previous.height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
            }
            else
            {
                *pix=QPixmap::fromImage(image);
            }

            if(!pix->isNull())
            {
                if(i>=m_canvasList.size())
                {
                    addPage();
                }
                if(i<m_canvasList.size())
                {
                    Canvas* canvas = m_canvasList[i];
                    if(nullptr!=canvas)
                    {
                        canvas->setPixmap(pix);
                        QString key = QStringLiteral("%2_background_%1.jpg").arg(lastCanvas+i).arg(id);
                        m_pixList.insert(key,pix);

                        m_imgProvider->insertPix(key,*pix);
                    }
                }
            }

        delete pdfPage;
    }
    }
    delete document;
#endif
}
void MainWindow::managePDFImport()
{
  m_pdf =new PdfManager(this);
  connect(m_pdf,SIGNAL(apply()),this,SLOT(openPDF()));
  connect(m_pdf,SIGNAL(accepted()),this,SLOT(openPDF()));
  openPDF();
  m_pdf->exec();

}

void MainWindow::openImage()
{
#ifdef WITH_PDF
    QString supportedFormat("Supported files (*.jpg *.png *.xpm *.pdf);;All Files (*.*)");
#else
    QString supportedFormat("Supported files (*.jpg,*.png);;All Files (*.*)");
#endif
    QString img = QFileDialog::getOpenFileName(this,tr("Open Background Image"),QDir::homePath(),supportedFormat);
    if(!img.isEmpty())
    {
        if(img.endsWith("pdf"))
        {
            //openPDF(img);
            m_pdfPath = img;
            managePDFImport();
        }
        else
        {
            QPixmap* pix = new QPixmap(img);
            if(!pix->isNull())
            {
                Canvas* canvas = m_canvasList[m_currentPage];
                canvas->setPixmap(pix);
                QString id = QUuid::createUuid().toString();
                QString key = QStringLiteral("%2_background_%1.jpg").arg(m_currentPage).arg(id);
                m_pixList.insert(key,pix);
                m_imgProvider->insertPix(key,*pix);
                //setFitInView(); //m_view->fitInView(QRectF(pix->rect()),Qt::KeepAspectRatioByExpanding);
            }
        }
    }

}
void MainWindow::setFitInView()
{
    if(m_fitInView->isChecked())
    {
        Canvas* canvas = m_canvasList[m_currentPage];
        QPixmap* pix = canvas->pixmap();
        m_view->fitInView(QRectF(pix->rect()),Qt::KeepAspectRatioByExpanding);
    }
    else
    {
        m_view->fitInView(QRectF(m_view->rect()));
    }
}

void MainWindow::menuRequestedFromView(const QPoint & pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);

    menu.addAction(m_fitInView);

    menu.exec(QCursor::pos());
}
void MainWindow::menuRequested(const QPoint & pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);

    menu.addAction(m_addCharacter);

    menu.exec(QCursor::pos());
}
void MainWindow::menuRequestedForFieldModel(const QPoint & pos)
{
    Q_UNUSED(pos);
    QMenu menu(this);

    QModelIndex index = ui->treeView->currentIndex();
    if(index.isValid())
    {
        menu.addAction(m_delItem);
        menu.addAction(m_applyValueOnAllLines);
        menu.addAction(m_applyValueOnSelection);
    }

    QAction* act = menu.exec(QCursor::pos());

    if(act == m_delItem)
    {
        m_model->removeItem(index);
    }
    else if( m_applyValueOnAllLines == act)
    {
        applyValue(index,false);
    }
    else if(m_applyValueOnSelection == act)
    {
        applyValue(index,true);
    }
}
void MainWindow::applyValue(QModelIndex& index, bool selection)
{
    if(!index.isValid())
        return;


    if(selection)
    {
        QVariant var = index.data(Qt::DisplayRole);
        int col = index.column();
        QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();
        for(QModelIndex modelIndex : list)
        {
            if(modelIndex.column() == col)
            {
                m_model->setData(modelIndex,var,Qt::EditRole);
            }
        }
    }
    else
    {
        m_model->setValueForAll(index);
    }
}

void MainWindow::columnAdded()
{
    int col = m_characterModel->columnCount();
    ui->m_characterView->resizeColumnToContents(col-2);
}

void MainWindow::setImage()
{
    int i = 0;
    m_pixList.clear();
    QString id = QUuid::createUuid().toString();//one id for all images.
    for(auto canvas : m_canvasList)
    {
        QPixmap* pix = canvas->pixmap();
        //m_view->fitInView(QRectF(pix->rect()),Qt::KeepAspectRatioByExpanding);
        setFitInView();
        if(NULL==pix)
        {
            pix=new QPixmap();
        }
        QString idList = QStringLiteral("%2_background_%1.jpg").arg(i).arg(id);
        m_imgProvider->insertPix(idList,*pix);
        m_pixList.insert(idList,pix);
        ++i;
    }
}

void MainWindow::setCurrentTool()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    for(auto canvas : m_canvasList)
    {
        canvas->setCurrentTool((Canvas::Tool)action->data().toInt());
    }
}
void MainWindow::saveAs()
{
    m_filename = QFileDialog::getSaveFileName(this,tr("Save CharacterSheet"),QDir::homePath(),tr("Rolisteam CharacterSheet (*.rcs)"));
    if(!m_filename.isEmpty())
    {
        if(!m_filename.endsWith(".rcs"))
        {
            m_filename.append(QStringLiteral(".rcs"));
        }
        save();
    }
}
void MainWindow::save()
{
    // m_filename = QFileDialog::getSaveFileName(this,tr("Select file to export files"),QDir::homePath());
    if(m_filename.isEmpty())
        saveAs();
    else if(!m_filename.isEmpty())
    {
        if(!m_filename.endsWith(".rcs"))
        {
            m_filename.append(".rcs");
            ///@Warning
        }
        QFile file(m_filename);
        if(file.open(QIODevice::WriteOnly))
        {
            //init Json
            QJsonDocument json;
            QJsonObject obj;



            //Get datamodel
            QJsonObject data;
            m_model->save(data);
            obj["data"]=data;

            //qml file
            QString qmlFile=ui->m_codeEdit->document()->toPlainText();
            if(qmlFile.isEmpty())
            {
                generateQML(qmlFile);
            }
            obj["qml"]=qmlFile;


            //background
            QJsonArray images;
            for(auto canvas : m_canvasList)
            {
                QPixmap* pix = canvas->pixmap();
                if(NULL!=pix)
                {
                    QJsonObject oj;

                    QByteArray bytes;
                    QBuffer buffer(&bytes);
                    buffer.open(QIODevice::WriteOnly);
                    pix->save(&buffer, "PNG");
                    oj["bin"]=QString(buffer.data().toBase64());
                    oj["key"]=m_pixList.key(pix);
                    images.append(oj);
                }
            }
            obj["background"]=images;
            m_characterModel->writeModel(obj,false);
            json.setObject(obj);
            file.write(json.toJson());


            setWindowTitle(m_title.arg(QFileInfo(m_filename).fileName()).arg("RCSE"));
            setWindowModified(false);

        }
        //
    }
}


void MainWindow::open()
{
    m_filename = QFileDialog::getOpenFileName(this,tr("Save CharacterSheet"),QDir::homePath(),tr("Rolisteam CharacterSheet (*.rcs)"));
    if(!m_filename.isEmpty())
    {
        QFile file(m_filename);
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonDocument json = QJsonDocument::fromJson(file.readAll());
            QJsonObject jsonObj = json.object();
            QJsonObject data = jsonObj["data"].toObject();

            QString qml = jsonObj["qml"].toString();

            ui->m_codeEdit->setPlainText(qml);

            QJsonArray images = jsonObj["background"].toArray();
            int i = 0;
            for(auto jsonpix : images)
            {
                QJsonObject oj = jsonpix.toObject();
                QString str = oj["bin"].toString();
                QString id = oj["key"].toString();
                QByteArray array = QByteArray::fromBase64(str.toUtf8());
                QPixmap* pix = new QPixmap();
                pix->loadFromData(array);
                if(i!=0)
                {
                    Canvas* canvas = new Canvas();
                    canvas->setPixmap(pix);
                    m_canvasList.append(canvas);
                    connect(canvas,SIGNAL(imageChanged()),this,SLOT(setImage()));
                }
                else
                {
                    m_canvasList[0]->setPixmap(pix);
                }
                m_pixList.insert(id,pix);
                //m_imgProvider->insertPix(QStringLiteral("%2_background_%1.jpg").arg(i).arg(id),*pix);
                m_imgProvider->insertPix(id,*pix);
                ++i;
            }
            m_model->load(data,m_canvasList);
            m_characterModel->readModel(jsonObj,false);
            updatePageSelector();
            setWindowTitle(m_title.arg(QFileInfo(m_filename).fileName()).arg("RCSE"));
            setWindowModified(false);
        }
    }
}
void MainWindow::updatePageSelector()
{
    QStringList list;
    ui->m_selectPageCb->clear();
    int i =0;
    for(Canvas* canvas: m_canvasList)
    {
        list << QStringLiteral("Page %1").arg(i+1);
        ++i;
    }
    ui->m_selectPageCb->addItems(list);
    ui->m_selectPageCb->setCurrentIndex(0);
}
void MainWindow::codeChanged()
{
    if(!ui->m_codeEdit->toPlainText().isEmpty())
    {
        m_editedTextByHand = true;
        setWindowModified(true);
    }
}

void MainWindow::generateQML(QString& qml)
{

    QTextStream text(&qml);
    QPixmap* pix = NULL;
    bool allTheSame=true;
    QSize size;
    for(QPixmap* pix2 : m_pixList.values())
    {
        qDebug()<< size << "size: "<< pix2->size() << m_pixList.size();
        if(size != pix2->size())
        {
            if(size.isValid())
                allTheSame=false;
            size = pix2->size();
        }
        pix = pix2;
    }
    // QPixmap pix = m_canvasList.pixmap();
    qDebug()<< allTheSame << "all the same";
    qreal ratio = 1;
    qreal ratioBis= 1;
    bool hasImage= false;
    if((allTheSame)&&(NULL!=pix)&&(!pix->isNull()))
    {
        ratio = (qreal)pix->width()/(qreal)pix->height();
        ratioBis = (qreal)pix->height()/(qreal)pix->width();
        hasImage=true;
    }

    QString key = m_pixList.key(pix);
    QStringList keyParts = key.split('_');
    if(!keyParts.isEmpty())
    {
        key = keyParts[0];
    }
    text << "import QtQuick 2.4\n";
    text << "import \"qrc:/resources/qml/\"\n";
    text << "\n";
    text << "Item {\n";
    text << "   id:root\n";
    text << "   focus: true\n";
    text << "   property int page: 0\n";
    text << "   property int maxPage:"<< m_canvasList.size()-1 <<"\n";
    text << "   onPageChanged: {\n";
    text << "       page=page>maxPage ? maxPage : page<0 ? 0 : page\n";
    text << "   }\n";
    text << "   Keys.onLeftPressed: --page\n";
    text << "   Keys.onRightPressed: ++page\n";
    text << "   signal rollDiceCmd(string cmd)\n";
    if(hasImage)
    {
        text << "   Image {\n";
        text << "       id:imagebg" << "\n";
        text << "       property real iratio :" << ratio << "\n";
        text << "       property real iratiobis :" << ratioBis << "\n";
        text << "       property real realscale: width/"<< pix->width() << "\n";
        text << "       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width" << "\n";
        text << "       height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width" << "\n";
        text << "       source: \"image://rcs/"+key+"_background_%1.jpg\".arg(root.page)" << "\n";
        m_model->generateQML(text,CharacterSheetItem::FieldSec);
        text << "\n";
        text << "  }\n";
    }
    else
    {
        text << "       property real realscale: 1\n";
        m_model->generateQML(text,CharacterSheetItem::FieldSec);
    }
    text << "}\n";
    text.flush();


}


void MainWindow::showQML()
{
    if(m_editedTextByHand)
    {
        QMessageBox::StandardButton btn = QMessageBox::question(this,tr("Do you want to erase current QML code ?"),tr("Generate QML code will override any change you made in the QML.<br/>Do you really want to generate QML code ?"),
                                                                QMessageBox::Yes | QMessageBox::Cancel,QMessageBox::Cancel);

        if(btn == QMessageBox::Cancel)
        {
            return;
        }
    }
    QString data;
    generateQML(data);
    ui->m_codeEdit->setPlainText(data);
    m_editedTextByHand=false;
    QHash<QString,QPixmap>* imgdata = RolisteamImageProvider::getData();

    QFile file("test.qml");
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data.toLatin1());
        file.close();
    }
    QLayout* layout = ui->m_qml->layout();
    if(NULL!=ui->m_quickview)
    {
        layout->removeWidget(ui->m_quickview);
        delete ui->m_quickview;
        ui->m_quickview = new QQuickWidget();
        layout->addWidget(ui->m_quickview);

    }
    ui->m_quickview->engine()->clearComponentCache();
    m_imgProvider = new RolisteamImageProvider();
    m_imgProvider->setData(imgdata);
    ui->m_quickview->engine()->addImageProvider(QLatin1String("rcs"),m_imgProvider);
    QList<CharacterSheetItem *> list = m_model->children();
    for(CharacterSheetItem* item : list)
    {
        //qDebug() <<"add item into qml" << item->getId();
        ui->m_quickview->engine()->rootContext()->setContextProperty(item->getId(),item);
    }
    ui->m_quickview->setSource(QUrl::fromLocalFile("test.qml"));
    displayWarningsQML(ui->m_quickview->errors());
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QObject* root = ui->m_quickview->rootObject();
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));
}
void MainWindow::displayWarningsQML(QList<QQmlError> list)
{
    if(!list.isEmpty())
    {
        QString errors;
        QTextStream out(&errors);
        for(auto error : list)
        {
            out << error.toString();
        }

        QMessageBox::information(this,tr("QML Error "),errors,QMessageBox::Ok);
    }
}

void MainWindow::showQMLFromCode()
{
    QString data = ui->m_codeEdit->document()->toPlainText();

    QString name(QStringLiteral("test.qml"));
    if(QFile::exists(name))
    {
        QFile::remove(name);
    }
    QFile file(name);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data.toLatin1());
        file.close();
    }

    //delete ui->m_quickview;
    ui->m_quickview->engine()->clearComponentCache();
    QHash<QString,QPixmap>* imgdata = RolisteamImageProvider::getData();
    m_imgProvider = new RolisteamImageProvider();
    m_imgProvider->setData(imgdata);
    ui->m_quickview->engine()->addImageProvider("rcs",m_imgProvider);

    QList<CharacterSheetItem *> list = m_model->children();
    for(CharacterSheetItem* item : list)
    {
        //qDebug() <<"add item into qml" << item->getId();
        ui->m_quickview->engine()->rootContext()->setContextProperty(item->getId(),item);
    }
    ui->m_quickview->setSource(QUrl::fromLocalFile(name));
    displayWarningsQML(ui->m_quickview->errors());
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QObject* root = ui->m_quickview->rootObject();
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));
}
void MainWindow::saveQML()
{
    QString qmlFile = QFileDialog::getOpenFileName(this,tr("Save CharacterSheet View"),QDir::homePath(),tr("CharacterSheet View (*.qml)"));
    if(!qmlFile.isEmpty())
    {
        QString data=ui->m_codeEdit->toPlainText();
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
    QString qmlFile = QFileDialog::getOpenFileName(this,tr("Save CharacterSheet View"),QDir::homePath(),tr("Rolisteam CharacterSheet View (*.qml)"));
    if(!qmlFile.isEmpty())
    {
        QFile file(m_filename);
        if(file.open(QIODevice::ReadOnly))
        {
            QString qmlContent = file.readAll();
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
    m_qmlGeneration = qmlGeneration;
}

Field* MainWindow::addFieldAt(QPoint pos)
{
    //qDebug() << "create Field";

    return NULL;
}
void MainWindow::rollDice(QString cmd)
{
    qDebug() << cmd;
}

void MainWindow::addPage()
{
    Canvas* previous = m_canvasList[m_currentPage];
    ++m_currentPage;
    Canvas* canvas = new Canvas();
    connect(canvas,SIGNAL(imageChanged()),this,SLOT(setImage()));

    canvas->setCurrentTool(previous->currentTool());

    canvas->setModel(m_model);
    m_canvasList.append(canvas);

    updatePageSelector();
    canvas->setCurrentPage(m_currentPage);
    currentPageChanged(m_currentPage);
    ui->m_selectPageCb->setCurrentIndex(m_currentPage);
    setWindowModified(true);
}
void MainWindow::currentPageChanged(int i)
{
    if((i>=0)&&(i<m_canvasList.size()))
    {
        m_view->setScene(m_canvasList[i]);
    }

}
void MainWindow::removePage()
{
    if(m_canvasList.size()>1)
    {
        Canvas* previous = m_canvasList[m_currentPage];
        m_canvasList.removeOne(previous);
        m_model->removePageId(m_currentPage);
        --m_currentPage;
        updatePageSelector();
    }
}
void MainWindow::editColor(QModelIndex index)
{
    if(!index.isValid())
    {
        return;
    }
    if(index.column()==CharacterSheetItem::BGCOLOR || CharacterSheetItem::TEXTCOLOR == index.column())
    {

        CharacterSheetItem* itm = static_cast<CharacterSheetItem*>(index.internalPointer());

        if(NULL!=itm)
        {
            QColor col = itm->getValueFrom((CharacterSheetItem::ColumnId)index.column(),Qt::EditRole).value<QColor>();//CharacterSheetItem::TEXTCOLOR
            col = QColorDialog::getColor(col,this,tr("Get Color"),QColorDialog::ShowAlphaChannel);

            itm->setValueFrom((CharacterSheetItem::ColumnId)index.column(),col);
        }
    }
}
