/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QtGui>
#include <QDebug>
#include <QScrollBar>
#include <QShortcut>
#include <QFileDialog>



#include "Image.h"
#include "network/networklink.h"
#include "mainwindow.h"
#include "network/networkmessagewriter.h"
#include "variablesGlobales.h"
#include "widgets/onlinepicturedialog.h"

/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/
Image::Image(/*QString title,QString identImage, QString identJoueur, QImage *image, QAction *action,*/ ImprovedWorkspace *parent)
    : MediaContainer(parent),m_NormalSize(0,0)
{
    setObjectName("Image");
    m_widgetArea = new QScrollArea();
    m_zoomLevel = 1;
    m_parent=parent;
    setWindowIcon(QIcon(":/picture.png"));
    m_idImage = QUuid::createUuid().toString();
    createActions();
    m_imageLabel = new QLabel(this);
    m_widgetArea->setAlignment(Qt::AlignCenter);
    m_imageLabel->setLineWidth(0);
    m_imageLabel->setFrameStyle(QFrame::NoFrame);
    m_widgetArea->setWidget(m_imageLabel);
    if(m_preferences->value("PictureAdjust",true).toBool())
    {
        m_fitWindowAct->setChecked(true);
    }
    else
    {
        m_fitWindowAct->setChecked(true);
    }

    setWidget(m_widgetArea);
}

Image::~Image()
{

}
void Image::initImage()
{
    if(!m_pixMap.isNull())
    {
        m_imageLabel->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
        m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        m_imageLabel->setScaledContents(true);
        m_imageLabel->resize(m_pixMap.size());

        m_ratioImage = (double)m_pixMap.size().width()/m_pixMap.size().height();
        m_ratioImageBis = (double)m_pixMap.size().height()/m_pixMap.size().width();
        //labelImage->installEventFilter(this);
        if(NULL!=m_parent)
        {
            fitWorkSpace();
        }
        fitWindow();
    }
}
void Image::setIdImage(QString s)
{
    m_idImage = s;
}

void Image::setIdOwner(QString s)
{
    m_idPlayer=s;
}


void Image::setImage(QImage& img)
{
    m_pixMap = QPixmap::fromImage(img);
    initImage();
}

bool Image::isImageOwner(QString id)
{
    return m_idPlayer == id;
}


QString Image::getImageId()
{
    return m_idImage;
}


void Image::fill(NetworkMessageWriter & message) const
{
    QByteArray baImage;
    QBuffer bufImage(&baImage);
    if (!m_imageLabel->pixmap()->save(&bufImage, "jpg", 70))
    {
        //   qDebug() << "png size:" << bufImage.size();
    }



    message.reset();
    message.string16(m_title);
    message.string8(m_idImage);
    message.string8(m_idPlayer);
    message.byteArray32(baImage);
}


void Image::saveImageToFile(QDataStream &out)
{
    QByteArray baImage;
    QBuffer bufImage(&baImage);
    if(!m_pixMap.isNull())
    {
        if (!m_pixMap.save(&bufImage, "jpg", 70))
        {
            error(tr("Image Compression fails (saveImageToFile - Image.cpp)"),this);
            return;
        }
        // Ecriture de l'image dans le fichier
        out<< m_title;
        out << pos();
        out << size();
        out << baImage;
    }
}

void Image::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton && m_currentTool == ToolsBar::Handler)
    {
        m_allowedMove = true;
        m_startingPoint = event->pos();
        m_horizontalStart = m_widgetArea->horizontalScrollBar()->value();
        m_verticalStart = m_widgetArea->verticalScrollBar()->value();
    }
    QMdiSubWindow::mousePressEvent(event);
}


void Image::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_allowedMove = false;
    }

    QMdiSubWindow::mouseReleaseEvent(event);
}


