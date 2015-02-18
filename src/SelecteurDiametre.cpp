/*
	Rolistik - logiciel collaboratif d'aide aux jeux de roles en ligne
	Copyright (C) 2007 - Romain Campioni  Tous droits réservés.

	Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
	modifier suivant les termes de la GNU General Public License telle que
	publiée par la Free Software Foundation : soit la version 2 de cette
	licence, soit (à votre gré) toute version ultérieure.

	Ce programme est distribué dans lespoir quil vous sera utile, mais SANS
	AUCUNE GARANTIE : sans même la garantie implicite de COMMERCIALISABILITÉ
	ni dADÉQUATION À UN OBJECTIF PARTICULIER. Consultez la Licence Générale
	Publique GNU pour plus de détails.

	Vous devriez avoir reçu une copie de la Licence Générale Publique GNU avec
	ce programme ; si ce nest pas le cas, consultez :
	<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>

	Par ailleurs ce logiciel est gratuit et ne peut en aucun cas être
	commercialisé, conformément à la "FMOD Non-Commercial License".
*/


	#include <QtGui>

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
		// Ajout du QSlider au layout
		layout->addWidget(diametre);

		#ifdef MACOS
			layout->setSpacing(0);
			diametre->setFixedHeight(20);
		#endif
		
		// Connection des signaux
		QObject::connect(diametre, SIGNAL(valueChanged(int)), disque, SLOT(changerDiametre(int)));
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
		
		// Mise a jour du slider (un signal est envoye a l'afficheur de disque)
		diametre->setValue(valeur);
	}
