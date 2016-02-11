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
#include "fielditem.h"

#include "borderlisteditor.h"

#include "qmlhighlighter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_qmlGeneration =true;
    setAcceptDrops(true);
    ui->setupUi(this);

    m_canvas = new Canvas();
    m_model = new FieldModel();
    ui->treeView->setModel(m_model);
    m_canvas->setModel(m_model);
    ui->treeView->setItemDelegateForColumn(Item::BORDER,new BorderListEditor);
    m_view = new QGraphicsView();
    m_view->setAcceptDrops(true);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_view->setViewport(new QOpenGLWidget());
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );


    m_view->setScene(m_canvas);
    ui->scrollArea->setWidget(m_view);

    ui->m_splitter->setStretchFactor(0,1);

    ui->m_addAct->setData(Canvas::ADD);
    ui->m_moveAct->setData(Canvas::MOVE);
    ui->m_deleteAct->setData(Canvas::DELETE);
    ui->m_addButtonAct->setData(Canvas::BUTTON);

    ui->m_addBtn->setDefaultAction(ui->m_addAct);
    ui->m_moveBtn->setDefaultAction(ui->m_moveAct);
    ui->m_deleteBtn->setDefaultAction(ui->m_deleteAct);
    ui->m_addButtonBtn->setDefaultAction(ui->m_addButtonAct);

    QmlHighlighter* highlighter = new QmlHighlighter(ui->m_codeEdit->document());


    connect(ui->m_addAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_moveAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_deleteAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_addButtonAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));


    connect(ui->actionQML_View,SIGNAL(triggered(bool)),this,SLOT(showQML()));
    connect(ui->actionCode_To_QML,SIGNAL(triggered(bool)),this,SLOT(showQMLFromCode()));

    connect(ui->m_saveAct,SIGNAL(triggered(bool)),this,SLOT(save()));
    connect(ui->actionSave_As,SIGNAL(triggered(bool)),this,SLOT(saveAs()));
    connect(ui->m_openAct,SIGNAL(triggered(bool)),this,SLOT(open()));

    m_imgProvider = new RolisteamImageProvider();

   // ui->m_quickview->engine()->addImageProvider("rcs",m_imgProvider);
  //  ui->m_quickview->engine()->rootContext()->setContextProperty("_model",m_model);
  //  ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);

    connect(m_canvas,SIGNAL(imageChanged()),this,SLOT(setImage()));

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setImage()
{
    m_imgProvider->insertPix("background.jpg",m_canvas->pixmap());
}

void MainWindow::setCurrentTool()
{
    QAction* action = dynamic_cast<QAction*>(sender());
    m_canvas->setCurrentTool((Canvas::Tool)action->data().toInt());
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
            QPixmap pix = m_canvas->pixmap();
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            obj["background"]=QString(buffer.data().toBase64());
            json.setObject(obj);


            file.write(json.toJson());

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


            ui->m_codeEdit->setText(qml);

            QString str = jsonObj["background"].toString();
            QByteArray array = QByteArray::fromBase64(str.toUtf8());
            QPixmap pix;
            pix.loadFromData(array);
            m_canvas->setPixmap(pix);
            m_imgProvider->insertPix("background.jpg",pix);

            m_model->load(data,m_canvas);
        }
    }
}

void MainWindow::generateQML(QString& qml)
{
    QTextStream text(&qml);
    QPixmap pix = m_canvas->pixmap();
    if(!pix.isNull())
    {
        text << "import QtQuick 2.4\n";
        text << "import \"qrc:/resources/qml/\"\n";
        text << "\n";
        text << "Item {\n";
        text << "   id:root\n";
        text << "   signal rollDiceCmd(string cmd)\n";
        text << "   Image {\n";
        text << "       id:imagebg" << "\n";
        qreal ratio = (qreal)pix.width()/(qreal)pix.height();
        qreal ratioBis = (qreal)pix.height()/(qreal)pix.width();
        text << "       property real iratio :" << ratio << "\n";
        text << "       property real iratiobis :" << ratioBis << "\n";
        text << "       property real realscale: width/"<< pix.width() << "\n";
        text << "       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width" << "\n";
        text << "       height:(parent.width>parent.height*iratio)?parent.height:iratiobis*parent.width" << "\n";
        text << "       source: \"image://rcs/background.jpg\"" << "\n";
        m_model->generateQML(text,Item::FieldSec);
        text << "\n";
        text << "       Connections {\n";
        text << "           target: _model\n";
        text << "           onValuesChanged:{\n";
        m_model->generateQML(text,Item::ConnectionSec);
        text << "       }\n";
        text << "   }\n";
        text << "   }\n";
        text << "}\n";


        text.flush();

    }
}


void MainWindow::showQML()
{
    QString data;
    generateQML(data);
    ui->m_codeEdit->setText(data);

    QFile file("test.qml");
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(data.toLatin1());
        file.close();
    }
    ui->m_quickview->engine()->clearComponentCache();
    ui->m_quickview->engine()->addImageProvider("rcs",m_imgProvider);
    ui->m_quickview->engine()->rootContext()->setContextProperty("_model",m_model);
    ui->m_quickview->setSource(QUrl::fromLocalFile("test.qml"));
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QObject* root = ui->m_quickview->rootObject();
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));
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
    //ui->m_quickview->engine()->addImageProvider("rcs",m_imgProvider);
    ui->m_quickview->engine()->rootContext()->setContextProperty("_model",m_model);
    ui->m_quickview->setSource(QUrl::fromLocalFile(name));
    QObject* root = ui->m_quickview->rootObject();
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);
}
void MainWindow::saveQML()
{
    QString qmlFile = QFileDialog::getOpenFileName(this,tr("Save CharacterSheet View"),QDir::homePath(),tr("CharacterSheet View (*.qml)"));
    if(!qmlFile.isEmpty())
    {
        QString data=ui->m_codeEdit->toPlainText();
        generateQML(data);
        ui->m_codeEdit->setText(data);

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
            ui->m_codeEdit->setText(qmlContent);
            showQMLFromCode();

        }
    }
}
bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    return QMainWindow::eventFilter(obj,ev);
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
    qDebug() << "create Field";

    return NULL;
}
void MainWindow::rollDice(QString cmd)
{
    qDebug() << cmd;
}
