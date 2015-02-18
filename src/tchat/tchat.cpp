/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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

#include "tchat.h"
#include "tchateditor.h"
//#include "types.h"




Tchat::Tchat(QString id, QWidget *parent)
: SubMdiWindows(parent)
{

    // On donne un nom a l'objet "Tchat" pour le differencier des autres fenetres du workspace
    setObjectName("Tchat");
    m_splitter = new QSplitter;
    m_active=false;
    // Initialisation des variables

    m_historicNumber = 0;
    m_type = SubMdiWindows::TCHAT;
    // On change l'icone de la fenetre
    setWindowIcon(QIcon(":/icones/vignette tchat.png"));

    // Mise a 0 de l'historique des messages
    m_messageHistoric.clear();

    // Les 2 parties du tchat seront positionnees verticalement dans la fenetre
    m_splitter->setOrientation(Qt::Vertical);
    // Les widgets contenus ne peuvent pas avoir une taille de 0
    m_splitter->setChildrenCollapsible(false);

    // Creation de la zone affichant le texte des utilisateurs tiers
    m_meetingRoom = new QTextEdit();
    m_meetingRoom->setReadOnly(true);
    m_meetingRoom->setMinimumHeight(30);

    // Creation de la zone editable contenant les messages de l'utilisateur local
    m_tchatEditor = new TchatEditor();
    m_tchatEditor->setReadOnly(false);
    m_tchatEditor->setMinimumHeight(30);
    m_tchatEditor->setAcceptRichText(false);

    // Ajout des 2 zones dans le splitter
    m_splitter->addWidget(m_meetingRoom);
    m_splitter->addWidget(m_tchatEditor);

    // Initialisation des tailles des 2 widgets
    QList<int> tailles;
    tailles.append(200);
    tailles.append(40);
    m_splitter->setSizes(tailles);


    connect(m_tchatEditor, SIGNAL(onEntrey()), this, SLOT(emettreTexte()));
    connect(m_tchatEditor, SIGNAL(onArrowUp()), this, SLOT(monterHistorique()));
    connect(m_tchatEditor, SIGNAL(basPressee()), this, SLOT(descendreHistorique()));
    setWidget(m_splitter);
}


Tchat::~Tchat()
{

}



