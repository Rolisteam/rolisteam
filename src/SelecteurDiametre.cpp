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

#include "SelecteurDiametre.h"

#include "AfficheurDisque.h"
#include "constantesGlobales.h"


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/    
SelecteurDiametre::SelecteurDiametre(QWidget *parent, bool plein, int min, int max)
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
    frame->setFixedHeight(TAILLE_ICONES * 2 + 12);
    frame->setMaximumWidth(TAILLE_ICONES * 2 + 12);
    
    // Creation du layout du QFrame
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);
    frameLayout->setMargin(0);
    
    // Creation de l'afficheur de disque
    disque = new AfficheurDisque(frame, plein, minimum);
    disque->changerDiametre(minimum);
    
    // Ajout de l'afficheur de disque au layout du QFrame
    frameLayout->addWidget(disque);

    // Ajout du QFrame contenant l'afficheur de disque au layout
    layout->addWidget(frame);
    
    // Creation du QSlider permettant de faire varier le diametre
    diametre = new QSlider(Qt::Horizontal, this);
    diametre->setRange(minimum, maximum);
    diametre->setValue(minimum);
    diametre->setTracking(false);
    // Ajout du QSlider au layout
    layout->addWidget(diametre);

    #ifdef MACOS
        layout->setSpacing(0);
        diametre->setFixedHeight(20);
    #endif
    
    // Connection des signaux
    connect(diametre, SIGNAL(sliderMoved(int)), disque, SLOT(changerDiametre(int)));
    connect(diametre, SIGNAL(sliderMoved(int)), this, SIGNAL(valueChanging(int)));
    connect(diametre, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
}

/********************************************************************/
/* Change la valeur du selecteur de diametre                        */
/********************************************************************/    
void SelecteurDiametre::changerDiametre(int valeur)
{
    // On verifie que la nouvelle valeur est entre les bornes min et max
    if (valeur < minimum)
        valeur = minimum;
    else if (valeur > maximum)
        valeur = maximum;
    
    // Mise a jour du slider (un signal est envoye a l'afficheur de disque)
    diametre->setValue(valeur);
}
