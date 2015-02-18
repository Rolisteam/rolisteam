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


/**!
* @brief
* This player can be used by the GM to play song.   
* Regular players can just change the volume level.                  			                           
*/


#ifndef LECTEUR_AUDIO_H
#define LECTEUR_AUDIO_H

#include <QWidget>
#include <QDockWidget>
#include <QLineEdit>
#include <QSlider>
#include <QAction>
#include <QLCDNumber>
#include <QListWidget>
#include <QList>
#include <QString>
#include <QVBoxLayout>

#include "types.h"
#ifdef FMOD
#include "fmod.h"
#endif

#ifdef PHONON
//#include <phonon>
#include <mediasource.h>
#include <seekslider.h>
#include <mediaobject.h>
#include <volumeslider.h>
#include <audiooutput.h>
#include <path.h>
#endif


	
class LecteurAudio : public QDockWidget
{
    Q_OBJECT

public :
    enum PlayingMode { LOOP, UNIQUE , NEXT };
	/**
    * @brief Not used with phonon - used for emit signal when the song is almost finished
	*/
        void arriveeEnFinDeTitre();
#ifdef FMOD
	/**
    * @brief Not used with phonon - Called by global stuff to define the time displayed by the LCDNumber
	*/
        void passageSurUnTag(QString tag);
#endif
	/**
    * @brief Send some informations to the given player
	*/
        void emettreEtat(int numeroLiaison);


	/**
    * @brief Hide the control panel if the current user is not the GM
	*/
        void autoriserOuIntedireCommandes();

	/**
    * @brief Players only, displays and plays the given filename
	*/
        void joueurNouveauFichier(QString nomFichier);

	/**
    * @brief Players only, start or re-start the play of the current song
	*/
        void joueurLectureMorceau();

	/**
    * @brief Players only, Pause the playing
	*/
        void joueurPauseMorceau();

	/**
    * @brief Players only, Stop the playing
	*/
        void joueurArretMorceau();

	/**
    * @brief Players only, Change position of the song
	*/
        void joueurChangerPosition(int position);

	/**
    * @brief provides the current volume level
	*/
        qreal volume();

	/**
    * @brief return a pointer to the unique audio player. Sigleton pattern
	*/
        static LecteurAudio*  getInstance(QWidget *parent = 0);

signals :
	/**
    * @brief emitted when current song is finished
	*/
        void finDeTitreSignal();

private :

	/**
    * @brief private constructor
	*/
        LecteurAudio(QWidget *parent = 0);

	/**
    * @brief enum playingState
	*/
        enum etatLecteur {pause, arret, lecture};

    void updateUi();
	/**
    * @brief add a new file in the list
	*/
        void nouveauTitre(QString titre, QString fichier);

	/**
    * @brief stop the playing
	*/
        void arreter();
#ifdef FMOD
	/**
    * @brief Fmod only
	*/
	void ajouterTags();
#endif
	/**
    * @brief send command to a client
	*/
        void emettreCommande(actionMusique action, QString nomFichier = "", quint64 position = 0, int numeroLiaison = -1);


        static LecteurAudio* singleton;//!< @brief static pointer to the unique instance of this audioplayer

#ifdef PHONON
	/**
    * @brief set the UI - Phonon only
	*/
        void setupUi();


