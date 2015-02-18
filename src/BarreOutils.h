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
/* DockWidget contenant la barre d'outils.                          */
/*                                                                  */
/********************************************************************/


#ifndef BARRE_OUTILS_H
#define BARRE_OUTILS_H

	#include <QAction>
	#include <QLineEdit>
	#include <QLCDNumber>
	#include <QDockWidget>
	#include <QResizeEvent>
	
	#include "SelecteurCouleur.h"
	#include "SelecteurDiametre.h"


	class BarreOutils : public QDockWidget
	{
	Q_OBJECT
	
	public :
		BarreOutils(QWidget *parent = 0);
		void majCouleursPersonnelles();
		QColor donnerCouleurPersonnelle(int numero);
		void autoriserOuInterdireCouleurs();

		QAction *actionCrayon;
		QAction *actionLigne;
		QAction *actionRectVide;
		QAction *actionRectPlein;
		QAction *actionElliVide;
		QAction *actionElliPlein;
		QAction *actionTexte;
		QAction *actionMain;
		QAction *actionAjoutPnj;
		QAction *actionSupprPnj;
		QAction *actionDeplacePnj;
		QAction *actionEtatPnj;
		QAction *actionRazChrono;

		// Outils selectionnables par l'utilisateur
		enum outilSelectionne {crayon, ligne, rectVide, rectPlein, elliVide, elliPlein, texte, main, ajoutPnj, supprPnj, deplacePerso, etatPerso};

	public slots :
		void incrementeNumeroPnj();
		void changeCouleurActuelle(QColor coul);
		void mettreAJourPnj(int diametre, QString nom);
	
	private :
		void creerActions();
		void creerOutils();

		QWidget *outils;
		QLineEdit *ligneDeTexte;
		QLineEdit *nomPnj;
		QLCDNumber *afficheNumeroPnj;
		SelecteurCouleur *couleur;
		SelecteurDiametre *diametreTrait;
		SelecteurDiametre *diametrePnj;

	private slots :
		void razNumeroPnj();
		void changementTaille(bool floating);
		void texteChange(const QString &texte);
		void nomPnjChange(const QString &texte);

		void crayonSelectionne();
		void ligneSelectionne();
		void rectVideSelectionne();
		void rectPleinSelectionne();
		void elliVideSelectionne();
		void elliPleinSelectionne();
		void texteSelectionne();
		void mainSelectionne();
		void ajoutPnjSelectionne();
		void supprPnjSelectionne();
		void deplacePersoSelectionne();
		void etatPersoSelectionne();

	};

#endif
