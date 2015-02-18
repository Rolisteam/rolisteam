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
#include <QDebug>
#include "Tchat.h"
#include "TextEditAmeliore.h"
#include "types.h"
#include "variablesGlobales.h"


/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/	
Tchat::Tchat(QString id, QAction *action, /*QString tmp,*/QWidget *parent)
: QSplitter(parent)
{
	// On donne un nom a l'objet "Tchat" pour le differencier des autres fenetres du workspace
	setObjectName("Tchat");

	// Initialisation des variables
	idJoueur = id;

	numHistorique = 0;
	actionAssociee = action;

    warnedEmoteUnavailable = false;

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

    m_keyWordList << "e " << "em " << "me " << "emote ";

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
    //TCHAT_MESSAGE, DICE_MESSAGE, EMOTE_MESSAGE
    quint8 action=DICE_MESSAGE;

	// On recupere le texte de la zone d'edition
	QString message = zoneEdition->toPlainText();
	
	// Si le message est issu de l'historique, on supprime le dernier element de la liste
	if (numHistorique < historiqueMessages.size())
		historiqueMessages.removeLast();
	
	// On ajoute le message a l'historique
	historiqueMessages.append(message);
	numHistorique = historiqueMessages.size();

	bool ok=true;
	QString tirage;
	int result;
	zoneEdition->clear();
    QTextStream out(stderr,QIODevice::WriteOnly);

	QString tmpmessage=message.simplified();
    QString messageCorps="";
    QString messageTitle="";
    QColor color;

    switch(tmpmessage[0].toAscii())
	{
		case '!':
			result = calculerJetDes(message, tirage, ok);
			if(ok)
			{
               			messageCorps = tr("avez obtenu %1 à votre jet de dés [%2]").arg(result).arg(tirage);
                		messageTitle = tr("Vous");
                		color = G_couleurJoueurLocal;
        	        	afficherMessage(messageTitle, color, messageCorps,DICE_MESSAGE);
	               		 message = QString(tr("a obtenu %1 à  son jet de dés [%2]").arg(result).arg(tirage));
			}
			else
            {
                messageCorps = tr("!1d6 ou !5d10+3 ou !2d20-3d10+1d6+5 etc... Le jet de dés est public (utilisez & pour un jet privé).");
                messageTitle = tr("Syntaxe");
                color = Qt::red;
                afficherMessage(messageTitle, color, messageCorps);
            }
            break;
		case '&':
			result = calculerJetDes(message, tirage, ok);
			if (ok)
            {
                messageCorps = tr("vous avez obtenu %1 à votre jet de dés secret [%2]").arg(result).arg(tirage);
                messageTitle = tr("Jet secret :");
                color = Qt::magenta;
                afficherMessage(messageTitle, color,messageCorps ,DICE_MESSAGE);
            }
			else
            {
                messageCorps = tr("!1d6 ou !5d10+3 ou !2d20-3d10+1d6+5 etc... Le jet de dés est public (utilisez & pour un jet privé).");
                messageTitle = tr("Syntaxe");
                color = Qt::red;
                afficherMessage(messageTitle, color, messageCorps);
            }
			return;
			break;
		case '*':
            {
                QRegExp exp("\\*[0-9]+D.*");
                if(exp.exactMatch(message))
                {
                    QString glitch;
                    result = calculerJetDesSR4(message, tirage, glitch, ok);
                    if (ok)
                    {
                        messageCorps = tr("avez obtenu %1 succès %2%3").arg(result).arg(glitch).arg(tirage);
                        messageTitle = tr("Vous");
                        // On affiche le resultat du tirage dans la zone d'affichage
                        afficherMessage(messageTitle, G_couleurJoueurLocal,messageCorps ,DICE_MESSAGE);
                        // On cree un nouveau message a envoyer aux autres utilisateurs
                        message = QString(tr("a obtenu %1 succès %2%3").arg(result).arg(glitch).arg(tirage));
                    }
                    else
                    {
                        messageCorps = tr("*12D ... ajoutez R pour rusher, G3 pour les Gremlins d'indice 3 et + pour relancer les 6 ... ajouter C pour ne pas afficher les détails du lancer, et S pour n'afficher que les résultats.");
                        messageTitle = tr("Syntaxe SR4");
                         color = Qt::red;
                        afficherMessage(messageTitle, color, messageCorps);
                    }
                    break;
                }
                else
                {
                        messageTitle = tr("Vous");
                        afficherMessage(messageTitle, G_couleurJoueurLocal, message);
                        // action is messageTchat only if there are no dices
                        action = TCHAT_MESSAGE;
                        break;
                }
            }

        case '/':
            {
                bool emote = false;

                for(int i = 0;((i < m_keyWordList.size())&&(!emote)); i++)
                {
                    QString s = m_keyWordList[i];

                    if(tmpmessage.indexOf(s) == 1)
                    {
                        emote = true;

                        message.remove(0,s.length()+1);

                       // tmpmessage.prepend(idJoueur);
                    }

                }
                if(emote)
                {
                    // Warn if some users don't have Emote(0) feature
                    if (!warnedEmoteUnavailable)
                    {
    		            if (idJoueur.isEmpty())
                        {   // Public chat
                            int nb_implemented = g_featuresList.countImplemented(QString("Emote"), 0);
                            int nb_users = G_listeUtilisateurs->tousLesUtilisateurs().size();
                            if (nb_implemented < nb_users)
                            {
                                messageTitle = tr("Attention");
                                messageCorps = tr("Certains utilisateurs risquent de ne pas voir vos emotes.");
                                color = Qt::red;
                                afficherMessage(messageTitle, color, messageCorps);
                                warnedEmoteUnavailable = true;
                            }
                        }
                        else
                        {   // Private chat
                            if (!g_featuresList.clientImplements(Feature(idJoueur, QString("Emote"), 0)))
                            {
                                messageTitle = tr("Attention");
                                messageCorps = tr("Votre interlocuteur risque de ne pas voir vos emotes.");
                                color = Qt::red;
                                afficherMessage(messageTitle, color, messageCorps);
                            }
                            warnedEmoteUnavailable = true;
                        }
                    }
                        
                    
                    m_owner = G_listeUtilisateurs->nomUtilisateur(G_idJoueurLocal);
                    afficherMessage(m_owner, G_couleurJoueurLocal, message,EMOTE_MESSAGE);
                    action = EMOTE_MESSAGE;
                    break;
                }
            }

		default:
            messageTitle = tr("Vous");
            afficherMessage(messageTitle, G_couleurJoueurLocal, message);
			// action is messageTchat only if there are no dices
            action = TCHAT_MESSAGE;
            break;
	}

	if(!ok)
		return;

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
void Tchat::afficherMessage(QString& utilisateur, QColor& couleur, QString& message, actionDiscussion msgtype)
{
	// On repositionne le curseur a la fin du QTexEdit
	zoneAffichage->moveCursor(QTextCursor::End);
	// Affichage du nom de l'utilisateur
	zoneAffichage->setTextColor(couleur);
    switch(msgtype)
    {
    case TCHAT_MESSAGE://TCHAT_MESSAGE, DICE_MESSAGE, EMOTE_MESSAGE
        zoneAffichage->setFontItalic(false);
        zoneAffichage->append(QString("%1 : ").arg(utilisateur));
        break;
    case EMOTE_MESSAGE:
        zoneAffichage->setFontItalic(true);
        zoneAffichage->append(QString("%1 ").arg(utilisateur));

        break;
    case DICE_MESSAGE:
        zoneAffichage->setFontItalic(false);
        zoneAffichage->append(QString("%1 ").arg(utilisateur));
        break;
    }



	// Affichage du message
	zoneAffichage->setTextColor(Qt::black);

	zoneAffichage->insertPlainText(message);
    if(msgtype==EMOTE_MESSAGE)
        zoneAffichage->setFontItalic(false);
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
int Tchat::calculerJetDes(QString &message, QString &tirage, bool &ok)
{
	//1 by default to add, -1 to substract.
	short signOperator=1;
	int result = 0;
	int nombre;
	int faces;
	int garde;
	bool relance10;

	// Mise a 0 de la chaine tirage
	tirage.clear();
	// Suppression des espaces dans le message
	message.remove(QChar(' '));
	// Suppresion du "!" en tete du message
	message.remove(0, 1);
	// Si la taille est nulle, on quitte avec une erreur
	if (!message.size())
		return (ok = false);

	// Analyse du message (uniquement si celui-ci n'est pas vide)
	while(message.size())
	{
	
        ok = GetNumber(message,&nombre);
		
		// 4 cas de figure :
		// le nombre est suivi par un "D" : on recupere le nombre qui suit (nbr de faces du de)
		if (message[0] == QChar('d') || message[0] == QChar('D'))
		{
			// On supprime le "D"
			message.remove(0, 1);

			// Si la taille est nulle, on quitte avec une erreur
			if (!message.size())
				return (ok = false);
			
            ok = GetNumber(message,&faces);

			// S'il y a un nombre de des ou de faces nul, on quitte
			if (!nombre || !faces)
				return (ok = false);

            // The dices rolling
            QList<int> listDices;
			unsigned short dice, sumDice=0;
            for(unsigned short u=0;u<nombre;u++)
			{
				qDebug()<<faces;
				dice=(rand()%faces)+1;
                listDices.append(dice);
				sumDice+=dice;
			}
			
			// Formatting the "tirage" text
            tirage.append(tr("%1d%2 (%3").arg(nombre).arg(faces).arg(listDices[0]));
            for(unsigned short u=1;u<listDices.size();u++)
			{
				tirage.append(QString(","));
				tirage.append(QString::number(listDices[u]));
			}
			tirage.append(QString(")"));
			
			result+=signOperator*sumDice;
		} // Fin de la lecture et du tirage des des

                // le nombre est suivi par un "G" : on recupere le nombre qui suit (nbr de des gardés)
		else if (message[0] == QChar('g') || message[0] == QChar('G'))
		{
			relance10=(message[0] == QChar('G'));
			// On supprime le "G"
			message.remove(0, 1);

			// Si la taille est nulle, on quitte avec une erreur
			if (!message.size())
				return (ok = false);

			ok = GetNumber(message,&garde);

			// S'il y a un nombre de des ou de faces nul, on quitte
			if (!nombre || !garde)
				return (ok = false);

            if(garde > nombre)
                garde = nombre;

            // The dices rolling
            QList<int> listDices;
            unsigned short tmpDice, dice, sumDice=0;
            unsigned short min = 11;

            for(unsigned short u=0;u<nombre;u++)
            {
                qDebug()<<garde;

                tmpDice=(rand()%10)+1;
                dice=tmpDice;
                if(relance10)
                    while((tmpDice==10))
                    {
                        tmpDice=(rand()%10)+1;
                        dice+=tmpDice;
                    }

                listDices.append(dice);
                //sumDice+=dice;
            }
            qSort(listDices.begin(), listDices.end(), qGreater<unsigned short>());
            for(int it=0;it<garde;++it)
            {
                sumDice+=listDices[it];
            }

            // Formatting the "tirage" text
            tirage.append(tr("%1g%2 (%3").arg(nombre).arg(garde).arg(listDices[0]));
            for(unsigned short u=1;u<listDices.size();u++)
            {
                tirage.append(QString(","));
                tirage.append(QString::number(listDices[u]));
            }
            tirage.append(QString(")"));

			result+=signOperator*sumDice;
		} // Fin de la lecture et du tirage des des

		// le nombre est suivi par un "s" : on recupere pas le nombre qui suit (il n'y en a pas)
		else if (message[0] == QChar('s') || message[0] == QChar('S'))
		{
			// On supprime le "S"
			message.remove(0, 1);

			// S'il y a un nombre de des ou de faces nul, on quitte
			if (!nombre)
				return (ok = false);

            // The dices rolling
            QList<int> listDices;
            unsigned short dice, sumDice=0;
            for(unsigned short u=0;u<nombre;u++)
            {
                dice=(rand()%6)+1;
                listDices.append(dice);
                if(dice>=5)
                   ++sumDice ;
            }

            // Formatting the "tirage" text
            tirage.append(tr("%1s (%3").arg(nombre).arg(listDices[0]));
            for(unsigned short u=1;u<listDices.size();u++)
            {
                tirage.append(QString(","));
                tirage.append(QString::number(listDices[u]));
            }
            tirage.append(QString(")"));

			result+=signOperator*sumDice;
		} // Fin de la lecture et du tirage des des

		// Le caractere suivant n'est pas un "D" : on ajoute ou on soustrait le nombre
		else
		{
			result+=signOperator*nombre;
			tirage+=QString("%1").arg(nombre);
		}

		// On regarde si le caractere suivant est un signe + ou -
		if (message.size())
		{
			if (message[0] == QChar('+'))
				signOperator = 1;
			else if (message[0] == QChar('-'))
				signOperator = -1;
			else
				return (ok = false);
				
			tirage+=' ';
			tirage+=message[0];
			tirage+=' ';
			
			// On supprime l'operateur
			message.remove(0, 1);
			// Le message ne doit pas etre vide apres
			if (message.size() == 0)
				return (ok = false);
		}
	}

	ok = true;
	return result;
}

// Ultyme
/********************************************************************/	
/* Analyse le message et calcule le jet de des selon les regles de  */
/* SR4 si la syntaxe est correcte (ok == true). Une Qstring est ge- */
/* neree pour expliquer le tirage, un autre pour le glitch.         */
/* Les Gremlins et la regle du 6 again (usage de l'edge) sont geres.*/
/********************************************************************/	
int Tchat::calculerJetDesSR4(QString &message, QString &tirage, QString &glitch, bool ok)
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

	tirage.clear();
	glitch.clear();

	message.remove(QChar(' '));
	message.remove(0, 1);

	if (message.size() == 0)
		return (ok=false);

    ok=GetNumber(message,&nbDes);

	if (message.size() == 0 || (message[0] != QChar('d') && message[0] != QChar('D')))
		return (ok=false);
	message.remove(0, 1);

	while (message.size()>0)
	{
		QChar firstChar=message[0];
		//if the first character of message is a letter, put it to lowercase
		if(firstChar.isLetter())
			firstChar.toCaseFolded ();
			
        switch(firstChar.toAscii())
		{
			case '+':
				if (sixAgainActif)
					return (ok=false);

				sixAgainActif = true;
				break;
			case 'g':
				if (gremlinsActif)
					return (ok=false);

				gremlinsActif = true;
				message.remove(0, 1);

				// the function need to have a message and that message[0] is a digit to continue
				if (!(message.size() && message[0].isDigit()))
					return (ok=false);
				indiceGremlinsStr = message.left(1);
				indiceGremlins = indiceGremlinsStr.toUInt(&estOK, 10);
				break;
			case 'r':
				if (rushActif)
					return (ok=false);

				rushActif = true;
				seuilGlitch = 2;
				break;
			case 'c':
				if (modeCourtActif)
					return (ok=false);

				modeCourtActif = true;
				break;
			case 's':
				if (modeSecretActif)
					return (ok=false);

				modeSecretActif = true;
				break;
			default:
				return (ok=false);
		}
		message.remove(0, 1);
	}



	if (nbDes == 0)
		return (ok=false);

	if (!modeSecretActif)
	{
                tirage.append(QString(tr(" (%1 dés").arg(nbDes)));
		if (rushActif)
			tirage.append(QString(tr(" en rushant")));
		if (gremlinsActif)
			tirage.append(QString(tr(" avec Gremlins %1").arg(indiceGremlinsStr)));
		if (sixAgainActif)
                        tirage.append(QString(tr(" - les 6 sont relancés")));
		if (!modeCourtActif)
			tirage.append(QString(tr(" : ")));
		else
			tirage.append(QString(tr(")")));
	}

	nbDesEnsuite = nbDes;
	while (nbDesEnsuite != 0)
	{
		nbDesEnCours = nbDesEnsuite;
		nbDesEnsuite = 0;
		nbPasses++;

		for (int u=0; u<nbDesEnCours ; u++)
		{
			// Tirage du de
			de = 1 + (int)((double)rand() / ((double)RAND_MAX + 1) * nbFaces); //rand()%nbFaces + 1;
			// Verification du succes
			if (de >= seuilSucces)
			{
				nbSucces++;
				//Si la regle des 6 again est activee, augmentation de la prochaine serie
				if (sixAgainActif && de == 6)
					nbDesEnsuite++;
			}
			// Verification du glitch (uniquement sur la premiere serie de lancer)
			// this is strange... nbPasses '=' 1 ??? not '==' ?
            if ((nbPasses = 1) && (de <= seuilGlitch))
				nbGlitch++;
			// Ajout du resultat a la chaine Tirage si pas en Mode court ou Mode Secret
			if (!modeCourtActif && !modeSecretActif)
			{
				tirage.append(QString::number(de));
				// Ajout d'un espace, d'un "puis" entre deux series ou d'une parenthese pour finir
				if (u < nbDesEnCours-1)
					tirage.append(QString(" "));
				else if (nbDesEnsuite > 0)
					tirage.append(QString(" puis "));
				else
					tirage.append(QString(")"));
			}
		}
	}

	if (nbGlitch>0 && nbGlitch>((nbDes/2)-indiceGremlins))
	{
		if (nbSucces == 0)
			glitch.append(QString(" et une complication critique !! "));
		else
			glitch.append(QString(" et une complication !! "));
	}


	ok = true;
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

bool Tchat::GetNumber(QString &str, int* value)
{
	bool error;
    int i=0;
	while(str[i].isDigit() && i<str.size())
		i++;
	// Length of 0 (No digit): exit with error
	if(!i)
		return false;

    *value = str.left(i).toUInt(&error, 10);
	str.remove(0, i);
	return error;
}
