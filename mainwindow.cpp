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


#include "borderlisteditor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
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
    //m_view->installEventFilter(this);
    ui->scrollArea->setWidget(m_view);

    ui->m_splitter->setStretchFactor(0,1);

    ui->m_addAct->setData(Canvas::ADD);
    ui->m_moveAct->setData(Canvas::MOVE);
    ui->m_deleteAct->setData(Canvas::DELETE);

    ui->m_addBtn->setDefaultAction(ui->m_addAct);
    ui->m_moveBtn->setDefaultAction(ui->m_moveAct);
    ui->m_deleteBtn->setDefaultAction(ui->m_deleteAct);


    connect(ui->m_addAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_moveAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));
    connect(ui->m_deleteAct,SIGNAL(triggered(bool)),this,SLOT(setCurrentTool()));

    connect(ui->m_saveAct,SIGNAL(triggered(bool)),this,SLOT(save()));
    connect(ui->m_openAct,SIGNAL(triggered(bool)),this,SLOT(open()));
    connect(ui->m_tabWidget,SIGNAL(currentChanged(int)),this,SLOT(generateQML()));

    connect(ui->m_rolisteamExport,SIGNAL(triggered(bool)),this,SLOT(exportToRolisteam()));

    ui->m_quickview->engine()->rootContext()->setContextProperty("_model",m_model);
    ui->m_quickview->setSource(QUrl::fromLocalFile("./cstest.qml"));
    ui->m_quickview->setResizeMode(QQuickWidget::SizeRootObjectToView);

   /* foreach(QQmlError error, ui->m_quickview->errors())
    {
        qDebug() << error.toString();
    }*/
}

MainWindow::~MainWindow()
{
    delete ui;
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
    if(m_filename.isEmpty())
        saveAs();
    QJsonDocument json;
    QJsonObject jsonObj;
    m_model->save(jsonObj);
    QPixmap pix = m_canvas->pixmap();
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    jsonObj["image"]=QString(buffer.data().toBase64());
    json.setObject(jsonObj);


    QFile file(m_filename);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(json.toJson());
    }
    else
    {
        QMessageBox::information(this,tr("Not Readable"), tr("The selected file is not readable"));
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
            QString str = jsonObj["image"].toString();
            QByteArray array = QByteArray::fromBase64(str.toUtf8());
            QPixmap pix;
            pix.loadFromData(array);
            m_canvas->setPixmap(pix);

            m_model->load(jsonObj,m_canvas);
        }
    }
}

void MainWindow::generateQML()
{
    QPixmap pix = m_canvas->pixmap();
    if(!pix.isNull())
    {
        QByteArray data;// = new QString;
        QTextStream text(&data);

        text << "import QtQuick 2.4\n";
        text << "import \"./Rcse/\"\n";
        text << "\n";
        text << "Item {\n";
        text << "   signal rollDiceCmd(string cmd)\n";
        text << "        Image {\n";
        text << "        id:imagebg" << "\n";
        qreal ratio = (qreal)pix.width()/(qreal)pix.height();
        qreal ratioBis = (qreal)pix.height()/(qreal)pix.width();
        text << "       property real iratio :" << ratio << "\n";
        text << "       property real iratioBis :" << ratioBis << "\n";
        text << "       property real realScale: width/"<< pix.width() << "\n";
        text << "       width:(parent.width>parent.height*iratio)?iratio*parent.height:parent.width" << "\n";
        text << "       height:(parent.width>parent.height*iratio)?parent.height:iratioBis*parent.width" << "\n";
        text << "       source: \"background.jpg\"" << "\n";
        m_model->generateQML(text,Item::FieldSec);
        text << "\n";
        text << "   Connections {\n";
        text << "   target: _model\n";
        text << "   onValuesChanged:{\n";
        m_model->generateQML(text,Item::ConnectionSec);
        text << "   }";
        text << "   }\n";
        text << "   }\n";
        text << "}\n";


        text.flush();
        QFile file("./cstest.qml");
        //file.setAutoRemove(false);
        if(file.open(QIODevice::WriteOnly))
        {
            QString dir = QFileInfo(file.fileName()).absolutePath();
            QFile imageFile(dir+"/background.jpg");
            if(imageFile.open(QIODevice::WriteOnly))
            {
                pix.save(&imageFile,"jpg");
            }
        }
        ui->m_quickview->setSource(QUrl::fromLocalFile(file.fileName()));
    }
}

void MainWindow::exportToRolisteam()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Select directory to export files"),QDir::homePath());

    if(!dir.isEmpty())
    {
        QString name=m_filename;
        if(name.isEmpty())
            name=QStringLiteral("Unknown");

        QFileInfo info(name);
        QString fileName = QStringLiteral("%1/%2.rcse").arg(dir).arg(info.baseName());
        if(QFile::exists(fileName))
        {

            ///@Warning
        }
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly))
        {
            QString qmlFile = QStringLiteral("%1.qml").arg(info.baseName());

            QJsonDocument json;
            QJsonObject jsonObj;
            jsonObj["qml"]=qmlFile;
            m_model->save(jsonObj,true);
            json.setObject(jsonObj);


        }
        //
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{

    return QMainWindow::eventFilter(obj,ev);
}
Field* MainWindow::addFieldAt(QPoint pos)
{
    qDebug() << "create Field";

    return NULL;
}
