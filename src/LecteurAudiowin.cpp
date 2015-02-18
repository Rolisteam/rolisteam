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

#include "LecteurAudio.h"
#include "variablesGlobales.h"
#include "constantesGlobales.h"
#include "types.h"
#include "fmod.h"

#include "preferencesmanager.h"

/********************************************************************/
/* Variables globales utilisees par tous les elements de            */
/* l'application                                                    */
/********************************************************************/
// Pointeur vers l'unique instance du lecteur audio
    LecteurAudio * LecteurAudio::singleton = NULL;

/********************************************************************/
/* Fonction de callback hors classe appelee par FMOD a chaque fois  */
/* que la lecture d'un titre se termine. Le parametre userdata      */
/* pointe sur l'instance du lecteur qu'il faut mettre a l'arret     */
/********************************************************************/
signed char F_CALLBACKAPI finTitre(FSOUND_STREAM *stream, void *buff, int len, void *userdata)
{
    ((LecteurAudio *)userdata)->arriveeEnFinDeTitre();
    return true;
}

/********************************************************************/
/* Fonction de callback hors classe appelee par FMOD a chaque fois  */
/* que la lecture passe sur un TAG. Le parametre userdata pointe    */
/* sur l'instance du lecteur qu'il faut prevenir                    */
/********************************************************************/
signed char F_CALLBACKAPI passageTag(FSOUND_STREAM *stream, void *buff, int len, void *userdata)
{
    ((LecteurAudio *)userdata)->passageSurUnTag(QString((char *)buff));
    return true;
}

	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    LecteurAudio::LecteurAudio(QWidget *parent)
        : QDockWidget(parent)
    {
        m_options = PreferencesManager::getInstance();
		// Initialisation de la variable globale
		G_lecteurAudio = this;

		// Initialisation de l'etat du lecteur
		etatActuel = arret;
		enBoucle = false;
		lectureUnique = false;
		repriseDeLecture = false;
		listeChemins.clear();
		titreCourant = 0;
		fluxAudio = 0;

		// Titre du dockWidget
		setWindowTitle(tr("Musiques d'ambiance"));
		// Parametrage du dockWidget
        setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		setFeatures(QDockWidget::AllDockWidgetFeatures);
		// On fixe la largeur du dockWidget
		setMinimumWidth(255);
		
		// Creation du widget principal
		widgetPrincipal = new QWidget();
		// Creation du widget destine aux joueurs
		widgetAffichage = new QWidget();
		// Creation du widget destine aux MJ
		widgetCommande = new QWidget();
		// Creation du layout principal
		layoutPrincipal = new QVBoxLayout(widgetPrincipal);
		layoutPrincipal->setMargin(0);

		// Creation d'un widget separateur
		QWidget *separateur1 = new QWidget();
		separateur1->setFixedHeight(2);
		layoutPrincipal->addWidget(separateur1);

		// Ajout des widgets d'affichage et de commande
		layoutPrincipal->addWidget(widgetAffichage);
		layoutPrincipal->addWidget(widgetCommande);

		// Creation du layout d'affichage
		QHBoxLayout *layoutAffichage = new QHBoxLayout(widgetAffichage);
		layoutAffichage->setMargin(0);
		// Creation de l'afficheur de titres
		afficheurTitre = new QLineEdit();
		afficheurTitre->setReadOnly(true);
		// Ajout de l'afficheur de titres au widget d'affichage
		layoutAffichage->addWidget(afficheurTitre);
		
		// Si l'utilisateur local est un joueur on rajoute un bouton pour qu'il puisse choisir le repertoire
		// ou se trouve les musiques qui vont etre jouees
		if (G_joueur)
		{
			QAction *actionChangerDossier = new QAction(QIcon(":/icones/dossier.png"), tr("Choisir le répertoire où sont stockées les musiques"), widgetAffichage);
			QToolButton *boutonChangerDossier = new QToolButton(widgetAffichage);
			boutonChangerDossier->setDefaultAction(actionChangerDossier);
			boutonChangerDossier->setFixedSize(20, 20);
			// Ajout du bouton au widget d'affichage
			layoutAffichage->addWidget(boutonChangerDossier);
			// Connexion de l'action a l'ouverture d'un selecteur de repertoire
			QObject::connect(actionChangerDossier, SIGNAL(triggered(bool)), this, SLOT(joueurChangerDossier()));
		}

		// Creation du selecteur de volume
		niveauVolume = new QSlider(Qt::Horizontal);
		niveauVolume->setStyle(new QPlastiqueStyle());
		niveauVolume->setFixedWidth(50);
		niveauVolume->setToolTip(tr("Volume"));
		niveauVolume->setRange(0, 255);
		// Si la variable d'initialisation est utilisable, on initialise le volume
		if (G_initialisation.initialisee)
			niveauVolume->setValue(G_initialisation.niveauVolume);
		// Sinon on met une valeur par defaut
		else
			niveauVolume->setValue(100);
		
		// Ajout du selecteur de volume au widget d'affichage
		layoutAffichage->addWidget(niveauVolume);

		// Remplissage du widget de commande
		QVBoxLayout *layoutCommande = new QVBoxLayout(widgetCommande);
		layoutCommande->setMargin(0);
		// Creation du layout de temps
		QHBoxLayout *layoutTemps = new QHBoxLayout();
		layoutTemps->setMargin(0);
		// Creation du selecteur de temps
		positionTemps = new QSlider(Qt::Horizontal);
		positionTemps->setStyle(new QCleanlooksStyle());
		positionTemps->setRange(0, 0);
		// Creation de l'afficheur de temps
		afficheurTemps = new QLCDNumber();
		afficheurTemps->setNumDigits(5);
		afficheurTemps->setSegmentStyle(QLCDNumber::Flat);
		afficheurTemps->display("0:00");
		afficheurTemps->setFixedWidth(40);
		// Ajout des widgets dans le layout
		layoutTemps->addWidget(positionTemps);
		layoutTemps->addWidget(afficheurTemps);
		// Ajout du layout de temps au layout de commande
		layoutCommande->addLayout(layoutTemps);
		// Creation du layout des boutons
		QHBoxLayout *layoutBoutons = new QHBoxLayout();
		layoutBoutons->setMargin(0);
		layoutBoutons->setSpacing(0);
		// Creation des actions
		actionLecture 	= new QAction(QIcon("resources\\icones\\lecture.png"), tr("Lecture"), widgetCommande);
		actionPause 	= new QAction(QIcon("resources\\icones\\pause.png"), tr("Pause"), widgetCommande);
		actionStop	 	= new QAction(QIcon("resources\\icones\\stop.png"), tr("Stop"), widgetCommande);
		actionBoucle	= new QAction(QIcon("resources\\icones\\boucle.png"), tr("Lecture en boucle"), widgetCommande);
		actionUnique	= new QAction(QIcon("resources\i\cones\\lecture unique.png"), tr("Lecture unique"), widgetCommande);
		actionAjouter 	= new QAction(tr("Ajouter"), widgetCommande);
		actionSupprimer	= new QAction(tr("Supprimer"), widgetCommande);
		// Modification des toolTip
		actionAjouter->setToolTip(tr("Ajouter un titre à la liste"));
		actionSupprimer->setToolTip(tr("Supprimer le titre sélectionné"));
		// Les actions lecture, pause et boucle sont checkables
		actionLecture->setCheckable(true);
		actionPause->setCheckable(true);
		actionBoucle->setCheckable(true);
		actionUnique->setCheckable(true);
		// Creation des boutons
		QToolButton *boutonLecture		= new QToolButton(widgetCommande);
		QToolButton *boutonPause		= new QToolButton(widgetCommande);
		QToolButton *boutonStop			= new QToolButton(widgetCommande);
		QToolButton *boutonBoucle		= new QToolButton(widgetCommande);
		QToolButton *boutonUnique		= new QToolButton(widgetCommande);
		QToolButton *boutonAjouter		= new QToolButton(widgetCommande);
		QToolButton *boutonSupprimer	= new QToolButton(widgetCommande);
		// Association des actions aux boutons
		boutonLecture	->setDefaultAction(actionLecture);
		boutonPause		->setDefaultAction(actionPause);
		boutonStop		->setDefaultAction(actionStop);
		boutonBoucle	->setDefaultAction(actionBoucle);
		boutonUnique	->setDefaultAction(actionUnique);
		boutonAjouter	->setDefaultAction(actionAjouter);
		boutonSupprimer	->setDefaultAction(actionSupprimer);
		// Boutons de commande en mode AutoRaise, plus lisible
		boutonLecture	->setAutoRaise(true);
		boutonPause		->setAutoRaise(true);
		boutonStop		->setAutoRaise(true);
		boutonBoucle	->setAutoRaise(true);
		boutonUnique	->setAutoRaise(true);
		// On fixe la taille des boutons
		boutonLecture	->setFixedSize(20, 20);
		boutonPause		->setFixedSize(20, 20);
		boutonStop		->setFixedSize(20, 20);
		boutonBoucle	->setFixedSize(20, 20);
		boutonUnique	->setFixedSize(20, 20);
		boutonAjouter	->setFixedSize(70, 20);
		boutonSupprimer	->setFixedSize(70, 20);
		// Creation d'un widget separateur
		QWidget *separateur2 = new QWidget();
		// Ajout des boutons au layout
		layoutBoutons->addWidget(boutonLecture);
		layoutBoutons->addWidget(boutonPause);
		layoutBoutons->addWidget(boutonStop);
		layoutBoutons->addWidget(boutonBoucle);
		layoutBoutons->addWidget(boutonUnique);
		layoutBoutons->addWidget(separateur2);
		layoutBoutons->addWidget(boutonAjouter);
		layoutBoutons->addWidget(boutonSupprimer);
		// Ajout du layout des boutons au layout de commande
		layoutCommande->addLayout(layoutBoutons);
		// Creation de la liste de titres
		listeTitres = new QListWidget();
		listeTitres->setSelectionMode(QAbstractItemView::SingleSelection);
		// Ajout de la liste au widget de commande
		layoutCommande->addWidget(listeTitres);

		// Connexion des actions des boutons avec les fonctions appropriees
		QObject::connect(actionLecture, SIGNAL(triggered(bool)), this, SLOT(appuiLecture(bool)));
		QObject::connect(actionPause, SIGNAL(triggered(bool)), this, SLOT(appuiPause(bool)));
		QObject::connect(actionStop, SIGNAL(triggered(bool)), this, SLOT(appuiStop(bool)));
		QObject::connect(actionBoucle, SIGNAL(triggered(bool)), this, SLOT(appuiBoucle(bool)));
		QObject::connect(actionUnique, SIGNAL(triggered(bool)), this, SLOT(appuiUnique(bool)));
		QObject::connect(actionAjouter, SIGNAL(triggered(bool)), this, SLOT(ajouterTitre()));
		QObject::connect(actionSupprimer, SIGNAL(triggered(bool)), this, SLOT(supprimerTitre()));
		// Connexion des slider (volume et temps) avec les fonctions appropriees
		QObject::connect(niveauVolume, SIGNAL(sliderMoved(int)), this, SLOT(changementVolume(int)));
		QObject::connect(positionTemps, SIGNAL(sliderReleased()), this, SLOT(changementTempsLecture()));
		QObject::connect(positionTemps, SIGNAL(sliderMoved(int)), this, SLOT(changementTempsAffichage(int)));
		// Connexion du double-clic sur la liste vers le changement de titre en cours
		QObject::connect(listeTitres, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(changementTitre(QListWidgetItem *)));
		// Connexion du traitement de fin de lecture du titre
		QObject::connect(this, SIGNAL(finDeTitreSignal()), this, SLOT(finDeTitreSlot()));

		// Au demarrage seul le bouton Ajouter et le reglage du volume sont actifs
		actionLecture->setEnabled(false);
		actionPause->setEnabled(false);
		actionStop->setEnabled(false);
		actionBoucle->setEnabled(false);
		actionUnique->setEnabled(false);
		actionSupprimer->setEnabled(false);
		positionTemps->setEnabled(false);

		// On autorise ou pas l'affichage du tableau de commande selon la nature de l'utilisateur (MJ ou joueur)
		autoriserOuIntedireCommandes();

		// Insertion du widget principal dans le dockWidget
		setWidget(widgetPrincipal);
	}
