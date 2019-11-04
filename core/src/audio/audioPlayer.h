/***************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                   *
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

#include <QAction>
#include <QCheckBox>
#include <QDockWidget>
#include <QEvent>
#include <QLCDNumber>
#include <QLineEdit>
#include <QList>
#include <QListView>
#include <QMediaPlayer>
#include <QMutex>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "musicmodel.h"
#include "network/networkreceiver.h"
#include "playerwidget.h"
#include "preferences/preferencesmanager.h"
class NetworkLink;
/**
 * @brief This player can be used by the GM to play songs.
 * Regular players can just change the volume level.
 */
class AudioPlayer : public QDockWidget, public NetWorkReceiver
{
    Q_OBJECT

public:
    /**
     * @brief private constructor
     */
    AudioPlayer(QWidget* parent= nullptr);

    ~AudioPlayer();
    /**
     * @brief updateUi
     */
    void updateUi(bool b);

    /**
     * @brief processMessage
     * @param msg
     */
    virtual NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg);

    /**
     * @brief openSongList
     * @param str
     */
    virtual void openSongList(QString str);
    /**
     * @brief openSong
     * @param str
     */
    virtual void openSong(QString str);


    void readSettings();

signals:
    void errorMessage(const QString& str);

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
    void onePlayerPlays(int, quint64);
    /**
     * @brief onePlayerHasNewSong
     */
    void onePlayerHasNewSong(int, QString);
    /**
     * @brief onePlayerHasChangedPosition
     */
    void onePlayerHasChangedPosition(int, quint64);
    /**
     * @brief volumeChangedOnOneAudioPlayer
     * @param id
     * @param volume
     */
    void volumeChangedOnOneAudioPlayer(int id, int volume);
    void setPlayerName(const QString& name);
    void askForControl();

protected:
    /**
     * @brief contextMenuEvent
     * @param ev
     */
    void contextMenuEvent(QContextMenuEvent* ev);

private slots:
    /**
     * @brief showMusicPlayer
     */
    void showMusicPlayer(bool);
    void volumeControlError();

private:
    /**
     * @brief playerWidget
     */
    void playerWidget();
    /**
     * @brief set the UI
     */
    void setupUi();

    // ################ MEMBERS ########################## //
private:
    QWidget* m_mainWidget; //!< @brief brings together all subwidget
    QVBoxLayout* m_mainLayout;
<<<<<<< HEAD
    QString m_playerName;

    QCheckBox* m_volumeControlled= nullptr;
    QCheckBox* m_gmControlVolume= nullptr;
    QSlider* m_globalVolume= nullptr;
||||||| merged common ancestors
=======
    QString m_playerName;
>>>>>>> AudioPlayer: setPlayername

    QCheckBox* m_volumeControlled= nullptr;
    QCheckBox* m_gmControlVolume= nullptr;
    QSlider* m_globalVolume= nullptr;

    PlayerWidget* m_mainPlayer;
    PlayerWidget* m_secondPlayer;
    PlayerWidget* m_thirdPlayer;

    QVector<PlayerWidget*> m_players;
    QVector<QAction*> m_playerActionsList;

    PreferencesManager* m_preferences;
    QMutex m_mutex;
    qint64 m_time; //!< @brief current time
    bool m_isGM;
    bool m_volumeControlable= false;
};

#endif
