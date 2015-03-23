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
    m_circle->changerDiametre(minimum);
    
    // Ajout de l'afficheur de disque au layout du QFrame
    frameLayout->addWidget(m_circle);

    // Ajout du QFrame contenant l'afficheur de disque au layout
    layout->addWidget(frame);
    
    // Creation du QSlider permettant de faire varier le diametre
    m_diameter = new QSlider(Qt::Horizontal, this);
    m_diameter->setRange(minimum, maximum);
    m_diameter->setValue(minimum);
    m_diameter->setTracking(false);
    // Ajout du QSlider au layout
    layout->addWidget(m_diameter);

    #ifdef MACOS
        layout->setSpacing(0);
        diametre->setFixedHeight(20);
    #endif
    
    // Connection des signaux
    connect(m_diameter, SIGNAL(sliderMoved(int)), m_circle, SLOT(changerDiametre(int)));
    connect(m_diameter, SIGNAL(sliderMoved(int)), this, SIGNAL(valueChanging(int)));
    connect(m_diameter, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
}

/********************************************************************/
/* Change la valeur du selecteur de diametre                        */
/********************************************************************/    
void DiameterSelector::changerDiametre(int valeur)
{
    // On verifie que la nouvelle valeur est entre les bornes min et max
    if (valeur < minimum)
        valeur = minimum;
    else if (valeur > maximum)
        valeur = maximum;
    
    // Mise a jour du slider (un signal est envoye a l'afficheur de disque)
    m_diameter->setValue(valeur);
}
