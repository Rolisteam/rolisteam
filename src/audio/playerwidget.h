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
#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H
#include <QSlider>
#include <QTime>
#include <QMediaPlayer>
#include <QMediaContent>




namespace Ui {
class AudioWidgetUI;
}

/**
 * @brief The PlayerWidget class
 */
class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief PlayerWidget
     * @param parent
     */
    PlayerWidget(QString title, QWidget* parent = NULL);
    /**
     * @brief startMedia
     */
    void startMedia(QMediaContent*);
    /**
     * @brief stop
     */
   // void stop();
    /**
     * @brief pause
     */
    //void pause();

private:
    /**
     * @brief setupUi
     */
    void setupUi();
    /**
     * @brief updateIcon
     */
    void updateIcon();

private slots:
    /**
     * @brief setTime
     * @param time
     */
    void setTime(int time);
    /**
     * @brief positionChanged
     * @param time
     */
    void positionChanged(qint64 time);
    /**
     * @brief setDuration
     * @param duration
     */
    void setDuration(qint64 duration);

    /**
     * @brief sourceChanged
     * @param media
     */
    void sourceChanged(const QMediaContent & media);

    /**
     * @brief playerStatusChanged
     * @param newState
     */
    void playerStatusChanged(QMediaPlayer::State newState);

signals:
    /**
     * @brief positionChanged
     */
    void positionChanged(int);
    /**
     * @brief volumeChanged
     */
    void volumeChanged(int);
    /**
     * @brief askNext
     */
    void askNext();
    /**
     * @brief askPrevious
     */
    void askPrevious();


private:
    QSlider* m_volume;
    QSlider* m_seek;
    QMediaPlayer m_player;
    QMediaContent* m_content;
    QAction* m_playAct;
    QAction* m_stopAct;
    QAction* m_pauseAct;
    QAction* m_uniqueAct;
    QAction* m_repeatAct;
    QAction* m_volumeMutedAct;

    Ui::AudioWidgetUI* m_ui;
};
#endif
