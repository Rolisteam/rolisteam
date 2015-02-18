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
#include "Image.h"
#include <QHBoxLayout>
#include <QFileInfo>
#include "improvedworkspace.h"
Image::Image(ImprovedWorkspace *parent)
    : SubMdiWindows(parent),m_NormalSize(0,0)
{

}

Image::Image( CleverURI* uri,  ImprovedWorkspace *parent)
: SubMdiWindows(parent),m_NormalSize(0,0)
{
    m_parent = parent;
    m_uri = uri;
    m_zoomLevel = 1;
    setUi();
    createActions();

    setObjectName("Image");

    setWindowTitle(m_uri->getShortName());
    setWindowIcon(QIcon(":/resources/icons/image.png"));


    m_labelImage = new QLabel(this);
    m_pixMap = QPixmap::fromImage(QImage(m_uri->getUri()));
   //m_labelImage->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_labelImage->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
    m_labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_labelImage->setScaledContents(true);
    m_labelImage->resize(m_pixMap.size());
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setWidget(m_labelImage);
    setWindowFlags(Qt::Window);
    m_labelImage->installEventFilter(this);
    fitWindow();

}


Image::~Image()
{
    delete m_labelImage;
    delete m_scrollArea;
    delete m_actionZoomIn;
    delete m_actionZoomOut;
    delete m_actionfitWorkspace;
    delete m_actionlittleZoom;
    delete m_actionNormalZoom;
    delete m_actionBigZoom;

}
void Image::createActions()
{
    m_actionZoomIn = new QAction(tr("Zoom In"),this);
    m_actionZoomIn->setShortcut(tr("Ctrl++"));
    m_actionZoomIn->setToolTip(tr("increase zoom level"));
    m_actionZoomIn->setIcon(QIcon(":/resources/icons/zoom-in-32.png"));
    connect(m_actionZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    m_actionZoomOut = new QAction(tr("Zoom out"),this);
    m_actionZoomOut->setShortcut(tr("Ctrl+-"));
    m_actionZoomOut->setIcon(QIcon(":/resources/icons/zoom-out-32.png"));
    m_actionZoomOut->setToolTip(tr("Reduce zoom level"));
    connect(m_actionZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));

    m_actionfitWorkspace = new QAction(tr("Fit the workspace"),this);
    m_actionfitWorkspace->setIcon(QIcon(":/resources/icons/fit-page-32.png"));
    m_actionfitWorkspace->setShortcut(tr("Ctrl+5"));
    m_actionfitWorkspace->setToolTip(tr("The window and the image fit the workspace"));
    connect(m_actionfitWorkspace,SIGNAL(triggered()),this,SLOT(onFitWindow()));

    m_actionlittleZoom = new QAction(tr("Little"),this);
    m_actionlittleZoom->setShortcut(tr("Ctrl+1"));
    m_actionZoomIn->setToolTip(tr("Set the zoom level at 20% "));
    connect(m_actionlittleZoom,SIGNAL(triggered()),this,SLOT(zoomLittle()));

    m_actionNormalZoom = new QAction(tr("Normal"),this);
    m_actionNormalZoom->setShortcut(tr("Ctrl+0"));
    m_actionZoomIn->setToolTip(tr("No Zoom"));
    connect(m_actionNormalZoom,SIGNAL(triggered()),this,SLOT(zoomNormal()));


    m_actionBigZoom = new QAction(tr("Big"),this);
    m_actionBigZoom->setShortcut(tr("Ctrl+2"));
    m_actionZoomIn->setToolTip(tr("Set the zoom level at 400%"));
    connect(m_actionBigZoom,SIGNAL(triggered()),this,SLOT(zoomBig()));
}

void Image::setUi()
{
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);

    QVBoxLayout* vlayout= new QVBoxLayout;

    vlayout->setContentsMargins(0,0,0,0);
    vlayout->addWidget(m_scrollArea);


    //installEventFilter(this);

    QWidget* tmp = new QWidget;
    tmp->setLayout(vlayout);
    setWidget(tmp);
    m_scrollArea->installEventFilter(this);

    //

}
void Image::setZoomLevel(double zoomlevel)
{

        m_zoomLevel = zoomlevel;
        resizeLabel();

}

