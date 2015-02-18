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


/********************************************************************/
/*                                                                  */
/* Permet de selectionner une couleur dans une palette predefinie   */
/* qui peut etre en partie modifiee.                                */
/*                                                                  */
/********************************************************************/


#ifndef SELECTEUR_COULEUR_H
#define SELECTEUR_COULEUR_H

	#include <QWidget>
	#include <QFrame>
	#include <QColor>
	#include <QLabel>

	#include "types.h"


	class SelecteurCouleur : public QWidget
	{
	public:
		SelecteurCouleur(QWidget *parent = 0);
		void changeCouleurActuelle(QColor couleur);
		void majCouleursPersonnelles();
		void autoriserOuInterdireCouleurs();
		QColor donnerCouleurPersonnelle(int numero);

	private:
		void clicUtilisateur(QPoint positionSouris, bool move = false);

		QLabel *couleurActuelle;
		QLabel *couleurEfface;
		QLabel *couleurMasque;
		QLabel *couleurDemasque;
		QWidget *couleurPredefinie[48];
		QWidget *couleurPersonnelle[16];
		QWidget *separateur1;
		QWidget *separateur2;
		QPixmap *efface_pix;
		QPixmap *masque_pix;
		QPixmap *demasque_pix;
		bool boutonEnfonce;

	protected:
		void mousePressEvent(QMouseEvent *event);		
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		#ifdef MACOS
			void mouseDoubleClickEvent (QMouseEvent *event);
		#endif
	};

#endif
