/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2010 by Renaud Guezennec                             *
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
/* DockWidget affichant la liste des utilisateurs connectes, ainsi  */
/* que la couleur leur correspondant. En cliquant sur un            */
/* utilisateur on ouvre un tchat prive avec lui.                    */
/*                                                                  */
/********************************************************************/


#ifndef LISTE_UTILISATEURS_H
#define LISTE_UTILISATEURS_H

    #include <QWidget>
    #include <QDockWidget>
    #include <QTreeWidget>
    #include <QTreeWidgetItem>
    #include <QString>
    #include <QPushButton>
    #include <QSlider>
    #include <QLabel>
    

    #include "types.h"
    
class Carte;
class MainWindow;
class ListeUtilisateurs : public QDockWidget
    {
	Q_OBJECT

    public :
        ListeUtilisateurs(MainWindow *parent = 0);
		bool ajouterJoueur(QString idJoueur, QString nomJoueur, QColor couleur, bool joueurLocal = false, bool mj = false);
		bool supprimerJoueur(QString idJoueur);
		bool ajouterPersonnage(QString idJoueur, QString idPerso, QString nomPerso, QColor couleur, bool joueurLocal = false, bool creerDessins = true);
		bool supprimerPersonnage(QString idPerso);
		bool estUnMj(int index);
		bool estUnMj(QString idJoueur);
		bool mjDansLaListe();
		void majTaillePj(int taille);
		void ajouterTousLesPj(Carte *carte);
		void majAffichagePj(Carte *carte);
		void cocherDecocherPj(QString idPerso, bool cocher);
		void ModifierNomJoueur(QString idJoueur, QString nomJoueur);
		void ModifierNomPerso(QString idPerso, QString nomPerso);
		void ModifierCouleurJoueur(QString idJoueur, QColor CouleurJoueur);
		void ModifierCouleurPerso(QString idPerso, QColor CouleurPerso);
		void emettreTousLesPj(QString idJoueur);
		void cocherCaseTchat(QString idJoueur);
		void decocherCaseTchat(QString idJoueur);
		void faireClignoter(QString idJoueur);
		void nePlusFaireClignoter(QString idJoueur);
		void casesPjNonSelectionnables();
		QColor couleurUtilisateur(QString idJoueur);
		QString indentifiantUtilisateur(int index);
		QString nomUtilisateur(QString idJoueur);
		int taillePj();
		int numeroUtilisateur(QString idJoueur);
		QList<utilisateur> tousLesUtilisateurs();

	signals :
		void ajouterPj(QString idPerso, QString nomPerso, QColor couleur);
		void renommerPj(QString idPerso, QString nouveauNom);
		void effacerPj(QString idPerso);
		void changerCouleurPerso(QString idPerso, QColor couleur);

    private :
		void supprimerPj(QTreeWidgetItem *item);

		QTreeWidget *treeWidget;			// Arbre contenant les joueurs et les personnages
		QWidget *widgetUtilisateurs;		// Widget contenant treeWidget et les boutons
		QPushButton *boutonTchatCommun;		// Bouton permettant d'ouvrir le tchat commun
		QPushButton *boutonNouveau;			// Bouton permettant d'ajouter un nouveau PJ
		QPushButton *boutonSupprimer;		// Bouton permettant de supprimer le PJ selectionne
		QSlider *selecteurTaillePj;			// Selecteur de la taille des PJ
		QLabel *afficheurTaillePj;			// Label affichant la taille des PJ
		bool autoriserSignauxListe;			// Autorise ou interdit la reception des signaux par la fonction changementEtatItem
		bool boutonTchatClignote;			// True si le bouton du tchat commun est en train de clignoter
		QList<QString> listeClignotants;	// Contient les ID des joueurs a faire clignoter
        MainWindow* m_mainWindow;

	private slots :
		void nouveauPj(bool checked);
		void supprimerPjSelectionne(bool checked);
		void changementEtatItem(QTreeWidgetItem *item, int colonne);
		void clicSurItem(QTreeWidgetItem *item, int colonne);
		void emettreChangementTaillePj();
		void afficherTchatCommun();
		void clignote();
	};

#endif
