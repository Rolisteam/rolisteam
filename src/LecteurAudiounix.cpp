/*
Rolistik - logiciel collaboratif d'aide aux jeux de roles en ligne
Copyright (C) 2007 - Romain Campioni  Tous droits rservs.

Ce programme est un logiciel libre ; vous pouvez le redistribuer ou le
modifier suivant les termes de la GNU General Public License telle que
publie par la Free Software Foundation : soit la version 2 de cette
licence, soit ( votre gr) toute version ultrieure.

Ce programme est distribué dans l'espoir qu'il vous sera utile, mais SANS
AUCUNE GARANTIE : sans même la garantie implicite de COMMERCIALISABILIT
ni dADQUATION  UN OBJECTIF PARTICULIER. Consultez la Licence Gnrale
Publique GNU pour plus de dtails.

Vous devriez avoir reu une copie de la Licence Gnrale Publique GNU avec
ce programme ; si ce nest pas le cas, consultez :
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>

Par ailleurs ce logiciel est gratuit et ne peut en aucun cas tre
commercialis, conformment  la "FMOD Non-Commercial License".
*/


#include <QtGui>

#include "LecteurAudio.h"
#include "variablesGlobales.h"
#include "constantesGlobales.h"
#include "types.h"



// Pointeur vers l'unique instance du lecteur audio
LecteurAudio * LecteurAudio::singleton = NULL;



/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/
LecteurAudio::LecteurAudio(QWidget *parent)
: QDockWidget(parent)
{

    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    path = new Phonon::Path();

    m_time = 0;
     connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
     this, SLOT(stateChanged(Phonon::State, Phonon::State)));

connect(mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
     this, SLOT(sourceChanged(const Phonon::MediaSource &)));

connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(finDeTitreSlot()));
        *path = Phonon::createPath(mediaObject, audioOutput);
setupUi();

        autoriserOuIntedireCommandes();
        setWidget(widgetPrincipal);
    }




