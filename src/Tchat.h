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
/* Fenetre permettant aux utilisateurs de communiquer entre eux par */
/* ecrit. Il existe un tchat par utilisateur distant, et un tchat   */
/* commun permettant de discuter avec tous les utilisateurs.        */
/*                                                                  */
/********************************************************************/	


#ifndef TCHAT_H
#define TCHAT_H

#include <QSplitter>
#include <QFile>
#include "types.h"

#include "TextEditAmeliore.h"


class Tchat : public QSplitter
{
Q_OBJECT

   // enum MessageType { TEXT,DICE,EMOTE};
//enum actionDiscussion {TCHAT_MESSAGE, DICE_MESSAGE, EMOTE_MESSAGE};
public :
    Tchat(QString id, QAction *action,/* QString tmp,*/QWidget *parent = 0);
    ~Tchat();
	QString identifiant();
    void afficherMessage(QString &utilisateur, QColor &couleur, QString &message, actionDiscussion msgtype = TCHAT_MESSAGE);
    //void afficherTirage(QString &utilisateur, QColor &couleur, QString &message);
	void majAction();
	void sauvegarderTchat(QFile &file);

protected :
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);
	/**
	 * @brief read and remove a number from a string.
	 * \param[in/out] string from wich the number will be read and removed
	 * \param[out] value of the readed number
	 * \return true if no problem occured
	 */
    bool GetNumber(QString &str, int* value);

private :
	int calculerJetDes(QString &message, QString &tirage, bool &ok);
	// Ultyme
    int calculerJetDesSR4(QString &message, QString &tirage, QString &glitch, bool ok);
	// FIN Ultyme

	QTextEdit *zoneAffichage;			// Zone de texte ou s'affichent les messages
	TextEditAmeliore *zoneEdition;		// Zone de texte ou l'utilisateur peut ecrire
	QString idJoueur;					// Contient l'ID du jouer a qui doit etre envoye les messages (vide = tous les utilisateurs)
	QList<QString> historiqueMessages;	// Contient l'historique des anciens messages
	int numHistorique;					// Numero de la ligne de l'historique actuellement affichee
	QAction *actionAssociee;			// Action permettant d'afficher/masquer le tchat

    QStringList m_keyWordList;

    QString m_owner;

private slots :
	void emettreTexte();
	void monterHistorique();
	void descendreHistorique();
	void changerEtatCase(bool coche);
};

#endif
