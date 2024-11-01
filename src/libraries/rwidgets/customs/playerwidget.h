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
#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QContextMenuEvent>
#include <QMenu>
#include <QPointer>
#include <QSlider>
#include <QTime>
#include <random>

#include "rwidgets_global.h"
namespace Ui
{
class PlayerWidgetUI;
}
class AudioPlayerController;

/**
 * @brief The PlayerWidget class manages all widgets requiered for playing music.
 */
class RWIDGET_EXPORT PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    PlayerWidget(AudioPlayerController* ctrl, QWidget* parent= nullptr);
    void updateUi();

    void addActionsIntoMenu(QMenu* menu);

protected:
    void contextMenuEvent(QContextMenuEvent* ev);
    void dropEvent(QDropEvent* event);

private:
    void setupUi();

private slots:
    void updateIcon();

    void addFiles();
    void openPlayList();
    bool askToDeleteAll();
    void openStream();
    void savePlaylist();

signals:
    void changePlayerDirectory();

private:
    QPointer<AudioPlayerController> m_ctrl;
    QSlider* m_volume;
    QSlider* m_seek;
    QAction* m_playAct;
    QAction* m_stopAct;
    QAction* m_pauseAct;
    QAction* m_uniqueAct;
    QAction* m_repeatAct;
    QAction* m_shuffleAct;
    QAction* m_volumeMutedAct;
    QAction* m_loadTableTopAudioPlayListAct;
    QAction* m_addAction; //!< @brief add song action
    QAction* m_addStreamAction;
    QAction* m_deleteAction; //!< @brief remove song action
    QAction* m_openPlayList;
    QAction* m_savePlayList;
    QAction* m_clearList;
    Ui::PlayerWidgetUI* m_ui;
};
#endif