LecteurAudio*  LecteurAudio::getInstance(QWidget *parent)
 {
        if(singleton==NULL)
            singleton = new LecteurAudio(parent);
        return singleton;
}
void LecteurAudio::setupUi()
{
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
                QAction *actionChangerDossier = new QAction(QIcon(":/resources/icones/dossier.png"), tr("Choisir le répertoire où sont stockées les musiques"), widgetAffichage);
                QToolButton *boutonChangerDossier = new QToolButton(widgetAffichage);
                boutonChangerDossier->setDefaultAction(actionChangerDossier);
                boutonChangerDossier->setFixedSize(20, 20);
                // Ajout du bouton au widget d'affichage
                layoutAffichage->addWidget(boutonChangerDossier);
                // Connexion de l'action a l'ouverture d'un selecteur de repertoire
                QObject::connect(actionChangerDossier, SIGNAL(triggered(bool)), this, SLOT(joueurChangerDossier()));
        }

        // Creation du selecteur de volume
        niveauVolume = new Phonon::VolumeSlider(this);
        niveauVolume->setAudioOutput(audioOutput);
        niveauVolume->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        // Si la variable d'initialisation est utilisable, on initialise le volume


        // Ajout du selecteur de volume au widget d'affichage
        layoutAffichage->addWidget(niveauVolume);

        // Remplissage du widget de commande
        QVBoxLayout *layoutCommande = new QVBoxLayout(widgetCommande);
        layoutCommande->setMargin(0);
        // Creation du layout de temps
        QHBoxLayout *layoutTemps = new QHBoxLayout();
        layoutTemps->setMargin(0);
        // Creation du selecteur de temps
        positionTemps = new Phonon::SeekSlider(this);
        positionTemps->setMediaObject(mediaObject);
        positionTemps->setStyle(new QCleanlooksStyle());

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
       // actionLecture 	= new QAction(QIcon(":/resources/icones/lecture.png"), tr("Lecture"), widgetCommande);
        //actionPause 	= new QAction(QIcon(":/resources/icones/pause.png"), tr("Pause"), widgetCommande);
        //actionStop	 	= new QAction(QIcon(":/resources/icones/stop.png"), tr("Stop"), widgetCommande);
        actionBoucle	= new QAction(QIcon(":/resources/icones/boucle.png"), tr("Lecture en boucle"), widgetCommande);
        actionUnique	= new QAction(QIcon(":/resources/icones/lecture unique.png"), tr("Lecture unique"), widgetCommande);
        actionAjouter 	= new QAction(tr("Ajouter"), widgetCommande);
        actionSupprimer	= new QAction(tr("Supprimer"), widgetCommande);


        actionLecture = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Lecture"), this);
        actionLecture->setShortcut(Qt::Key_Space);
        actionLecture->setDisabled(true);
        actionPause = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
        actionPause->setDisabled(true);
        actionStop = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
        actionStop->setDisabled(true);



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


        connect(actionLecture, SIGNAL(triggered()), mediaObject, SLOT(play()));
        connect(actionLecture, SIGNAL(triggered(bool)), this, SLOT(appuiLecture(bool)));

        connect(actionPause, SIGNAL(triggered()), mediaObject, SLOT(pause()));
        connect(actionStop, SIGNAL(triggered()), mediaObject, SLOT(stop()));


        QObject::connect(actionBoucle, SIGNAL(triggered(bool)), this, SLOT(appuiBoucle(bool)));
        QObject::connect(actionUnique, SIGNAL(triggered(bool)), this, SLOT(appuiUnique(bool)));
        QObject::connect(actionAjouter, SIGNAL(triggered(bool)), this, SLOT(ajouterTitre()));
        QObject::connect(actionSupprimer, SIGNAL(triggered(bool)), this, SLOT(supprimerTitre()));

        QObject::connect(positionTemps, SIGNAL(sliderReleased()), this, SLOT(changementTempsLecture()));
        QObject::connect(positionTemps, SIGNAL(sliderMoved(int)), this, SLOT(changementTempsAffichage(int)));

        QObject::connect(listeTitres, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(changementTitre(QListWidgetItem *)));

        QObject::connect(this, SIGNAL(finDeTitreSignal()), this, SLOT(finDeTitreSlot()));

        // Au demarrage seul le bouton Ajouter et le reglage du volume sont actifs
        actionLecture->setEnabled(false);
        actionPause->setEnabled(false);
        actionStop->setEnabled(false);
        actionBoucle->setEnabled(false);
        actionUnique->setEnabled(false);
        actionSupprimer->setEnabled(false);
        positionTemps->setEnabled(false);

}
/********************************************************************************/
/* Fonction appelée quand l'utilisateur doubleclique sur un élément de la liste */
/********************************************************************************/
void LecteurAudio::changementTitre(QListWidgetItem * p)
{
             currentsource = new Phonon::MediaSource(listeChemins[listeTitres->row(p)]);
             mediaObject->setCurrentSource(*currentsource);
             titreCourant = listeTitres->row(p);
             emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
             mediaObject->play();




}
/***************************************************************************/
/* Fonction de notification pour la barre d'avancement du temps du lecteur */
/***************************************************************************/
void LecteurAudio::tick(qint64 time)
{
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

            if((!G_joueur) && ((time>m_time+(2*mediaObject->tickInterval()))||(time<m_time)))
                    emettreCommande(nouvellePositionMorceau, "", time, -1);


        m_time = time;
        afficheurTemps->display(displayTime.toString("mm:ss"));
        //  out << mediaObject->totalTime() << " " << time << endl;
       /* if(mediaObject->totalTime()==time)
                next();*/
}
/***************************************************************************/
/* Fonction éxécuté à chaque changement de fichier                         */
/***************************************************************************/
void LecteurAudio::sourceChanged(const Phonon::MediaSource &source)
{


     afficheurTitre->setText(source.fileName().right(source.fileName().length()-source.fileName().lastIndexOf("/")-1));
     afficheurTemps->display("00:00");
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



/********************************************************************************/
/* Not used                                                                      */
/*                                                                              */
/********************************************************************************/
 bool LecteurAudio::eventFilter(QObject *object, QEvent *event)
 {
     return false;
 }
void LecteurAudio::stateChanged(Phonon::State newState, Phonon::State /*oldState*/)
{

     switch (newState) {
         case Phonon::ErrorState:

             break;
         case Phonon::PlayingState:
                 actionLecture->setEnabled(false);
                 actionPause->setEnabled(true);
                 actionStop->setEnabled(true);
                  emettreCommande(lectureMorceau);
                 break;
         case Phonon::StoppedState:
                 emettreCommande(arretMorceau);
                 arreter();
                 actionStop->setEnabled(false);
                 actionLecture->setEnabled(true);
                 actionPause->setEnabled(false);
                 afficheurTemps->display("00:00");
                 break;
         case Phonon::PausedState:
                 emettreCommande(pauseMorceau);
                 actionStop->setEnabled(true);
                 etatActuel = pause;
                 actionLecture->setEnabled(true);
                 actionPause->setEnabled(true);
                 break;
         case Phonon::BufferingState:
                 break;
         default:
              break;
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
        QStringList listeFichiers = QFileDialog::getOpenFileNames(this, tr("Ajouter un titre"), G_dossierMusiquesMj, tr("Fichiers audio (*.wav *.mp2 *.mp3 *.ogg)"));

        // Si l'utilisateur a clique sur "Annuler" on quitte
        if (listeFichiers.isEmpty())
                return;

        // On met a jour le chemin vers les musiques
        int dernierSlash = listeFichiers[0].lastIndexOf("/");
        G_dossierMusiquesMj = listeFichiers[0].left(dernierSlash);


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

        QList<QListWidgetItem *> titreSelectionne = listeTitres->selectedItems();

        if (titreSelectionne.isEmpty())
                return;


        int ligne = listeTitres->row(titreSelectionne[0]);


        listeTitres->takeItem(ligne);
        listeChemins.takeAt(ligne);


        if (ligne == titreCourant)
        {


                if (titreCourant < listeChemins.size())
                {

                        emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());

                        nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);

                }


                else if (titreCourant != 0)
                {
                        titreCourant--;

                        emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());

                        nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);
                }

                else
                {

                        emettreCommande(nouveauMorceau, "");

                        arreter();
                        afficheurTitre->clear();
                        afficheurTitre->setToolTip("");

                        actionLecture->setEnabled(false);
                        actionPause->setEnabled(false);
                        actionStop->setEnabled(false);
                        actionBoucle->setEnabled(false);
                        actionUnique->setEnabled(false);
                        actionSupprimer->setEnabled(false);
                        positionTemps->setEnabled(false);
                }
        }


        else if (ligne < titreCourant)
                titreCourant--;
}



