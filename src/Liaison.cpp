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


#include <QtGui>
#include <QtNetwork>

#include "Carte.h"
#include "CarteFenetre.h"
#include "DessinPerso.h"
#include "Liaison.h"
#include "ClientServeur.h"
#include "Tchat.h"
#include "variablesGlobales.h"
#include "types.h"


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/
    Liaison::Liaison(QTcpSocket *socket, QObject * parent)
		: QThread(parent)
    {
		// Initialisation des variables
                G_lecteurAudio = LecteurAudio::getInstance();
		socketTcp = socket;
		receptionEnCours = false;

		// Connexion de la fin du thread a la fonction de fermeture de l'application ou de suppression d'un client
		QObject::connect(this, SIGNAL(finished()), G_clientServeur, SLOT(finDeLiaison()));
		// Connexion du signal de reception
		QObject::connect(socketTcp, SIGNAL(readyRead()), this, SLOT(reception()));
		// Connexion du signal d'erreur
		QObject::connect(socketTcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurDeConnexion(QAbstractSocket::SocketError)));

		// Si l'ordi local est un client, on ajoute tt de suite la liaison a la liste, et on connecte le signal d'emission des donnees
		// Le serveur effectue cette operation a la fin de la procedure de connexion du client
		if (G_client)
		{
			// Ajout de la liaison a la liste
			G_clientServeur->ajouterLiaison(this);
			// Connexion de la demande d'emission de donnees du client/serveur a la liaison
			QObject::connect(G_clientServeur, SIGNAL(emissionDonnees(char *, quint32, Liaison *)), this, SLOT(emissionDonnees(char *, quint32, Liaison *)));
		}
	}

	/********************************************************************/
	/* Lance le thread (appele par start())                             */
	/********************************************************************/
	void Liaison::run()
	{
		exec();
	}

	/********************************************************************/
	/* Erreur de connexion                                              */
	/********************************************************************/
	void Liaison::erreurDeConnexion(QAbstractSocket::SocketError erreur)
	{
		qDebug("Une erreur réseau est survenue : %d", erreur);

		// Si la connexion est perdue on quitte le thread
		if (erreur == QAbstractSocket::RemoteHostClosedError)
		{
			// Destruction du socket
			socketTcp->~QTcpSocket();
			// Onception du plan quitte le thread
			quit();
		}
	}

	/********************************************************************/
	/* Emission des donnees vers le socket de la liaison, sauf si le    */
	/* parametre "sauf" correspond a l'instance de la liaison           */
	/********************************************************************/
	void Liaison::emissionDonnees(char *donnees, quint32 taille, Liaison *sauf)
	{
		if (sauf != this)
		{
			// Emission des donnees
			int t = socketTcp->write(donnees, taille);
			qDebug("Emission - Taille donnees : %d", t);
		}
	}

	/********************************************************************/	
	/* Reception de donnees sur un socket. Cette fonction n'est jamais  */
	/* appelee recursivement (jamais de nouvel appel a la fonction      */
	/* lorsque celle-ci est en cours d'execution) : cela permet         */
	/* d'utiliser des variables globales pour l'entete et le corps du   */
	/* message. Il est part contre necessaire de verifier a la fin de   */
	/* chaque reception si nouveau message n'est pas arrive pendant le  */
	/* traitement                                                       */
	/********************************************************************/	
	void Liaison::reception()
	{
		quint32 lu;
		
		// La boucle permet de lire plusieurs messages concatenes
		while (socketTcp->bytesAvailable())
		{
			// S'il s'agit d'un nouveau message
			if (!receptionEnCours)
			{
				// On recupere l'entete du message
				socketTcp->read((char *)&entete, sizeof(enteteMessage));
				// Reservation du tampon
				tampon = new char[entete.tailleDonnees];
				// Initialisation du restant a lire
				restant = entete.tailleDonnees;
			}
	
			// Lecture des donnees a partir du dernier point
			lu = socketTcp->read(&(tampon[entete.tailleDonnees-restant]), restant);
	
			// Si toutes les donnees n'ont pu etre lues
			if (lu < restant)
			{
				// On met a jour le restant a lire
				restant-=lu;
				// On indique qu'une reception est en cours
				receptionEnCours = true;
	
				qDebug("Reception par morceau");
			}
	
			// Si toutes les donnees ont pu etre lu
			else
			{
				// Envoie la notification sur la mainWindows
				QApplication::alert(G_mainWindow);

				// On aiguille vers le traitement adapte
				switch((categorieAction)(entete.categorie))
				{
					case connexion :
						receptionMessageConnexion();
						break;
					case joueur :
						receptionMessageJoueur();
						break;
					case persoJoueur :
						receptionMessagePersoJoueur();
						break;
					case persoNonJoueur :
						receptionMessagePersoNonJoueur();
						break;
					case personnage :
						receptionMessagePersonnage();
						break;
					case dessin :
						receptionMessageDessin();
						break;
					case plan :
						receptionMessagePlan();
						break;
					case image :
						receptionMessageImage();
						break;
					case discussion :
						receptionMessageDiscussion();
						break;
					case musique :
						receptionMessageMusique();
						break;
					case parametres :
						receptionMessageParametres();
						break;
					default :
						qWarning("Categorie d'entete inconnue (reception - Liaison.cpp)");
						return;
				}
				// On libere le tampon
				delete[] tampon;
				// On indique qu'aucun message n'est en cours de reception
				receptionEnCours = false;
				qDebug("Reception terminee");
			}

		} // Fin du while
	}

	/********************************************************************/
	/* Reception d'un message de categorie Joueur                       */
	/********************************************************************/
	void Liaison::receptionMessageConnexion()
	{
		qDebug("Reception d'un message de categorie Connexion");
		int p = 0;
                Q_UNUSED(p)
		// Le serveur indique que la processus de connexion vient de se terminer
		if (entete.action == finProcessusConnexion)
		{
			// Message sur le log utilisateur
			ecrireLogUtilisateur(tr("Fin du processus de connexion"));
			// On met a jour l'espace de travail
			G_mainWindow->mettreAJourEspaceTravail();
		}
	}

	/********************************************************************/
	/* Reception d'un message de categorie Joueur                       */
	/********************************************************************/
	void Liaison::receptionMessageJoueur()
	{
		qDebug("Reception d'un message de categorie Joueur");
		int p = 0;

		// Un nouveau joueur vient de se connecter au serveur (serveur uniquement)
		if (entete.action == connexionJoueur)
		{
			// On protege le code par un mutex : ceci evite des incoherences de connexion lorsque tous les clients se
			// connectent au meme moment (joueurs declares en double, tchat inoperationnel, etc...)
			G_mutexConnexion.lock();

			// Ajout de la liaison a la liste
			G_clientServeur->ajouterLiaison(this);
			// Connexion de la demande d'emission de donnees du client/serveur a la liaison
			QObject::connect(G_clientServeur, SIGNAL(emissionDonnees(char *, quint32, Liaison *)), this, SLOT(emissionDonnees(char *, quint32, Liaison *)));

			// On recupere le nom
			quint16 tailleNom;
			memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauNom = new QChar[tailleNom];
			memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
			p+=tailleNom*sizeof(QChar);
			QString nomJoueur(tableauNom, tailleNom);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idJoueur(tableauId, tailleId);
			// On recupere la couleur
			QRgb rgb;
			memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
			p+=sizeof(QRgb);
			QColor couleurJoueur(rgb);
			// On recupere la nature de l'utilisateur (MJ ou joueur)
			quint8 mj;
			memcpy(&mj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// S'il y a deja un MJ dans la liste, le nouvel utilisateur est forcement un joueur
			mj = G_listeUtilisateurs->mjDansLaListe()?false:mj;
			
			// Creation de l'utilisateur
			utilisateur util;
			util.idJoueur = idJoueur;
			util.nomJoueur = nomJoueur;
			util.couleurJoueur = couleurJoueur;
			util.mj = mj;
			
			// On recupere la liste des utilisateurs deja connectes
			QList<utilisateur> utilisateursConnectes = G_listeUtilisateurs->tousLesUtilisateurs();
			// On envoie la liste des utilisateurs au nouveau joueur
			int nbrUtilisateurs = utilisateursConnectes.size();
			for (int i=0; i<nbrUtilisateurs; i++)
				emettreUtilisateur(&(utilisateursConnectes[i]));

			// Ajout du joueur nouvellement connecte a la liste d'utilisateurs
			G_listeUtilisateurs->ajouterJoueur(idJoueur, nomJoueur, couleurJoueur, false, mj);
			
			// On envoie le nouveau joueur a l'ensemble des clients
			emettreUtilisateur(&util, true);

			// On affiche un message dans la fenetre de log utilisateur
			ecrireLogUtilisateur(util.nomJoueur + tr(" vient de rejoindre la partie"));

			// Si le nouvel utilisateur n'est pas un MJ, on emet l'etat actuel du lecteur audio
			if (!mj)
				G_lecteurAudio->emettreEtat(idJoueur);

			// On emet tous les plans deja ouverts au nouveau client
			G_mainWindow->emettreTousLesPlans(idJoueur);

			// On emet toutes les images deja ouvertes au nouveau client
			G_mainWindow->emettreToutesLesImages(idJoueur);

			// On emet l'ensemble des personnages joueurs deja crees (et on demande qu'ils ne creent pas de DessinPerso associes sur les cartes)
			G_listeUtilisateurs->emettreTousLesPj(idJoueur);

			// Liberation de la memoire allouee
			delete[] tableauNom;
			delete[] tableauId;

			// On indique au nouveau joueur que le processus de connexion vient d'arriver a son terme
			// Creation de l'entete du message
			enteteMessage uneEntete;
			uneEntete.categorie = connexion;
			uneEntete.action = finProcessusConnexion;
			uneEntete.tailleDonnees = 0;
			
			// Emission du message
			emissionDonnees((char *)&uneEntete, sizeof(enteteMessage));

			// Ajout du tchat correspondant au nouveau joueur
			G_mainWindow->ajouterTchat(idJoueur, nomJoueur);

			// Deblocage du mutex : les autres threads peuvent entrer dans la zone protegee
			G_mutexConnexion.unlock();
		}

		// L'hote demande au soft local d'ajouter un joueur a la liste des utilisateurs
		else if (entete.action == ajouterJoueur)
		{
			// On recupere le nom
			quint16 tailleNom;
			memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauNom = new QChar[tailleNom];
			memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
			p+=tailleNom*sizeof(QChar);
			QString nomJoueur(tableauNom, tailleNom);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idJoueur(tableauId, tailleId);
			// On recupere la couleur
			QRgb rgb;
			memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
			p+=sizeof(QRgb);
			QColor couleurJoueur(rgb);
			// On recupere la nature de l'utilisateur (MJ ou joueur)
			quint8 mj;
			memcpy(&mj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// On recupere l'info indiquant si l'ajout de l'utilisateur doit etre affiche (1) ou pas (0)
			quint8 affiche;
			memcpy(&affiche, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);

			// S'il ne s'agit pas de la reception des autres utilisateurs dans le cadre de la procedure de connexion, on affiche un message
			if (affiche && idJoueur != G_idJoueurLocal)
				ecrireLogUtilisateur(nomJoueur + tr(" vient de rejoindre la partie"));

			// Si l'utilisateur recu est l'utilisateur local (procedure de connexion)
			if (idJoueur == G_idJoueurLocal)
			{
				// On verifie si la nature recue (MJ ou joueur) correspond a celle envoyee, dans le cas contraire il n'existe qu'une seule possiblite :
				// l'utilisateur local a demande a etre MJ mais un MJ etait deja connecte au serveur avant lui, il devient donc simple joueur
				if (G_joueur != !mj)
				{
					// On change la nature de l'utilisateur local : il devient joueur au lieu de MJ
					G_joueur = true;
					// On met a jour l'espace de travail en consequence (limitation des droits)
					G_mainWindow->changementNatureUtilisateur();
					// On affiche un message dans le log utilisateur
                    ecrireLogUtilisateur(tr("Un MJ est déjà  connecté au serveur, celui-ci vous donne donc le statut de joueur"));
				}
			}

			// Ajout du joueur a la liste
			G_listeUtilisateurs->ajouterJoueur(idJoueur, nomJoueur, couleurJoueur, idJoueur==G_idJoueurLocal, mj);
			// Ajout du tchat s'il ne s'agit pas de l'utilisateur local
			if (idJoueur != G_idJoueurLocal)
				G_mainWindow->ajouterTchat(idJoueur, nomJoueur);

			// Liberation de la memoire allouee
			delete[] tableauNom;
			delete[] tableauId;
		}

		// Suppression d'un joueur qui vient de se deconnecter
		else if (entete.action == supprimerJoueur)
		{
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idJoueur(tableauId, tailleId);

			// Si l'utilisateur etait le MJ, on reinitialise le lecteur audio
			if (G_listeUtilisateurs->estUnMj(idJoueur))
				G_lecteurAudio->joueurNouveauFichier("");
			// On supprime le joueur de la liste
			G_listeUtilisateurs->supprimerJoueur(idJoueur);
			// On supprime le tchat
			G_mainWindow->supprimerTchat(idJoueur);

			// Liberation de la memoire allouee
			delete[] tableauId;
		}
		
		// L'hote demande au soft local de changer le nom d'un joueur
		else if (entete.action == changerNomJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement de nom a l'ensemble des clients
			faireSuivreMessage(false);

			// On recupere le nom
			quint16 tailleNom;
			memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauNom = new QChar[tailleNom];
			memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
			p+=tailleNom*sizeof(QChar);
			QString nomJoueur(tableauNom, tailleNom);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idJoueur(tableauId, tailleId);

			// On change le nom du joueur dans la liste
			G_listeUtilisateurs->ModifierNomJoueur(idJoueur, nomJoueur);
						
			// Liberation de la memoire allouee
			delete[] tableauNom;
			delete[] tableauId;
		}

		// L'hote demande au soft local de changer la couleur d'un joueur
		else if (entete.action == changerCouleurJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement de couleur a l'ensemble des clients
			faireSuivreMessage(false);

			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idJoueur(tableauId, tailleId);
			// On recupere la couleur
			QRgb rgb;
			memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
			p+=sizeof(QRgb);
			QColor couleurJoueur(rgb);

			// Changement de la couleur du joueur dans la liste
			G_listeUtilisateurs->ModifierCouleurJoueur(idJoueur, couleurJoueur);

			// Liberation de la memoire allouee
			delete[] tableauId;
		}
		
		else
		{
			qWarning("Action Joueur inconnue (receptionMessageJoueur - Liaison.cpp)");
			return;
		}
	}
	
	/********************************************************************/
	/* Reception d'un message de categorie PersoJoueur                  */
	/********************************************************************/
	void Liaison::receptionMessagePersoJoueur()
	{
		qDebug("Reception d'un message de categorie PersoJoueur");
		int p = 0;
		
		if (entete.action == ajouterPersoJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre la creation de PJ aux autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant du joueur
			quint8 tailleIdJoueur;
			memcpy(&tailleIdJoueur, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdJoueur = new QChar[tailleIdJoueur];
			memcpy(tableauIdJoueur, &(tampon[p]), tailleIdJoueur*sizeof(QChar));
			p+=tailleIdJoueur*sizeof(QChar);
			QString idJoueur(tableauIdJoueur, tailleIdJoueur);
			// On recupere l'identifiant du PJ
			quint8 tailleIdPj;
			memcpy(&tailleIdPj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPj = new QChar[tailleIdPj];
			memcpy(tableauIdPj, &(tampon[p]), tailleIdPj*sizeof(QChar));
			p+=tailleIdPj*sizeof(QChar);
			QString idPerso(tableauIdPj, tailleIdPj);
			// On recupere le nom
			quint16 tailleNom;
			memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauNom = new QChar[tailleNom];
			memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
			p+=tailleNom*sizeof(QChar);
			QString nomPerso(tableauNom, tailleNom);
			// On recupere la couleur
			QRgb rgb;
			memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
			p+=sizeof(QRgb);
			QColor couleurPerso(rgb);
			// On recupere l'info indiquant s'il faut creer (1) ou pas (0) les DessinPerso associes au PJ
			quint8 dessin;
			memcpy(&dessin, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);

			// Ajout du PJ a la liste d'utilisateurs
			G_listeUtilisateurs->ajouterPersonnage(idJoueur, idPerso, nomPerso, couleurPerso, false, dessin);

			// S'il ne s'agit pas d'un ajout de PJ consecutif a la connexion, on ecrit un message sur le log utilisateur
			if (dessin)
				ecrireLogUtilisateur(G_listeUtilisateurs->nomUtilisateur(idJoueur) + tr(" vient de crÃ©er un nouveau personnage"));

			// Liberation de la memoire allouee
			delete[] tableauIdJoueur;
			delete[] tableauIdPj;
			delete[] tableauNom;
		}

		else if (entete.action == supprimerPersoJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre la suppression de PJ aux autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant du PJ
			quint8 tailleIdPj;
			memcpy(&tailleIdPj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPj = new QChar[tailleIdPj];
			memcpy(tableauIdPj, &(tampon[p]), tailleIdPj*sizeof(QChar));
			p+=tailleIdPj*sizeof(QChar);
			QString idPerso(tableauIdPj, tailleIdPj);

			// On supprime le personnage de la liste et des cartes
			G_listeUtilisateurs->supprimerPersonnage(idPerso);

			// Liberation de la memoire allouee
			delete[] tableauIdPj;
		}
		
		else if (entete.action == afficherMasquerPersoJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre l'affichage/masquage de PJ aux autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere l'identifiant du PJ
			quint8 tailleIdPj;
			memcpy(&tailleIdPj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPj = new QChar[tailleIdPj];
			memcpy(tableauIdPj, &(tampon[p]), tailleIdPj*sizeof(QChar));
			p+=tailleIdPj*sizeof(QChar);
			QString idPerso(tableauIdPj, tailleIdPj);
			// On recupere l'info affichage/masquage
			quint8 affichage;
			memcpy(&affichage, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);

			// On recherche la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche un message d'erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'une demande d'affichage/masquage de PJ (receptionMessagePersoJoueur - Liaison.cpp)");

			// Si la carte a ete trouvee
			else
			{
				// On affiche/masque le PJ sur la carte
				carte->affichageDuPj(idPerso, affichage);
				// On coche/decoche le PJ dans la liste d'utilisateurs si la carte est actuellement active
				if (G_mainWindow->estLaFenetreActive(G_mainWindow->trouverCarteFenetre(idPlan)))
					G_listeUtilisateurs->cocherDecocherPj(idPerso, affichage);
			}

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
			delete[] tableauIdPj;
		}
		
		else if (entete.action == changerTaillePersoJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement de taille de PJ aux autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere la nouvelle taille des PJ
			quint8 taillePj;
			memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			
			// On recherche la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche un message d'erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'une demande de changement de taille des PJ (receptionMessagePersoJoueur - Liaison.cpp)");

			// Si la carte a ete trouvee
			else
			{
				// On met a jour les PJ sur la carte
				carte->changerTaillePjCarte(taillePj);
				// On met a jour le selecteur de taille si la carte est la fenetre active
				G_mainWindow->mettreAJourSelecteurTaille(idPlan, taillePj);
			}

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
		}
		
		else if (entete.action == changerNomPersoJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement de nom a l'ensemble des clients
			faireSuivreMessage(false);
			
			// On recupere le nom
			quint16 tailleNom;
			memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauNom = new QChar[tailleNom];
			memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
			p+=tailleNom*sizeof(QChar);
			QString nomPerso(tableauNom, tailleNom);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPerso(tableauId, tailleId);

			// On change le nom du personnage dans la liste
			G_listeUtilisateurs->ModifierNomPerso(idPerso, nomPerso);
			
			// Liberation de la memoire allouee
			delete[] tableauNom;
			delete[] tableauId;
		}
		
		else if (entete.action == changerCouleurPersoJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement de couleur a l'ensemble des clients
			faireSuivreMessage(false);

			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPerso(tableauId, tailleId);
			// On recupere la couleur
			QRgb rgb;
			memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
			p+=sizeof(QRgb);
			QColor couleurPerso(rgb);

			// Changement de la couleur du personnage dans la liste
			G_listeUtilisateurs->ModifierCouleurPerso(idPerso, couleurPerso);

			// Liberation de la memoire allouee
			delete[] tableauId;
		}

		else
		{
			qWarning("Action persoJoueur inconnue (receptionMessagePersoJoueur - Liaison.cpp)");
			return;
		}
	}
	
	/********************************************************************/
	/* Reception d'un message de categorie PersoNonJoueur               */
	/********************************************************************/
	void Liaison::receptionMessagePersoNonJoueur()
	{
		qDebug("Reception d'un message de categorie Pnj");
		int p = 0;
		
		if (entete.action == ajouterPersoNonJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre l'ajout de PNJ a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);

			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un PNJ a ajouter (receptionMessagePersoNonJoueur - Liaison.cpp)");
			// Sinon on ajoute le PNJ a la carte
			else
				extrairePersonnage(carte, &(tampon[p]));

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
		}
		
		else if (entete.action == supprimerPersoNonJoueur)
		{
			// Si l'ordinateur local est le serveur, on fait suivre l'ajout de PNJ a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere l'identifiant du PNJ
			quint8 tailleIdPnj;
			memcpy(&tailleIdPnj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPnj = new QChar[tailleIdPnj];
			memcpy(tableauIdPnj, &(tampon[p]), tailleIdPnj*sizeof(QChar));
			p+=tailleIdPnj*sizeof(QChar);
			QString idPerso(tableauIdPnj, tailleIdPnj);

			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un PNJ a supprimer (receptionMessagePersoNonJoueur - Liaison.cpp)");
			// Sinon on supprime le PNJ de la carte
			else
				carte->effacerPerso(idPerso);

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
			delete[] tableauIdPnj;
		}

		else
		{
			qWarning("Action persoNonJoueur inconnue (receptionMessagePnj - Liaison.cpp)");
			return;
		}
	}
	
	/********************************************************************/
	/* Reception d'un message de categorie Personnage                   */
	/********************************************************************/
	void Liaison::receptionMessagePersonnage()
	{
		qDebug("Reception d'un message de categorie Personnage");
		int p = 0;
		
		if (entete.action == ajouterListePerso)
		{
			// Si l'ordinateur local est le serveur on fait suivre la liste de persos a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPlan(tableauId, tailleId);
			// On recupere le nombre de personnages presents dans le message
			quint16 nbrPersonnages;
			memcpy(&nbrPersonnages, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			
			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'une liste de personnages a ajouter (receptionMessagePersonnage - Liaison.cpp)");
			// Sinon on recupere l'ensemble des personnages et on les ajoute a la carte
			else
				for (int i=0; i<nbrPersonnages; i++)
					p += extrairePersonnage(carte, &(tampon[p]));
			
			// Liberation de la memoire allouee
			delete[] tableauId;
		}
		
		else if (entete.action == deplacerPerso)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le deplacement de perso a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere l'identifiant du personnage
			quint8 tailleIdPerso;
			memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPerso = new QChar[tailleIdPerso];
			memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
			p+=tailleIdPerso*sizeof(QChar);
			QString idPerso(tableauIdPerso, tailleIdPerso);
			// On recupere le nbr de points du deplacement
                        qint32 nbrPoints;
			memcpy(&nbrPoints, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			// On recupere les points du deplacement
			QList<QPoint> listeDeplacement;
			QPoint point;
			for (int i=0; i<nbrPoints; i++)
			{
				qint16 pointX, pointY;
				memcpy(&pointX, &(tampon[p]), sizeof(qint16));
				p+=sizeof(qint16);
				memcpy(&pointY, &(tampon[p]), sizeof(qint16));
				p+=sizeof(qint16);
				point = QPoint(pointX, pointY);
				listeDeplacement.append(point);
			}
			
			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un deplacement de personnage (receptionMessagePersonnage - Liaison.cpp)");
			// Sinon lance le deplacement du personnage
			else
				carte->lancerDeplacementPersonnage(idPerso, listeDeplacement);
			
			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
			delete[] tableauIdPerso;
		}
		
		else if (entete.action == changerEtatPerso)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement d'etat a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere l'identifiant du perso
			quint8 tailleIdPerso;
			memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPerso = new QChar[tailleIdPerso];
			memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
			p+=tailleIdPerso*sizeof(QChar);
			QString idPerso(tableauIdPerso, tailleIdPerso);
			// On recupere le numero d'etat de sante
			quint16 numEtat;
			memcpy(&numEtat, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);

			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un changement d'etat de perso (receptionMessagePersonnage - Liaison.cpp)");
			// Sinon on change l'etat du perso
			else
			{
				DessinPerso *perso = carte->trouverPersonnage(idPerso);
				// Si le personnage existe
				if (perso)
					perso->changerEtatDeSante(numEtat);
				// S'il est introuvable
				else
					qWarning("Personnage introuvable a la reception d'un changement d'etat de perso (receptionMessagePersonnage - Liaison.cpp)");
			}

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
			delete[] tableauIdPerso;
		}
		
		else if (entete.action == changerOrientationPerso)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement d'orientation a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere l'identifiant du perso
			quint8 tailleIdPerso;
			memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPerso = new QChar[tailleIdPerso];
			memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
			p+=tailleIdPerso*sizeof(QChar);
			QString idPerso(tableauIdPerso, tailleIdPerso);
			// On recupere la nouvelle orientation
			qint16 pointX, pointY;
			memcpy(&pointX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&pointY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QPoint orientation(pointX, pointY);

			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un changement d'orientation (receptionMessagePersonnage - Liaison.cpp)");
			// Sinon on change l'orientation du personnage
			else
			{
				DessinPerso *perso = carte->trouverPersonnage(idPerso);
				// Si le personnage existe
				if (perso)
					perso->nouvelleOrientation(orientation);
				// S'il est introuvable
				else
					qWarning("Personnage introuvable a la reception d'un changement d'orientation (receptionMessagePersonnage - Liaison.cpp)");
			}

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
			delete[] tableauIdPerso;
		}
		
		else if (entete.action == afficherMasquerOrientationPerso)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le changement d'affichage d'orientation a l'ensemble des autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere l'identifiant du perso
			quint8 tailleIdPerso;
			memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPerso = new QChar[tailleIdPerso];
			memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
			p+=tailleIdPerso*sizeof(QChar);
			QString idPerso(tableauIdPerso, tailleIdPerso);
			// On recupere l'info d'affichage/masquage d'orientation
			quint8 afficheOrientation;
			memcpy(&afficheOrientation, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);

			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche une erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un affichage/masquage d'orientation (receptionMessagePersonnage - Liaison.cpp)");
			// Sinon on change l'etat de l'affichage de l'orientation du personnage
			else
			{
				DessinPerso *perso = carte->trouverPersonnage(idPerso);
				// Si le personnage existe
				if (perso)
					perso->afficherOrientation(afficheOrientation);
				// S'il est introuvable
				else
					qWarning("Personnage introuvable a la reception d'un affichage/masquage d'orientation (receptionMessagePersonnage - Liaison.cpp)");
			}

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
			delete[] tableauIdPerso;
		}
	}

	/********************************************************************/
	/* Reception d'un message de categorie Dessin                       */
	/********************************************************************/
	void Liaison::receptionMessageDessin()
	{
		qDebug("Reception d'un message de categorie Dessin");
		int p = 0;

		if (entete.action == traceCrayon)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le trace a l'ensemble des clients
			// (y compris celui qui l'a emis, afin de conserver la coherence des plans)
			faireSuivreMessage(true);
			
			// On recupere l'identifiant du joueur
			quint8 tailleIdJoueur;
			memcpy(&tailleIdJoueur, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdJoueur = new QChar[tailleIdJoueur];
			memcpy(tableauIdJoueur, &(tampon[p]), tailleIdJoueur*sizeof(QChar));
			p+=tailleIdJoueur*sizeof(QChar);
			QString idJoueur(tableauIdJoueur, tailleIdJoueur);
			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);
			// On recupere le nombre de points
                        qint32 nbrPoints;
			memcpy(&nbrPoints, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			// On recupere les points du trace
			QList<QPoint> listePoints;
			QPoint point;
			for (int i=0; i<nbrPoints; i++)
			{
				qint16 pointX, pointY;
				memcpy(&pointX, &(tampon[p]), sizeof(qint16));
				p+=sizeof(qint16);
				memcpy(&pointY, &(tampon[p]), sizeof(qint16));
				p+=sizeof(qint16);
				point = QPoint(pointX, pointY);
				listePoints.append(point);
			}
			// On recupere le rectangle a rafraichir
			qint16 zoneX, zoneY, zoneW, zoneH;
			memcpy(&zoneX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneW, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneH, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QRect zoneARafraichir(zoneX, zoneY, zoneW, zoneH);
			// On recupere le diametre
			quint8 diametre;
			memcpy(&diametre, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// On recupere la couleur
			couleurSelectionee couleur;
			memcpy(&couleur, &(tampon[p]), sizeof(couleurSelectionee));
			p+=sizeof(couleurSelectionee);
			
			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche un message d'erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un trace de crayon (receptionMessageDessin - Liaison.cpp)");
			// Sinon on demande le dessin du trace
			else
				carte->dessinerTraceCrayon(&listePoints, zoneARafraichir, diametre, couleur, idJoueur==G_idJoueurLocal);

			// Liberation de la memoire allouee
			delete[] tableauIdJoueur;
			delete[] tableauIdPlan;
		}
		
		else if (entete.action == traceTexte)
		{
			// Si l'ordinateur local est le serveur, on fait suivre le trace a l'ensemble des clients
			// (y compris celui qui l'a emis, afin de conserver la coherence des plans)
			faireSuivreMessage(true);
			
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPlan(tableauId, tailleId);
			// On recupere le texte
			quint16 tailleTexte;
			memcpy(&tailleTexte, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauTexte = new QChar[tailleTexte];
			memcpy(tableauTexte, &(tampon[p]), tailleTexte*sizeof(QChar));
			p+=tailleTexte*sizeof(QChar);
			QString texte(tableauTexte, tailleTexte);
			// On recupere le point d'arrivee
			qint16 positionSourisX, positionSourisY;
			memcpy(&positionSourisX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&positionSourisY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QPoint positionSouris(positionSourisX, positionSourisY);
			// On recupere le rectangle a rafraichir
			qint16 zoneX, zoneY, zoneW, zoneH;
			memcpy(&zoneX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneW, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneH, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QRect zoneARafraichir(zoneX, zoneY, zoneW, zoneH);
			// On recupere la couleur
			couleurSelectionee couleur;
			memcpy(&couleur, &(tampon[p]), sizeof(couleurSelectionee));
			p+=sizeof(couleurSelectionee);
			
			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche un message d'erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un trace texte (receptionMessageDessin - Liaison.cpp)");
			// Sinon on demande le dessin du trace
			else
				carte->dessinerTraceTexte(texte, positionSouris, zoneARafraichir, couleur);

			// Liberation de la memoire allouee
			delete[] tableauId;
			delete[] tableauTexte;
		}
		
		else if (entete.action == traceMain)
		{
		}
		
		// Tous les autres outils
		else
		{
			// Si l'ordinateur local est le serveur, on fait suivre le trace a l'ensemble des clients
			// (y compris celui qui l'a emis, afin de conserver la coherence des plans)
			faireSuivreMessage(true);
			
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPlan(tableauId, tailleId);
			// On recupere le point de depart
			qint16 departX, departY;
			memcpy(&departX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&departY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QPoint pointDepart(departX, departY);
			// On recupere le point d'arrivee
			qint16 arriveeX, arriveeY;
			memcpy(&arriveeX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&arriveeY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QPoint pointArrivee(arriveeX, arriveeY);
			// On recupere le rectangle a rafraichir
			qint16 zoneX, zoneY, zoneW, zoneH;
			memcpy(&zoneX, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneY, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneW, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			memcpy(&zoneH, &(tampon[p]), sizeof(qint16));
			p+=sizeof(qint16);
			QRect zoneARafraichir(zoneX, zoneY, zoneW, zoneH);
			// On recupere le diametre
			quint8 diametre;
			memcpy(&diametre, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// On recupere la couleur
			couleurSelectionee couleur;
			memcpy(&couleur, &(tampon[p]), sizeof(couleurSelectionee));
			p+=sizeof(couleurSelectionee);
			
			// Recherche de la carte concernee
			Carte *carte = G_mainWindow->trouverCarte(idPlan);
			// Si la carte est introuvable on affiche un message d'erreur
			if (!carte)
				qWarning("Carte introuvable a la reception d'un trace general (receptionMessageDessin - Liaison.cpp)");
			// Sinon on demande le dessin du trace
			else
				carte->dessinerTraceGeneral((actionDessin)(entete.action), pointDepart, pointArrivee, zoneARafraichir, diametre, couleur);

			// Liberation de la memoire allouee
			delete[] tableauId;
		}
	}
	
	/********************************************************************/
	/* Reception d'un message de categorie Plan                         */
	/********************************************************************/
	void Liaison::receptionMessagePlan()
	{
		qDebug("Reception d'un message de categorie Plan");
		int p = 0;

		// L'hote demande a l'ordinateur local de creer un nouveau plan vide
		if (entete.action == nouveauPlanVide)
		{
			// Si l'ordinateur local est le serveur, on renvoie le messages aux clients
			faireSuivreMessage(false);

			// On recupere le titre
			quint16 tailleTitre;
			memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauTitre = new QChar[tailleTitre];
			memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
			p+=tailleTitre*sizeof(QChar);
			QString titre(tableauTitre, tailleTitre);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPlan(tableauId, tailleId);
			// On recupere la couleur
			QRgb rgb;
			memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
			p+=sizeof(QRgb);
			QColor couleur(rgb);
			// On recupere les dimensions du plan
			quint16 largeur, hauteur;
			memcpy(&largeur, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			memcpy(&hauteur, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			// On recupere la taille des PJ
			quint8 taillePj;
			memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);

			// On cree la carte
			G_mainWindow->creerNouveauPlanVide(titre, idPlan, couleur, largeur, hauteur, taillePj);
			
			// Message sur le log utilisateur
			ecrireLogUtilisateur(tr("Nouveau plan \"") + titre + "\"");

			// Liberation de la memoire allouee
			delete[] tableauTitre;
			delete[] tableauId;
		}

		// L'hote demande a l'ordinateur local de creer un plan a partir de l'image passee dans le message
		else if (entete.action == chargerPlan)
		{
			// Si l'ordinateur local est le serveur, on renvoie le messages aux clients
			faireSuivreMessage(false);

			// On recupere le titre
			quint16 tailleTitre;
			memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauTitre = new QChar[tailleTitre];
			memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
			p+=tailleTitre*sizeof(QChar);
			QString titre(tableauTitre, tailleTitre);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPlan(tableauId, tailleId);
			// On recupere la taille des PJ
			quint8 taillePj;
			memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// On recupere l'information "plan masque a l'ouverture?"
			quint8 masquerPlan;
			memcpy(&masquerPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// On recupere l'image
			quint32 tailleImage;
			memcpy(&tailleImage, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			QByteArray byteArray(tailleImage, 0);
			memcpy(byteArray.data(), &(tampon[p]), tailleImage);
			p+=tailleImage;

			// Creation de l'image
			QImage image;
			bool ok = image.loadFromData(byteArray, "jpeg");
			if (!ok)
				qWarning("Probleme de decompression de l'image (receptionMessagePlan - Liaison.cpp)");

			// Creation de la carte
		    Carte *carte = new Carte(idPlan, &image, taillePj, masquerPlan);
			// Creation de la CarteFenetre
			CarteFenetre *carteFenetre = new CarteFenetre(carte);
			// Ajout de la carte au workspace
			G_mainWindow->ajouterCarte(carteFenetre, titre);

			// Message sur le log utilisateur
			ecrireLogUtilisateur(tr("Réception du plan \"") + titre + "\"");
			
			// Liberation de la memoire allouee
			delete[] tableauTitre;
			delete[] tableauId;
		}

		// Reception d'un plan complet
		else if (entete.action == importerPlanComplet)
		{
			// Si l'ordinateur local est le serveur, on renvoie le messages aux clients
			faireSuivreMessage(false);

			// On recupere le titre
			quint16 tailleTitre;
			memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauTitre = new QChar[tailleTitre];
			memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
			p+=tailleTitre*sizeof(QChar);
			QString titre(tableauTitre, tailleTitre);
			// On recupere l'identifiant
			quint8 tailleId;
			memcpy(&tailleId, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauId = new QChar[tailleId];
			memcpy(tableauId, &(tampon[p]), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			QString idPlan(tableauId, tailleId);
			// On recupere la taille des PJ
			quint8 taillePj;
			memcpy(&taillePj, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			// On recupere l'intensite de la couche alpha
			quint8 intensiteAlpha;
			memcpy(&intensiteAlpha, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);			
			// On recupere le fond original
			quint32 tailleFondOriginal;
			memcpy(&tailleFondOriginal, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			QByteArray baFondOriginal(tailleFondOriginal, 0);
			memcpy(baFondOriginal.data(), &(tampon[p]), tailleFondOriginal);
			p+=tailleFondOriginal;
			// On recupere le fond
			quint32 tailleFond;
			memcpy(&tailleFond, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			QByteArray baFond(tailleFond, 0);
			memcpy(baFond.data(), &(tampon[p]), tailleFond);
			p+=tailleFond;
			// On recupere le fond original
			quint32 tailleAlpha;
			memcpy(&tailleAlpha, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			QByteArray baAlpha(tailleAlpha, 0);
			memcpy(baAlpha.data(), &(tampon[p]), tailleAlpha);
			p+=tailleAlpha;

			bool ok;
			// Creation de l'image de fond original
			QImage fondOriginal;
			ok = fondOriginal.loadFromData(baFondOriginal, "jpeg");
			if (!ok)
				qWarning("Probleme de decompression du fond original (receptionMessagePlan - Liaison.cpp)");
			// Creation de l'image de fond
			QImage fond;
			ok = fond.loadFromData(baFond, "jpeg");
			if (!ok)
				qWarning("Probleme de decompression du fond (receptionMessagePlan - Liaison.cpp)");
			// Creation de la couche alpha
			QImage alpha;
			ok = alpha.loadFromData(baAlpha, "jpeg");
			if (!ok)
				qWarning("Probleme de decompression de la couche alpha (receptionMessagePlan - Liaison.cpp)");

			// Creation de la carte
		    Carte *carte = new Carte(idPlan, &fondOriginal, &fond, &alpha, taillePj);
			// On adapte la couche alpha a la nature de l'utilisateur local (MJ ou joueur)
			carte->adapterCoucheAlpha(intensiteAlpha);
			// Creation de la CarteFenetre
			CarteFenetre *carteFenetre = new CarteFenetre(carte);
			// Ajout de la carte au workspace
			G_mainWindow->ajouterCarte(carteFenetre, titre);

			// Message sur le log utilisateur
			ecrireLogUtilisateur(tr("Réception du plan \"") + titre + "\"");

			// Liberation de la memoire allouee
			delete[] tableauTitre;
			delete[] tableauId;
		}

		// Demande de fermeture d'un plan
		else if (entete.action == fermerPlan)
		{
			// Si l'ordinateur local est le serveur, on fait suivre la fermeture de plan aux autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdPlan;
			memcpy(&tailleIdPlan, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdPlan = new QChar[tailleIdPlan];
			memcpy(tableauIdPlan, &(tampon[p]), tailleIdPlan*sizeof(QChar));
			p+=tailleIdPlan*sizeof(QChar);
			QString idPlan(tableauIdPlan, tailleIdPlan);

			// On recherche la CarteFenetre concernee
			CarteFenetre *carteFenetre = G_mainWindow->trouverCarteFenetre(idPlan);
			// Si la CarteFenetre est introuvable on affiche un message d'erreur
			if (!carteFenetre)
				qWarning("CarteFenetre introuvable a la reception d'une demande de fermture d'un plan (receptionMessagePlan - Liaison.cpp)");

			// Si la CarteFenetre a ete trouvee, on la supprime
			else
			{
				// Message sur le log utilisateur
				ecrireLogUtilisateur(tr("Le plan \"") + carteFenetre->windowTitle() + tr("\" vient d'Ãªtre fermÃ© par le MJ"));
				// Suppression du plan
				carteFenetre->~CarteFenetre();
			}

			// Liberation de la memoire allouee
			delete[] tableauIdPlan;
		}

		else
		{
			qWarning("Action plan inconnue (receptionMessagePlan - Liaison.cpp)");
			return;
		}
	}

	/********************************************************************/
	/* Reception d'un message de categorie Image                        */
	/********************************************************************/
	void Liaison::receptionMessageImage()
	{
		qDebug("Réception d'un message de categorie Image");
		int p = 0;

		// L'hote demande a l'ordinateur local de creer l'image passee dans le message
		if (entete.action == chargerImage)
		{
			// Si l'ordinateur local est le serveur, on renvoie le messages aux clients
			faireSuivreMessage(false);

			// On recupere le titre
			quint16 tailleTitre;
			memcpy(&tailleTitre, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauTitre = new QChar[tailleTitre];
			memcpy(tableauTitre, &(tampon[p]), tailleTitre*sizeof(QChar));
			p+=tailleTitre*sizeof(QChar);
			QString titre(tableauTitre, tailleTitre);
			// On recupere l'identifiant de l'image
			quint8 tailleIdImage;
			memcpy(&tailleIdImage, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdImage = new QChar[tailleIdImage];
			memcpy(tableauIdImage, &(tampon[p]), tailleIdImage*sizeof(QChar));
			p+=tailleIdImage*sizeof(QChar);
			QString idImage(tableauIdImage, tailleIdImage);
			// On recupere l'identifiant du joueur
			quint8 tailleIdJoueur;
			memcpy(&tailleIdJoueur, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdJoueur = new QChar[tailleIdJoueur];
			memcpy(tableauIdJoueur, &(tampon[p]), tailleIdJoueur*sizeof(QChar));
			p+=tailleIdJoueur*sizeof(QChar);
			QString idJoueur(tableauIdJoueur, tailleIdJoueur);
			// On recupere l'image
			quint32 tailleImage;
			memcpy(&tailleImage, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			QByteArray byteArray(tailleImage, 0);
			memcpy(byteArray.data(), &(tampon[p]), tailleImage);
			p+=tailleImage;

			// Creation de l'image
			QImage *img = new QImage;
			bool ok = img->loadFromData(byteArray, "jpeg");
			if (!ok)
				qWarning("Problème de decompression de l'image (réceptionMessageImage - Liaison.cpp)");

			// Creation de l'Image
		    Image *imageFenetre = new Image(idImage, idJoueur, img);
			// Ajout de la carte au workspace
			G_mainWindow->ajouterImage(imageFenetre, titre);

			// Message sur le log utilisateur
			ecrireLogUtilisateur(tr("Réception de l'image \"") + titre.left(titre.size() - QString(tr(" (Image)")).size() ) + "\"");
			
			// Liberation de la memoire allouee
			delete[] tableauTitre;
			delete[] tableauIdImage;
			delete[] tableauIdJoueur;
		}

		// Demande de fermeture d'une image
		else if (entete.action == fermerImage)
		{
			// Si l'ordinateur local est le serveur, on fait suivre la fermeture de l'image aux autres clients
			faireSuivreMessage(false);

			// On recupere l'identifiant de la carte
			quint8 tailleIdImage;
			memcpy(&tailleIdImage, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdImage = new QChar[tailleIdImage];
			memcpy(tableauIdImage, &(tampon[p]), tailleIdImage*sizeof(QChar));
			p+=tailleIdImage*sizeof(QChar);
			QString idImage(tableauIdImage, tailleIdImage);

			// On recherche l'image concernee
			Image *imageFenetre = G_mainWindow->trouverImage(idImage);
			// Si l'image est introuvable on affiche un message d'erreur
			if (!imageFenetre)
				qWarning("Image introuvable a la reception d'une demande de fermeture d'une image (receptionMessageImage - Liaison.cpp)");

			// Si l'Image a ete trouvee, on la supprime
			else
			{
				// Message sur le log utilisateur
				QString titre = imageFenetre->windowTitle();
				ecrireLogUtilisateur(tr("L'image \"") + titre.left(titre.size() - QString(tr(" (Image)")).size() ) + tr("\" vient d'être fermée"));
				// Suppression de l'image
				imageFenetre->~Image();
			}

			// Liberation de la memoire allouee
			delete[] tableauIdImage;
		}

		else
		{
			qWarning("Action image inconnue (réceptionMessageImage - Liaison.cpp)");
			return;
		}
	}
	
	/********************************************************************/
	/* Reception d'un message de categorie Discussion                   */
	/********************************************************************/
	void Liaison::receptionMessageDiscussion()
	{
		qDebug("Reception d'un message de categorie Discussion");
		int p = 0;

		// Reception d'un message a afficher, et eventuellement a retransmettre
        //TCHAT_MESSAGE, DICE_MESSAGE, EMOTE_MESSAGE
        if (entete.action == TCHAT_MESSAGE || entete.action == DICE_MESSAGE || entete.action == EMOTE_MESSAGE)
		{
			// On recupere l'identifiant du joueur emetteur
			quint8 tailleIdJoueurEmetteur;
			memcpy(&tailleIdJoueurEmetteur, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdJoueurEmetteur = new QChar[tailleIdJoueurEmetteur];
			memcpy(tableauIdJoueurEmetteur, &(tampon[p]), tailleIdJoueurEmetteur*sizeof(QChar));
			p+=tailleIdJoueurEmetteur*sizeof(QChar);
			QString idJoueurEmetteur(tableauIdJoueurEmetteur, tailleIdJoueurEmetteur);
			// On recupere l'identifiant du joueur recepteur
			quint8 tailleIdJoueurRecepteur;
			memcpy(&tailleIdJoueurRecepteur, &(tampon[p]), sizeof(quint8));
			p+=sizeof(quint8);
			QChar *tableauIdJoueurRecepteur = new QChar[tailleIdJoueurRecepteur];
			memcpy(tableauIdJoueurRecepteur, &(tampon[p]), tailleIdJoueurRecepteur*sizeof(QChar));
			p+=tailleIdJoueurRecepteur*sizeof(QChar);
			QString idJoueurRecepteur(tableauIdJoueurRecepteur, tailleIdJoueurRecepteur);
			// On recupere le message
			quint32 tailleMessage;
			memcpy(&tailleMessage, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			QChar *tableauMessage = new QChar[tailleMessage];
			memcpy(tableauMessage, &(tampon[p]), tailleMessage*sizeof(QChar));
			p+=tailleMessage*sizeof(QChar);
			QString message(tableauMessage, tailleMessage);
			
			// Si l'ID du joueur de destination est vide ou est egale a celui du joueur local
			// alors on affiche le message dans le tchat du joueur emetteur
			if (idJoueurRecepteur == G_idJoueurLocal || idJoueurRecepteur.isEmpty())
			{
				Tchat *tchat;

				// Si le destinataire est vide, il s'agit du tchat commun
				if (idJoueurRecepteur.isEmpty())
					tchat = G_mainWindow->trouverTchat(idJoueurRecepteur);
				// Sinon on recupere le tchat prive concerne
				else
					tchat = G_mainWindow->trouverTchat(idJoueurEmetteur);
					
				// Recherche du nom du joueur emetteur
				QString emetteur = G_listeUtilisateurs->nomUtilisateur(idJoueurEmetteur);
				// Recherche de la couleur du joueur emetteur
				QColor couleur = G_listeUtilisateurs->couleurUtilisateur(idJoueurEmetteur);
				// Affichage du message dans le tchat, au format message ou tirage
/*                if (entete.action == TCHAT_MESSAGE)
                    tchat->afficherMessage(emetteur, couleur, message);*/

                 tchat->afficherMessage(emetteur, couleur, message,(actionDiscussion)entete.action);
			}
			
			// Si l'ordinateur local est le serveur il doit faire suivre le message
			if (!G_client)
			{
				// Le message est destine a tt le monde : on retransmet vers l'ensemble des utilisateur sauf l'emetteur
				if (idJoueurRecepteur.isEmpty())
					faireSuivreMessage(false);
				
				// Le message a un destinataire precis : on retransmet vers cet utilisateur
				else if (idJoueurRecepteur != G_idJoueurLocal)
				{
					char *donnees = new char[entete.tailleDonnees + sizeof(enteteMessage)];
					// Recopie de l'entete
					memcpy(donnees, &entete, sizeof(enteteMessage));
					// Recopie du corps du message
					memcpy(&(donnees[sizeof(enteteMessage)]), tampon, entete.tailleDonnees);
					// On recupere le numero de liaison correspondant a l'identifiant du joueur
					// (on soustrait 1 car le 1er utilisateur est toujours le serveur et qu'il
					// n'a pas de liaison associee)
					int numeroLiaison = G_listeUtilisateurs->numeroUtilisateur(idJoueurRecepteur) - 1;
					// Emission du message
					emettre(donnees, sizeof(enteteMessage) + entete.tailleDonnees, numeroLiaison);
					
					// Liberation de la memoire allouee
					delete[] donnees;
				}				
			}
		
			// Liberation de la memoire allouee
			delete[] tableauIdJoueurEmetteur;
			delete[] tableauIdJoueurRecepteur;
			delete[] tableauMessage;
		}
	}

	/********************************************************************/
	/* Reception d'un message de categorie Musique                      */
	/********************************************************************/
	void Liaison::receptionMessageMusique()
	{
		qDebug("Reception d'un message de categorie Musique");
		int p = 0;

		// Si l'ordinateur local est le serveur, on fait suivre la commande du lecteur audio aux autres clients
		faireSuivreMessage(false);

		// L'hote demande a l'ordinateur local de charger la musique dont le nom est passe en parametre
		if (entete.action == nouveauMorceau)
		{
			// On recupere le nom du fichier
			quint16 tailleNomFichier;
			memcpy(&tailleNomFichier, &(tampon[p]), sizeof(quint16));
			p+=sizeof(quint16);
			QChar *tableauNomFichier = new QChar[tailleNomFichier];
			memcpy(tableauNomFichier, &(tampon[p]), tailleNomFichier*sizeof(QChar));
			p+=tailleNomFichier*sizeof(QChar);
			QString nomFichier(tableauNomFichier, tailleNomFichier);
			
			// On charge le nouveau fichier dans le lecteur
                        qDebug() << nomFichier <<endl;
			G_lecteurAudio->joueurNouveauFichier(nomFichier);
			
			// Liberation de la memoire allouee
			delete[] tableauNomFichier;
		}

		// Demande de lecture de la musique
		else if (entete.action == lectureMorceau)
		{
			// On demande au lecteur audio de mettre la lecture en pause
			G_lecteurAudio->joueurLectureMorceau();
		}

		// Demande de mise en pause de la lecture
		else if (entete.action == pauseMorceau)
		{
			// On demande au lecteur audio de mettre la lecture en pause
			G_lecteurAudio->joueurPauseMorceau();
		}

		// Demande d'arret de la lecture
		else if (entete.action == arretMorceau)
		{
			// On demande au lecteur audio de mettre la lecture en pause
			G_lecteurAudio->joueurArretMorceau();
		}

		// Demande de changement de position de la lecture
		else if (entete.action == nouvellePositionMorceau)
		{
			// On recupere la nouvelle position
			quint32 nouvellePosition;
			memcpy(&nouvellePosition, &(tampon[p]), sizeof(quint32));
			p+=sizeof(quint32);
			
			// On demande au lecteur audio de changer la position de la lecture
			G_lecteurAudio->joueurChangerPosition(nouvellePosition);
		}

		else
		{
			qWarning("Action musique inconnue (receptionMessageMusique - Liaison.cpp)");
			return;
		}
	}
	
	/********************************************************************/
	/* Reception d'un message de categorie Parametres                   */
	/********************************************************************/
	void Liaison::receptionMessageParametres()
	{
		qDebug("Reception d'un message de categorie Paramètres");
	}

	/********************************************************************/
	/* Fait suivre le message recu a l'ensemble des clients si tous =   */
	/* true, ou bien a tous a l'exception de l'emetteur initial si tous */
	/* = false (serveur uniquement)                                     */
	/********************************************************************/
	void Liaison::faireSuivreMessage(bool tous)
	{
		// Uniquement si l'ordinateur local est le serveur
		if (!G_client)
		{
			char *donnees = new char[entete.tailleDonnees + sizeof(enteteMessage)];
			// Recopie de l'entete
			memcpy(donnees, &entete, sizeof(enteteMessage));
			// Recopie du corps du message
			memcpy(&(donnees[sizeof(enteteMessage)]), tampon, entete.tailleDonnees);
			if (tous)
				// On envoie le message a l'ensemble des clients
				emettre(donnees, entete.tailleDonnees + sizeof(enteteMessage));
			else
				// On envoie le message a l'ensemble des clients sauf a l'emetteur initial
				emettre(donnees, entete.tailleDonnees + sizeof(enteteMessage), this);		
			delete[] donnees;
		}
	}

	/********************************************************************/
	/* Emission de l'identite d'un utilisateur vers le socket gere par  */
	/* le thread si multi = false, ou vers l'ensemble des sockets si    */
	/* multi = true                                                     */
	/********************************************************************/
	void Liaison::emettreUtilisateur(utilisateur *util, bool multi)
	{
		// Taille des donnees
		quint32 tailleCorps =
			// Taille du nom
			sizeof(quint16) + util->nomJoueur.size()*sizeof(QChar) +
			// Taille de l'identifiant
			sizeof(quint8) + util->idJoueur.size()*sizeof(QChar) +
			// Taille de la couleur
			sizeof(QRgb) +
			// Taille de la nature de l'utilisateur (MJ ou joueur)
			sizeof(quint8) +
			// Taille de l'info d'affichage ou pas dans le log utilisateur de l'ajout d'un joueur
			sizeof(quint8);

		// Buffer d'emission
		char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

		// Creation de l'entete du message
		enteteMessage *uneEntete;
		uneEntete = (enteteMessage *) donnees;
		uneEntete->categorie = joueur;
		uneEntete->action = ajouterJoueur;
		uneEntete->tailleDonnees = tailleCorps;
		
		// Creation du corps du message
		int p = sizeof(enteteMessage);
		// Ajout du nom
		quint16 tailleNom = util->nomJoueur.size();
		memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
		p+=sizeof(quint16);
		memcpy(&(donnees[p]), util->nomJoueur.data(), tailleNom*sizeof(QChar));
		p+=tailleNom*sizeof(QChar);
		// Ajout de l'identifiant
		quint8 tailleId = util->idJoueur.size();
		memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
		p+=sizeof(quint8);
		memcpy(&(donnees[p]), util->idJoueur.data(), tailleId*sizeof(QChar));
		p+=tailleId*sizeof(QChar);
		// Ajout de la couleur
		QRgb rgb = util->couleurJoueur.rgb();
		memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
		p+=sizeof(QRgb);
		// Ajout de la nature de l'utilisateur (MJ ou joueur)
		quint8 mj = util->mj;
		memcpy(&(donnees[p]), &mj, sizeof(quint8));
		p+=sizeof(quint8);
		// Ajout d'une information indiquant au recepteur s'il doit afficher l'ajout de l'utilisateur (1) ou pas (0)
		quint8 affiche = multi;
		memcpy(&(donnees[p]), &affiche, sizeof(quint8));
		p+=sizeof(quint8);
		
		// Emission des donnees vers l'ensemble des liaisons (utilise par le serveur)
		if (multi)
			emettre(donnees, tailleCorps + sizeof(enteteMessage));

		// Emission des donnees vers le socket gere par la liaison
		else
			emissionDonnees(donnees, tailleCorps + sizeof(enteteMessage));

		// Liberation du buffer d'emission
		delete[] donnees;
	}

	/********************************************************************/
	/* Extrait un personnage d'un message et l'ajoute a la carte passee */
	/* en parametre                                                     */
	/********************************************************************/
	int Liaison::extrairePersonnage(Carte *carte, char *tampon)
	{
		int p=0;
		
		// On recupere le nom
		quint16 tailleNom;
		memcpy(&tailleNom, &(tampon[p]), sizeof(quint16));
		p+=sizeof(quint16);
		QChar *tableauNom = new QChar[tailleNom];
		memcpy(tableauNom, &(tampon[p]), tailleNom*sizeof(QChar));
		p+=tailleNom*sizeof(QChar);
		QString nomPerso(tableauNom, tailleNom);
		// On recupere l'identifiant du perso
		quint8 tailleIdPerso;
		memcpy(&tailleIdPerso, &(tampon[p]), sizeof(quint8));
		p+=sizeof(quint8);
		QChar *tableauIdPerso = new QChar[tailleIdPerso];
		memcpy(tableauIdPerso, &(tampon[p]), tailleIdPerso*sizeof(QChar));
		p+=tailleIdPerso*sizeof(QChar);
		QString idPerso(tableauIdPerso, tailleIdPerso);
		// On recupere le type du personnage
		quint8 type;
		memcpy(&type, &(tampon[p]), sizeof(quint8));
		p+=sizeof(quint8);
		DessinPerso::typePersonnage typePerso = (DessinPerso::typePersonnage)type;
		// On recupere le numero de PNJ
		quint8 numeroPnj;
		memcpy(&numeroPnj, &(tampon[p]), sizeof(quint8));
		p+=sizeof(quint8);
		// On recupere le diametre
		quint8 diametre;
		memcpy(&diametre, &(tampon[p]), sizeof(quint8));
		p+=sizeof(quint8);
		// On recupere la couleur
		QRgb rgb;
		memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
		p+=sizeof(QRgb);
		QColor couleurPerso(rgb);
		// On recupere le point central du perso
		qint16 centreX, centreY;
		memcpy(&centreX, &(tampon[p]), sizeof(qint16));
		p+=sizeof(qint16);
		memcpy(&centreY, &(tampon[p]), sizeof(qint16));
		p+=sizeof(qint16);
		QPoint centre(centreX, centreY);
		// On recupere l'orientation du perso
		qint16 orientationX, orientationY;
		memcpy(&orientationX, &(tampon[p]), sizeof(qint16));
		p+=sizeof(qint16);
		memcpy(&orientationY, &(tampon[p]), sizeof(qint16));
		p+=sizeof(qint16);
		QPoint orientation(orientationX, orientationY);
		// On recupere la couleur de l'etat de sante
		memcpy(&rgb, &(tampon[p]), sizeof(QRgb));
		p+=sizeof(QRgb);
		QColor couleurEtat(rgb);
		// On recupere le nom de l'etat
		quint16 tailleEtat;
		memcpy(&tailleEtat, &(tampon[p]), sizeof(quint16));
		p+=sizeof(quint16);
		QChar *tableauEtat = new QChar[tailleEtat];
		memcpy(tableauEtat, &(tampon[p]), tailleEtat*sizeof(QChar));
		p+=tailleEtat*sizeof(QChar);
		QString nomEtat(tableauEtat, tailleEtat);
		// On recupere le numero de l'etat de sante
		quint16 numEtat;
		memcpy(&numEtat, &(tampon[p]), sizeof(quint16));
		p+=sizeof(quint16);
		// On recupere l'information visible/cache
		quint8 visible;
		memcpy(&visible, &(tampon[p]), sizeof(quint8));
		p+=sizeof(quint8);
		// On recupere l'information orientation affichee/masquee
		quint8 orientationAffichee;
		memcpy(&orientationAffichee, &(tampon[p]), sizeof(quint8));
		p+=sizeof(quint8);

		// Creation du PNJ dans la carte
		DessinPerso *pnj = new DessinPerso(carte, idPerso, nomPerso, couleurPerso, diametre, centre, typePerso, numeroPnj);
		// S'il doit etre visible, on l'affiche (s'il s'agit d'un PNJ et que l'utilisateur est le MJ, alors on affiche automatiquement le perso)
		if (visible || (typePerso == DessinPerso::pnj && !G_joueur))
			pnj->afficherPerso();
		// On m.a.j l'orientation
		pnj->nouvelleOrientation(orientation);
		// Affichage de l'orientation si besoin
		if (orientationAffichee)
			pnj->afficheOuMasqueOrientation();
		// M.a.j de l'etat de sante du personnage
		DessinPerso::etatDeSante sante;
		sante.couleurEtat = couleurEtat;
		sante.nomEtat = nomEtat;
		pnj->nouvelEtatDeSante(sante, numEtat);
		// Affiche ou masque le PNJ selon qu'il se trouve sur une zone masquee ou pas
		carte->afficheOuMasquePnj(pnj);

		// Liberation de la memoire allouee
		delete[] tableauNom;
		delete[] tableauIdPerso;
		delete[] tableauEtat;
		
		// On renvoie le nbr d'octets lus
		return p;
	}
