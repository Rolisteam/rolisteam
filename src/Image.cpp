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


Image::Image( QString& filename,  QWidget *parent)
: SubMdiWindows(parent),m_image(NULL)
{

    m_filename = filename;
    setUi();
    m_image = new QImage(m_filename);

	setObjectName("Image");

    m_zoomLevel = 1;
	setWindowIcon(QIcon(":/icones/vignette image.png"));


    m_labelImage = new QLabel(this);
    m_pixMap = QPixmap::fromImage(*m_image);
    m_labelImage->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
    //m_labelImage->resize(m_image->width()*m_zoomLevel, m_image->height()*m_zoomLevel);





    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setWidget(m_labelImage);
    m_scrollArea->resize(m_image->width()+2, m_image->height()+2);
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
    m_zoomSlider->setMaximum(800);
    m_zoomSlider->setMinimum(25);
    m_zoomSpinBox=new QSpinBox;
    m_zoomSpinBox->setMaximum(800);
    m_zoomSpinBox->setMinimum(25);
    m_zoomSpinBox->setSuffix("%");

    connect(m_zoomSpinBox,SIGNAL(valueChanged(int)),m_zoomSlider,SLOT(setValue(int)));
    connect(m_zoomSlider,SIGNAL(valueChanged(int)),m_zoomSpinBox,SLOT(setValue(int)));
    connect(m_zoomSlider,SIGNAL(valueChanged(int)),this,SLOT(setZoomLevel(int)));
    connect(m_zoomSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setZoomLevel(int)));
    m_zoomSpinBox->setValue(m_zoomLevel*100);
    m_zoomSlider->setValue(m_zoomLevel*100);
    hlayout->addWidget(m_zoomLabel);
    hlayout->addWidget(m_zoomSlider);
    hlayout->addWidget(m_zoomSpinBox);

    vlayout->addWidget(m_scrollArea);
    vlayout->addLayout(hlayout);

    QWidget* tmp = new QWidget;
    tmp->setLayout(vlayout);
    setWidget(tmp);
}
void Image::setZoomLevel(int zoomlevel)
{
    if((float)zoomlevel/100!=m_zoomLevel)
    {
        m_zoomLevel = (float)zoomlevel/100;
        m_labelImage->setPixmap(m_pixMap.scaled(m_pixMap.width()*m_zoomLevel,m_pixMap.height()*m_zoomLevel));
        m_labelImage->resize(m_image->width()*m_zoomLevel, m_image->height()*m_zoomLevel);
    }
}

void Image::closeEvent(QCloseEvent *event)
{
	hide();
	event->ignore();
}













/*void Image::mousePressEvent(QMouseEvent *event)
{
	// Si l'utilisateur a clique avec la bouton gauche et que l'outil main est selectionne
    if (event->button() == Qt::LeftButton && G_outilCourant == BarreOutils::main)
	{
		// Le deplacement est autorise
		deplacementAutorise = true;
		// On memorise la position du point de depart
		pointDepart = event->pos();
		// On releve les valeurs des barres de defilement
        horizontalDepart = m_scrollArea->horizontalScrollBar()->value();
        verticalDepart = m_scrollArea->verticalScrollBar()->value();
    }
}*/


/*void Image::mouseReleaseEvent(QMouseEvent *event)
{
	// Si le bouton gauche est relache on interdit le deplacement de la carte
	if (event->button() == Qt::LeftButton)
		deplacementAutorise = false;
}*/


/*void Image::mouseMoveEvent(QMouseEvent *event)
{
	// Si le deplacement est autorise
	if (deplacementAutorise)
	{
		// On calcule la cifference de position entre le depart et maintenant
		QPoint diff = pointDepart - event->pos();
		// On change la position des barres de defilement
        m_scrollArea->horizontalScrollBar()->setValue(horizontalDepart + diff.x());
        m_scrollArea->verticalScrollBar()->setValue(verticalDepart + diff.y());
	}
}*/
		

void Image::pointeurMain()
{
    m_labelImage->setCursor(Qt::OpenHandCursor);
}


void Image::pointeurNormal()
{
    m_labelImage->setCursor(Qt::ForbiddenCursor);
}