/********************************************************************/
/* Le fichier passe en parametre est ouvert et le lecteur est       */
/* prepare a sa lecture                                             */
/********************************************************************/
void LecteurAudio::nouveauTitre(QString titre, QString fichier)
{

        QTextStream cout(stderr,QIODevice::WriteOnly);

        // On affiche le titre du nouveau morceau
        afficheurTitre->setText(titre);
        afficheurTitre->setCursorPosition(0);
        afficheurTitre->setToolTip(fichier);

        // Ouverture du nouveau fichier
        cout << fichier << endl;
        currentsource = new Phonon::MediaSource(fichier);
        mediaObject->setCurrentSource(*currentsource);

}

/********************************************************************/
/* Met le lecteur a l'arret                                         */
/********************************************************************/
void LecteurAudio::arreter()
{
        // On relache les boutons lecture et pause
        actionPause->setEnabled(false);
        actionLecture->setEnabled(false);
        // On met le lecteur a l'arret
        etatActuel = arret;

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
        //emit finDeTitreSignal();
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

                mediaObject->enqueue(mediaObject->currentSource());

                // On met l'afficheur de temps a 0

        }

        // Si la lecture unique est activee
        else if (lectureUnique)
        {
                // On arrete les lecteurs des joueurs
                emettreCommande(arretMorceau);
                // On arrete le lecteur local

        }

        // Sinon on passe au titre suivant
        else
        {
                // Il existe un titre suivant
                if (titreCourant < listeChemins.size()-1)
                {

                        titreCourant++;

                        emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());

                        emettreCommande(lectureMorceau);



                        etatActuel = lecture;
                        currentsource = new Phonon::MediaSource(listeChemins[titreCourant]);
                        mediaObject->enqueue(*currentsource);

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
//		int longueurMorceau = FSOUND_Stream_GetLengthMs(fluxAudio);
        int longueurMorceau = 0;
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
//			if (!FSOUND_Stream_AddSyncPoint(fluxAudio, secondes * FREQUENCE_AUDIO, stringTag.toLatin1().data()))
                //	qWarning ("Impossible de creer un point de synchronisation (ajouterTags - LecteurAudio.cpp)");
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
/*		if (!positionTemps->isSliderDown() && !repriseDeLecture)
        {
                // On affiche le tag (qui contient la position de la lecture) sur lequel le lecteur vient de passer
                afficheurTemps->display(tag);
                // On deplace le curseur pour qu'il corresponde au temps actuel
//			positionTemps->setValue(FSOUND_Stream_GetTime(fluxAudio));
                positionTemps->update();
        }*/
}

