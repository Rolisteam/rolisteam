/***************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                  *
 *    Copyright (C) 2009 by Renaud Guezennec                             *
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


#ifdef PHONON
//#include <phonon>
#include <mediasource.h>
#include <seekslider.h>
#include <mediaobject.h>
#include <volumeslider.h>
#include <audiooutput.h>
#include <path.h>
#endif

class Liaison;
    
class LecteurAudio : public QDockWidget
{
    Q_OBJECT

public :
    enum PlayingMode { LOOP, UNIQUE , NEXT };
        /**
        * @brief provides the current volume level
        */
    qreal volume();

    /**
    * @brief return a pointer to the unique audio player. Sigleton pattern
    */
    static LecteurAudio*  getInstance(QWidget *parent = 0);

    void pplay();
    void ppause();
    void pstop();
    void pselectNewFile(QString file);
    void pseek(quint32 position);

signals :


private :
        /**
        * @brief private constructor
        */
    LecteurAudio(QWidget *parent = 0);

    void updateUi();

    void playerWidget();


        /**
        * @brief send command to a client
        */
    void emettreCommande(actionMusique action, QString nomFichier = "", quint64 position = 0, Liaison * link = NULL);


    static LecteurAudio* m_singleton;//!< @brief static pointer to the unique instance of this audioplayer


        /**
        * @brief set the UI - Phonon only
        */
    void setupUi();
    qint64 m_time;//!< @brief current time
    Phonon::MediaSource *m_currentSource;//!< @brief current audio source
    Phonon::SeekSlider *seekSlider; //!< @brief Allows to seek in the song (Phonon only)
    Phonon::MediaObject *m_mediaObject; //!<  (Phonon only)
    Phonon::AudioOutput *audioOutput; //!< (Phonon only)
    Phonon::VolumeSlider *volumeSlider; //!< @brief Allows to adjust the sound volume (Phonon only)
    Phonon::Path* path; //!< (Phonon only)
    Phonon::VolumeSlider *niveauVolume;//!< @brief Allows to adjust the sound volume (Phonon only)
    Phonon::SeekSlider *positionTemps;//!< @brief Allows to seek in the song (Phonon only)

    QWidget *widgetPrincipal;        //!< @brief brings together all subwidget
    QWidget *widgetAffichage;        //!< @brief Displays some gauges (for Player and GM.)
    QWidget *widgetCommande;        //!< @brief Displays the control panel (GM only)
    QVBoxLayout *layoutPrincipal;    //!< @brief layout
    QLineEdit *m_titleDisplay;        //!< @brief Displays the title of the played song

    QString m_currentFile;
    QListWidgetItem* m_currentItemFile;
    QListWidgetItem* m_formerItemFile;


    QLCDNumber *afficheurTemps;        //!< @brief displays the past time of the playing
    QListWidget *m_songList;        //!< @brief displays all avaliable songs
    QList<QString> m_pathList;            //!< @brief Path list
    QActionGroup* m_playingMode;
    QAction *m_playAction;            //!< @brief Play action
    QAction *m_pauseAction;            //!< @brief Pause action
    QAction *m_stopAction;            //!< @brief Stop action
    QAction *m_loopAction;            //!< @brief loop playing action
    QAction *m_uniqueAction;            //!< @brief one song playing mode action
    QAction *m_addAction;            //!< @brief add song action
    QAction *m_deleteAction;        //!< @brief remove song action


    PlayingMode m_currentPlayingMode;
    bool m_endFile;



private slots :
    /**
    * @brief received the time
    */
    void tick(qint64 time);

    /**
    * @brief called when state has been changed
    */
    void stateChanged(Phonon::State newState, Phonon::State oldState);

    /**
    * @brief called when the audio source has been changed
    */
    void sourceChanged(const Phonon::MediaSource &source);
    /**
    * @brief Send some informations to the given player
    */
    void updatePlayingMode();

    /**
    * @brief slot which manage the click on playing
    */
    void onfinished();

    /**
    * @brief  slot which manage the click on add song button
    */
    void clickOnList(QListWidgetItem * p);
    /**
    * @brief  slot which manage the click on remove song button
    */
    void removeFile();
    /**
    * @brief  slot which manage the click on remove song button
    */
    void addFiles();

    /**
    * @brief  slot which is called when song is finished
    */
    void isAboutToFinish();

    /**
    * @brief  slot which manage the player's root directory change
    */
    void pChangeDirectory();

    /**
    * @brief Send some informations to the given player
    */
    void emettreEtat(Liaison * link);
};

#endif
