/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                                  *
 *	Copyright (C) 2009 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "audioPlayer.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QFileDialog>
#include <QListView>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <array>

#include "controller/audiocontroller.h"

namespace
{
QString musicStatus= QStringLiteral("music_player_%1_status");
}

AudioPlayer::AudioPlayer(AudioController* ctrl, QWidget* parent) : QDockWidget(parent), m_ctrl(ctrl)
{
    setObjectName("MusicPlayer");
    setupUi();
    setWidget(m_mainWidget);
    m_mainWidget->setVisible(true);
}

void AudioPlayer::contextMenuEvent(QContextMenuEvent* ev)
{
    QMenu menu;
    if(m_ctrl->localIsGM())
    {
        for(auto& tmp : m_players)
        {
            if(tmp->isVisible() && tmp->geometry().contains(ev->pos(), true))
            {
                tmp->addActionsIntoMenu(&menu);
                menu.addSeparator();
            }
        }
    }
    for(auto& act : m_playerActionsList)
    {
        menu.addAction(act);
    }

    menu.exec(ev->globalPos());
    ev->accept();
}
AudioPlayer::~AudioPlayer()
{
    delete m_mainWidget;
}

void AudioPlayer::setupUi()
{
    setWindowTitle(tr("Background Music"));
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setMinimumWidth(255);
    m_mainWidget= new QWidget(this);

    m_mainLayout= new QVBoxLayout();
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(QMargins());

    if(m_ctrl)
    {

        std::array<AudioPlayerController*, 3> array{m_ctrl->firstController(), m_ctrl->secondController(),
                                                    m_ctrl->thirdController()};

        for(int i= 0; i < 3; ++i)
        {
            auto* playerWidget= new PlayerWidget(array[i], this);
            connect(playerWidget, &PlayerWidget::changePlayerDirectory, this, &AudioPlayer::changePlayerDirectory);

            m_players.append(playerWidget);
            auto* act= new QAction(tr("Show/hide Player %1").arg(i + 1), this);
            act->setCheckable(true);
            act->setChecked(array[i]->visible());
            connect(act, &QAction::triggered, array[i], &AudioPlayerController::setVisible);
            m_playerActionsList.append(act);
            m_mainLayout->addWidget(m_players[i]);
            playerWidget->setVisible(array[i]->visible());
        }
    }

    m_mainWidget->setLayout(m_mainLayout);
}

/**/
