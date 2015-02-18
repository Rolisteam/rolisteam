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
/* Variables globales utilisees dans l'application.                 */
/*                                                                  */
/********************************************************************/


#ifndef VARIABLES_GLOBALES_H
#define VARIABLES_GLOBALES_H

	#include <QCursor>
	#include <QString>
	#include <QList>
	#include <QMutex>

	#include "SelecteurCouleur.h"
	#include "BarreOutils.h"
	#include "DessinPerso.h"
	#include "ListeUtilisateurs.h"
	#include "MainWindow.h"
	#include "Liaison.h"
	#include "ClientServeur.h"
	#include "LecteurAudio.h"


	// Fonction globale, declaree dans MainWindow.cpp
	void ecrireLogUtilisateur(QString message);
	
	// Fonctions globales, declaree dans ClientServeur.cpp
	void emettre(char *donnees, quint32 taille, Liaison *sauf = 0);
	void emettre(char *donnees, quint32 taille, int numeroLiaison);


	// Declarees dans main.cpp
	extern initialisation G_initialisation;

	// Declarees dans ClientServeur.cpp
	extern ClientServeur *G_clientServeur;
	extern QMutex G_mutexConnexion;
	extern bool G_joueur;
	extern bool G_client;
	extern QString G_idJoueurLocal;
	extern QColor G_couleurJoueurLocal;

	// Declarees dans DessinPerso.cpp, initialiser dans MainWindow.cpp
	extern QList<DessinPerso::etatDeSante> G_etatsDeSante;

	// Declarees dans SelecteurCouleur.cpp
	extern couleurSelectionee G_couleurCourante;
	extern QColor G_couleurMasque;

	// Declarees dans BarreOutils.cpp
	extern BarreOutils::outilSelectionne G_outilCourant;
	extern QString G_texteCourant;
	extern QString G_nomPnjCourant;
	extern int G_numeroPnjCourant;

	// Declarees dans AfficheurDisque.cpp
	extern int G_diametreTraitCourant;
	extern int G_diametrePnjCourant;
	
	// Declarees dans MainWindow.cpp
	extern MainWindow *G_mainWindow;
	extern bool G_affichageNomPj;
	extern bool G_affichageNomPnj;
	extern bool G_affichageNumeroPnj;
	extern bool G_carteFenetreActive;
	extern QCursor *G_pointeurDessin;
	extern QCursor *G_pointeurTexte;
	extern QCursor *G_pointeurPipette;
	extern QCursor *G_pointeurDeplacer;
	extern QCursor *G_pointeurOrienter;
	extern QCursor *G_pointeurAjouter;
	extern QCursor *G_pointeurSupprimer;
	extern QCursor *G_pointeurEtat;
	extern QString G_dossierMusiquesMj;
	extern QString G_dossierMusiquesJoueur;
	extern QString G_dossierImages;
	extern QString G_dossierPlans;
	extern QString G_dossierScenarii;
	extern QString G_dossierNotes;
	extern QString G_dossierTchats;
	
	// Declarees dans ListeUtilisateur.cpp
	extern ListeUtilisateurs *G_listeUtilisateurs;
	



#endif