LecteurAudio*  LecteurAudio::getInstance(QWidget *parent)
 {
        if(singleton==NULL)
            singleton = new LecteurAudio(parent);
        return singleton;
}
	/********************************************************************/
	/* Masque le widget de commande si l'utilisateur local n'est pas MJ */
	/********************************************************************/
	void LecteurAudio::autoriserOuIntedireCommandes()
	{
		// Si l'utilisateur local est un joueur
		if (G_joueur)
		{
			// Initialement il n'y a aucun titre dans le lecteur
			afficheurTitre->setToolTip(tr("Aucun titre"));
			// On masque le widget de commande
			widgetCommande->hide();
			// On ajoute un widget de separation
			QWidget *separateur3 = new QWidget();
			separateur3->setFixedHeight(2);
			layoutPrincipal->addWidget(separateur3);
			// On fixe la hauteur du dockWidget
			setFixedHeight(66);
		}
	}

	/********************************************************************/
	/* L'utilisateur vient d'appuyer sur Pause                          */
	/********************************************************************/
	void LecteurAudio::appuiPause(bool etatBouton)
	{
		// Le bouton vient d'etre enfonce
		if (etatBouton)
		{
			// Le lecture etait en train de lire un titre : on met en pause
			if (etatActuel == lecture)
			{
				// On met en pause les lecteurs des joueurs
				emettreCommande(pauseMorceau);
				// On passe l'etat en pause
				etatActuel = pause;
				// On relache le bouton Lecture
				actionLecture->setChecked(false);
				// On met la lecture en pause
				FSOUND_SetPaused(canalAudio, true);
			}
			
			// Dans tous les autres etats, on relache le bouton de pause
			else
				actionPause->setChecked(false);
		}

		// Le bouton vient d'etre relache
		else
		{
			// Le lecture etait en pause : on reprend la lecture
			if (etatActuel == pause)
			{
				// On remet en lecture les lecteurs des joueurs
				emettreCommande(lectureMorceau);
				// Nouvel etat : lecture
				etatActuel = lecture;
				// On enfonce le bouton Lecture
				actionLecture->setChecked(true);
				// On reprend la lecture du titre
				FSOUND_SetPaused(canalAudio, false);
			}

			// Dans les autres cas il y a un probleme
			else
			{
				qWarning("Le bouton pause etait enfonce alors que le lecteur n'etait pas en pause (appuiPause - LecteurAudio.cpp)");
				return;
			}
		}
	}

	/********************************************************************/
	/* L'utilisateur vient d'appuyer sur Stop                           */
	/********************************************************************/
	void LecteurAudio::appuiStop(bool etatBouton)
	{
		// On arrete les lecteurs des joueurs
		emettreCommande(arretMorceau);
		// On met le lecteur a l'arret
		arreter();
		// On arrete la lecture et on revient au debut du titre
		FSOUND_Stream_Stop(fluxAudio);
	}
	
	/********************************************************************/
	/* L'utilisateur vient d'appuyer sur Lecture                        */
	/********************************************************************/
	void LecteurAudio::appuiLecture(bool etatBouton)
	{
		// Le bouton vient d'etre enfonce
		if (etatBouton)
		{
			// On (re)met en lecture les lecteurs des joueurs
			emettreCommande(lectureMorceau);
			// L'etat passe a Lecture
			etatActuel = lecture;
			// Si le bouton pause etait enfonce il s'agit d'une reprise de pause
			if (actionPause->isChecked())
			{
				// On relache le bouton pause
				actionPause->setChecked(false);
				// On reprend la lecture du titre
				FSOUND_SetPaused(canalAudio, false);
			}

			// Sinon il s'agit d'une premiere lecture
			else
			{
				// On rend inoperant le passage sur un tag : sans cela un tag peut deplacer le curseur de temps entre
				// le lancement de la lecture et le repositionnement dans le temps
				repriseDeLecture = true;
				// On initialise le canal audio en commencant la lecture
				canalAudio = FSOUND_Stream_Play(FSOUND_FREE, fluxAudio);
				// On se positionne a l'endroit choisi par l'utilisateur
				FSOUND_Stream_SetTime(fluxAudio, positionTemps->value());
				// On regle le volume
				FSOUND_SetVolume(canalAudio, niveauVolume->value());
				// Le passage sur un tag est a nouveau operationnel
				repriseDeLecture = false;
			}
		}

		// Le bouton vient d'etre relache
		else
		{
			// Le lecteur etait dans l'etat Lecture
			if (etatActuel == lecture)
			{
				// On met en pause les lecteurs des joueurs
				emettreCommande(pauseMorceau);
				// On met le lecteur en Pause
				etatActuel = pause;
				// On enfonce le bouton pause
				actionPause->setChecked(true);
				// On met la lecture en pause
				FSOUND_SetPaused(canalAudio, true);
			}
			
			// Dans les autres cas il y a un probleme
			else
			{
				qWarning("Le bouton lecture etait enfonce alors que le lecteur n'etait pas en en train de lire (appuiLecture - LecteurAudio.cpp)");
				return;
			}
		}
	}

	/********************************************************************/
	/* L'utilisateur vient d'enfoncer ou relacher le bouton de lecture  */
	/* en boucle                                                        */
	/********************************************************************/
	void LecteurAudio::appuiBoucle(bool etatBouton)
	{
		// On lit en boucle
		enBoucle = etatBouton;

		// Si le bouton a ete enfonce
		if (etatBouton)
		{
			// On relache le bouton lecture unique
			actionUnique->setChecked(false);
			// On ne lit plus en lecture unique
			lectureUnique = false;
		}
	}

	/********************************************************************/
	/* L'utilisateur vient d'enfoncer ou relacher le bouton de lecture  */
	/* unique (apres la lecture, le lecteur s'arrete et ne passe pas au */
	/* titre suivant)                                                   */
	/********************************************************************/
	void LecteurAudio::appuiUnique(bool etatBouton)
	{
		lectureUnique = etatBouton;

		// Si le bouton a ete enfonce
		if (etatBouton)
		{
			// On relache le bouton lecture en boucle
			actionBoucle->setChecked(false);
			// On ne lit plus en boucle
			enBoucle = false;
		}
	}

	/********************************************************************/
	/* Demande a l'utilisateur de selectionner un ou plusieurs titres a */
	/* ajouter a la liste                                               */
	/********************************************************************/
	void LecteurAudio::ajouterTitre()
	{
		// Ouverture d'un selecteur de fichier, et recuperation de la liste des fichiers selectionnes
        QStringList listeFichiers = QFileDialog::getOpenFileNames(this, tr("Ajouter un titre"), m_options->value("MusicDirectory").toString(), tr("Fichiers audio (*.wav *.mp2 *.mp3 *.ogg)"));

		// Si l'utilisateur a clique sur "Annuler" on quitte
		if (listeFichiers.isEmpty())
			return;

		// On met a jour le chemin vers les musiques
		int dernierSlash = listeFichiers[0].lastIndexOf("/");
        m_options->registerValue("MusicDirectory",listeFichiers[0].left(dernierSlash));
        //G_dossierMusiquesMj = ;listeFichiers[0].left(dernierSlash)


		// Tant qu'il y a un element dans la liste, on l'ajoute a la liste des titres
		while (!listeFichiers.isEmpty())
		{
			// On recupere le 1er fichier de la liste
			QString fichier = listeFichiers.takeFirst();
			// On extrait le titre du chemin complet
			int dernierSlash = fichier.lastIndexOf("/");
			QString titre = fichier.right(fichier.length()-dernierSlash-1);
	
			// Si la liste etait vide (elle a maintenant 1 element) : le nouveau titre devient le titre actif
			if (listeChemins.isEmpty())
			{
				// On envoie le nouveau titre aux lecteurs des joueurs
				emettreCommande(nouveauMorceau, titre);
	
				// On active tous les boutons
				actionLecture->setEnabled(true);
				actionPause->setEnabled(true);
				actionStop->setEnabled(true);
				actionBoucle->setEnabled(true);
				actionUnique->setEnabled(true);
				actionSupprimer->setEnabled(true);
				positionTemps->setEnabled(true);
	
				// Le titre en cours est le numero 0 dans la liste
				titreCourant = 0;
				// Le nouveau morceau est "introduit" dans le lecteur
				nouveauTitre(titre, fichier);
			}
			
			// On ajoute le morceau a la liste
			QListWidgetItem *morceau = new QListWidgetItem(titre, listeTitres);
			morceau->setToolTip(fichier);
			// On ajoute le chemin complet a la liste des chemins
			listeChemins.append(fichier);
		}
	}
	
	/********************************************************************/
	/* Supprime le titre selectionne de la liste                        */
	/********************************************************************/
	void LecteurAudio::supprimerTitre()
	{
		// On recupere la liste des element selectionnes (il ne peut y en avoir qu'un seul au maximum)
		QList<QListWidgetItem *> titreSelectionne = listeTitres->selectedItems();
		
		// Si aucun titre n'est selectionne, on quitte la fonction
		if (titreSelectionne.isEmpty())
			return;
		
		// On recupere le numero de la ligne dans la liste des titres
		int ligne = listeTitres->row(titreSelectionne[0]);

		// On supprime l'element de la liste des titres et des chemins
		listeTitres->takeItem(ligne);
		listeChemins.takeAt(ligne);

		// On regarde si le titre supprime etait le titre en cours
		if (ligne == titreCourant)
		{
			// On ferme le titre qui etait dans le lecteur
			if (!FSOUND_Stream_Close(fluxAudio))
			{
				qWarning("Probleme a la fermeture d'un flux audio (supprimerTitre - LecteurAudio.cpp)");
				return;
			}
			// On met le flux audio a NULL
			fluxAudio = 0;

			// On regarde s'il existe un titre apres celui qui vient d'etre supprime
			// Si oui, il devient le nouveau titre en cours
			if (titreCourant < listeChemins.size())
			{
				// On envoie le nouveau titre aux lecteurs des joueurs
				emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
				// Insertion du nouveau titre dans le lecteur
				nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);

			}
				
			// Sinon on regarde s'il y a un titre avant : celui-ci devient le titre en cours
			else if (titreCourant != 0)
			{
				titreCourant--;
				// On envoie le nouveau titre aux lecteurs des joueurs
				emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
				// Insertion du nouveau titre dans le lecteur
				nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);
			}
			
			// Sinon, on vient de supprimer le dernier element : on arrete le lecteur et on decoche les boutons
			else
			{
				// On envoie un titre vide aux lecteurs des joueurs pour qu'ils s'arretent
				emettreCommande(nouveauMorceau, "");
				// On arrete le lecteur
				arreter();
				// L'afficheur de titre est nettoye
				afficheurTitre->clear();
				afficheurTitre->setToolTip("");
				// Le temps est mis a 0
				positionTemps->setRange(0, 0);
				// Les boutons sont desactives
				actionLecture->setEnabled(false);
				actionPause->setEnabled(false);
				actionStop->setEnabled(false);
				actionBoucle->setEnabled(false);
				actionUnique->setEnabled(false);
				actionSupprimer->setEnabled(false);
				positionTemps->setEnabled(false);
			}
		}
		
		// Si le titre supprime se trouvait avant le titre courant, celui-ci remonte d'une place
		else if (ligne < titreCourant)
			titreCourant--;
	}

	/********************************************************************/
	/* L'utilisateur vient de deplacer le curseur du volume : on ajuste */
	/* niveau sonore                                                    */
	/********************************************************************/
	void LecteurAudio::changementVolume(int valeur)
	{
		// Si le lecteur est en lecture ou en pause on regle le volume
		if (etatActuel == lecture || etatActuel == pause)
			FSOUND_SetVolume(canalAudio, valeur);
	}

	/********************************************************************/
	/* L'utilisateur vient de deplacer le curseur du temps et de le     */
	/* relacher : on lit a partir de la nouvelle position               */
	/********************************************************************/
	void LecteurAudio::changementTempsLecture()
	{
		// On envoie la nouvelle position aux lecteurs des joueurs
		emettreCommande(nouvellePositionMorceau, "", positionTemps->value());

		// Si le lecteur est en lecture ou en pause on le positionne a l'endroit selectionne
		if (etatActuel == lecture || etatActuel == pause)
			FSOUND_Stream_SetTime(fluxAudio, positionTemps->value());
	}

	/********************************************************************/
	/* L'utilisateur vient de deplacer le curseur du temps : on met a   */
	/* jour l'affichage du temps                                        */
	/********************************************************************/
	void LecteurAudio::changementTempsAffichage(int valeur)
	{
		// On calcule le nbr de minutes
		int minutes = valeur / 60000;
		// On calcule le nbr de secondes
		int secondes = (valeur % 60000) / 1000;
		// Conversion des minutes et des secondes en QString
		QString minutesString = QString::number(minutes);
		QString secondesString = QString::number(secondes);
		// Ajout d'un eventuel 0 devant les secondes
		secondesString = (secondesString.size()==1?"0":"") + secondesString;
		
		// Mise a jour de l'affichage
		afficheurTemps->display(minutesString + ":" + secondesString);
	}

	/********************************************************************/
	/* Le titre sur lequel l'utilisateur vient de double-cliquer        */
	/* devient le titre courant                                         */
	/********************************************************************/
	void LecteurAudio::changementTitre(QListWidgetItem *morceau)
	{
		// On met a jour le numero de titre courant
		titreCourant = listeTitres->row(morceau);
		// On envoie le nouveau titre aux lecteurs des joueurs
		emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
		// Changement de titre en cours
		nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);
	}

	/********************************************************************/
	/* Le fichier passe en parametre est ouvert et le lecteur est       */
	/* prepare a sa lecture                                             */
	/********************************************************************/
	void LecteurAudio::nouveauTitre(QString titre, QString fichier)
	{
		// Si un fichier etait en cours de lecture, on le ferme
		if (fluxAudio)
		{
			// Si la fermeture s'est mal passee on affiche un message
			if (!FSOUND_Stream_Close(fluxAudio))
			{
				qWarning("Probleme a la fermeture d'un flux audio (nouveauTitre - LecteurAudio.cpp)");
				return;
			}
		}

		// On affiche le titre du nouveau morceau
		afficheurTitre->setText(titre);
		afficheurTitre->setCursorPosition(0);
		afficheurTitre->setToolTip(fichier);

		// Ouverture du nouveau fichier
		fluxAudio = FSOUND_Stream_Open(fichier.toLatin1().data(), FSOUND_NORMAL, 0, 0);
		if (!fluxAudio)
		{
			qWarning("Impossible d'ouvrir le fichier audio (nouveauTitre - LecteurAudio.cpp)");
			return;
		}

		// Mise en place de la fonction de callback appelee en fin de lecture
		if (!FSOUND_Stream_SetEndCallback(fluxAudio, finTitre, this))
		{
			qWarning("Impossible d'initialiser la fonction de callback de fin de lecture (nouveauTitre - LecteurAudio.cpp)");
			return;
		}

		// Mise en place de la fonction de callback appelee a chaque passage sur un TAG
		if (!FSOUND_Stream_SetSyncCallback(fluxAudio, passageTag, this))
		{
			qWarning("Impossible d'initialiser la fonction de callback de passage sur un TAG (nouveauTitre - LecteurAudio.cpp)");
			return;
		}

		// On ajoute tous les tag indicateurs de temps, a chaque seconde du morceau
		ajouterTags();
		// On recupere la duree totale du fichier
		int dureeMs = FSOUND_Stream_GetLengthMs(fluxAudio);
		// Initialisation du selecteur de temps
		positionTemps->setMaximum(dureeMs);

		// On met le lecteur a l'arret
		arreter();
	}

	/********************************************************************/
	/* Met le lecteur a l'arret                                         */
	/********************************************************************/
	void LecteurAudio::arreter()
	{
		// On relache les boutons lecture et pause
		actionPause->setChecked(false);
		actionLecture->setChecked(false);
		// On met le lecteur a l'arret
		etatActuel = arret;
		// On remet la barre de temps au depart
		positionTemps->setValue(0);
		positionTemps->update();
		// On met l'afficheur de temps a 0
		afficheurTemps->display("0:00");
	}

	/********************************************************************/
	/* La lecture du titre vient de toucher a sa fin : soit on passe au */
	/* titre suivant (s'il existe), soit on boucle sur le meme          */
	/********************************************************************/
	void LecteurAudio::arriveeEnFinDeTitre()
	{
		// Traitement de la fin de lecture en asynchrone, par le biais d'un signal : cela evite de surcharger la fonction de callback
		// qui doit etre breve par nature. Dans la pratique si tout le traitement a lieu dans la fonction de callback il y a un probleme
		// lors de l'emission des donnees; cette technique resoud le probleme
		emit finDeTitreSignal();
	}
	
	/********************************************************************/
	/* Fonction appelee de facon asynchrone par arriveeEnFinDeTitre     */
	/* pour traiter la fin de lecture en dehors de la fonction de       */
	/* callback                                                         */
	/********************************************************************/
	void LecteurAudio::finDeTitreSlot()
	{
		// Si la lecture en boucle est activee
		if (enBoucle)
		{
			// On arrete les lecteurs des joueurs
			emettreCommande(arretMorceau);
			// On remet en lecture les lecteurs des joueurs
			emettreCommande(lectureMorceau);
			// On initialise le canal audio en commencant la lecture
			canalAudio = FSOUND_Stream_Play(FSOUND_FREE, fluxAudio);
			// On regle le volume
			FSOUND_SetVolume(canalAudio, niveauVolume->value());
			// On remet la barre de temps au depart
			positionTemps->setValue(0);
			positionTemps->update();
			// On met l'afficheur de temps a 0
			afficheurTemps->display("0:00");
		}

		// Si la lecture unique est activee
		else if (lectureUnique)
		{
			// On arrete les lecteurs des joueurs
			emettreCommande(arretMorceau);
			// On arrete le lecteur local
			arreter();
		}

		// Sinon on passe au titre suivant
		else
		{
			// Il existe un titre suivant
			if (titreCourant < listeChemins.size()-1)
			{
				// On passe au titre suivant
				titreCourant++;
				// On envoie le nouveau titre aux lecteurs des joueurs
				emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
				// On demande aux lecteurs des joueurs de commencer a le lire
				emettreCommande(lectureMorceau);
				// Changement de titre en cours
				nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);
				// On met le lecteur en lecture
				etatActuel = lecture;
				// On initialise le canal audio en commencant la lecture
				canalAudio = FSOUND_Stream_Play(FSOUND_FREE, fluxAudio);
				// On regle le volume
				FSOUND_SetVolume(canalAudio, niveauVolume->value());
				// On enfonce le bouton Lecture
				actionLecture->setChecked(true);
			}
			
			// Il n'existe pas de titre suivant : on arrete le lecteur
			else
			{
				// On emet une demande d'arret aux autres utilisateurs
				emettreCommande(arretMorceau);
				// On arrete le lecteur
				arreter();
			}
		}
	}

	/********************************************************************/
	/* Ajoute un tag a chaque seconde du morceau, contenant le temps    */
	/* ecoule depuis le debut de celui-ci                               */
	/********************************************************************/
	void LecteurAudio::ajouterTags()
	{
		// Indice servant a parcourir le morceau seconde par seconde
		int secondes = 1;
		// Minutes et secondes utilisees pour l'affichage (deduis de l'indice "secondes")
		QString minutesString;
		QString secondesString;
		// String contenant la chaine de caractere finale
		QString stringTag;
		// On recupere la longueur du morceau (en ms)
		int longueurMorceau = FSOUND_Stream_GetLengthMs(fluxAudio);
		
		// On avance de seconde en seconde
		while(secondes*1000 < longueurMorceau)
		{
			minutesString = QString::number((int)(secondes / 60));
			secondesString = QString::number(secondes % 60);
			// Ajout d'un eventuel 0 devant les secondes
			secondesString = (secondesString.size()==1?"0":"") + secondesString;
			// Creation de la chaine finale
			stringTag = minutesString + ":" + secondesString;
			// Ajout du tag a l'endroit correspondant
			if (!FSOUND_Stream_AddSyncPoint(fluxAudio, secondes * FREQUENCE_AUDIO, stringTag.toLatin1().data()))
				qWarning ("Impossible de creer un point de synchronisation (ajouterTags - LecteurAudio.cpp)");
			// On incremente les secondes
			secondes++;
		}
	}

	/********************************************************************/
	/* Appelee par la fonction hors classe passageTag. Le parametre tag */
	/* contient le temps qu'il faut afficher dans l'afficheur de temps  */
	/********************************************************************/
	void LecteurAudio::passageSurUnTag(QString tag)
	{
		// Si l'utilisateur n'est pas en train de deplacer le curseur de temps
		if (!positionTemps->isSliderDown() && !repriseDeLecture)
		{
			// On affiche le tag (qui contient la position de la lecture) sur lequel le lecteur vient de passer
			afficheurTemps->display(tag);
			// On deplace le curseur pour qu'il corresponde au temps actuel
			positionTemps->setValue(FSOUND_Stream_GetTime(fluxAudio));
			positionTemps->update();
		}
	}

	/********************************************************************/
	/* Emet une commande pour le lecteur audio vers le serveur ou       */
	/* les clients                                                      */
	/********************************************************************/
	void LecteurAudio::emettreCommande(actionMusique action, QString nomFichier, quint32 position, int numeroLiaison)
	{
		int p;
		quint16 tailleNomFichier;
		
		// Donnees a emettre
		char *donnees;

		// Creation de l'entete du message
		enteteMessage uneEntete;
		uneEntete.categorie = musique;
		uneEntete.action = action;

		// Remplissage de l'entete et du corps en fonction de la commande envoyee
		switch(action)
		{
			case nouveauMorceau :
				uneEntete.tailleDonnees = sizeof(quint16) + nomFichier.size()*sizeof(QChar);
				donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
				// Ajout du nom du fichier dans le message
				p = sizeof(enteteMessage);
				tailleNomFichier = nomFichier.size();
				memcpy(&(donnees[p]), &tailleNomFichier, sizeof(quint16));
				p+=sizeof(quint16);
				memcpy(&(donnees[p]), nomFichier.data(), tailleNomFichier*sizeof(QChar));
				break;
			
			case lectureMorceau :
				uneEntete.tailleDonnees = 0;
				donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
				break;
			
			case pauseMorceau :
				uneEntete.tailleDonnees = 0;
				donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
				break;
			
			case arretMorceau :
				uneEntete.tailleDonnees = 0;
				donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
				break;
			
			case nouvellePositionMorceau :
				uneEntete.tailleDonnees = sizeof(quint32);
				donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
				// Ajout de la position dans le message
				memcpy(&(donnees[sizeof(enteteMessage)]), &position, sizeof(quint32));
				break;
			
			default :
				qWarning("Commande inconnue envoyee aux lecteurs audio des autres utilisateurs");
				break;
		}

		// Recopie de l'entete en debut de message
		memcpy(donnees, &uneEntete, sizeof(enteteMessage));

		// Emission du message vers le serveur ou les clients...
		if (numeroLiaison == -1)
			emettre(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage));

		// ...ou bien vers un client en particulier
		else
			emettre(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage), numeroLiaison);
		
		// Liberation du buffer d'emission
		delete[] donnees;
	}

	/********************************************************************/
	/* Emet une serie de commandes vers l'utilisateur dont l'ID est     */
	/* passe en parametre, de sorte que son lecteur audio se retrouve   */
	/* dans le meme etat que le lecteur local                           */
	/********************************************************************/
	void LecteurAudio::emettreEtat(QString idJoueur)
	{
		// On recupere le numero de liaison correspondant a l'identifiant du joueur
		// (on soustrait 1 car le 1er utilisateur est toujours le serveur et qu'il
		// n'a pas de liaison associee)
		int numeroLiaison = G_listeUtilisateurs->numeroUtilisateur(idJoueur) - 1;
		
		// S'il n'y a pas de titre dans le lecteur (on regarde s'il y a qq chose dans la zone d'affichage, car la condition peut
		// etre remplie par le MJ et les joueurs)
		if (afficheurTitre->text().isEmpty())
		{
			// On emet un nouveau titre vide et on quitte la fonction
			emettreCommande(nouveauMorceau, "", 0, numeroLiaison);
			return;
		}

		// Dans le cas contraire on emet le titre en cours
		emettreCommande(nouveauMorceau, afficheurTitre->text(), 0, numeroLiaison);

		// Puis on emet les commandes necessaires en fonction de l'etat actuel du lecteur
		switch(etatActuel)
		{
			// Dans le cas de la pause on releve le point de lecture actuel et on l'envoie
			case pause :
				emettreCommande(nouvellePositionMorceau, "", FSOUND_Stream_GetTime(fluxAudio), numeroLiaison);
				break;

			// A l'arret on emet le position du curseur de temps
			case arret :
				emettreCommande(nouvellePositionMorceau, "", G_joueur?joueurPositionTemps:positionTemps->value(), numeroLiaison);
				break;
			
			// Pour la lecture on emet la position actuelle et l'ordre de lecture
			case lecture :
				emettreCommande(nouvellePositionMorceau, "", FSOUND_Stream_GetTime(fluxAudio), numeroLiaison);
				emettreCommande(lectureMorceau, "", 0, numeroLiaison);
				break;
			
			default :
				qWarning("Etat du lecteur audio inconnu lors de l'emission de l'etat (emettreEtat - LecteurAudio.cpp)");
				break;
		}
	}

	/********************************************************************/
	/* Retourne le volume courant                                       */
	/********************************************************************/
	int LecteurAudio::volume()
	{
		return niveauVolume->value();
	}

	/********************************************************************/
	/* Les fonctions qui suivent ne concernent que les joueurs. La      */
	/* gestion du lecteur pour les joueurs ne tient pas compte du       */
	/* widget de commande (qui n'est pas accessible). Toutes les        */
	/* fonctions relatives aux joueurs commencent par "joueur".         */
	/********************************************************************/

	/********************************************************************/
	/* Joueur seulement - ouvre le fichier passe en parametre, et       */
	/* affiche son nom dans l'afficheur de titre                        */
	/********************************************************************/
	void LecteurAudio::joueurNouveauFichier(QString nomFichier)
	{
		// Si un fichier etait en cours de lecture, on le ferme
		if (fluxAudio)
		{
			// Si la fermeture s'est mal passee on affiche un message
			if (!FSOUND_Stream_Close(fluxAudio))
			{
				qWarning("Probleme a la fermeture d'un flux audio (joueurNouveauFichier - LecteurAudio.cpp)");
				return;
			}
		}

		// Si le nom de fichier est vide, il faut arreter le lecteur
		if (nomFichier.isEmpty())
		{
			// On efface l'afficheur de titre
			afficheurTitre->clear();
			afficheurTitre->setToolTip(tr("Aucun titre"));
			// On met le lecteur a l'arret
			etatActuel = arret;
			// On sort de la fonction
			return;
		}

		// Creation du chemin complet du fichier
		QString chemin(G_dossierMusiquesJoueur + "/" + nomFichier);
		
		// Ouverture du nouveau fichier
		fluxAudio = FSOUND_Stream_Open(chemin.toLatin1().data(), FSOUND_NORMAL, 0, 0);

		// Si l'ouverture s'est mal passee on affiche un message
		if (!fluxAudio)
		{
			qWarning("Impossible d'ouvrir le fichier audio (joueurNouveauFichier - LecteurAudio.cpp)");
			// On affiche le message en clair
			afficheurTitre->setEchoMode(QLineEdit::Normal);
			// Changement de la couleur du texte en rouge
			QPalette palette(afficheurTitre->palette());
			palette.setColor(QPalette::Normal, QPalette::Text, Qt::red);
			afficheurTitre->setPalette(palette);
			// On affiche le titre du nouveau morceau en rouge (indique que le fichier n'est pas present ou impossible a ouvrir)
			afficheurTitre->setText(nomFichier + tr(" : fichier introuvable ou impossible à ouvrir"));
			afficheurTitre->setCursorPosition(0);
			afficheurTitre->setToolTip(tr("Fichier introuvable ou impossible à ouvrir : ") + chemin);
			// On quitte la fonction
			return;
		}

		// Si l'ouverture du fichier s'est bien passee on ecrit en noir avec des asterisques
		else
		{
			// On masque le titre
			afficheurTitre->setEchoMode(QLineEdit::Password);
			// On ecrit en noir
			QPalette palette(afficheurTitre->palette());
			palette.setColor(QPalette::Normal, QPalette::Text, Qt::black);
			afficheurTitre->setPalette(palette);
		}

		// On affiche le titre du nouveau morceau
		afficheurTitre->setText(nomFichier);
		afficheurTitre->setCursorPosition(0);
		afficheurTitre->setToolTip(tr("Titre masqué"));

		// On met le lecteur a l'arret
		etatActuel = arret;
		// La lecture reprend depuis le debut
		joueurPositionTemps = 0;
	}

	/********************************************************************/
	/* Joueur seulement - commence ou reprend la lecture du fichier     */
	/* prealablement ouvert avec joueurNouveauFichier                   */
	/********************************************************************/
	void LecteurAudio::joueurLectureMorceau()
	{
		// Si aucun fichier audio n'est actuellement ouvert, on quitte la fonction
		if (!fluxAudio)
		{
			qWarning("Demande de lecture alors qu'aucun fichier n'est ouvert (joueurLectureMorceau - LecteurAudio.cpp)");
			return;
		}

		// Si le lecteur etait a l'arret
		if (etatActuel == arret)
		{
			// L'etat passe a Lecture
			etatActuel = lecture;
			// On initialise le canal audio en commencant la lecture
			canalAudio = FSOUND_Stream_Play(FSOUND_FREE, fluxAudio);
			// On se positionne a l'endroit indique par le MJ
			FSOUND_Stream_SetTime(fluxAudio, joueurPositionTemps);
			// On regle le volume
			FSOUND_SetVolume(canalAudio, niveauVolume->value());
		}
		
		// Si le lecteur etait en pause
		else if (etatActuel == pause)
		{
			// L'etat passe a Lecture
			etatActuel = lecture;
			// On reprend la lecture du titre
			FSOUND_SetPaused(canalAudio, false);
		}
		
		// Dans tous les autres cas il s'agit d'une erreur
		else
			qWarning("Demande de lecture d'un titre alors que le lecteur n'est ni en pause, ni a l'arret (joueurLectureMorceau - LecteurAudio.cpp)");
	}

	/********************************************************************/
	/* Joueur seulement - met la lecture en pause                       */
	/********************************************************************/
	void LecteurAudio::joueurPauseMorceau()
	{
		// Si aucun fichier audio n'est actuellement ouvert, on quitte la fonction
		if (!fluxAudio)
		{
			qWarning("Demande de mise en pause alors qu'aucun fichier n'est ouvert (joueurPauseMorceau - LecteurAudio.cpp)");
			return;
		}

		// Si le lecteur etait en train de lire
		if (etatActuel == lecture)
		{
			// On met le lecteur en Pause
			etatActuel = pause;
			// On met la lecture en pause
			FSOUND_SetPaused(canalAudio, true);
		}
		
		// Dans tous les autres cas il s'agit d'une erreur
		else
			qWarning("Demande de mis en pause du lecteur alors qu'il n'etait pas en lecture (joueurPauseMorceau - LecteurAudio.cpp)");
	}
		
	/********************************************************************/
	/* Joueur seulement - arrete la lecture du morceau                  */
	/********************************************************************/
	void LecteurAudio::joueurArretMorceau()
	{
		// Si aucun fichier audio n'est actuellement ouvert, on quitte la fonction
		if (!fluxAudio)
		{
			qWarning("Demande d'arret de lecture alors qu'aucun fichier n'est ouvert (joueurArretMorceau - LecteurAudio.cpp)");
			return;
		}
		
		// On met le lecteur a l'arret
		etatActuel = arret;
		// On arrete la lecture et on revient au debut du titre
		FSOUND_Stream_Stop(fluxAudio);
		// La lecture reprend depuis le debut
		joueurPositionTemps = 0;
	}

	/********************************************************************/
	/* Joueur seulement - deplace la lecture du fichier a l'emplacement */
	/* choisi par le MJ                                                 */
	/********************************************************************/
	void LecteurAudio::joueurChangerPosition(int position)
	{
		// Si aucun fichier audio n'est actuellement ouvert, on quitte la fonction
		if (!fluxAudio)
		{
			qWarning("Demande de changement de position de lecture alors qu'aucun fichier n'est ouvert (joueurChangerPosition - LecteurAudio.cpp)");
			return;
		}
		// On change la position de debut de lecture
		joueurPositionTemps = position;
		// Si le lecteur est en lecture ou en pause on le positionne a l'endroit selectionne
		if (etatActuel == lecture || etatActuel == pause)
			FSOUND_Stream_SetTime(fluxAudio, joueurPositionTemps);
	}

	/********************************************************************/
	/* Joueur seulement - permet au joueur de selectionner le dossier   */
	/* dans lequel le lecteur devra chercher les musiques a lire        */
	/********************************************************************/
	void LecteurAudio::joueurChangerDossier()
	{
		// On met a jour le dossier des musiques du joueur
		G_dossierMusiquesJoueur = QFileDialog::getExistingDirectory(0 , tr("Choix du répertoire des musiques"), G_dossierMusiquesJoueur,
			QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	}

