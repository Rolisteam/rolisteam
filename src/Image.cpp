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

#include "WorkspaceAmeliore.h"


Image::Image( QString& filename,  ImprovedWorkspace *parent)
: SubMdiWindows(parent)
{
    m_parent = parent;
    m_filename = filename;
    m_zoomLevel = 1;
    setUi();
   // m_image = ;

	setObjectName("Image");


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
void Image::setUi()
{
    m_scrollArea = new QScrollArea;
    QHBoxLayout* hlayout=new QHBoxLayout;
    QVBoxLayout* vlayout= new QVBoxLayout;
    m_zoomLabel= new QLabel(tr("Zoom level"));
    m_zoomSlider=new QSlider(Qt::Horizontal);
    m_zoomSlider->setMaximum(300);
    m_zoomSlider->setMinimum(25);
    m_zoomSpinBox=new QSpinBox;
    m_zoomSpinBox->setMaximum(300);
    m_zoomSpinBox->setMinimum(25);
    m_zoomSpinBox->setSuffix("%");

    m_zoomSpinBox->setValue(m_zoomLevel*100);
    m_zoomSlider->setValue(m_zoomLevel*100);

    connect(m_zoomSpinBox,SIGNAL(valueChanged(int)),m_zoomSlider,SLOT(setValue(int)));
    connect(m_zoomSlider,SIGNAL(valueChanged(int)),m_zoomSpinBox,SLOT(setValue(int)));
    connect(m_zoomSlider,SIGNAL(valueChanged(int)),this,SLOT(setZoomLevel(int)));
    connect(m_zoomSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setZoomLevel(int)));

    hlayout->addWidget(m_zoomLabel);
    hlayout->addWidget(m_zoomSlider);
    hlayout->addWidget(m_zoomSpinBox);

    vlayout->addWidget(m_scrollArea);
    vlayout->addLayout(hlayout);
    m_scrollArea->installEventFilter(this);

    QWidget* tmp = new QWidget;
    tmp->setLayout(vlayout);
    setWidget(tmp);
}
void Image::setZoomLevel(int zoomlevel)
{
    if(zoomlevel > 300)
    {
        m_zoomSlider->setSingleStep(20);
    }
    else
    {
         m_zoomSlider->setSingleStep(1);
    }
    if((float)zoomlevel/100!=m_zoomLevel)
    {
        m_zoomLevel = (double)zoomlevel/100;
        resizeLabel();
    }
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
            int delta = event->delta();
            int currentZoom = m_zoomSpinBox->value();
            if(delta > 0)//zoomin
            {
                currentZoom +=20;

            }
            else//zoomOut
            {
                currentZoom -=20;
            }
            m_zoomSpinBox->setValue(currentZoom);
            return true;
        }

    }

    return QObject::eventFilter(obj, e);
}




void Image::fitWindow()
{
    QSize windowsize = m_parent->viewport()->size();
    while((windowsize.height()<(m_zoomLevel * m_pixMap.height()))&&(windowsize.width()<(m_zoomLevel * m_pixMap.width())))
    {
        m_zoomLevel -= 0.2;
    }
    resizeLabel();

    if(m_labelImage->rect().contains(geometry()))
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