void Image::mouseMoveEvent(QMouseEvent *event)
{
    if (m_allowedMove)
    {
        QPoint diff = m_startingPoint - event->pos();
        m_widgetArea->horizontalScrollBar()->setValue(m_horizontalStart + diff.x());
        m_widgetArea->verticalScrollBar()->setValue(m_verticalStart + diff.y());
    }
    QMdiSubWindow::mouseMoveEvent(event);
}

void Image::resizeLabel()
{
    if(m_zoomLevel<0.2)
    {
        m_zoomLevel=0.2;
    }
    else if(m_zoomLevel>3.0)
    {
        m_zoomLevel=3.0;
    }
    if(m_fitWindowAct->isChecked())
    {
        int w = m_widgetArea->viewport()->rect().width();
        int h = m_widgetArea->viewport()->rect().height();
        if(w>h*m_ratioImage)
        {
            m_imageLabel->resize(h*m_ratioImage,h);
        }
        else
        {
            m_imageLabel->resize(w,w*m_ratioImageBis);
        }
    }
    else if((m_NormalSize.height()!=0)&&(m_NormalSize.width()!=0))
    {
        m_imageLabel->resize(m_zoomLevel * m_NormalSize);
    }
    else
    {
        m_imageLabel->resize(m_zoomLevel * m_pixMap.size());
        m_NormalSize = widget()->size();
    }
}
void Image::pointeurMain()
{
    m_imageLabel->setCursor(Qt::OpenHandCursor);
}
void Image::pointeurNormal()
{
    m_imageLabel->setCursor(Qt::ForbiddenCursor);
}
void Image::zoomIn()
{
    m_zoomLevel +=0.2;
    resizeLabel();
}

void Image::zoomOut()
{
    m_zoomLevel -=0.2;
    resizeLabel();
}

void Image::onFitWorkSpace()
{
    if(NULL!=parentWidget())
    {
        m_imageLabel->resize(parentWidget()->size());
        fitWorkSpace();
    }
    //m_zoomLevel = 1;
}

void Image::fitWorkSpace()
{
    if(NULL!=parentWidget())
    {
        QSize windowsize = parentWidget()->size();//right size
        while((windowsize.height()<(m_zoomLevel * m_pixMap.height()))||(windowsize.width()<(m_zoomLevel * m_pixMap.width())))
        {
            m_zoomLevel -= 0.1;
        }
        m_imageLabel->resize(m_pixMap.size());
        m_NormalSize = QSize(0,0);
        resizeLabel();
        setGeometry(m_imageLabel->rect().adjusted(0,0,4,4));
        m_zoomLevel = 1.0;
    }
}
void Image::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier)
    {
        int delta = event->delta();
        if(delta > 0)//zoomin
        {
            m_zoomLevel +=0.2;
        }
        else//zoomOut
        {
            m_zoomLevel -=0.2;
        }
        resizeLabel();
        event->ignore();

    }
    QMdiSubWindow::wheelEvent(event);
}

void Image::paintEvent ( QPaintEvent * event )
{
    QMdiSubWindow::paintEvent(event);
    if(m_fitWindowAct->isChecked())
    {
        resizeLabel();
    }
    else if((m_NormalSize * m_zoomLevel) != m_imageLabel->size())
    {
        m_NormalSize = m_imageLabel->size() / m_zoomLevel;
        m_windowSize = size();
    }
}
void Image::setZoomLevel(double zoomlevel)
{

    m_zoomLevel = zoomlevel;
    resizeLabel();

}
void Image::zoomLittle()
{
    m_zoomLevel =0.2;
    resizeLabel();

}

