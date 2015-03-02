#include "playerwidget.h"

#include "ui_audiowidget.h"
#include "networkmessage.h"

PlayerWidget::PlayerWidget(QString title,QWidget* parent)
    : QWidget(parent),m_ui(new Ui::AudioWidgetUI)
{
    m_ui->setupUi(this);
    setupUi();
    m_ui->m_groupBox->setTitle(title);
}

void PlayerWidget::startMedia(QMediaContent* p)
{
    m_content = p;
    m_player.setMedia(*m_content);
    m_ui->m_timeSlider->setMinimum(0);
    m_ui->m_label->setText(p->canonicalUrl().fileName());

    m_player.play();
}
void PlayerWidget::setDuration(qint64 duration)
{
    m_ui->m_timeSlider->setMaximum(duration);
}

void PlayerWidget::positionChanged(qint64 time)
{
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
     /*if((!G_joueur) && ((time>m_time+(FACTOR_WAIT*m_mediaPlayer->notifyInterval()))||(time<m_time)))
     {
          emettreCommande(nouvellePositionMorceau, "", time);
          emitCurrentState();
     }*/
    // m_time = time;
     m_ui->m_timeSlider->setValue(time);
     //m_timerDisplay->display(displayTime.toString("mm:ss"));

}

void PlayerWidget::setupUi()
{
    m_playAct = new QAction(style()->standardIcon(QStyle::SP_MediaPlay),tr("Play"),this);
    m_pauseAct = new QAction(style()->standardIcon(QStyle::SP_MediaPause),tr("Pause"),this);
    m_stopAct = new QAction(style()->standardIcon(QStyle::SP_MediaStop),tr("Stop"),this);
    m_uniqueAct = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward),tr("Next"),this);
    m_repeatAct = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward),tr("Previous"),this);
    m_volumeMutedAct = new QAction(this);
    updateIcon();

    m_ui->m_volumeMutedButton->setDefaultAction(m_volumeMutedAct);
    m_ui->m_playButton->setDefaultAction(m_playAct);
    m_ui->m_pauseButton->setDefaultAction(m_pauseAct);
    m_ui->m_stopButton->setDefaultAction(m_stopAct);
    m_ui->m_uniqueMode->setDefaultAction(m_uniqueAct);
    m_ui->m_repeatMode->setDefaultAction(m_repeatAct);

    m_ui->m_timeSlider->setMinimum(0);

    connect(&m_player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    connect(&m_player,SIGNAL(durationChanged(qint64)),this,SLOT(setDuration(qint64)));

    connect(m_playAct,SIGNAL(triggered()),&m_player,SLOT(play()));
    connect(m_stopAct,SIGNAL(triggered()),&m_player,SLOT(stop()));
    connect(m_pauseAct,SIGNAL(triggered()),&m_player,SLOT(pause()));

    connect(m_ui->m_timeSlider,SIGNAL(sliderMoved(int)),this,SLOT(setTime(int)));


    m_ui->m_volumeSlider->setRange(0,100);
    connect(m_ui->m_volumeSlider,SIGNAL(valueChanged(int)),&m_player,SLOT(setVolume(int)));

    connect(&m_player,SIGNAL(currentMediaChanged(QMediaContent)),this,SLOT(sourceChanged(QMediaContent)));
    connect(&m_player,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(playerStatusChanged(QMediaPlayer::State)));
}


void PlayerWidget::updateIcon()
{
    if(m_volumeMutedAct->isChecked())
    {
        m_volumeMutedAct->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    }
    else
    {
       m_volumeMutedAct->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    }
}
void PlayerWidget::setTime(int time)
{
    m_player.setPosition(time);
}
void PlayerWidget::sourceChanged(const QMediaContent & media)
{

}
void PlayerWidget::playerStatusChanged(QMediaPlayer::State newState)
{
    switch (newState)
    {
    case QMediaPlayer::StoppedState:
        //sendCommand(NetMsg::StopSong);
        break;
    case QMediaPlayer::PlayingState:
        //sendCommand(NetMsg::PlaySong,"",m_time);
        break;
    case QMediaPlayer::PausedState:
        //sendCommand(NetMsg::PauseSong);
        break;
    }
}