void Image::closeEvent(QCloseEvent *event)
{
	hide();
	event->ignore();
}

bool Image::eventFilter(QObject *obj,QEvent *e)
{

    if(e->type() == QEvent::Wheel)
    {

      QWheelEvent *event = static_cast<QWheelEvent *>(e);
        wheelEvent(event);
        return true;

    }
    else
    return QObject::eventFilter(obj, e);
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
        event->accept();
    }
    else
    {
        SubMdiWindows::wheelEvent(event);
    }
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

void Image::onFitWindow()
{
    resize(m_windowSize);
    m_zoomLevel = 1;
}

void Image::fitWindow()
{
    QSize windowsize = m_parent->viewport()->size();
    //qDebug()<< m_pixMap.height() << m_pixMap.width() << m_labelImage->rect() << geometry();
    while((windowsize.height()<(m_zoomLevel * m_pixMap.height()))||(windowsize.width()<(m_zoomLevel * m_pixMap.width())))
    {
        m_zoomLevel -= 0.2;
    }
    m_labelImage->resize(m_pixMap.size());
    resizeLabel();
   /*if(!m_labelImage->rect().contains(geometry()))
    {*/
        setGeometry(m_labelImage->rect());
    //}
     qDebug()<< m_pixMap.width() << m_pixMap.height() << m_labelImage->rect() << geometry();
    m_zoomLevel = 1.0;
}


void Image::resizeLabel()
{
    //qDebug()<< " cdcec"<< m_pixMap.height() << m_pixMap.width() << m_labelImage->rect() << geometry();

    if(m_zoomLevel<=0)
    {
        m_zoomLevel=0.2;
    }
    if((m_NormalSize.height()!=0)&&(m_NormalSize.width()!=0))
    {
        m_labelImage->resize(m_zoomLevel * m_NormalSize);
    }
    else
    {

        m_labelImage->resize(m_zoomLevel * m_pixMap.size());
        m_NormalSize = m_scrollArea->widget()->size();
    }
    //  qDebug()<< m_pixMap.height() << m_pixMap.width() << m_labelImage->rect() << geometry();
}

void Image::pointeurMain()
{
    m_labelImage->setCursor(Qt::OpenHandCursor);
}


void Image::pointeurNormal()
{
    m_labelImage->setCursor(Qt::ForbiddenCursor);
}
bool  Image::defineMenu(QMenu* menu)
{
    menu->setTitle(tr("Picture Viewer"));
    menu->addAction(m_actionZoomIn);
    menu->addAction(m_actionZoomOut);
    menu->addSeparator();
    menu->addAction( m_actionfitWorkspace);
    menu->addSeparator();
    menu->addAction(m_actionlittleZoom);
    menu->addAction(m_actionNormalZoom);
    menu->addAction(m_actionBigZoom);

    return true;
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
void Image::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(m_actionZoomIn);
    menu.addAction(m_actionZoomOut);
    menu.addSeparator();
    menu.addAction( m_actionfitWorkspace);
    menu.addSeparator();
    menu.addAction(m_actionlittleZoom);
    menu.addAction(m_actionNormalZoom);
    menu.addAction(m_actionBigZoom);


    menu.exec(event->globalPos ()/*event->pos()*/);

}
void Image::paintEvent ( QPaintEvent * event )
{
    SubMdiWindows::paintEvent(event);
    if(m_scrollArea->widgetResizable())
    {

        m_scrollArea->setWidgetResizable(false);

    }
    if((m_NormalSize * m_zoomLevel) != m_labelImage->size())
    {
        m_NormalSize = m_labelImage->size() / m_zoomLevel;
        m_windowSize = size();
    }
}

void Image::zoomBig()
{
    m_zoomLevel =4.0;
    resizeLabel();
}
void Image::saveFile(const QString & file)
{
    if(!file.isEmpty())
    {

    }
}

void Image::openFile(const QString& file)
{
    if(!file.isEmpty())
    {

    }
}

bool Image::hasDockWidget() const
{
    return false;
}
QDockWidget* Image::getDockWidget()
{
    return NULL;
}