void Image::zoomNormal()
{

    m_zoomLevel =1.0;
    resizeLabel();

}
void Image::zoomBig()
{
    m_zoomLevel =4.0;
    resizeLabel();
}
void Image::createActions()
{
    m_actionZoomIn = new QAction(tr("Zoom In"),this);
    m_actionZoomIn->setToolTip(tr("increase zoom level"));
    m_actionZoomIn->setIcon(QIcon(":/resources/icons/zoom-in-32.png"));
    connect(m_actionZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    m_zoomInShort = new QShortcut(QKeySequence(tr("Ctrl++", "Zoom In")), this);
    m_zoomInShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_zoomInShort, SIGNAL(activated ()), this, SLOT(zoomIn()));
    m_actionZoomIn->setShortcut(m_zoomInShort->key());

    m_actionZoomOut = new QAction(tr("Zoom out"),this);
    m_actionZoomOut->setIcon(QIcon(":/resources/icons/zoom-out-32.png"));
    m_actionZoomOut->setToolTip(tr("Reduce zoom level"));
    connect(m_actionZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));

    m_zoomOutShort = new QShortcut(QKeySequence(tr("Ctrl+-", "Zoom Out")), this);
    m_zoomOutShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_zoomOutShort, SIGNAL(activated ()), this, SLOT(zoomOut()));
    m_actionZoomOut->setShortcut(m_zoomOutShort->key());

    m_actionfitWorkspace = new QAction(tr("Fit the workspace"),this);
    m_actionfitWorkspace->setIcon(QIcon(":/fit-page.png"));
    m_actionfitWorkspace->setToolTip(tr("The window and the image fit the workspace"));
    connect(m_actionfitWorkspace,SIGNAL(triggered()),this,SLOT(onFitWorkSpace()));

    m_fitShort = new QShortcut(QKeySequence(tr("Ctrl+m", "Fit the workspace")), this);
    m_fitShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_fitShort, SIGNAL(activated ()), this, SLOT(onFitWorkSpace()));
    m_actionfitWorkspace->setShortcut(m_fitShort->key());


    m_fitWindowAct =  new QAction(tr("Fit Window"),this);
    m_fitWindowAct->setCheckable(true);
    m_fitWindowAct->setIcon(QIcon(":/fit-window.png"));
    m_fitWindowAct->setToolTip(tr("Image will take the best dimension to fit the window."));
    connect(m_fitWindowAct, SIGNAL(triggered()), this, SLOT(fitWindow()));

    m_fitWindowShort  = new QShortcut(QKeySequence(tr("Ctrl+f", "Fit the window")), this);
    m_fitWindowShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_fitWindowShort, SIGNAL(activated ()), this, SLOT(fitWindow()));
    m_fitWindowAct->setShortcut(m_fitWindowShort->key());



    m_actionlittleZoom = new QAction(tr("Little"),this);
    m_actionlittleZoom->setToolTip(tr("Set the zoom level at 20% "));
    connect(m_actionlittleZoom,SIGNAL(triggered()),this,SLOT(zoomLittle()));

    m_littleShort = new QShortcut(QKeySequence(tr("Ctrl+l", "Set the zoom level at 20%")), this);
    m_littleShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_littleShort, SIGNAL(activated ()), this, SLOT(zoomLittle()));
    m_actionlittleZoom->setShortcut(m_littleShort->key());

    m_actionNormalZoom = new QAction(tr("Normal"),this);
    m_actionNormalZoom->setToolTip(tr("No Zoom"));
    connect(m_actionNormalZoom,SIGNAL(triggered()),this,SLOT(zoomNormal()));

    m_normalShort = new QShortcut(QKeySequence(tr("Ctrl+n", "Normal")), this);
    m_normalShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_normalShort, SIGNAL(activated ()), this, SLOT(zoomNormal()));
    m_actionNormalZoom->setShortcut(m_normalShort->key());


    m_actionBigZoom = new QAction(tr("Big"),this);
    m_actionBigZoom->setToolTip(tr("Set the zoom level at 400%"));
    connect(m_actionBigZoom,SIGNAL(triggered()),this,SLOT(zoomBig()));

    m_bigShort = new QShortcut(QKeySequence(tr("Ctrl+b", "Zoom Out")), this);
    m_bigShort->setContext(Qt::WidgetWithChildrenShortcut);
    connect(m_bigShort, SIGNAL(activated ()), this, SLOT(zoomBig()));
    m_actionBigZoom->setShortcut(m_bigShort->key());




}
void Image::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(m_actionZoomIn);
    menu.addAction(m_actionZoomOut);
    menu.addSeparator();
    menu.addAction( m_actionfitWorkspace);
    menu.addAction( m_fitWindowAct);
    menu.addSeparator();
    menu.addAction(m_actionlittleZoom);
    menu.addAction(m_actionNormalZoom);
    menu.addAction(m_actionBigZoom);
    menu.exec(event->globalPos ()/*event->pos()*/);

}
void Image::fitWindow()
{

    QObject* senderObj = sender();
    if(senderObj == m_fitWindowShort)
    {
        m_fitWindowAct->setChecked(! m_fitWindowAct->isChecked() );
    }

    resizeLabel();
    if(m_fitWindowAct->isChecked())
    {
        m_widgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_widgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_actionZoomIn->setEnabled(false);
        m_actionZoomOut->setEnabled(false);
        m_actionlittleZoom->setEnabled(false);
        m_actionNormalZoom->setEnabled(false);
        m_actionBigZoom->setEnabled(false);
    }
    else
    {
        m_widgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_widgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_actionZoomIn->setEnabled(true);
        m_actionZoomOut->setEnabled(true);
        m_actionlittleZoom->setEnabled(true);
        m_actionNormalZoom->setEnabled(true);
        m_actionBigZoom->setEnabled(true);
    }

    update();
}
void Image::resizeEvent(QResizeEvent *event)
{
    if(m_fitWindowAct->isChecked())
    {
        resizeLabel();
    }

    QMdiSubWindow::resizeEvent(event);

}

