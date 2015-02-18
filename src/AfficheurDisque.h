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
/* Widget affichant un disque : utilise par SelecteurDiametre.      */
/*                                                                  */
/********************************************************************/


#ifndef AFFICHEUR_DISQUE_H
#define AFFICHEUR_DISQUE_H

	#include <QWidget>
	

	class AfficheurDisque : public QWidget
	{
	Q_OBJECT
	
	public :
		AfficheurDisque(QWidget *parent = 0, bool plein = true, int minimum = 1);
	
	public slots :
		void changerDiametre(int diametre);

    protected:
        void paintEvent(QPaintEvent *event);
	
	private :
		int diametreCourant;
		int diametreMinimum;
		bool disquePlein;
	
	};

#endif