/********************************************************************/
/* Emet une commande pour le lecteur audio vers le serveur ou       */
/* les clients                                                      */
/********************************************************************/
void LecteurAudio::emettreCommande(actionMusique action, QString nomFichier, quint64 position, int numeroLiaison)
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
                                emettreCommande(nouvellePositionMorceau, "", m_time, numeroLiaison);
                        break;

                // A l'arret on emet le position du curseur de temps
                case arret :
                                 emettreCommande(nouvellePositionMorceau, "", G_joueur?joueurPositionTemps:m_time, numeroLiaison);
                        break;

                // Pour la lecture on emet la position actuelle et l'ordre de lecture
                case lecture :
                            emettreCommande(nouvellePositionMorceau, "", m_time, numeroLiaison);
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
qreal LecteurAudio::volume()
{
//		return niveauVolume->value();
        return audioOutput->volume();
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
        QFileInfo fileInfo(chemin);
       /* QTextStream cout(stderr,QIODevice::WriteOnly);
        cout << "joueur "<<chemin << endl;*/
        // Si l'ouverture s'est mal passee on affiche un message
        if (!fileInfo.exists())
        {
                qWarning("Impossible d'ouvrir le fichier audio (joueurNouveauFichier - LecteurAudio.cpp)");
                // On affiche le message en clair
                afficheurTitre->setEchoMode(QLineEdit::Normal);
                // Changement de la couleur du texte en rouge
                QPalette palette(afficheurTitre->palette());
                palette.setColor(QPalette::Normal, QPalette::Text, Qt::red);
                afficheurTitre->setPalette(palette);
                // On affiche le titre du nouveau morceau en rouge (indique que le fichier n'est pas present ou impossible a ouvrir)
                afficheurTitre->setText(nomFichier + tr(" : fichier introuvable ou impossible  ouvrir"));
                afficheurTitre->setCursorPosition(0);
                afficheurTitre->setToolTip(tr("Fichier introuvable ou impossible  ouvrir : ") + chemin);
                // On quitte la fonction
                return;
        }

        // Si l'ouverture du fichier s'est bien passee on ecrit en noir avec des asterisques
        else
        {
                currentsource = new Phonon::MediaSource(chemin);
                mediaObject->setCurrentSource(*currentsource);
                mediaObject->play();
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
    if((mediaObject->state()==Phonon::PausedState)||(mediaObject->state()==Phonon::StoppedState))
    {
        mediaObject->play();
        etatActuel = lecture;


    }
}

/********************************************************************/
/* Joueur seulement - met la lecture en pause                       */
/********************************************************************/
void LecteurAudio::joueurPauseMorceau()
{
    if(mediaObject->state()==Phonon::PlayingState)
    {
        mediaObject->pause();
        etatActuel = pause;


    }
}

/********************************************************************/
/* Joueur seulement - arrete la lecture du morceau                  */
/********************************************************************/
void LecteurAudio::joueurArretMorceau()
{

if(mediaObject->state()==Phonon::PlayingState)
    {
        mediaObject->stop();
        etatActuel = arret;

        joueurPositionTemps = 0;
    }
}

/********************************************************************/
/* Joueur seulement - deplace la lecture du fichier a l'emplacement */
/* choisi par le MJ                                                 */
/********************************************************************/
void LecteurAudio::joueurChangerPosition(int position)
{
    if(mediaObject->isSeekable())
            mediaObject->seek(position);
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

