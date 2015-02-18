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


	#include <QtGui>
	#include <QtNetwork> 

	#include "ClientServeur.h"
	#include "MainWindow.h"
	#include "constantesGlobales.h"
	#include "variablesGlobales.h"
	#include "types.h"


	/********************************************************************/
	/* Variables globales utilisees par tous les elements de            */
	/* l'application                                                    */
	/********************************************************************/	
	// Mutex protegeant le code de connexion des joueurs
	QMutex G_mutexConnexion;
	// Pointeur vers l'unique instance du ClientServeur
	ClientServeur *G_clientServeur;
	// True si l'utilisateur est un joueur, false s'il est MJ
	bool G_joueur;
	// True si l'ordinateur local est client, false s'il est serveur
	bool G_client;
	// Identifiant du joueur en local sur la machine
	QString G_idJoueurLocal;
	// Couleur du joueur local
	QColor G_couleurJoueurLocal;
	

	/********************************************************************/
	/* Emet des donnees vers les clients ou le serveur                  */
	/********************************************************************/
	void emettre(char *donnees, quint32 taille, Liaison *sauf)
	{
		G_clientServeur->emettreDonnees(donnees, taille, sauf);
	}

	/********************************************************************/
	/* Emet des donnees vers une liaison                                */
	/********************************************************************/
	void emettre(char *donnees, quint32 taille, int numeroLiaison)
	{
		G_clientServeur->emettreDonnees(donnees, taille, numeroLiaison);
	}
	
	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    ClientServeur::ClientServeur()
		: QObject()
    {
                G_lecteurAudio=NULL;
		// Initialisation des variables
		liaisons.clear();
		G_clientServeur = this;
		
		// Connexion de l'etablissement de connexion avec le lancement du programme principal
		QObject::connect(this, SIGNAL(connexionEtablie()), this, SLOT(programmePrincipal()));

		// Creation de la fenetre de connexion
		creerFenetreConnexion();
	}

	/********************************************************************/
	/* Emet des donnees vers toutes les liaisons                        */
	/********************************************************************/
	void ClientServeur::emettreDonnees(char *donnees, quint32 taille, Liaison *sauf)
	{
		// Demande d'emission vers toutes les liaisons
		emit emissionDonnees(donnees, taille, sauf);
	}

	/********************************************************************/
	/* Emet des donnees vers une liaison en particulier                 */
	/********************************************************************/
	void ClientServeur::emettreDonnees(char *donnees, quint32 taille, int numeroLiaison)
	{
		// Emission des donnees vers la liaison selectionnee
		liaisons[numeroLiaison]->emissionDonnees(donnees, taille);
	}

	/********************************************************************/	
	/* Lance la procedure de connexion                                  */
	/********************************************************************/	
	void ClientServeur::demandeConnexion()
	{
		// Si les parametres ne sont pas corrects on retourne a la fenetre de connexion
                if (!verifierParametres())
                        return;

		// On peut lancer la procedure de connexion

		// Creation de la boite d'alerte
		QMessageBox msgBox;
		msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(tr("Erreur de connexion"));
		msgBox.move(fenetreConnexion->pos() + QPoint(50,100));
		// On supprime l'icone de la barre de titre
		Qt::WindowFlags flags = msgBox.windowFlags();
		msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);

		// Initialisation de la variable indiquant si l'utilisateur est joueur ou MJ
		G_joueur = boutonJoueur->isChecked();
		// Initialisation de la variable indiquant si l'ordinateur local est client ou serveur
		G_client = boutonClient->isChecked();
		// Initialisation de l'identifiant du joueur local
		G_idJoueurLocal = QUuid::createUuid().toString();
		// Initialisation des variables temporaires sauvegardant le nom et la couleur du joueur
		tempNomJoueur = nomJoueur->text();
		G_couleurJoueurLocal = (couleurJoueur->palette()).color(QPalette::Window);

		// Si l'ordinateur local est un serveur, on le met en place avant de lancer l'application principale
		if (!G_client)
		{
			// Creation du serveur TCP
			serveurTcp = new QTcpServer();
			// Connexion du signal de nouvelle connexion avec l'ajout d'un nouveau client
			QObject::connect(serveurTcp, SIGNAL(newConnection()), this, SLOT(nouveauClientConnecte()));
			// Lancement du serveur
			bool ok, serveurOk;
			serveurOk = serveurTcp->listen(QHostAddress::Any, portClient->text().toInt(&ok));
			// En cas de pb au lancement du serveur
			if (!serveurOk)
			{
				msgBox.setText(tr("Impossible de mettre en place le serveur TCP"));
				msgBox.exec();
				return;
			}

			// Une fois la connexion etablie on peut lancer l'application principale...
			emit connexionEtablie();
			// On met a jour la variable d'initialisation en vue de la future sauvegarde du fichier d'initialisation
			majParametresInitialisation();
			// ...et detruire la fenetre de connexion
			detruireFenetreConnexion();
			// On rajoute l'utilisateur local dans la liste des utilisateurs
			G_listeUtilisateurs->ajouterJoueur(G_idJoueurLocal, tempNomJoueur, G_couleurJoueurLocal, true, !G_joueur);
			// On affiche un message dans la fenetre de log utilisateur
			ecrireLogUtilisateur(tr("Serveur en place sur le port ") + QString::number(serveurTcp->serverPort()));
		}

		// Si l'ordinateur local est un client, on essaie de contacter le serveur
		else
		{
			// Creation du socket
			socketTcp = new QTcpSocket();
			// Connexion des signaux du socket aux actions appropriees
			QObject::connect(socketTcp, SIGNAL(connected()), this, SLOT(connexionAuServeurEtablie()));
			QObject::connect(socketTcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurDeConnexion(QAbstractSocket::SocketError)));
			// Affichage de la fenetre d'attente de connexion
			creerFenetreAttenteConnexion();
			// Connexion au serveur
			bool ok;
                        socketTcp->connectToHost(adresseServeur->text()/*QHostAddress(adresseServeur->text())*/, portServeur->text().toInt(&ok));
		}
	}

	/********************************************************************/
	/* Le client vient de se connecter au serveur (ne peut etre appele  */
	/* que par un client) : il cree une liaison qui est rajoutee a la   */
	/* liste                                                            */
	/********************************************************************/
	void ClientServeur::connexionAuServeurEtablie()
	{
		// Deconnexion des signaux d'erreur et de connexion etablie du socket
		QObject::disconnect(socketTcp, SIGNAL(connected()), this, SLOT(connexionAuServeurEtablie()));
		QObject::disconnect(socketTcp, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurDeConnexion(QAbstractSocket::SocketError)));
		// Creation de la liaison qui va gerer le socket
		Liaison *liaison = new Liaison(socketTcp, this);
		// Fermeture de la fenetre d'attente
		detruireFenetreAttenteConnexion();
		// Une fois la connexion etablie on peut lancer l'application principale...
		emit connexionEtablie();
		// On met a jour la variable d'initialisation en vue de la future sauvegarde du fichier d'initialisation
		majParametresInitialisation();
		// On affiche un message dans la fenetre de log utilisateur
		ecrireLogUtilisateur(tr("Vous êtes connecté au serveur (Adresse IP : ") + adresseServeur->text() + ", Port : " + portClient->text() + ")");
		// ...et on detruit la fenetre de connexion
		detruireFenetreConnexion();
		// Mise en route de la liaison
		liaison->start();
		// Envoie de l'identite du joueur au serveur
		emettreIdentite();
	}

	/********************************************************************/
	/* Un utilisateur vient de se connecter au serveur (ne peut etre    */
	/* appele que par le serveur) : on cree une liaison que l'on ajoute */
	/* a la liste)                                                      */
	/********************************************************************/
	void ClientServeur::nouveauClientConnecte()
	{
		// Recuperation du socket lie a la demande de connexion
		QTcpSocket *socketTcp = serveurTcp->nextPendingConnection();

		// Creation de la liaison qui va gerer le socket
		Liaison *liaison = new Liaison(socketTcp, this);
		// Mise en route de la liaison
		liaison->start();
	}

	/********************************************************************/
	/* Erreur de connexion : la connexion au serveur n'a pu avoir lieu  */
	/* (ne peut etre appele que par un client)                          */
	/********************************************************************/
	void ClientServeur::erreurDeConnexion(QAbstractSocket::SocketError srror)
	{
		// Message d'erreur
		qDebug("Une erreur réseau est survenue");

                 QMessageBox::warning(NULL, tr("Rolisteam"),
                                tr("Une erreur réseau est survenue.\n"
                                   "Voici le message d'erreur: %1").arg(socketTcp->errorString()),
                                QMessageBox::Ok,QMessageBox::Ok
                                );


                QTextStream cout(stderr,QIODevice::WriteOnly);

		// Fermeture de la connexion et du socket
                cout << "not good " << endl;
		connexionAnnulee();
	}

	/********************************************************************/
	/* La demande de connexion vient d'etre annulee par l'utilisateur   */
	/* (ne peut etre appele que par un client)                          */
	/********************************************************************/
	void ClientServeur::connexionAnnulee()
	{
		// Fermeture de la fenetre d'attente
		detruireFenetreAttenteConnexion();
		// Annulation de la connexion en cours
		socketTcp->abort();
		// Destruction du socket
		socketTcp->~QTcpSocket();
	}

	/********************************************************************/
	/* Declaration du joueur aupres du serveur (ne peut etre appele que */
	/* par le client)                                                   */
	/********************************************************************/
	void ClientServeur::emettreIdentite()
	{
		// Taille des donnees
		quint32 tailleCorps =
			// Taille du nom
			sizeof(quint16) + tempNomJoueur.size()*sizeof(QChar) +
			// Taille de l'identifiant
			sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
			// Taille de la couleur
			sizeof(QRgb) +
			// Taille de la nature de l'utilisateur (MJ ou joueur)
			sizeof(quint8);

		// Buffer d'emission
		char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

		// Creation de l'entete du message
		enteteMessage *entete;
		entete = (enteteMessage *) donnees;
		entete->categorie = joueur;
		entete->action = connexionJoueur;
		entete->tailleDonnees = tailleCorps;
		
		// Creation du corps du message
		int p = sizeof(enteteMessage);
		// Ajout du nom
		quint16 tailleNom = tempNomJoueur.size();
		memcpy(&(donnees[p]), &tailleNom, sizeof(quint16));
		p+=sizeof(quint16);
		memcpy(&(donnees[p]), tempNomJoueur.data(), tailleNom*sizeof(QChar));
		p+=tailleNom*sizeof(QChar);
		// Ajout de l'identifiant
		quint8 tailleId = G_idJoueurLocal.size();
		memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
		p+=sizeof(quint8);
		memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleId*sizeof(QChar));
		p+=tailleId*sizeof(QChar);
		// Ajout de la couleur
		QRgb rgb = G_couleurJoueurLocal.rgb();
		memcpy(&(donnees[p]), &rgb, sizeof(QRgb));
		p+=sizeof(QRgb);
		// Ajout de la nature de l'utilisateur (MJ ou joueur)
		quint8 mj = !G_joueur;
		memcpy(&(donnees[p]), &mj, sizeof(quint8));
		p+=sizeof(quint8);
		// Emission des donnees
		emettre(donnees, tailleCorps + sizeof(enteteMessage));
		// Liberation du buffer d'emission
		delete[] donnees;
	}

	/********************************************************************/
	/* Une liaison avec un client ou le serveur vient d'etre perdue     */
	/********************************************************************/
	void ClientServeur::finDeLiaison()
	{
		// Si l'ordinateur local est un client
		if (G_client)
		{
			// On quitte l'application	
			G_mainWindow->quitterApplication(true);
		}
		
		// Si l'ordinateur local est le serveur
		else
		{
			bool trouve = false;
			int i;
			int n = liaisons.size();
			// On recherche le thread qui vient de se terminer
			for (i = 0; i<n & !trouve; i++)
				if (liaisons[i]->isFinished())
					trouve = true;
			i--;
			
			if (!trouve)
			{
				qWarning("Un thread vient de se terminer, mais celui-ci est introuvable (finDeLiaison - ClientServeur.cpp)");
				return;
			}
			
			// On supprime la liaison de la liste, apres l'avoir detruite
			liaisons[i]->~Liaison();
			liaisons.removeAt(i);
			// Recuperation de l'identifiant du joueur correspondant a la liaison
			// (en tenant compte du fait que le 1er utilisateur est toujours le serveur)
			QString identifiant = G_listeUtilisateurs->indentifiantUtilisateur(i+1);
			// Si l'utilisateur etait le MJ, on reinitialise le lecteur audio
			if (G_listeUtilisateurs->estUnMj(i+1))
                        {
                            if(G_lecteurAudio==NULL)
                                 G_lecteurAudio = LecteurAudio::getInstance();
                            G_lecteurAudio->joueurNouveauFichier("");
                        }
			// Suppression de l'utilisateur dans la liste
			G_listeUtilisateurs->supprimerJoueur(identifiant);
			// On supprime le tchat associe
			G_mainWindow->supprimerTchat(identifiant);
			
			// On envoie un message a l'ensemble des clients

			// Taille des donnees
			quint32 tailleCorps =
				// Taille de l'identifiant
				sizeof(quint8) + identifiant.size()*sizeof(QChar);
	
			// Buffer d'emission
			char *donnees = new char[tailleCorps + sizeof(enteteMessage)];
			// Creation de l'entete du message
			enteteMessage *uneEntete;
			uneEntete = (enteteMessage *) donnees;
			uneEntete->tailleDonnees = tailleCorps;
			uneEntete->categorie = joueur;
			uneEntete->action = supprimerJoueur;
			// Creation du corps du message
			int p = sizeof(enteteMessage);
			// Ajout de l'identifiant
			quint8 tailleId = identifiant.size();
			memcpy(&(donnees[p]), &tailleId, sizeof(quint8));
			p+=sizeof(quint8);
			memcpy(&(donnees[p]), identifiant.data(), tailleId*sizeof(QChar));
			p+=tailleId*sizeof(QChar);
			
			// Emission de la demande de suppression du joueur a l'ensemble des clients
			emettre(donnees, tailleCorps + sizeof(enteteMessage));
			// Liberation du buffer d'emission
			delete[] donnees;
		}
	}

	/********************************************************************/
	/* Affiche une fenetre d'attente de connexion (pendant la connexion */
	/* du client)                                                       */
	/********************************************************************/
	void ClientServeur::creerFenetreAttenteConnexion()
	{
		// Creation de la fenetre
		fenetreAttente = new QDialog();
		#ifdef WIN32
			fenetreAttente->setFixedSize(260, 65);
		#elif defined (MACOS)
			fenetreAttente->setFixedSize(350, 110);
		#endif
		// On supprime de bouton d'aide contextuelle et l'icone de la barre de titre
		Qt::WindowFlags flags = fenetreAttente->windowFlags();
		fenetreAttente->setWindowFlags(flags ^ Qt::WindowContextHelpButtonHint ^ Qt::WindowSystemMenuHint);

        // M.a.j de l'icone de la fenetre
                fenetreAttente->setWindowIcon(QIcon(":/resources/icones/" + QString(NOM_APPLICATION) + ".png"));

		// Creation des layouts
		QVBoxLayout *layoutVertical = new QVBoxLayout(fenetreAttente);
		QHBoxLayout *layoutHorizontal = new QHBoxLayout();

		// Creation du label
		QLabel *label = new QLabel("Connexion en cours...");
		// Creation de la barre de progression defilante
		QProgressBar *progressBar = new QProgressBar();
		progressBar->setTextVisible(false);
		#ifdef WIN32
			progressBar->setFixedHeight(14);
		#endif
		progressBar->setRange(0,0);
		// Creation du bouton d'annulation de connexion
		QPushButton *annuler = new QPushButton("Annuler");
		QObject::connect(annuler, SIGNAL(clicked()), this, SLOT(connexionAnnulee()));
		
		// Ajout des elements aux layouts
		layoutHorizontal->addWidget(progressBar);
		layoutHorizontal->setStretchFactor(progressBar, 10);
		layoutHorizontal->addWidget(annuler);
		layoutVertical->addWidget(label);
		layoutVertical->addLayout(layoutHorizontal);

		// On bloque la souris et le clavier sur cette fenetre
		fenetreAttente->setModal(true);
		// On affiche la fenetre
		fenetreAttente->move(fenetreConnexion->pos() + QPoint(50,100));
		fenetreAttente->show();
	}

	/********************************************************************/
        /* Detruit la fenetre d'attente de connexion                        */
	/********************************************************************/
	void ClientServeur::detruireFenetreAttenteConnexion()
	{
			fenetreAttente->~QDialog();
	}

	/********************************************************************/	
	/* Renvoie true si les parametres de la fenetre de connexion sont   */
	/* valides                                                          */
	/********************************************************************/	
	bool ClientServeur::verifierParametres()
	{
		// Creation de la boite d'alerte
		QMessageBox msgBox(fenetreConnexion);
		msgBox.addButton(tr("Annuler"), QMessageBox::RejectRole);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(tr("Paramètre invalide"));
		msgBox.move(fenetreConnexion->pos() + QPoint(50,100));
		// On supprime l'icone de la barre de titre
		Qt::WindowFlags flags = msgBox.windowFlags();
		msgBox.setWindowFlags(flags ^ Qt::WindowSystemMenuHint);
                int iporhostname= 0;// 0 = error, 1 = Ip and -1 = hostname
		// Si l'ordinateur local est un client on verifie le format d'adresse et de port du serveur
		if (boutonClient->isChecked())
		{
			QString ipDuServeur = adresseServeur->text();
			QString portDuServeur = portServeur->text();
			
                        if (ipDuServeur.count(".") == 3)
			{
                                /*msgBox.setText(tr("Adresse IP du serveur incorrecte\nElle doit être au format xxx.xxx.xxx.xxx\noù xxx est compris entre 0 et 255"));
                                msgBox.exec();*/
                                iporhostname = 1;
                                //return false;
                                bool ok1, ok2, ok3, ok4;
                                int num1 = ipDuServeur.section('.', 0, 0).toInt(&ok1);
                                int num2 = ipDuServeur.section('.', 1, 1).toInt(&ok2);
                                int num3 = ipDuServeur.section('.', 2, 2).toInt(&ok3);
                                int num4 = ipDuServeur.section('.', 3, 3).toInt(&ok4);
                                if (!ok1 || !ok2 || !ok3 || !ok4 || num1<0 || num1>255 || num2<0 || num2>255 ||
                                        num3<0 || num3>255 || num4<0 || num4>255)
                                {
                                        msgBox.setText(tr("Adresse IP du serveur incorrecte\nElle doit être au format xxx.xxx.xxx.xxx\noù xxx est compris entre 0 et 255"));
                                        msgBox.exec();
                                        return false;
                                }

                                // On verifie la validite du port
                                bool ok5;
                                int num5 = portDuServeur.toInt(&ok5);
                                if (!ok5 || num5<0 || num5>65535)
                                {
                                        msgBox.setText(tr("Le numéro de port du serveur est incorrecte\nIl doit être compris entre 0 et 65535"));
                                        msgBox.exec();
                                        return false;
                                }
			}
                        else
                        {
                            iporhostname = -1;
                            if((ipDuServeur.contains(' '))&&(ipDuServeur.contains('@')))
                            {
                                        msgBox.setText(tr("Le nom d'hôte semble contenir des caractères interdits\nLes espaces et les @ sont interdit."));
                                        msgBox.exec();
                                        return false;

                            }

                        }

			// On recupere les 4 champs de l'adresse

		}
		
		// Si l'ordinateur local est le serveur, on verifie le format du port entre par l'utilisateur
		else
		{
			QString portLocal = portClient->text();

			// On verifie la validite du port
			bool ok6;
			int num6 = portLocal.toInt(&ok6);
			if (!ok6 || num6<0 || num6>65535)
			{
				msgBox.setText(tr("Le numéros de port indiqué est incorrecte\nIl doit être compris entre 0 et 65535"));
				msgBox.exec();
				return false;
			}
		}
		
		// Tout c'est passe correctement
		return true;
	}

	/********************************************************************/	
	/* Affiche l'identifiant client (port a donner aux clients)         */
	/********************************************************************/	
	void ClientServeur::afficherIdentifiantClient()
	{
		identifiantServeur->hide();
		identifiantClient->show();
	}
	
	/********************************************************************/	
	/* Affiche l'identifiant serveur (IP et port du serveur)            */
	/********************************************************************/	
	void ClientServeur::afficherIdentifiantServeur()
	{
		identifiantClient->hide();
		identifiantServeur->show();
	}

	/********************************************************************/	
	/* Lance le programme principal en ouvrant la MainWindow            */
	/********************************************************************/	
	void ClientServeur::programmePrincipal()
	{
		// Creation de la fenetre principale
		mainWindow = new MainWindow;
		// Mise a jour du titre de la fenetre principale
		mainWindow->setWindowTitle(NOM_APPLICATION);
		// Ouverture de la fenetre principale en mode plein ecran
		mainWindow->showMaximized();
	}
		
	/********************************************************************/	
	/* Creation de la fenetre de connexion                              */
	/********************************************************************/	
	void ClientServeur::creerFenetreConnexion()
	{
		// Creation de la fenetre de connexion
		fenetreConnexion = new QDialog();
		// On supprime de bouton d'aide contextuelle et on rajoute le bouton d'iconisation
		Qt::WindowFlags flags = fenetreConnexion->windowFlags();
		fenetreConnexion->setWindowFlags((flags ^ Qt::WindowContextHelpButtonHint) | Qt::WindowMinimizeButtonHint);
		// Creation du layout principal
		QVBoxLayout *layoutPrincipal = new QVBoxLayout(fenetreConnexion);
		// Creation du layout du haut
		QHBoxLayout *layoutHaut = new QHBoxLayout();
		// Creation du layout du bas
		QHBoxLayout *layoutBas = new QHBoxLayout();
		// Ajout des layout haut et bas dans le layout principal
		layoutPrincipal->addLayout(layoutHaut);
		layoutPrincipal->addLayout(layoutBas);
		
		// Creation du selecteur d'identite
		QGroupBox *groupeIdentite = new QGroupBox(tr("Votre identité"));
		// Selecteur de nom
		QHBoxLayout *layoutNom = new QHBoxLayout();
		QLabel *nomLabel = new QLabel(tr("Nom"));
		nomJoueur = new QLineEdit();
		nomJoueur->setToolTip(tr("Entrez votre nom de joueur"));
		layoutNom->addWidget(nomLabel);
		layoutNom->addWidget(nomJoueur);
		// Selecteur de couleur
		QHBoxLayout *layoutCouleur = new QHBoxLayout();
		QLabel *couleurLabel = new QLabel(tr("Couleur"));
		couleurJoueur = new QPushButton();
		couleurJoueur->setFlat(true);
		couleurJoueur->setDefault(false);
		couleurJoueur->setAutoDefault(false);
		couleurJoueur->setStyle(new QPlastiqueStyle());
		#ifdef WIN32
			couleurJoueur->setFixedSize(25,15);
		#elif defined (MACOS)
			couleurJoueur->setFixedSize(30,20);
		#endif
		couleurJoueur->setToolTip(tr("Choisissez une couleur"));
		couleurJoueur->setAutoFillBackground(true);
		layoutCouleur->addWidget(couleurLabel);
		layoutCouleur->addWidget(couleurJoueur);
		layoutCouleur->setAlignment(couleurJoueur,Qt::AlignLeft | Qt::AlignHCenter);
		layoutCouleur->setStretchFactor(couleurJoueur, 1);
		// Connexion du bouton de selection de la couleur a l'ouverture du selecteur de couleur
		QObject::connect(couleurJoueur, SIGNAL(pressed()), this, SLOT(ouvrirSelecteurCouleur()));
		// Ajout des layouts au selecteur d'identite
		QVBoxLayout *layoutIdentite = new QVBoxLayout();
		layoutIdentite->addLayout(layoutNom);
		layoutIdentite->addLayout(layoutCouleur);
		groupeIdentite->setLayout(layoutIdentite);
		// Ajout au layout principal
		layoutHaut->addWidget(groupeIdentite);

		// Creation du selecteur de role (joueur ou MJ)
		QGroupBox *groupeRole = new QGroupBox(tr("Vous êtes"));
		// Creation des boutons
		boutonJoueur = new QRadioButton(tr("Joueur"));
		boutonMj = new QRadioButton(tr("MJ"));
		QButtonGroup *groupeBoutonsRole = new QButtonGroup();
		groupeBoutonsRole->addButton(boutonJoueur);
		groupeBoutonsRole->addButton(boutonMj);
		// Ajout des boutons au selecteur de role
		QVBoxLayout *layoutRole = new QVBoxLayout();
		layoutRole->addWidget(boutonJoueur);
		layoutRole->addWidget(boutonMj);
		groupeRole->setLayout(layoutRole);
		// Ajout au layout principal
		layoutHaut->addWidget(groupeRole);
		#ifdef MACOS
			groupeRole->setFixedWidth(95);
		#endif

		// Creation du selecteur de client/serveur
		QGroupBox *groupeClientServeur = new QGroupBox(tr("Votre ordinateur est"));
		// Creation des boutons
		boutonClient = new QRadioButton(tr("Client"));
		boutonServeur = new QRadioButton(tr("Serveur"));
		QButtonGroup *groupeBoutonsClientServeur = new QButtonGroup();
		groupeBoutonsClientServeur->addButton(boutonClient);
		groupeBoutonsClientServeur->addButton(boutonServeur);
		// Connexion du bouton Client a l'affichage de l'identifiant serveur
		QObject::connect(boutonClient, SIGNAL(clicked()), this, SLOT(afficherIdentifiantServeur()));
		// Connexion du bouton Serveur a l'affichage de l'identifiant client
		QObject::connect(boutonServeur, SIGNAL(clicked()), this, SLOT(afficherIdentifiantClient()));

		// Creation de la ligne d'adresse du serveur
		identifiantServeur = new QWidget();
		QLabel *labelIpServeur = new QLabel(tr("Adresse IP du serveur"));
		QFontMetrics fm1(labelIpServeur->font());
		int largeurLabelIpServeur = fm1.width(labelIpServeur->text());
		adresseServeur = new QLineEdit();
                //adresseServeur->setMaxLength(15);
		#ifdef WIN32
			adresseServeur->setFixedWidth(94);
		#elif defined (MACOS)
			adresseServeur->setFixedWidth(118);
		#endif
		QWidget *separateur1 = new QWidget();
		separateur1->setFixedWidth(5);
		QLabel *labelPortServeur = new QLabel(tr("Port"));
		QFontMetrics fm2(labelPortServeur->font());
		int largeurLabelPortServeur = fm2.width(labelPortServeur->text());
		labelPortServeur->setFixedWidth(largeurLabelPortServeur);
		portServeur = new QLineEdit();
		portServeur->setMaxLength(5);
		#ifdef WIN32
			portServeur->setFixedWidth(38);
		#elif defined (MACOS)
			portServeur->setFixedWidth(50);
		#endif
		QHBoxLayout *layoutIdentifiantServeur = new QHBoxLayout(identifiantServeur);
		layoutIdentifiantServeur->addWidget(labelIpServeur);
		layoutIdentifiantServeur->addWidget(adresseServeur);
		layoutIdentifiantServeur->addWidget(separateur1);
		layoutIdentifiantServeur->addWidget(labelPortServeur);
		layoutIdentifiantServeur->addWidget(portServeur);
		int largeurIdentifiantServeur = largeurLabelIpServeur + 94 + 5 + largeurLabelPortServeur + 38;
		#ifdef WIN32
			identifiantServeur->setFixedWidth(largeurIdentifiantServeur+42);
		#elif defined (MACOS)
			identifiantServeur->setFixedWidth(largeurIdentifiantServeur+100);
		#endif

		// Creation de la ligne de port du client (cachee au lancement du programme)
		identifiantClient = new QWidget();
		QLabel *labelPortClient = new QLabel(tr("Numéro de port"));
		QFontMetrics fm3(labelPortClient->font());
		int largeurLabelPortClient = fm3.width(labelPortClient->text());
		labelPortClient->setFixedWidth(largeurLabelPortClient);
		portClient = new QLineEdit();
		portClient->setMaxLength(5);
		#ifdef WIN32
			portClient->setFixedWidth(38);
		#elif defined (MACOS)
			portClient->setFixedWidth(50);
		#endif
		
		QLabel *labelDefautClient = new QLabel(tr("(conseillé : ") + QString::number(PORT_SERVEUR) + ")");
		QFontMetrics fm4(labelDefautClient->font());
		int largeurLabelDefautClient = fm4.width(labelDefautClient->text());
		labelDefautClient->setFixedWidth(largeurLabelDefautClient);
		QHBoxLayout *layoutIdentifiantClient = new QHBoxLayout(identifiantClient);
		layoutIdentifiantClient->addWidget(labelPortClient);
		layoutIdentifiantClient->addWidget(portClient);
		layoutIdentifiantClient->addWidget(labelDefautClient);
		int largeurIdentifiantClient = largeurLabelPortClient + 38 + largeurLabelDefautClient;
		#ifdef WIN32
			identifiantClient->setFixedWidth(largeurIdentifiantClient+40);
		#elif defined (MACOS)
			identifiantClient->setFixedWidth(largeurIdentifiantClient+70);
		#endif
		identifiantClient->hide();

		// On fixe la taille de la fenetre
		int tailleMaxIdentifiant = largeurIdentifiantServeur>largeurIdentifiantClient?largeurIdentifiantServeur:largeurIdentifiantClient;
		#ifdef WIN32
			fenetreConnexion->setFixedSize(tailleMaxIdentifiant+86, 264);
		#elif defined (MACOS)
			identifiantClient->setFixedHeight(50);
			identifiantServeur->setFixedHeight(50);
			fenetreConnexion->setFixedSize(tailleMaxIdentifiant+170, 360);
		#endif

		// Ajout des boutons et des identifiants client et serveur
		QVBoxLayout *layoutClientServeur = new QVBoxLayout();
		layoutClientServeur->addWidget(boutonClient);
		#ifdef MACOS
			// Creation d'un widget de separation
			QWidget *separateur2 = new QWidget();
			separateur2->setFixedHeight(5);
			layoutClientServeur->addWidget(separateur2);
			layoutClientServeur->setSpacing(5);
		#endif
		layoutClientServeur->addWidget(boutonServeur);
		layoutClientServeur->addWidget(identifiantServeur);
		layoutClientServeur->addWidget(identifiantClient);
		groupeClientServeur->setLayout(layoutClientServeur);
		// Ajout au layout principal
		layoutBas->addWidget(groupeClientServeur);

		#ifdef WIN32
			// Creation d'un widget de separation
			QWidget *separateur3 = new QWidget();
			separateur3->setFixedHeight(5);
			layoutPrincipal->addWidget(separateur3);
		#endif

		// Creation des boutons de connexion
		QPushButton *boutonConnexion = new QPushButton(tr("Connexion"));
		QPushButton *boutonAnnuler = new QPushButton(tr("Annuler"));
		#ifdef WIN32
			boutonConnexion->setFixedWidth(80);
			boutonAnnuler->setFixedWidth(80);
		#elif defined (MACOS)
			boutonConnexion->setFixedWidth(110);
			boutonAnnuler->setFixedWidth(110);
		#endif
		boutonConnexion->setDefault(true);
		boutonConnexion->setAutoDefault(true);
		QHBoxLayout *layoutBoutons = new QHBoxLayout();
		layoutBoutons->addWidget(boutonConnexion);
		layoutBoutons->addWidget(boutonAnnuler);
		layoutBoutons->setAlignment(boutonConnexion, Qt::AlignRight | Qt::AlignHCenter);
		layoutBoutons->setAlignment(boutonAnnuler, Qt::AlignLeft | Qt::AlignHCenter);
		// Connexion du bouton Connexion a la demande de connexion
		QObject::connect(boutonConnexion, SIGNAL(clicked()), this, SLOT(demandeConnexion()));
		// Connexion du bouton Annuler a la fermeture de l'application
		QObject::connect(boutonAnnuler, SIGNAL(clicked()), qApp, SLOT(quit()));
		// Ajout au layout principal
		layoutPrincipal->addLayout(layoutBoutons);

		// On initialise les parametres avec les valeurs du fichier d'initilisation si celui-ci existe
		if (G_initialisation.initialisee)
		{
			nomJoueur->setText(G_initialisation.nomUtilisateur);
			couleurJoueur->setPalette(QPalette(G_initialisation.couleurUtilisateur));
			
			if (G_initialisation.joueur)
				boutonJoueur->setChecked(true);
			else
				boutonMj->setChecked(true);
				
			if (G_initialisation.client)
			{
				boutonClient->setChecked(true);
				afficherIdentifiantServeur();
			}
			else
			{
				boutonServeur->setChecked(true);
				afficherIdentifiantClient();
			}
				
			adresseServeur->setText(G_initialisation.ipServeur);
			portServeur->setText(G_initialisation.portServeur);
			portClient->setText(G_initialisation.portClient);
		}
		
		// Le fichier d'initialisation n'etait pas present : on met des valeurs par defaut
		else
		{
			QColor couleur;
			couleur.setHsv(rand()%360, rand()%200 + 56, rand()%190 + 50);
			couleurJoueur->setPalette(QPalette(couleur));
			boutonJoueur->setChecked(true);
			boutonClient->setChecked(true);
			portServeur->setText(QString::number(PORT_SERVEUR));
			portClient->setText(QString::number(PORT_SERVEUR));
		}

		// M.a.j du titre de la fenetre de connexion
		fenetreConnexion->setWindowTitle(QString(NOM_APPLICATION) + " - " + tr("Fenêtre de connexion"));
		
		// Affichage de la fenetre
		#ifdef WIN32
			fenetreConnexion->move(400, 300);
		#endif
		fenetreConnexion->show();
	}

	/********************************************************************/	
	/* Detruit la fenetre de connexion                                  */
	/********************************************************************/	
	void ClientServeur::detruireFenetreConnexion()
	{
		fenetreConnexion->~QWidget();
	}
	
	/********************************************************************/	
	/* Affiche le selecteur de couleur et m.a.j la couleur du bouton de */
	/* selection de couleur du joueur                                   */
	/********************************************************************/	
	void ClientServeur::ouvrirSelecteurCouleur()
	{
		// Ouverture du selecteur de couleur
		QColor couleur = QColorDialog::getColor((couleurJoueur->palette()).color(QPalette::Window));

		// Si l'utilisateur a clique sur OK on m.a.j la couleur du bouton
		if (couleur.isValid())
			couleurJoueur->setPalette(QPalette(couleur));
	}

	/********************************************************************/	
	/* Ajoute la liaison a la liste des liaisons                        */
	/********************************************************************/	
	void ClientServeur::ajouterLiaison(Liaison *liaison)
	{
		liaisons.append(liaison);
	}

	/********************************************************************/	
	/* Met a jour la variable d'initialisation afin qu'elle puisse etre */
	/* sauvegardee a la fermeture de l'application                      */
	/********************************************************************/	
	void ClientServeur::majParametresInitialisation()
	{
		// On met a jour le nom de l'utilisateur
		G_initialisation.nomUtilisateur = nomJoueur->text();
		// ...la couleur de l'utilisateur
		G_initialisation.couleurUtilisateur = G_couleurJoueurLocal;
		// ...la nature de l'utilisateur (joueur ou MJ)
		G_initialisation.joueur = G_joueur;
		// ...la nature de l'ordinateur local
		G_initialisation.client = G_client;
		// ...l'adresse IP du serveur
		G_initialisation.ipServeur = adresseServeur->text();
		// ...le port du serveur
		G_initialisation.portServeur = portServeur->text();
		// ...le port de connexion pour les clients
		G_initialisation.portClient = portClient->text();
	}