void Image::setParent(ImprovedWorkspace *parent)
{
    m_parent=parent;
    QMdiSubWindow::setParent(parent);
    if(m_parent)
        fitWorkSpace();
}

void Image::setCurrentTool(ToolsBar::SelectableTool tool)
{
    m_currentTool = tool;

    /// @todo code inline to remove useless functions.
    switch(m_currentTool)
    {
    case ToolsBar::Handler :
        pointeurMain();
        break;
    default :
        pointeurNormal();
        break;
    }
}
bool Image::readFileFromUri()
{
    if(NULL==m_uri)
    {
        return false;
    }
    if(CleverURI::PICTURE == m_uri->getType())
    {
        m_preferences->registerValue("ImageDirectory",m_uri->getAbsolueDir());
        QImage img(m_uri->getUri());
        if (img.isNull())
        {
            error(tr("Unsupported file format"),this);
            return false;
        }
        setImage(img);
        setTitle(m_uri->getShortName()+tr(" (Picture)"));
    }
    else if(CleverURI::ONLINEPICTURE == m_uri->getType())
    {
        if(!m_pixMap.isNull())
        {
            initImage();
        }
        else
        {
            return false;
        }
    }

    NetworkMessageWriter message(NetMsg::PictureCategory, NetMsg::AddPictureAction);
    fill(message);
    message.sendAll();
    return true;
}
bool Image::openMedia()
{
    QString filepath;
    /*if(CleverURI::PICTURE == m_uri->getType())
    {
        QString filter = tr("Supported Image formats %1").arg("(*.jpg *.jpeg *.png *.bmp *.svg)");

        QString title = tr("Open Picture");
        QString folder = m_preferences->value(QString("ImageDirectory"),".").toString();
        filepath = QFileDialog::getOpenFileName(this,title,folder,filter);
    }
    else*/
    if(CleverURI::ONLINEPICTURE == m_uri->getType())
    {
        OnlinePictureDialog dialog;
        if(QDialog::Accepted == dialog.exec())
        {
            filepath = dialog.getPath();
            m_pixMap = dialog.getPixmap();
            setTitle(dialog.getTitle()+tr(" (Picture)"));
        }
    }
    if(filepath.isEmpty())
    {
        return false;
    }
    else
    {
        m_uri->setUri(filepath);
        return true;
    }
}
