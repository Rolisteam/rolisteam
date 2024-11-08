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

#include "model/musicmodel.h"
#include "rwidgets/customs/playerwidget.h"

class AudioController;
/**
 * @brief This player can be used by the GM to play songs.
 * Regular players can just change the volume level.
 */
class RWIDGET_EXPORT AudioPlayer : public QWidget
{
    Q_OBJECT

public:
    AudioPlayer(AudioController* ctrl, QWidget* parent= nullptr);
    ~AudioPlayer();

    void updateState();

protected:
    void contextMenuEvent(QContextMenuEvent* ev);

private:
    void playerWidget();
    void setupUi();
signals:
    void changePlayerDirectory();

private:
    QPointer<AudioController> m_ctrl;
    QVBoxLayout* m_mainLayout;

    QList<PlayerWidget*> m_players;
    QList<QAction*> m_playerActionsList;

    qint64 m_time; //!< @brief current time
};

#endif
