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

//#include "types.h"



#include <phonon/MediaObject>
#include <phonon/MediaSource>
#include <phonon/Path>
#include <phonon/SeekSlider>
#include <phonon/VolumeSlider>
#include <phonon/AudioOutput>



class PreferencesManager;
/**
* @brief This player can be used by the GM to play songs.
* Regular players can just change the volume level.
*/
class AudioPlayer : public QDockWidget
{
    Q_OBJECT

public :

	/**
	* @brief Send some informations to the given player
	*/
        void emitState(QString idJoueur);


	/**
	* @brief Players only, displays and plays the given filename
	*/
        //void playerNewFile(QString nomFichier);

	/**
	* @brief Players only, start or re-start the play of the current song
	*/
        //void playerPlaying();

	/**
	* @brief Players only, Pause the playing
	*/
        //void playerPause();

	/**
	* @brief Players only, Stop the playing
	*/
        //void stop();

	/**
	* @brief Players only, Change position of the song
	*/
        void seek(int position);

	/**
	* @brief provides the current volume level
	*/
        qreal volume();

	/**
	* @brief add a new file in the list
	*/
        void addSong(QString titre, QString fichier);
	
	/**
	* @brief return a pointer to the unique audio player. Sigleton pattern
	*/
        static AudioPlayer*  getInstance(QWidget *parent = 0);

signals :

	/**
	* @brief emitted when current song is finished
	*/
        void songFinished();

        void changeVisibility(bool);

protected:
             void closeEvent ( QCloseEvent * event );

private :

	/**
	* @brief private constructor
	*/
        AudioPlayer(QWidget *parent = 0);

	/**
        * @brief enum playerState
	*/
        enum PlayerState {PAUSE, STOP, PLAYING};



	
	
	
        


        static AudioPlayer* singleton;//!< @brief static pointer to the unique instance of this audioplayer

	/**
        * @brief set the UI
	*/
        void setupUi();

	/**
	* @brief event filter to catch all signal emitted by phonon classes
	*/
	bool eventFilter(QObject *object, QEvent *event);

        qint64 m_time;//!< @brief current time
        Phonon::MediaSource* m_currentsource;//!< @brief current audio source
        Phonon::SeekSlider* m_seekSlider; //!< @brief Allows to seek in the song (Phonon only)
        Phonon::MediaObject* m_mediaObject; //!<  (Phonon only)
        Phonon::AudioOutput* m_audioOutput; //!< (Phonon only)
        Phonon::VolumeSlider* m_volumeSlider; //!< @brief Allows to adjust the sound volume (Phonon only)
        Phonon::Path* m_path; //!< (Phonon only)
        Phonon::VolumeSlider* m_volumeLevel;//!< @brief Allows to adjust the sound volume (Phonon only)
        Phonon::SeekSlider* m_timeSlider;//!< @brief Allows to seek in the song (Phonon only)

        QWidget* m_mainWidget;		//!< @brief brings together all subwidget
        QWidget* m_displayWidget;		//!< @brief Displays some gauges (for Player and GM.)
        QWidget* m_commandWidget;		//!< @brief Displays the control panel (GM only)
        QVBoxLayout* m_mainLayout;	//!< @brief layout
        QLineEdit * m_informationScreen;	//!< @brief Displays the title of the played song



        QLCDNumber * m_timeCounter;		//!< @brief displays the past time of the playing
        QListWidget *m_songListWidget;		//!< @brief displays all avaliable songs 
        QList<QString> m_pathList;	//!< @brief Path list 
        QAction *m_playAct;			//!< @brief Play action
        QAction *m_pauseAct;			//!< @brief Pause action 
        QAction *m_stopAct;			//!< @brief Stop action 
        QAction *m_repeatAct;			//!< @brief loop playing action 
        QAction *m_oneperoneAct;			//!< @brief one song playing mode action
        QAction *m_addAct;			//!< @brief add song action
        QAction *m_removeAct;		//!< @brief remove song action

        /**
          * pointer to the unique instance of preference manager.
          */
        PreferencesManager* m_options;


        PlayerState m_currentState;				//!< @brief current state of the player
        int m_currentSong;	//!< @brief index of the current song (must die)
        int m_audioChannel;		//!< @brief index for fmod (must die)
        bool m_repeated;		//!< @brief bool true if the playing is in loop mode. otherwise false.(must die)
        bool m_oneperone;     //!< @brief bool true if the playing is in only one song mode. Otherwise false. (must die)
        bool m_restartPlaying;			//!< @brief bool (must die)
        int playerTimePosition;//!< @brief int starting time for players only
        
        


private slots :
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
      * @brief Phonon only - called when user press the play button.
      */
       void pressPlay(bool state);
	/**
	* @brief  slot which setis/unsets loop playing
	*/
        void onRepeated(bool etatBouton);

	/**
	* @brief  slot which sets/unsets only one song playing mode
	*/
        void onOnePerOne(bool etatBouton);

	/**
	* @brief  slot which manages the click on add song button
	*/
        void addSong();


	/**
	* @brief  slot which manages the click on remove song button
	*/
        void removeSong();

	/**
	* @brief  slot which manages the tilte change
	*/
        void changeSong(QListWidgetItem * p);

	/**
	* @brief  slot which is called when song is finished
	*/
        void finishedSongSlot();

	/**
	* @brief  slot which manages the player's root directory change
	*/
        void changeSongDirectory();



};

#endif
