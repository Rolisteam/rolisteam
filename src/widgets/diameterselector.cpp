/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
 *                                                                       *
 *    http://www.rolisteam.org/                                          *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include <QVBoxLayout>
#include <QFrame>

#include "widgets/diameterselector.h"

#include "widgets/circledisplayer.h"


#define DEFAULT_ICON_SIZE 20
/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/    
DiameterSelector::DiameterSelector(QWidget *parent, bool plein, int min, int max)
    : QWidget(parent)
{
    // Initialisation des minimum et maximum
    minimum = min;
    maximum = max;

    // Creation du layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    #ifdef MACOS
        layout->setSpacing(0);
    #endif
    
    // Creation du QFrame qui va contenir l'afficheur de disque
    QFrame *frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    frame->setLineWidth(2);
    frame->setMidLineWidth(2);
    frame->setFixedHeight(DEFAULT_ICON_SIZE * 2 + 12);
    frame->setMaximumWidth(DEFAULT_ICON_SIZE * 2 + 12);
    
    // Creation du layout du QFrame
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    frameLayout->setMargin(0);
    
    // Creation de l'afficheur de disque
    m_circle = new CircleDisplayer(frame, plein, minimum,maximum);
	m_circle->changeDiameter(minimum);
    
    // Ajout de l'afficheur de disque au layout du QFrame
    frameLayout->addWidget(m_circle);

    // Ajout du QFrame contenant l'afficheur de disque au layout
    layout->addWidget(frame);
    layout->setAlignment(m_circle,Qt::AlignCenter);

	m_diameterSlider = new QSlider(Qt::Horizontal, this);
	m_diameterSlider->setRange(minimum, maximum);
	m_diameterSlider->setValue(minimum);
	m_diameterSlider->setTracking(false);

	layout->addWidget(m_diameterSlider);

	connect(m_diameterSlider, SIGNAL(sliderMoved(int)), m_circle, SLOT(changeDiameter(int)));
	connect(m_diameterSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(diameterChanged(int)));
}

void DiameterSelector::changerDiametre(int valeur)
{
    if (valeur < minimum)
        valeur = minimum;
    else if (valeur > maximum)
        valeur = maximum;

	m_diameterSlider->setValue(valeur);
}
int DiameterSelector::getCurrentValue()const
{
    return m_diameterSlider->value();
}
