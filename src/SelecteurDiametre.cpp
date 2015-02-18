/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			     *
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

#include "SelecteurDiametre.h"
#include "displaydisk.h"
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
	
    // Creation du QFrame qui va contenir l'afficheur de disk
	QFrame *frame = new QFrame(this);
	frame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	frame->setLineWidth(2);
	frame->setMidLineWidth(2);
	frame->setFixedHeight(TAILLE_ICONES * 2 + 12);
	frame->setMaximumWidth(TAILLE_ICONES * 2 + 12);
	
	// Creation du layout du QFrame
	QVBoxLayout *frameLayout = new QVBoxLayout(frame);
	frameLayout->setMargin(0);
	
    // Creation de l'afficheur de disk
    disk = new DisplayDisk(frame, plein, minimum);
    disk->changeDiameter(minimum);
	
    // Ajout de l'afficheur de disk au layout du QFrame
    frameLayout->addWidget(disk);

    // Ajout du QFrame contenant l'afficheur de disk au layout
	layout->addWidget(frame);
	
	// Creation du QSlider permettant de faire varier le diametre
	diametre = new QSlider(Qt::Horizontal, this);
	diametre->setRange(minimum, maximum);
	diametre->setValue(minimum);
	// Ajout du QSlider au layout
	layout->addWidget(diametre);

	#ifdef MACOS
		layout->setSpacing(0);
		diametre->setFixedHeight(20);
	#endif
	
	// Connection des signaux
    QObject::connect(diametre, SIGNAL(valueChanged(int)), disk, SLOT(changeDiameter(int)));
}

/********************************************************************/
/* Change la valeur du selecteur de diametre                        */
/********************************************************************/	
void SelecteurDiametre::changerDiametre(int nouvelleValeur)
{
	int valeur = nouvelleValeur;
	
	// On verifie que la nouvelle valeur est entre les bornes min et max
	if (nouvelleValeur < minimum)
		valeur = minimum;
	if (nouvelleValeur > maximum)
		valeur = maximum;
	
    // Mise a jour du slider (un signal est envoye a l'afficheur de disk)
	diametre->setValue(valeur);
}
