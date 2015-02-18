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

	#include "AfficheurDisque.h"


	/********************************************************************/
	/* Variables globales utilisees par tous les elements de            */
	/* l'application                                                    */
	/********************************************************************/	
	// Definit la taille courante du trait
	int G_diametreTraitCourant;
	// Definit la taille courante des Pnj
	int G_diametrePnjCourant;


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
	AfficheurDisque::AfficheurDisque(QWidget *parent, bool plein, int minimum)
		: QWidget(parent)
	{
		// Initialisation des variables de la classe
		diametreCourant = minimum;
		diametreMinimum = minimum;
		disquePlein = plein;

		// Mise a jour des variables globales
		if (disquePlein)
			G_diametreTraitCourant = minimum;
		else
			G_diametrePnjCourant = minimum;
	}
	
	/********************************************************************/
	/* Dessine le contenu du widget                                     */
	/********************************************************************/	
	void AfficheurDisque::paintEvent(QPaintEvent *event)
	{
		int diametreAffiche;
		// Creation du painter
		QPainter painter(this);
		// Antialiasing pour les dessins
		painter.setRenderHint(QPainter::Antialiasing, true);
		
		// Si le disque est plein (utilise pour le diametre du trait)
		if (disquePlein)
		{
	        painter.setPen(Qt::NoPen);
    	    painter.setBrush(Qt::black);
    	    diametreAffiche = diametreCourant;
		}
		// Si le disque est vide (utilise pour le diametre des PNJ)
		else
		{
			QPen pen(Qt::black);
			pen.setWidth(4);
	        painter.setPen(pen);
    	    painter.setBrush(Qt::white);
    	    diametreAffiche = diametreCourant - diametreMinimum +1;
		}
		
		// Dessin du disque
		painter.drawEllipse((width()-diametreCourant)/2, (height()-diametreCourant)/2, diametreCourant, diametreCourant);
        painter.setPen(Qt::darkGray);
		// Affichage du diametre
		painter.drawText(0, 0, width(), height(), Qt::AlignRight | Qt::AlignBottom, QString::number(diametreAffiche));
	}

	/********************************************************************/
	/* Modifie le diametre du disque et le redessine                    */
	/********************************************************************/	
	void AfficheurDisque::changerDiametre(int diametre)
	{
		diametreCourant = diametre;

		// Mise a jour des variables globales
		if (disquePlein)
			G_diametreTraitCourant = diametre;
		else
			G_diametrePnjCourant = diametre;

		// Rafraichissement du widget
		update();
	}
	
