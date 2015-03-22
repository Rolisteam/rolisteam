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
class NetworkLink;
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
    /**
     * @brief updateUi
     */
    void updateUi();

    /**
     * @brief processMessage
     * @param msg
     */
    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);


public slots:
    /**
     * @brief onePlayerHasStopped
     */
    void onePlayerHasStopped(int);
    /**
     * @brief onePlayerIsPaused
     */
    void onePlayerIsPaused(int);
    /**
     * @brief onePlayerPlays
     */
    void onePlayerPlays(int,quint64);
    /**
     * @brief onePlayerHasNewSong
     */
    void onePlayerHasNewSong(int,QString);
    /**
     * @brief onePlayerHasChangedPosition
     */
    void onePlayerHasChangedPosition(int,quint64);


protected:
    /**
     * @brief contextMenuEvent
     * @param ev
     */
    void contextMenuEvent(QContextMenuEvent* ev);

private slots :
    /**
    * @brief  slot which manage the player's root directory change
    */
    void pChangeDirectory();
    /**
     * @brief showMusicPlayer
     */
    void showMusicPlayer(bool);
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
    void sendCommand(actionMusique action, QString nomFichier = "", quint64 position = 0, NetworkLink * link = NULL);


    static AudioPlayer* m_singleton;//!< @brief static pointer to the unique instance of this audioplayer


	/**
	* @brief set the UI - Phonon only
	*/
    void setupUi();

// ################ MEMBERS ########################## //
private:
    QWidget* m_mainWidget;        //!< @brief brings together all subwidget
    QVBoxLayout* m_mainLayout;


    PlayerWidget* m_mainPlayer;
    PlayerWidget* m_secondPlayer;
    PlayerWidget* m_thirdPlayer;

    QList<PlayerWidget*> m_players;
    QList<QAction*> m_playerActionsList;


    PreferencesManager* m_preferences;
    QMutex m_mutex;
    qint64 m_time;//!< @brief current time
};

#endif
