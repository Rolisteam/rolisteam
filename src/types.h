/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2013 by Renaud Guezennec                             *
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

#ifndef TYPES_H
#define TYPES_H

	#include <QColor>
	#include <QString>



	enum typeCouleur {qcolor, efface, masque, demasque};


	typedef struct
	{
		typeCouleur type;
		QColor color;
	} couleurSelectionee;

	// Categories de messages envoyes et recus via les sockets
	enum categorieAction {connexion, joueur, persoJoueur, persoNonJoueur, personnage, dessin, plan, image, discussion, musique, parametres};
	enum actionConnexion {finProcessusConnexion};
	enum actionJoueur {connexionJoueur, ajouterJoueur, supprimerJoueur, changerNomJoueur, changerCouleurJoueur};
	enum actionPj {ajouterPersoJoueur, supprimerPersoJoueur, afficherMasquerPersoJoueur, changerTaillePersoJoueur, changerNomPersoJoueur, changerCouleurPersoJoueur};
	enum actionPnj {ajouterPersoNonJoueur, supprimerPersoNonJoueur};
	enum actionPersonnages {ajouterListePerso, deplacerPerso, changerEtatPerso, changerOrientationPerso, afficherMasquerOrientationPerso};
	enum actionDessin {traceCrayon, traceLigne, traceRectangleVide, traceRectanglePlein, traceEllipseVide, traceEllipsePleine, traceTexte, traceMain};
	enum actionPlan {nouveauPlanVide, chargerPlan, importerPlanComplet, fermerPlan};
	enum actionImage {chargerImage, fermerImage};
    enum actionDiscussion {TCHAT_MESSAGE, DICE_MESSAGE, EMOTE_MESSAGE};
	enum actionMusique {nouveauMorceau, lectureMorceau, pauseMorceau, arretMorceau, nouvellePositionMorceau};
        //enum actionMusic {nouveauMorceau, lectureMorceau, pauseMorceau, arretMorceau, nouvellePositionMorceau};
	enum actionParametres {changerListeEtats, autoriserDeplacementPersos};
	
	// Entete des messages envoyes et recus via les sockets
	typedef struct
	{ 
		quint8 categorie;				// Contient une categorie d'action
		quint8 action;					// Contient l'action a effectuer
		quint32 tailleDonnees;			// Indique la taille des donnees suivant l'entete
	} enteteMessage;

	// Definition d'un utilisateur
	typedef struct
	{
		QString idJoueur;				// Identifiant du joueur
		QString nomJoueur;				// Nom du joueur
		QColor couleurJoueur;			// Couleur du joueur
		bool mj;						// True si l'utilisateur est un MJ, false sinon
	} utilisateur;

#endif
