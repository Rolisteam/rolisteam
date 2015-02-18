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
        #include  "LecteurAudio.h"

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
                LecteurAudio* G_lecteurAudio;
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
