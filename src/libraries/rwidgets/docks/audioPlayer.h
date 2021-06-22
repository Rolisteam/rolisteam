/***************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                               *
 *   https://rolisteam.org/contact                   *
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

#include "core/model/musicmodel.h"
#include "core/network/networkreceiver.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets/customs/playerwidget.h"
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
    virtual void openSongList(const QString& str);
    /**
     * @brief openSong
     * @param str
     */
    virtual void openSong(const QString& str);

    void readSettings();

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
    void onePlayerPlays(int, qint64);
    /**
     * @brief onePlayerHasNewSong
     */
    void onePlayerHasNewSong(int, const QString&);
    /**
     * @brief onePlayerHasChangedPosition
     */
    void onePlayerHasChangedPosition(int, qint64);

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

private:
    /**
     * @brief playerWidget
     */
    void playerWidget();

    //!< @brief static pointer to the unique instance of this audioplayer
    /**
     * @brief set the UI
     */
    void setupUi();

private:
    QWidget* m_mainWidget; //!< @brief brings together all subwidget
    QVBoxLayout* m_mainLayout;

    PlayerWidget* m_mainPlayer;
    PlayerWidget* m_secondPlayer;
    PlayerWidget* m_thirdPlayer;

    QList<PlayerWidget*> m_players;
    QList<QAction*> m_playerActionsList;

    PreferencesManager* m_preferences;
    QMutex m_mutex;
    qint64 m_time; //!< @brief current time
    bool m_isGM;
};

#endif
