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
#include "improvedworkspace.h"


Image::Image( QString& filename,  ImprovedWorkspace *parent)
: SubMdiWindows(parent)
{
    m_parent = parent;
    m_filename = filename;
    m_zoomLevel = 1;
    setUi();
    createActions();

	setObjectName("Image");
    m_type = SubMdiWindows::PICTURE;

	setWindowIcon(QIcon(":/icones/vignette image.png"));


    m_labelImage = new QLabel(this);
    m_pixMap = QPixmap::fromImage(QImage(m_filename));
   //m_labelImage->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    m_labelImage->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
    m_labelImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_labelImage->setScaledContents(true);
    m_labelImage->resize(m_pixMap.size());
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setWidget(m_labelImage);


    fitWindow();

}


Image::~Image()
{




}
void Image::createActions()
{
    m_actionZoomIn = new QAction(tr("Zoom In"),this);
    m_actionZoomIn->setShortcut(tr("Ctrl++"));
    m_actionZoomIn->setToolTip(tr("increase zoom level"));
    connect(m_actionZoomIn,SIGNAL(triggered()),this,SLOT(zoomIn()));

    m_actionZoomOut = new QAction(tr("Zoom out"),this);
    m_actionZoomOut->setShortcut(tr("Ctrl+-"));
    m_actionZoomOut->setToolTip(tr("Reduce zoom level"));
    connect(m_actionZoomOut,SIGNAL(triggered()),this,SLOT(zoomOut()));

    m_actionfitWorkspace = new QAction(tr("Fit the workspace"),this);
    m_actionfitWorkspace->setShortcut(tr("Ctrl+5"));
    m_actionfitWorkspace->setToolTip(tr("The window and the image fit the workspace"));
    connect(m_actionfitWorkspace,SIGNAL(triggered()),this,SLOT(fitWindow()));

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

    QVBoxLayout* vlayout= new QVBoxLayout;


    vlayout->addWidget(m_scrollArea);

    m_scrollArea->installEventFilter(this);
    this->installEventFilter(this);

    QWidget* tmp = new QWidget;
    tmp->setLayout(vlayout);
    setWidget(tmp);
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

bool  Image::eventFilter(QObject *obj,QEvent *e)
{
    if(e->type() == QEvent::Wheel)
    {
      QWheelEvent *event = static_cast<QWheelEvent *>(e);
        if(event->modifiers() == Qt::ControlModifier)
        {
            qDebug()<< "event filter";
            int delta = event->delta();
            ;
            if(delta > 0)//zoomin
            {
                m_zoomLevel +=0.2;

            }
            else//zoomOut
            {
                m_zoomLevel -=0.2;
            }
           // m_zoomSpinBox->setValue(currentZoom);
            resizeLabel();
            event->accept();
            return true;
        }

    }

    return QObject::eventFilter(obj, e);
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


void Image::fitWindow()
{
    QSize windowsize = m_parent->viewport()->size();
    while((windowsize.height()<(m_zoomLevel * m_pixMap.height()))||(windowsize.width()<(m_zoomLevel * m_pixMap.width())))
    {
        m_zoomLevel -= 0.2;
    }
    resizeLabel();

    if(!m_labelImage->rect().contains(geometry()))
        setGeometry(m_labelImage->rect());

}


void Image::resizeLabel()
{
    m_labelImage->resize(m_zoomLevel * m_pixMap.size());
}

void Image::pointeurMain()
{
    m_labelImage->setCursor(Qt::OpenHandCursor);
}


void Image::pointeurNormal()
{
    m_labelImage->setCursor(Qt::ForbiddenCursor);
}
void  Image::defineMenu(QMenu* menu)
{
    menu->addAction(m_actionZoomIn);
    menu->addAction(m_actionZoomOut);
    menu->addSeparator();
    menu->addAction( m_actionfitWorkspace);
    menu->addSeparator();
    menu->addAction(m_actionlittleZoom);
    menu->addAction(m_actionNormalZoom);
    menu->addAction(m_actionBigZoom);
}
void Image::zoomLittle()
{
    m_zoomLevel =0.2;
    resizeLabel();
    if(!m_labelImage->rect().contains(geometry()))
        setGeometry(m_labelImage->rect());
}

void Image::zoomNormal()
{
    m_zoomLevel =1.0;
    resizeLabel();
    if(!m_labelImage->rect().contains(geometry()))
        setGeometry(m_labelImage->rect());
}

void Image::zoomBig()
{
    m_zoomLevel =4.0;
    resizeLabel();
}
