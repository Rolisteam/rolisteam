/***************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                               *
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


#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <QWidget>
#include <QDockWidget>
#include <QLineEdit>
#include <QSlider>
#include <QAction>
#include <QLCDNumber>
#include <QListView>
#include <QList>
#include <QString>
#include <QVBoxLayout>
#include <QMutex>
#include <QMediaPlayer>
#include <QEvent>

#include "types.h"
#include "preferencesmanager.h"
#include "musicmodel.h"
#include "playerwidget.h"
#include "network/networkreceiver.h"
class Liaison;
/**
    * @brief This player can be used by the GM to play songs.
    * Regular players can just change the volume level.
    */
class AudioPlayer : public QDockWidget, public NetWorkReceiver
{
    Q_OBJECT

public :


    ~AudioPlayer();
    /**
    * @brief return a pointer to the unique audio player. Sigleton pattern
    */
    static AudioPlayer*  getInstance(QWidget *parent = 0);

    void pplay();
    void ppause();
    //void pstop();
    void pselectNewFile(QString file);
    void pseek(quint32 position);

    //QMediaContent* setSource(QModelIndex p);

    void updateUi();

    virtual void processMessage(NetworkMessageReader* msg);


public slots:
    void pstop();





    void onePlayerHasStopped(int);
    void onePlayerIsPaused(int);
    void onePlayerPlays(int,quint64);
    void onePlayerHasNewSong(int,QString);
    void onePlayerHasChangedPosition(int,quint64);


protected:
    void contextMenuEvent(QContextMenuEvent* ev);

private :
        /**
        * @brief private constructor
        */
    AudioPlayer(QWidget *parent = 0);


    QString getDirectoryKey();
    void playerWidget();

        /**
        * @brief send command to a client
        */
    void sendCommand(actionMusique action, QString nomFichier = "", quint64 position = 0, Liaison * link = NULL);


    static AudioPlayer* m_singleton;//!< @brief static pointer to the unique instance of this audioplayer


        /**
        * @brief set the UI - Phonon only
        */
    void setupUi();
    qint64 m_time;//!< @brief current time



    QWidget* m_mainWidget;        //!< @brief brings together all subwidget
    QVBoxLayout* m_mainLayout;


    QActionGroup* m_playingMode;
    QAction* m_loopAction;            //!< @brief loop playing action
    QAction* m_uniqueAction;            //!< @brief one song playing mode action
    QAction* m_addAction;            //!< @brief add song action
    QAction* m_deleteAction;        //!< @brief remove song action


    PlayerWidget* m_mainPlayer;
    PlayerWidget* m_secondPlayer;
    PlayerWidget* m_thirdPlayer;

    QList<PlayerWidget*> m_players;
    QList<QAction*> m_playerActionsList;


    PreferencesManager* m_preferences;
    QMutex m_mutex;
private slots :
    /**
    * @brief received the time
    */
    //void tick(qint64 time);

    /**
    * @brief statusChanged called when state has been changed
    */
    void statusChanged(QMediaPlayer::MediaStatus status);

    /**
    * @brief called when the audio source has been changed
    */
    void playerStatusChanged(QMediaPlayer::State state);


    /**
    * @brief Send some informations to the given player
    */
    void updatePlayingMode();

    /**
    * @brief slot which manage the click on playing
    */
    void onfinished();


    /**
    * @brief  slot which is called when song is finished
    */
//    void isAboutToFinish();

    /**
    * @brief  slot which manage the player's root directory change
    */
    void pChangeDirectory();

    /**
    * @brief Send some informations to the given player
    */
 //   void emettreEtat(Liaison * link);
    /**
    * @brief called when selection on list has changed
    */
    void selectionHasChanged();

    void emitCurrentState();

    void showMusicPlayer(bool);
};

#endif
