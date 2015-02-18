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

	#include "CarteFenetre.h"
	#include "variablesGlobales.h"


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    CarteFenetre::CarteFenetre(Carte *uneCarte, QWidget *parent)
        : QScrollArea(parent)
    {
		// On donne un nom a l'objet "CarteFenetre" pour le differencier des autres fenetres du workspace
		setObjectName("CarteFenetre");
		// On change l'icone de la fenetre
                setWindowIcon(QIcon(":/resources/icones/vignette plan.png"));
		// Sauvegarde de la reference de la carte
        carteAssociee = uneCarte;
		// On aligne la carte au centre de la CarteFenetre
        setAlignment(Qt::AlignCenter);
		// Association de la carte avec le scrollArea
		setWidget(carteAssociee);
		// Redimentionement de la taille du scrollArea
		#ifdef WIN32
			resize(carteAssociee->width()+2, carteAssociee->height()+2);
		#elif defined (MACOS)
			resize(carteAssociee->width()+4, carteAssociee->height()+4);
                #elif defined Q_WS_X11
                        resize(carteAssociee->width()+2, carteAssociee->height()+2);
		#endif
		// Connexion des signaux de deplacement de la Carte
		QObject::connect(carteAssociee, SIGNAL(commencerDeplacementCarteFenetre(QPoint)), this, SLOT(commencerDeplacement(QPoint)));
		QObject::connect(carteAssociee, SIGNAL(deplacerCarteFenetre(QPoint)), this, SLOT(deplacer(QPoint)));
	}

	/********************************************************************/	
	/* Destructeur                                                      */
	/********************************************************************/	
	CarteFenetre::~CarteFenetre()
	{
		// Destruction de l'action associee
		actionAssociee->~QAction();
		// On enleve la carte de la liste des cartes existantes
		G_mainWindow->enleverCarteDeLaListe(carteAssociee->identifiantCarte());
	}

	/********************************************************************/
	/* Cache la fenetre au lieu de la detruire                          */
	/********************************************************************/
	void CarteFenetre::closeEvent(QCloseEvent *event)
	{
		// Masquage de la fenetre
		hide();
		// Deselection de l'action associee
		actionAssociee->setChecked(false);
		// Arret de la procedure de fermeture		
		event->ignore();
	}
	
	/********************************************************************/
	/* Associe l'action d'affichage/masquage a la carte                 */
	/********************************************************************/
	void CarteFenetre::associerAction(QAction *action)
	{
		actionAssociee = action;
	}

	/********************************************************************/
	/* Renvoie la carte contenue dans la scrollArea                     */
	/********************************************************************/
	Carte * CarteFenetre::carte()
	{
		return carteAssociee;
	}

	/********************************************************************/
	/* Initialise le point de depart du deplacement dans la fenetre     */
	/********************************************************************/
	void CarteFenetre::commencerDeplacement(QPoint position)
	{
		// On memorise la position du point de depart
		pointDepart = position;
		// On releve les valeurs des barres de defilement
		horizontalDepart = horizontalScrollBar()->value();
		verticalDepart = verticalScrollBar()->value();
	}
	
	/********************************************************************/
	/* Deplace le contenu de la fenetre en comparant le point de depart */
	/* du deplacement a la position passee en parametre                 */
	/********************************************************************/
	void CarteFenetre::deplacer(QPoint position)
	{
		// On calcule la cifference de position entre le depart et maintenant
		QPoint diff = pointDepart - position;
		// On change la position des barres de defilement
		horizontalScrollBar()->setValue(horizontalDepart + diff.x());
		verticalScrollBar()->setValue(verticalDepart + diff.y());
	}
