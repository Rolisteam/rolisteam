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
/* Editeur de notes permettant aux utilisateurs de prendre des      */
/* notes en partie                                                  */
/*                                                                  */
/********************************************************************/	


#ifndef EDITEUR_NOTES_H
#define EDITEUR_NOTES_H

    #include <QWidget>
    #include <QMainWindow>
    #include <QTextEdit>
    #include <QCloseEvent>
    #include <QToolBar>
	#include <QComboBox>
	#include <QFile>


    class EditeurNotes : public QMainWindow
    {
	Q_OBJECT

    public :
	    EditeurNotes();
	    void sauvegarderNotes(QFile &file);
	    void ouvrirNotes(QFile &file);
		QSize sizeHint() const;

    private :
		QTextEdit *notes;			// Contient toutes les notes de l'utilisateur
		QToolBar *barreFontes;		// Contient les boutons de formatage du texte
		QComboBox *selecteurTaille;	// Permet de choisir la taille de la fonte

    private slots :
		void styleNormal();
		void styleGras();
		void styleItalique();
		void styleSouligne();
		void mettreTailleAJour();
		void changementTaille(int index);

	protected :
		void closeEvent(QCloseEvent *event);

	};

#endif
