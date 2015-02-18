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

	#include "Tchat.h"
	#include "TextEditAmeliore.h"
	#include "types.h"
	#include "variablesGlobales.h"


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    Tchat::Tchat(QString id, QAction *action, QWidget *parent)
        : QSplitter(parent)
    {
		// On donne un nom a l'objet "Tchat" pour le differencier des autres fenetres du workspace
		setObjectName("Tchat");

		// Initialisation des variables
		idJoueur = id;
		numHistorique = 0;
		actionAssociee = action;
	
		// On change l'icone de la fenetre
		setWindowIcon(QIcon(":/icones/vignette tchat.png"));

		// Connexion de l'action du sous-menu Tchats au cochage/decochage de la case dans la liste d'utilisateurs
		QObject::connect(action, SIGNAL(triggered(bool)), this, SLOT(changerEtatCase(bool)));
		
		// Mise a 0 de l'historique des messages
		historiqueMessages.clear();

		// Les 2 parties du tchat seront positionnees verticalement dans la fenetre
		setOrientation(Qt::Vertical);
		// Les widgets contenus ne peuvent pas avoir une taille de 0
		setChildrenCollapsible(false);

		// Creation de la zone affichant le texte des utilisateurs tiers
		zoneAffichage = new QTextEdit();
		zoneAffichage->setReadOnly(true);
		zoneAffichage->setMinimumHeight(30);
		
		// Creation de la zone editable contenant les messages de l'utilisateur local
		zoneEdition = new TextEditAmeliore();
		zoneEdition->setReadOnly(false);
		zoneEdition->setMinimumHeight(30);
		zoneEdition->setAcceptRichText(false);

		// Ajout des 2 zones dans le splitter
 	    addWidget(zoneAffichage);
 	    addWidget(zoneEdition);

		// Initialisation des tailles des 2 widgets
		QList<int> tailles;
		tailles.append(200);
		tailles.append(40);
		setSizes(tailles);

		// Connexion du signal de validation du text de la zone d'edition a la demande d'emission
		QObject::connect(zoneEdition, SIGNAL(entreePressee()), this, SLOT(emettreTexte()));
		// Connexion des signaux haut et bas au defilement des anciens messages
		QObject::connect(zoneEdition, SIGNAL(hautPressee()), this, SLOT(monterHistorique()));
		QObject::connect(zoneEdition, SIGNAL(basPressee()), this, SLOT(descendreHistorique()));
	}

	/********************************************************************/	
	/* Destructeur                                                      */
	/********************************************************************/	
	Tchat::~Tchat()
    {
		// Destruction de l'action associee au Tchat
		actionAssociee->~QAction();
	}

	/********************************************************************/	
	/* La zone d'edition est recopiee dans la zone d'affichage, puis    */
	/* envoyee aux autres utilisateurs, avant d'etre effacee            */
	/********************************************************************/	
    void Tchat::emettreTexte()
    {
		quint8 action;

		// On recupere le texte de la zone d'edition
		QString message = zoneEdition->toPlainText();
		
		// Si le message est issu de l'historique, on supprime le dernier element de la liste
		if (numHistorique < historiqueMessages.size())
			historiqueMessages.removeLast();
		
		// On ajoute le message a l'historique
		historiqueMessages.append(message);
		numHistorique = historiqueMessages.size();

		// Si le 1er caractere (hors espaces) est un "!" alors il s'agit d'un jet de des
		if (message.simplified().startsWith(QChar('!')))
		{
			bool ok;
			QString tirage;
			// Calcul du jet de des
			int result = calculerJetDes(message, &tirage, &ok);

			// Si le calcul est OK)
			if (ok)
			{
				// On affiche le resultat du tirage dans la zone d'affichage
				QString messageTemp = QString(tr("avez obtenu  ")) + QString::number(result) + QString(tr("  à votre jet de dés  [")) + tirage + QString("]");
				afficherTirage(tr("Vous"), G_couleurJoueurLocal, messageTemp);
				// On cree un nouveau message a envoyer aux autres utilisateurs
				message = QString(tr("a obtenu  ")) + QString::number(result) + QString(tr("  à son jet de dés  [")) + tirage + QString("]");
				// M.a.j de l'action a emettre
				action = messageTirage;
			}
			
			// S'il y a eu une erreur de syntaxe
			else
			{
				afficherMessage(tr("Syntaxe"), Qt::red, tr("!1d6 ou !5d10+3 ou !2d20-3d10+1d6+5 etc... Le jet de dés est public (utilisez & pour un jet privé)."));
				// On efface la zone d'edition
				zoneEdition->clear();
				// On quitte la fonction
				return;
			}
		}

		// Sinon si le 1er caractere (hors espaces) est un "&" alors il s'agit d'un jet de des prive (qui ne sera pas affiche chez les autres joueurs)
		else if (message.simplified().startsWith(QChar('&')))
		{
			bool ok;
			QString tirage;
			// Calcul du jet de des
			int result = calculerJetDes(message, &tirage, &ok);

			// Si le calcul est OK)
			if (ok)
			{
				// On affiche le resultat du tirage dans la zone d'affichage
				QString messageTemp = QString(tr("vous avez obtenu  ")) + QString::number(result) + QString(tr("  à votre jet de dés secret  [")) + tirage + QString("]");
				afficherTirage(tr("Jet secret :"), Qt::magenta, messageTemp);
				// On efface la zone d'edition
				zoneEdition->clear();
				// On quitte la fonction sans emettre le message
				return;
			}
			
			// S'il y a eu une erreur de syntaxe
			else
			{
				afficherMessage(tr("Syntaxe"), Qt::red, tr("&1d6 ou &5d10+3 ou &2d20-3d10+1d6+5 etc... Le jet de dés ne s'affiche pas chez les autres utilisateurs (utilisez ! pour un jet public)."));
				// On efface la zone d'edition
				zoneEdition->clear();
				// On quitte la fonction
				return;
			}
		}

		// Ultyme 
        // Sinon si le 1er caractere (hors espaces) est un "*" alors il s'agit d'un jet de des SR4 public 
		else if (message.simplified().startsWith(QChar('*')))
		{
			bool ok;
			QString tirage;
			QString glitch;
			// Calcul du jet de des
			int result = calculerJetDesSR4(message, &tirage, &glitch, &ok);

			// Si le calcul est OK
			if (ok)
			{
				// On affiche le resultat du tirage dans la zone d'affichage
				QString messageTemp = QString(tr("avez obtenu ")) + QString::number(result) + QString(tr(" succès")) + glitch + tirage;
				afficherTirage(tr("Vous"), G_couleurJoueurLocal, messageTemp);
				// On cree un nouveau message a envoyer aux autres utilisateurs
				message = QString(tr("a obtenu ")) + QString::number(result) + QString(tr(" succès")) + glitch + tirage;
				// M.a.j de l'action a emettre
				action = messageTirage;
			}
			// S'il y a eu une erreur de syntaxe
			else
			{
				afficherMessage(tr("Syntaxe SR4"), Qt::red, tr("*12D ... ajoutez R pour rusher, G3 pour les Gremlins d'indice 3 et + pour relancer les 6 ... ajouter C pour ne pas afficher les détails du lancer, et S pour n'afficher que les résultats."));
				// On efface la zone d'edition
				zoneEdition->clear();
				// On quitte la fonction
				return;
			}
		}
		// FIN Ultyme

		// Sinon on recopie le message tel quel dans la zone d'affichage
		else
		{
			afficherMessage(tr("Vous"), G_couleurJoueurLocal, message);
			// M.a.j de l'action a emettre
			action = messageTchat;

		}

		// On efface la zone d'edition
		zoneEdition->clear();

		// Emission du message au serveur, a un ou a l'ensemble des clients

		// Taille des donnees
		quint32 tailleCorps =
			// Taille de l'identifiant du joueur local
			sizeof(quint8) + G_idJoueurLocal.size()*sizeof(QChar) +
			// Taille de l'identifiant du joueur de destination
			sizeof(quint8) + idJoueur.size()*sizeof(QChar) +
			// Taille du message
			sizeof(quint32) + message.size()*sizeof(QChar);

		// Buffer d'emission
		char *donnees = new char[tailleCorps + sizeof(enteteMessage)];

		// Creation de l'entete du message
		enteteMessage *uneEntete;
		uneEntete = (enteteMessage *) donnees;
		uneEntete->categorie = discussion;
		uneEntete->action = action;
		uneEntete->tailleDonnees = tailleCorps;
				
		// Creation du corps du message
		int p = sizeof(enteteMessage);
		// Ajout de l'identifiant du joueur
		quint8 tailleIdJoueurLocal = G_idJoueurLocal.size();
		memcpy(&(donnees[p]), &tailleIdJoueurLocal, sizeof(quint8));
		p+=sizeof(quint8);
		memcpy(&(donnees[p]), G_idJoueurLocal.data(), tailleIdJoueurLocal*sizeof(QChar));
		p+=tailleIdJoueurLocal*sizeof(QChar);
		// Ajout de l'identifiant du joueur destinataire
		quint8 tailleIdJoueur = idJoueur.size();
		memcpy(&(donnees[p]), &tailleIdJoueur, sizeof(quint8));
		p+=sizeof(quint8);
		memcpy(&(donnees[p]), idJoueur.data(), tailleIdJoueur*sizeof(QChar));
		p+=tailleIdJoueur*sizeof(QChar);
		// Ajout du message
		quint32 tailleMessage = message.size();
		memcpy(&(donnees[p]), &tailleMessage, sizeof(quint32));
		p+=sizeof(quint32);
		memcpy(&(donnees[p]), message.data(), tailleMessage*sizeof(QChar));
		p+=tailleMessage*sizeof(QChar);

		// Il y a 2 cas de figure :
		// L'ordinateur local est le serveur : on envoie le message soit a
		// tous les utilisateurs soit a un utilisateur en particulier
		if (!G_client)
		{
			// Si l'idJoueur est vide il s'agit du tchat commun : on emet le message a tt le monde
			if (idJoueur.isEmpty())
				emettre(donnees, tailleCorps + sizeof(enteteMessage));

			// Sinon il s'agit d'un tchat prive : on envoie le message a un seul utilisateur
			else
			{
				// On recupere le numero de liaison correspondant a l'identifiant du joueur
				// (on soustrait 1 car le 1er utilisateur est toujours le serveur et qu'il
				// n'a pas de liaison associee)
				int numeroLiaison = G_listeUtilisateurs->numeroUtilisateur(idJoueur) - 1;
				emettre(donnees, tailleCorps + sizeof(enteteMessage), numeroLiaison);
			}
		}
		
		// L'ordinateur local est un client : on envoie vers le serveur
		else
			emettre(donnees, tailleCorps + sizeof(enteteMessage));

		// Liberation du buffer d'emission
		delete[] donnees;
	}

	
	/********************************************************************/	
	/* Renvoie l'identifiant du tchat                                   */
	/********************************************************************/	
	QString Tchat::identifiant()
	{
		return idJoueur;
	}

	/********************************************************************/	
	/* Ecrit le message dans la zone d'affichage, precede par le nom de */
	/* l'emetteur (avec la couleur passee en parametre)                 */
	/********************************************************************/	
    void Tchat::afficherMessage(QString utilisateur, QColor couleur, QString message)
	{
		// On repositionne le curseur a la fin du QTexEdit
		zoneAffichage->moveCursor(QTextCursor::End);
		// Affichage du nom de l'utilisateur
		zoneAffichage->setTextColor(couleur);
		zoneAffichage->append(utilisateur + " : ");
		// Affichage du message
   		zoneAffichage->setTextColor(Qt::black);
		zoneAffichage->insertPlainText(message);
		// On repositionne la barre de defilement, pour pouvoir voir le texte qui vient d'etre affiche
		zoneAffichage->verticalScrollBar()->setSliderPosition(zoneAffichage->verticalScrollBar()->maximum());

		// Si le tchat n'est pas en 1er plan, on fait clignoter l'utilisateur associe
		if (!G_mainWindow->estLaFenetreActive(this))
			G_listeUtilisateurs->faireClignoter(idJoueur);
	}

	/********************************************************************/	
	/* Ecrit le resultat du tirage dans la zone d'afficahage, precede   */
	/* par le nom de l'emetteur (avec la couleur passee en parametre)   */
	/********************************************************************/	
    void Tchat::afficherTirage(QString utilisateur, QColor couleur, QString message)
	{
		// On repositionne le curseur a la fin du QTexEdit
		zoneAffichage->moveCursor(QTextCursor::End);
		// Affichage du nom de l'utilisateur
		zoneAffichage->setTextColor(couleur);
		zoneAffichage->append(utilisateur + " ");
		// Affichage du message
   		zoneAffichage->setTextColor(Qt::black);
		zoneAffichage->insertPlainText(message);
		// On repositionne la barre de defilement, pour pouvoir voir le texte qui vient d'etre affiche
		zoneAffichage->verticalScrollBar()->setSliderPosition(zoneAffichage->verticalScrollBar()->maximum());

		// Si le tchat n'est pas en 1er plan, on fait clignoter l'utilisateur associe
		if (!G_mainWindow->estLaFenetreActive(this))
			G_listeUtilisateurs->faireClignoter(idJoueur);
	}

	/********************************************************************/
	/* Cache la fenetre au lieu de la detruire et demande le decochage  */
	/* de la case de l'utilisateur correspondant dans la liste des      */
	/* utilisateurs                                                     */
	/********************************************************************/	
	void Tchat::closeEvent(QCloseEvent *event)
	{
		// Masquage de la fenetre
		hide();
		// Si l'identifiant n'est pas vide (il ne s'agit pas du tchat commun)
		if (!idJoueur.isEmpty())
			// On demande le decochage de la case correspondante au joueur
			G_listeUtilisateurs->decocherCaseTchat(idJoueur);
		// Arret de la procedure de fermeture
		event->ignore();
		// Decoche l'action associee dans le sous-menu Tchats
		actionAssociee->setChecked(false);
	}

	/********************************************************************/	
	/* Analyse le message et calcule le jet de des si la syntaxe est    */
	/* correcte (ok == true), sinon ok == false. Une QString est        */
	/* generee pour expliquer le tirage                                 */
	/********************************************************************/	
	int Tchat::calculerJetDes(QString message, QString *tirage, bool *ok)
	{
		int i;
		bool stop;
		// Operateur par defaut
		char operateur = '+';
		// Resultat du calcul
		int result = 0;
		// Dernier nombre sous forme de QString
		QString nombreStr;
		// Dernier nombre sous forme de int
		int nombre;
		// Nbr de faces du de sous forme de QString
		QString facesStr;
		// Nbr de faces du de
		int faces;
	
		// Mise a 0 de la chaine tirage
		tirage->clear();
		// Suppression des espaces dans le message
		message.remove(QChar(' '));
		// Suppresion du "!" en tete du message
		message.remove(0, 1);
		// La taille du message est recalculee a chaque iteration
		int tailleMessage = message.size();
		// Si la taille est nulle, on quitte avec une erreur
		if (!tailleMessage)
		{
			*ok = false;
			return 0;
		}

		// Analyse du message (uniquement si celui-ci n'est pas vide)
		while(tailleMessage)
		{
			stop = false;
			// On regarde la longueur du nombre
			for (i=0; i<tailleMessage && !stop; i++)
				if (message[i].digitValue() == -1)
					stop = true;
			if (stop)
				i--;
			// S'il n'y a aucun chiffre, on quitte et on renvoie une erreur
			if (!i)
			{
				*ok = false;
				return 0;
			}
			// On recupere le nombre sous forme de QString
			nombreStr = message.left(i);
			// On le convertit en int
			nombre = nombreStr.toUInt(&stop, 10);
			// Suppression du nombre dans le message
			message.remove(0, i);
						
			// 2 cas de figure :
			// le nombre est suivi par un "D" : on recupere le nombre qui suit (nbr de faces du de)
			if (message[0] == QChar('d') || message[0] == QChar('D'))
			{
				// On supprime le "D"
				message.remove(0, 1);
				// On recalcule la longueur du message
				tailleMessage = message.size();
				// Si la taille est nulle, on quitte avec une erreur
				if (!tailleMessage)
				{
					*ok = false;
					return 0;
				}
				
				// dans le cas contraire on recupere le nbr de faces du de
				stop = false;
				// On regarde la longueur du nombre
				for (i=0; i<tailleMessage && !stop; i++)
					if (message[i].digitValue() == -1)
						stop = true;
				if (stop)
					i--;
				// S'il n'y a aucun chiffre, on quitte et on renvoie une erreur
				if (!i)
				{
					*ok = false;
					return 0;
				}
				// On recupere le nombre sous forme de QString
				facesStr = message.left(i);
				// On le convertit en int
				faces = facesStr.toUInt(&stop, 10);
				// Suppression du nombre dans le message
				message.remove(0, i);

				// S'il y a un nombre de des ou de faces nul, on quitte
				if (!nombre || !faces)
				{
					*ok = false;
					return 0;
				}

				// Inscription du jet de des dans la chaine tirage
				tirage->append(nombreStr + QString("d") + facesStr + QString(" (") );

				int de;
				int resultatDes = 0;
				// Enfin on effectue le jet de des
				for (int u=0; u<nombre; u++)
				{
					// Tirage du de
					de = rand()%faces + 1;
					// Ajout du de au resultat final
					resultatDes += de;
					// Ajout du resultat a la chaine tirage
					tirage->append(QString::number(de));
					// Ajout d'une virgule ou d'une parenthese fermante
					if (u < nombre-1)
						tirage->append(QString(","));
					else
						tirage->append(QString(")"));
				}

				// Le resultat des des est ajoute ou soustrait au resultat final
				if (operateur == '+')
					result += resultatDes;
				else
					result -= resultatDes;
			} // Fin de la lecture et du tirage des des

			// Le caractere suivant n'est pas un "D" : on ajoute ou on soustrait le nombre
			else
			{
				if (operateur == '+')
					result += nombre;
				else
					result -= nombre;
					
				// Ajout du nombre a la chaine tirage
				tirage->append(nombreStr);
			}

			// On recalcule la taille du message
			tailleMessage = message.size();
			// On regarde si le caractere suivant est un signe + ou -
			if (tailleMessage)
			{
				if (message[0] == QChar('+'))
				{
					operateur = '+';
					tirage->append(QString(" + "));
				}
				else if (message[0] == QChar('-'))
				{
					operateur = '-';
					tirage->append(QString(" - "));
				}
				else
				{
					*ok = false;
					return 0;
				}
				
				// On supprime l'operateur
				message.remove(0, 1);
				// Le message ne doit pas etre vide apres
				if (message.size() == 0)
				{
					*ok = false;
					return 0;
				}
			}

			// La taille du message est recalculee a chaque iteration
			tailleMessage = message.size();
		}

		*ok = true;
		return result;
	}

	// Ultyme
	/********************************************************************/	
	/* Analyse le message et calcule le jet de des selon les regles de  */
    /* SR4 si la syntaxe est correcte (ok == true). Une Qstring est ge- */
	/* neree pour expliquer le tirage, un autre pour le glitch.         */
	/* Les Gremlins et la regle du 6 again (usage de l'edge) sont geres.*/
	/********************************************************************/	
	int Tchat::calculerJetDesSR4(QString message, QString *tirage, QString *glitch, bool *ok)
	{
      //Initialisation du nombre de succes, de glitches, et des parametres du lancer
      int nbDes;
      int nbDesEnCours;
      int nbDesEnsuite;
      QString nbDesStr;
      int nbSucces = 0;
      int nbGlitch = 0;
      bool sixAgainActif = false;
      bool gremlinsActif = false;
      bool rushActif = false;
      bool modeCourtActif = false;
      bool modeSecretActif = false;
      int indiceGremlins = 0;
      QString indiceGremlinsStr;
      int nbFaces = 6;
      QString nbFacesStr = "6";
      int seuilSucces = 5;
      int seuilGlitch = 1;
      int nbPasses = 0;
      int de;
      bool estOK;
      
      
      // Mise a 0 de la chaine tirage et du glitch
	  tirage->clear();
	  glitch->clear();
      // Suppression des espaces dans le message
	  message.remove(QChar(' '));
	  // Suppresion du "*" en tete du message
	  message.remove(0, 1);
	  // La taille du message est initialisee
	  int tailleMessage = message.size();
      
      // RECUPERATION DU NOMBRE DE DES
      //Si le message est vide, erreur de syntaxe
      if (tailleMessage == 0)
      {
        *ok = false;
        return 0;
      }
      //On repère le premier caractere qui n'est pas un chiffre
      int i=0;
      while (i<tailleMessage && message[i].digitValue() != -1)
        i++;
      //Si aucun chiffre, erreur de syntaxe
      if (i < 0)
      {
        *ok = false;
        return 0;
      }	  
      // On recupere le nombre sous forme de QString
	  nbDesStr = message.left(i);
 	  // On le convertit en int
	  nbDes = nbDesStr.toUInt(&estOK, 10);
	  // Suppression du nombre dans le message
	  message.remove(0, i);
	  // On recalcule la longueur du message
	  tailleMessage = message.size();
      
      //SUPPRESSION DU "D" AVANT RECUPERATION DES PARAMETRES
      //Si le message est desormais vide ou ne contient pas un "D", erreur de syntaxe
      if (tailleMessage == 0 || (message[0] != QChar('d') && message[0] != QChar('D')))
      {
        *ok = false;
        return 0;
      }
      // Suppression du "D" dans le message et recalcul de la longueur du message
	  message.remove(0, 1);
	  tailleMessage = message.size();
            
      //RECUPERATION DES PARAMETRES
      //Tant que la chaine qui reste n'est pas vide, on interprete les parametres
      while (tailleMessage>0)
      {
        //S'il y a un "+" pour la premiere fois, alors activation du "Six Again"
        if (message[0] == '+')
        {
          //Si "Six Again" est deja actif, erreur de syntaxe
          if (sixAgainActif)
          {
            *ok = false;
            return 0;                  
          }
          //Sinon on active le "Six Again"
          sixAgainActif = true;
          //On supprime le "+" et on recalcule la longueur du message                
	      message.remove(0, 1);
	      tailleMessage = message.size();
        }
        //S'il y a un G pour la premiere fois, alors recuperation du parametre Gremlins
        else if (message[0] == QChar('g') || message[0] == QChar('G'))
        {
          //Si "Gremlins" est deja actif, erreur de syntaxe
          if (gremlinsActif)
          {
            *ok = false;
            return 0;                  
          }
          //Sinon on active les "Gremlins"
          gremlinsActif = true;
          //On supprime le "G" et on recalcule la longueur du message                
	      message.remove(0, 1);
	      tailleMessage = message.size();
          //Si le message est vide ou le prochain caractere n'est pas un chiffre, 
          //alors erreur (on attend l'indice "Gremlins")
          if (tailleMessage == 0 || message[0].digitValue() == -1)
          {
            *ok = false;
            return 0;                  
          }
          //Sinon, recuparation de l'indice sous forme de QString
	      indiceGremlinsStr = message.left(1);
 	      // On le convertit en int
	      indiceGremlins = indiceGremlinsStr.toUInt(&estOK, 10);
	      // Suppression du nombre dans le message
	      message.remove(0, 1);
	      // On recalcule la longueur du message
	      tailleMessage = message.size();     
        }
        //S'il y a un "R" pour la premiere fois, alors activation du "Rush" (glitches sur 1 et 2)
        else if (message[0] == 'r' || message[0] == 'R')
        {
          //Si "Rush" est deja actif, erreur de syntaxe
          if (rushActif)
          {
            *ok = false;
            return 0;                  
          }
          //Sinon on active le "Rush"
          rushActif = true;
          seuilGlitch = 2;
          //On supprime le "R" et on recalcule la longueur du message                
	      message.remove(0, 1);
	      tailleMessage = message.size();
        }
        //S'il y a un "C" pour la premiere fois, alors activation du "Mode court" (pas de détail du jet)
        else if (message[0] == 'C' || message[0] == 'c')
        {
          //Si "Mode court" est deja actif, erreur de syntaxe
          if (modeCourtActif)
          {
            *ok = false;
            return 0;                  
          }
          //Sinon on active le "Mode court"
          modeCourtActif = true;
          //On supprime le "Mode Court" et on recalcule la longueur du message                
	      message.remove(0, 1);
	      tailleMessage = message.size();
        }
        //S'il y a un "S" pour la premiere fois, alors activation du "Mode secret" (pas d'informations autres que le résultat)
        else if (message[0] == 'S' || message[0] == 's')
        {
          //Si "Mode secret" est deja actif, erreur de syntaxe
          if (modeSecretActif)
          {
            *ok = false;
            return 0;                  
          }
          //Sinon on active le "Mode secret"
          modeSecretActif = true;
          //On supprime le "Mode Court" et on recalcule la longueur du message                
	      message.remove(0, 1);
	      tailleMessage = message.size();
        }
        //Si ce n'est pas un "+", un "R", un "C", un "S" ou un "G", erreur de syntaxe
        else
        {
          *ok = false;
          return 0;    
        }                            
      }      
      
      //CALCUL DU RESULTAT
      //Si le nombre de des vaut 0, erreur de syntaxe
      if (nbDes == 0)
      {
        *ok = false;
        return 0;
      }
      // Inscription du jet de des dans la chaine tirage (si mode Secret inactif)
	  if (!modeSecretActif)
      {
        tirage->append(QString(" (") + nbDesStr + QString(" dés"));
	    if (rushActif)
          tirage->append(QString(" en rushant"));
        if (gremlinsActif)
	      tirage->append(QString(" avec Gremlins ") + indiceGremlinsStr);
        if (sixAgainActif)
          tirage->append(QString(" - les 6 sont relancés"));
        if (!modeCourtActif)
          tirage->append(QString(" : "));
        else
          tirage->append(QString(")"));
      }
      //Lancer de chaque de
      //Afin de gerer les 6 again, on lance une serie de des et prepare la serie qui suit, generee par les 6
      nbDesEnsuite = nbDes;
      while (nbDesEnsuite != 0)
      {
        nbDesEnCours = nbDesEnsuite;
        nbDesEnsuite = 0;
        nbPasses++;
        for (int u=0; u<nbDesEnCours ; u++)
        {
          // Tirage du de
		  de = rand()%nbFaces + 1;
		  // Verification du succes
		  if (de >= seuilSucces)
		  {
            nbSucces++;
            //Si la regle des 6 again est activee, augmentation de la prochaine serie
            if (sixAgainActif && de == 6) 
              nbDesEnsuite++;       
          }
          // Verification du glitch (uniquement sur la premiere serie de lancer)
          if (nbPasses = 1 && de <= seuilGlitch)
          {
            nbGlitch++;       
          }
          // Ajout du resultat a la chaine Tirage si pas en Mode court ou Mode Secret
		  if (!modeCourtActif && !modeSecretActif)
          {
            tirage->append(QString::number(de));
            // Ajout d'un espace, d'un "puis" entre deux series ou d'une parenthèse pour finir
		    if (u < nbDesEnCours-1)
		      tirage->append(QString(" "));
		    else if (nbDesEnsuite > 0)
		      tirage->append(QString(" puis "));
		    else
		      tirage->append(QString(")"));
          }  
        }  
      }
      //VERIFICATION D'UN GLITCH OU D'UN GLITCH CRITIQUE
      if (nbGlitch>0 && nbGlitch>((nbDes/2)-indiceGremlins))
      {
        if (nbSucces == 0)
          glitch->append(QString(" et une complication critique !! "));
        else
          glitch->append(QString(" et une complication !! "));                 
      }
      
      //RETOUR DU RESULTAT
      *ok = true;
      return nbSucces;               
	}
	// FIN Ultyme
	
	/********************************************************************/	
	/* Affiche le message precedent                                     */
	/********************************************************************/	
	void Tchat::monterHistorique()
	{
		// Ne s'applique que si la liste n'est pas vide
		if (historiqueMessages.isEmpty())
			return;
		
		// C'est la 1ere fois que l'utilisateur appuie sur la touche : on memorise la ligne actuelle
		if (numHistorique == historiqueMessages.size())
			historiqueMessages.append(zoneEdition->toPlainText());

		// Si on a atteint le 1er message on arrete de remonter
		if (numHistorique)
			numHistorique--;
			
		// Affichage de l'ancien message dans la zone d'edition
		zoneEdition->clear();
		zoneEdition->append(historiqueMessages[numHistorique]);
	}
		
	/********************************************************************/	
	/* Affiche le message suivant                                       */
	/********************************************************************/	
	void Tchat::descendreHistorique()
	{
		// Uniquement si on a deja appuye sur fleche haut
		if (numHistorique < historiqueMessages.size() - 1)
		{
			numHistorique++;
			// Affichage du nouveau message dans la zone d'edition
			zoneEdition->clear();
			zoneEdition->append(historiqueMessages[numHistorique]);
		}
	}

	/********************************************************************/
	/* Met a jour l'action liee au Tchat dans le sous-menu Tchats       */
	/********************************************************************/
	void Tchat::majAction()
	{
		actionAssociee->setChecked(isVisible());
	}
	
	/********************************************************************/
	/* Met a jour l'etat de la case (coche, decoche) dans la liste des  */
	/* utilisateurs                                                     */
	/********************************************************************/
	void Tchat::changerEtatCase(bool coche)
	{
		// S'il s'agit du tchat commun, on quitte
		if (idJoueur.isEmpty())
			return;
			
		// Sinon on met a jour la case du joueur correspondant
		if (coche)
			G_listeUtilisateurs->cocherCaseTchat(idJoueur);
		else
			G_listeUtilisateurs->decocherCaseTchat(idJoueur);		
	}
	
	/********************************************************************/
	/* Sauvegarde le tchat dans un fichier portant son nom              */
	/********************************************************************/
	void Tchat::sauvegarderTchat(QFile &file)
	{
		// On recupere le document contenant la discussion
		QTextDocument *document = zoneAffichage->document();
		// On convertit le document en HTML
		QString html = document->toHtml(QByteArray("UTF-8"));
		// On cree un flux de donnees rattache au fichier
		QTextStream fichier(&file);
		// On envoie le document HTML dans le flux
		fichier << html;
	}

	/********************************************************************/
	/* Lorsque la fenetre s'ouvre, on positionne le curseur sur la zone */
	/* d'edition                                                        */
	/********************************************************************/
	void Tchat::showEvent(QShowEvent *event)
	{
		// On place le curseur sur la zone d'edition
		zoneEdition->setFocus(Qt::OtherFocusReason);
	}


