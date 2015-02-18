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
#include "ToolBar.h"
#include "DessinPerso.h"
#include "ListeUtilisateurs.h"
#include "MainWindow.h"
#include "Liaison.h"
#include "ClientServeur.h"



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
//extern BarreOutils::outilSelectionne G_outilCourant;
extern QString G_texteCourant;
extern QString G_nomPnjCourant;
extern int G_numeroPnjCourant;

// Declarees dans AfficheurDisque.cpp
extern int g_currentDiameterLine;
extern int g_currentNPCDiameter;

// Declarees dans MainWindow.cpp
extern MainWindow *G_mainWindow;
extern bool G_affichageNomPj;
extern bool G_affichageNomPnj;
extern bool G_affichageNumeroPnj;
extern bool G_carteFenetreActive;



// Declarees dans ListeUtilisateur.cpp
extern ListeUtilisateurs *G_listeUtilisateurs;




#endif