        qint64 m_time;//!< @brief current time
        Phonon::MediaSource *currentsource;//!< @brief current audio source
        Phonon::SeekSlider *seekSlider; //!< @brief Allows to seek in the song (Phonon only)
        Phonon::MediaObject *mediaObject; //!<  (Phonon only)
        Phonon::AudioOutput *audioOutput; //!< (Phonon only)
        Phonon::VolumeSlider *volumeSlider; //!< @brief Allows to adjust the sound volume (Phonon only)
        Phonon::Path* path; //!< (Phonon only)
        Phonon::VolumeSlider *niveauVolume;//!< @brief Allows to adjust the sound volume (Phonon only)
        Phonon::SeekSlider *positionTemps;//!< @brief Allows to seek in the song (Phonon only)
#endif

#ifdef FMOD
        FSOUND_STREAM *fluxAudio; //!< @brief fmod pointer
#endif
        QWidget *widgetPrincipal;		//!< @brief brings together all subwidget
        QWidget *widgetAffichage;		//!< @brief Displays some gauges (for Player and GM.)
        QWidget *widgetCommande;		//!< @brief Displays the control panel (GM only)
        QVBoxLayout *layoutPrincipal;	//!< @brief layout
        QLineEdit *afficheurTitre;		//!< @brief Displays the title of the played song

#ifdef FMOD
        QSlider *niveauVolume;			//!< @brief Allows to adjust the sound volume
        QSlider *positionTemps;//!< @brief Allows to seek in the song
#endif


        QLCDNumber *afficheurTemps;		//!< @brief displays the past time of the playing
        QListWidget *listeTitres;		//!< @brief displays all avaliable songs
        QList<QString> listeChemins;	//!< @brief Path list
        QActionGroup* m_playingMode;
        QAction *actionLecture;			//!< @brief Play action
        QAction *actionPause;			//!< @brief Pause action
        QAction *actionStop;			//!< @brief Stop action
        QAction *actionBoucle;			//!< @brief loop playing action
        QAction *actionUnique;			//!< @brief one song playing mode action
        QAction *actionAjouter;			//!< @brief add song action
        QAction *actionSupprimer;		//!< @brief remove song action
        int etatActuel;				//!< @brief current state of the player
        int titreCourant;	//!< @brief index of the current song (must die)
        int canalAudio;		//!< @brief index for fmod (must die)

        PlayingMode m_currentPlayingMode;
        //bool enBoucle;		//!< @brief bool true if the playing is in loop mode. otherwise false.(must die)
        //bool lectureUnique;     //!< @brief bool true if the playing is in only one song mode. Otherwise false. (must die)

#ifdef FMOD
        bool repriseDeLecture;			//!< @brief bool (must die)
#endif
        int joueurPositionTemps;//!< @brief int starting time for players only


private slots :
#ifdef PHONON
	/**
    * @brief Phonon only - received the time
	*/
        void tick(qint64 time);

	/**
    * @brief Phonon only - called when state has been changed
	*/
        void stateChanged(Phonon::State newState, Phonon::State oldState);

	/**
    * @brief Phonon only - called when the audio source has been changed
	*/
        void sourceChanged(const Phonon::MediaSource &source);
        /**
        * @brief Send some informations to the given player
        */
        void updatePlayingMode();
#endif
#ifdef FMOD
	/**
    * @brief Fmod only - slot which manage the click on pauseButton
	*/
        void appuiPause(bool etatBouton);

	/**
    * @brief Fmod only - slot which manage the click on stopButton
	*/
        void appuiStop(bool etatBouton);

	/**
    * @brief Fmod only - slot which manage the click on playing
	*/
        void appuiLecture(bool etatBouton);

	/**
    * @brief Fmod only - slot which manage the volume  change
	*/
        void changementVolume(int valeur);

	/**
    * @brief Fmod only - slot which manage seeking
	*/
        void changementTempsLecture();

	/**
    * @brief Fmod only - slot which manage seeking
	*/
        void changementTempsAffichage(int valeur);

#endif
	/**
    * @brief  slot which set/unset loop playing
	*/
        void appuiBoucle(bool etatBouton);

	/**
    * @brief  slot which set/unset only one song playing mode
	*/
        void appuiUnique(bool etatBouton);

	/**
    * @brief  slot which manage the click on add song button
	*/
        void ajouterTitre();


	/**
    * @brief  slot which manage the click on remove song button
	*/
        void supprimerTitre();

	/**
    * @brief  slot which manage the tilte change
	*/
        void changementTitre(QListWidgetItem * p);

	/**
    * @brief  slot which is called when song is finished
	*/
        void finDeTitreSlot();

	/**
    * @brief  slot which manage the player's root directory change
	*/
        void joueurChangerDossier();



};

#endif
