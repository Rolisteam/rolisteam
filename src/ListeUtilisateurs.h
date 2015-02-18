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
    
    #include "Carte.h"
    #include "types.h"
    

    class ListeUtilisateurs : public QDockWidget
    {
	Q_OBJECT

    public :
        ListeUtilisateurs(QWidget *parent = 0);
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