void Tchat::showMessage(QString utilisateur, QColor couleur, QString message)
{
        m_meetingRoom->moveCursor(QTextCursor::End);
        m_meetingRoom->setTextColor(couleur);
        m_meetingRoom->append(utilisateur + " : ");
        m_meetingRoom->setTextColor(Qt::black);
        m_meetingRoom->insertPlainText(message);
        m_meetingRoom->verticalScrollBar()->setSliderPosition(m_meetingRoom->verticalScrollBar()->maximum());
}
void Tchat::showDiceRoll(QString utilisateur, QColor couleur, QString message)
{
        m_meetingRoom->moveCursor(QTextCursor::End);
        m_meetingRoom->setTextColor(couleur);
        m_meetingRoom->append(utilisateur + " ");
        m_meetingRoom->setTextColor(Qt::black);
        m_meetingRoom->insertPlainText(message);
        m_meetingRoom->verticalScrollBar()->setSliderPosition(m_meetingRoom->verticalScrollBar()->maximum());
}
void Tchat::closeEvent(QCloseEvent *event)
{
	hide();
        event->ignore();
}
int Tchat::rollDices(QString message, QString *tirage, bool *ok)
{
	int i;
	bool stop;
	char operateur = '+';
	int result = 0;
	QString nombreStr;

	int nombre;

	QString facesStr;

	int faces;


	tirage->clear();

	message.remove(QChar(' '));

	message.remove(0, 1);

	int tailleMessage = message.size();

	if (!tailleMessage)
	{
		*ok = false;
		return 0;
	}


	while(tailleMessage)
	{
		stop = false;

		for (i=0; i<tailleMessage && !stop; i++)
			if (message[i].digitValue() == -1)
				stop = true;
		if (stop)
			i--;

		if (!i)
		{
			*ok = false;
			return 0;
		}

		nombreStr = message.left(i);

		nombre = nombreStr.toUInt(&stop, 10);

		message.remove(0, i);

		if (message[0] == QChar('d') || message[0] == QChar('D'))
		{

			message.remove(0, 1);

			tailleMessage = message.size();

			if (!tailleMessage)
			{
				*ok = false;
				return 0;
			}
			

			stop = false;

			for (i=0; i<tailleMessage && !stop; i++)
				if (message[i].digitValue() == -1)
					stop = true;
			if (stop)
				i--;

			if (!i)
			{
				*ok = false;
				return 0;
			}

			facesStr = message.left(i);

			faces = facesStr.toUInt(&stop, 10);

			message.remove(0, i);


			if (!nombre || !faces)
			{
				*ok = false;
				return 0;
			}


			tirage->append(nombreStr + QString("d") + facesStr + QString(" (") );

			int de;
			int resultatDes = 0;

			for (int u=0; u<nombre; u++)
			{

                                de = 1 + ((double)faces*rand())/((double)RAND_MAX+1);


				resultatDes += de;

				tirage->append(QString::number(de));

				if (u < nombre-1)
					tirage->append(QString(","));
				else
					tirage->append(QString(")"));
			}


			if (operateur == '+')
				result += resultatDes;
			else
				result -= resultatDes;
                }


		else
		{
			if (operateur == '+')
				result += nombre;
			else
				result -= nombre;
				

			tirage->append(nombreStr);
		}
		tailleMessage = message.size();
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
                        message.remove(0, 1);
			if (message.size() == 0)
			{
				*ok = false;
				return 0;
			}
		}
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
int Tchat::rollDicesSR4(QString message, QString *tirage, QString *glitch, bool *ok)
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
          de = 1 + (int)((double)rand() / ((double)RAND_MAX + 1) * (nbFaces+1)); //rand()%nbFaces + 1;
	  // Verification du succes
	  if (de >= seuilSucces)
	  {
    nbSucces++;
    //Si la regle des 6 again est activee, augmentation de la prochaine serie
    if (sixAgainActif && de == 6) 
      nbDesEnsuite++;       
  }
  // Verification du glitch (uniquement sur la premiere serie de lancer)
  if ((nbPasses = 1) && (de <= seuilGlitch))
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

void Tchat::getUpHistoric()
{
	// Ne s'applique que si la liste n'est pas vide
        if (m_messageHistoric.isEmpty())
		return;
	
	// C'est la 1ere fois que l'utilisateur appuie sur la touche : on memorise la ligne actuelle
        if (m_historicNumber == m_messageHistoric.size())
                m_messageHistoric.append(m_tchatEditor->toPlainText());

	// Si on a atteint le 1er message on arrete de remonter
        if (m_historicNumber)
                m_historicNumber--;
		
	// Affichage de l'ancien message dans la zone d'edition
        m_tchatEditor->clear();
        m_tchatEditor->append(m_messageHistoric[m_historicNumber]);
}

void Tchat::getDownHistoric()
{
	// Uniquement si on a deja appuye sur fleche haut
        if (m_historicNumber < m_messageHistoric.size() - 1)
	{
                m_historicNumber++;
		// Affichage du nouveau message dans la zone d'edition
                m_tchatEditor->clear();
                m_tchatEditor->append(m_messageHistoric[m_historicNumber]);
	}
}


void Tchat::saveFile(QString &file)
{

      /*  QTextDocument *document = m_meetingRoom->document();
	QString html = document->toHtml(QByteArray("UTF-8"));
	QTextStream fichier(&file);
        fichier << html;*/
}
void Tchat::openFile(QString& file)
{

}

void Tchat::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

        m_tchatEditor->setFocus(Qt::OtherFocusReason);
}


bool Tchat::defineMenu(QMenu* /*menu*/)
{
    return false;
}
