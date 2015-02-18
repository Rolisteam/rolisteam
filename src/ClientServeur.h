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
/* Client/serveur et fenetre de connexion au lancement du soft. Le  */
/* Client/serveur cree une Liaison pour chaque connexion avec un    */
/* hote distant (client ou serveur), c'est ensuite la Liaison qui   */
/* gere la reception et l'emission des messages.                    */
/*                                                                  */
/********************************************************************/	


#ifndef CLIENT_SERVEUR_H
#define CLIENT_SERVEUR_H

#include <QtNetwork>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>

#include "MainWindow.h"
#include "ListeUtilisateurs.h"
#include "Liaison.h"


    class ClientServeur : public QObject
    {
	Q_OBJECT

    public :
		ClientServeur();
		void emettreDonnees(char *donnees, quint32 taille, Liaison *sauf);
		void emettreDonnees(char *donnees, quint32 taille, int numeroLiaison);
		void ajouterLiaison(Liaison *liaison);
		
    signals :
		void connexionEtablie();
		void emissionDonnees(char *donnees, quint32 taille, Liaison *sauf);

    private :
		void creerFenetreConnexion();
		void detruireFenetreConnexion();
		void creerFenetreAttenteConnexion();
		void detruireFenetreAttenteConnexion();
		void emettreIdentite();
		void majParametresInitialisation();
		bool verifierParametres();
		
		MainWindow *mainWindow;				// Principale fenetre de l'application
		QLineEdit *nomJoueur;				// Contient le nom choisi par le joueur
		QLineEdit *adresseServeur;			// Contient l'adresse du serveur
		QLineEdit *portServeur;				// Contient le port du serveur
		QLineEdit *portClient;				// Contient le port de la machine locale, a donner aux clients
		QDialog *fenetreAttente;			// Fenetre affichee pendant l'etablissement de la connexion
		QDialog *fenetreConnexion;			// Fenetre de connexion
		QWidget *identifiantServeur;		// Ligne affichant l'adresse et le port du serveur
		QWidget *identifiantClient;			// Ligne affichant l'adresse et le port de la machine, a donner aux clients
		QPushButton *couleurJoueur;			// Contient la couleur choisie par le joueur
		QRadioButton *boutonJoueur;			// Bouton enfonce si l'utilisateur est un joueur
		QRadioButton *boutonMj;				// Bouton enfonce si l'utilisateur est le MJ
		QRadioButton *boutonClient;			// Bouton enfonce si l'ordinateur local est un client
		QRadioButton *boutonServeur;		// Bouton enfonce si l'ordinateur local est le serveur
		QTcpServer *serveurTcp;				// Serveur TCP (si l'ordinateur local est le serveur)
		QTcpSocket *socketTcp;				// Socket utilise temporairement par un client lors de l'etablissement de la connexion
		QList<Liaison *> liaisons;			// Liste des liaisons vers le serveur ou les clients
		bool client;						// True si l'ordinateur local est un client, false s'il est le serveur
		bool enAttente;						// True si la fenetre d'attente de connexion est affichee
		QString tempNomJoueur;				// Contient le nom du joueur avant son ajout a la liste des utilisateurs
#ifndef NULL_PLAYER
        LecteurAudio* G_lecteurAudio;
#endif		
	private slots :
		void programmePrincipal();
		void demandeConnexion();
		void afficherIdentifiantClient();
		void afficherIdentifiantServeur();
		void ouvrirSelecteurCouleur();
		void nouveauClientConnecte();
		void connexionAuServeurEtablie();
		void erreurDeConnexion(QAbstractSocket::SocketError);
		void connexionAnnulee();
		void finDeLiaison();

	};

#endif
