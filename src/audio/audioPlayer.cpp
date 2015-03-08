/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
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
#include "audioPlayer.h"


#include <QToolButton>
#include <QFileDialog>
#include <QPushButton>
#include <QMenu>
#include <QContextMenuEvent>
#include <QListView>
#include <QDebug>


#include "ClientServeur.h"
#include "Liaison.h"
#include "variablesGlobales.h"
#include "types.h"
#include "network/networkmessagewriter.h"




AudioPlayer * AudioPlayer::m_singleton = NULL;


AudioPlayer::AudioPlayer(QWidget *parent)
    : QDockWidget(parent)//,m_currentSource(NULL)
{
    m_preferences = PreferencesManager::getInstance();
    setObjectName("MusicPlayer");


  //  m_currentPlayingMode = NEXT;

    if(G_joueur)/// fully defined by the GM
    {
            //m_currentPlayingMode=UNIQUE;
    }
    //m_mediaPlayer = new QMediaPlayer(this);



    setupUi();


    setWidget(m_mainWidget);
}

AudioPlayer*  AudioPlayer::getInstance(QWidget *parent)
 {
        if(m_singleton==NULL)
        {
            m_singleton = new AudioPlayer(parent);
        }
        return m_singleton;
}
void AudioPlayer::contextMenuEvent(QContextMenuEvent* ev)
{
    QMenu menu;
    if(!PreferencesManager::getInstance()->value("isPlayer",true).toBool())
    {
        foreach(PlayerWidget* tmp,m_players)
        {
            if((tmp->geometry().contains(ev->pos(),true))&&(tmp->isVisible()))
            {
                tmp->addActionsIntoMenu(&menu);
                menu.addSeparator();
            }
        }
    }
    foreach(QAction* act,m_playerActionsList)
    {
        menu.addAction(act);
    }

    menu.exec(ev->globalPos());
     ev->accept();
}


AudioPlayer::~AudioPlayer()
{
    //m_preferences->registerValue("MusicVolume",audioOutput->volume());
    delete m_mainWidget;
   // delete path;
}

void AudioPlayer::setupUi()
{
        setWindowTitle(tr("Background Music"));
        setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        setFeatures(QDockWidget::AllDockWidgetFeatures);
        setMinimumWidth(255);
        m_mainWidget = new QWidget();

        m_mainLayout = new QVBoxLayout();
        m_mainLayout->setSpacing(0);
        m_mainLayout->setMargin(0);

        for(int i = 0;i < 3; ++i)
        {
            PlayerWidget*  playerWidget = new PlayerWidget(i,this);
            connect(playerWidget,SIGNAL(newSongPlayed(int,QString)),this,SLOT(onePlayerHasNewSong(int,QString)));
            connect(playerWidget,SIGNAL(playerIsPaused(int)),this,SLOT(onePlayerIsPaused(int)));
            connect(playerWidget,SIGNAL(playerStopped(int)),this,SLOT(onePlayerHasStopped(int)));
            connect(playerWidget,SIGNAL(playerIsPlaying(int,quint64)),this,SLOT(onePlayerPlays(int,quint64)));
            connect(playerWidget,SIGNAL(playerPositionChanged(int,quint64)),this,SLOT(onePlayerHasChangedPosition(int,quint64)));

            m_players.append(playerWidget);
            QAction* act = new QAction(tr("Show/hide Player %1").arg(i),this);
            act->setCheckable(true);
            act->setChecked(m_preferences->value(QString("music_player_%1_status").arg(i),true).toBool());
            connect(act,SIGNAL(toggled(bool)),this,SLOT(showMusicPlayer(bool)));
            m_playerActionsList.append(act);
            m_mainLayout->addWidget(m_players[i]);
        }
        m_mainLayout->addStretch(1);


        /*m_mainLayout->addWidget(m_secondPlayer);
        m_mainLayout->addWidget(m_thirdPlayer);*/


        m_mainWidget->setLayout(m_mainLayout);

        //connect(m_songList,SIGNAL(itemSelectionChanged()),this,SLOT(selectionHasChanged()),Qt::QueuedConnection);

}
void AudioPlayer::showMusicPlayer(bool status)
{
    QAction* act = qobject_cast<QAction*>(sender());

    if(NULL!=act)
    {
        int i = m_playerActionsList.indexOf(act);

        if(-1!=1)
        {
            PlayerWidget* tmp = m_players[i];
            tmp->setVisible(status);
            m_preferences->registerValue(QString("music_player_%1_status").arg(i),status);
        }
    }
}

//void AudioPlayer::sourceChanged( QMediaContent const & source)
//{
  //   qDebug() << "sourceChanged" << source.fileName() << m_currentItemFile;


     /*if(m_currentItemFile==NULL)
     {
         return;
     }

     m_titleDisplay->setText(source.canonicalUrl().toString().right(source.canonicalUrl().toString().length()-source.canonicalUrl().toString().lastIndexOf("/")-1));



     if(m_formerItemFile!=NULL)
     {
         QFont ft = m_formerItemFile->font();
         ft.setBold(false);
         m_formerItemFile->setFont(ft);
     }
     QFont ft2 = m_currentItemFile->font();
     ft2.setBold(true);
     m_currentItemFile->setFont(ft2);

     m_timerDisplay->display("00:00");*/
//}
void AudioPlayer::emitCurrentState()
{
   /* switch (m_mediaObject->state())
    {
        case Phonon::ErrorState:
            break;
        case Phonon::PlayingState:
                emettreCommande(lectureMorceau,"",m_time);
                break;
        case Phonon::StoppedState:
                emettreCommande(arretMorceau);
                break;
        case Phonon::PausedState:
                emettreCommande(pauseMorceau);
                break;
        case Phonon::BufferingState:
        case Phonon::LoadingState:
        default:
                qDebug() << "default State";
             break;
    }*/
}
void AudioPlayer::updateUi()
{
    foreach(PlayerWidget* tmp,m_players)
    {
        tmp->updateUi();
    }
    for(int i = 0; i< m_players.size(); ++i)
    {
        m_playerActionsList[i]->setChecked(m_preferences->value(QString("music_player_%1_status").arg(i),true).toBool());
        //setVisible();
    }
}
void AudioPlayer::onfinished()
{
    /// @todo the m_currentsource must be checked
//    if (m_currentPlayingMode==LOOP)
//    {
//        if(m_currentSource!=NULL)
//        {
//            m_mediaObject->setCurrentSource(*m_currentSource);
//            m_mediaObject->play();

//        }
//        else
//        {
//            m_mediaObject->stop();
//        }
//    }
//    if(G_joueur)
//    {
//        m_mediaObject->stop();
//    }
}
void AudioPlayer::statusChanged(QMediaPlayer::MediaStatus newState)
{

}

void AudioPlayer::playerStatusChanged(QMediaPlayer::State newState)
{

    if(G_joueur)
        return;

     switch (newState)
     {
      /*   case Phonon::ErrorState:
            qDebug() << "error State" << m_mediaObject->errorString();
             break;
         case Phonon::PlayingState:
             qDebug() << "playing State";
                 m_playAction->setEnabled(false);
                 m_playAction->setChecked(true);
                 m_pauseAction->setEnabled(true);
                 m_pauseAction->setChecked(false);
                 m_stopAction->setEnabled(true);
                 m_stopAction->setChecked(false);
                 m_timePosition->setEnabled(true);
                 emettreCommande(lectureMorceau,"",m_time);
                 break;
         case Phonon::StoppedState:
                     emettreCommande(arretMorceau);
                     m_stopAction->setEnabled(false);
                     m_playAction->setEnabled(true);
                     m_playAction->setChecked(false);
                     m_pauseAction->setChecked(false);
                     m_pauseAction->setEnabled(false);
                     m_timePosition->setEnabled(true);
                     m_timerDisplay->display("00:00");
                 break;
         case Phonon::PausedState:
                 qDebug() << "paused State";
                 /// @attention Workaround for phonon issue with some files. Pause state is sometime called
                     if(m_stopAction->isChecked())
                     {

                     }
                     else if(!m_pauseAction->isChecked())
                     {
                         //isAboutToFinish();
                         m_mediaObject->play();
                     }
                     else if(m_pauseAction->isChecked())
                     {
                         emettreCommande(pauseMorceau);
                         m_stopAction->setEnabled(true);
                         m_playAction->setEnabled(true);
                         m_playAction->setChecked(false);
                         m_pauseAction->setEnabled(false);
                    }
                 break;
         case Phonon::BufferingState:
                 qDebug() << "buffering State";
                 break;
         case Phonon::LoadingState:
                 qDebug() << "Phonon::LoadingState State";
              break;*/
         default:
                 qDebug() << "default State";
              break;
     }

}

void AudioPlayer::updatePlayingMode()
{
    QAction* tmp = static_cast<QAction*>(sender());
    if((tmp==m_loopAction)&&(m_loopAction->isChecked())&&(m_uniqueAction->isChecked()))
    {
        m_uniqueAction->setChecked(false);
    }
    if((tmp==m_uniqueAction)&&(m_loopAction->isChecked())&&(m_uniqueAction->isChecked()))
    {
        m_loopAction->setChecked(false);
    }


    if(m_loopAction->isChecked())
    {
       // m_currentPlayingMode=LOOP;
    }
    else if(m_uniqueAction->isChecked())
    {
     //   m_currentPlayingMode=UNIQUE;
    }
    else if((!m_uniqueAction->isChecked())&&(!m_loopAction->isChecked()))
    {
      //  m_currentPlayingMode=NEXT;
    }
}

void AudioPlayer::sendCommand(actionMusique action, QString nomFichier, quint64 position, Liaison * link)
{

        int p;
        quint16 tailleNomFichier;

        char *donnees = NULL;

        enteteMessage uneEntete;
        uneEntete.categorie = musique;
        uneEntete.action = action;
        switch(action)
        {
                case nouveauMorceau :
                        uneEntete.tailleDonnees = sizeof(quint16) + nomFichier.size()*sizeof(QChar);
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        p = sizeof(enteteMessage);
                        tailleNomFichier = nomFichier.size();
                        memcpy(&(donnees[p]), &tailleNomFichier, sizeof(quint16));
                        p+=sizeof(quint16);
                        memcpy(&(donnees[p]), nomFichier.data(), tailleNomFichier*sizeof(QChar));
                        break;
                case lectureMorceau :
                        uneEntete.tailleDonnees = 0;
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        break;

                case pauseMorceau :
                        uneEntete.tailleDonnees = 0;
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        break;

                case arretMorceau :
                        uneEntete.tailleDonnees = 0;
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        break;

                case nouvellePositionMorceau :
                        uneEntete.tailleDonnees = sizeof(quint32);
                        donnees = new char[uneEntete.tailleDonnees + sizeof(enteteMessage)];
                        // Ajout de la position dans le message
                        memcpy(&(donnees[sizeof(enteteMessage)]), &position, sizeof(quint32));
                        break;

                default :
                        qWarning() <<(tr("Unknown command send to users"));
                        break;
        }
        if(donnees == NULL)
            return;

        memcpy(donnees, &uneEntete, sizeof(enteteMessage));

        if (link == NULL)
        {
            emettre(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage));
        }
        else
        {
            link->emissionDonnees(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage));
        }

        delete[] donnees;
}

void AudioPlayer::onePlayerHasStopped(int id)
{
    qDebug() << "audio player onePlayerHasStopped"<< id;
    if(!m_preferences->value("isPlayer",false).toBool())
    {
    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::StopSong);
    message.uint8(id);
    message.sendAll();
    }
}

void AudioPlayer::onePlayerIsPaused(int id)
{
     qDebug() << "audio player onePlayerIsPaused"<< id;
     if(!m_preferences->value("isPlayer",false).toBool())
     {
    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::PauseSong);
    message.uint8(id);
    message.sendAll();
     }
}

void AudioPlayer::onePlayerPlays(int id,quint64 pos)
{
     qDebug() << "audio player onePlayerPlays"<< id;
     if(!m_preferences->value("isPlayer",false).toBool())
     {
    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::PlaySong);
    message.uint8(id);
    message.uint64(pos);
    message.sendAll();
     }
}


void AudioPlayer::onePlayerHasNewSong(int id,QString str)
{
     qDebug() << "audio player onePlayerHasNewSong"<< id;
     if(!m_preferences->value("isPlayer",false).toBool())
     {
    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::NewSong);
    message.uint8(id);
    message.string32(str);
    message.sendAll();
     }
}

void AudioPlayer::onePlayerHasChangedPosition(int id,quint64 pos)
{
     qDebug() << "audio player onePlayerHasChangedPosition"<< id;
     if(!m_preferences->value("isPlayer",false).toBool())
     {
        NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::ChangePositionSong);
        message.uint8(id);
        message.uint64(pos);
        message.sendAll();
     }
}



void AudioPlayer::pplay()
{
  //  m_mediaObject->play();
}

void AudioPlayer::ppause()
{
   // m_mediaObject->pause();
}

void AudioPlayer::pstop()
{
    //m_mediaObject->stop();
}
void AudioPlayer::pselectNewFile(QString file)
{
    /*m_currentFile = file;
    if(m_currentFile.isEmpty())
    {
        m_titleDisplay->clear();
        m_titleDisplay->setToolTip(tr("No songs"));
       // m_mediaObject->stop();
    }
    else
    {
        QString key = getDirectoryKey();

        QString path(tr("%1/%2").arg(m_preferences->value(key,QDir::homePath()).toString()).arg(m_currentFile));

        QFileInfo fileInfo(path);
        if (!fileInfo.exists())
        {
            //qDebug() << " file n'existe pas = " << path;
            QPalette palette(m_titleDisplay->palette());
            palette.setColor(QPalette::Normal, QPalette::Text, Qt::red);
            m_titleDisplay->setEchoMode(QLineEdit::Normal);
            m_titleDisplay->setPalette(palette);
            m_titleDisplay->setText(tr("%1 : file can not be found or opened").arg(path));
            m_titleDisplay->setToolTip(tr("File can not be found or opened : %1").arg(path));
        }
        else
        {
           // qDebug() << " file existe = " << path;
            //m_currentSource = new Phonon::MediaSource(path);
            //setSource(path);
          //  m_mediaObject->setCurrentSource(*m_currentSource);
            m_titleDisplay->setEchoMode(QLineEdit::Password);
            QPalette palette(m_titleDisplay->palette());
            palette.setColor(QPalette::Normal, QPalette::Text, Qt::black);
            m_titleDisplay->setPalette(palette);
            m_titleDisplay->setText(m_currentFile);
        }



    }*/
}
void AudioPlayer::pseek(quint32 position)
{
   // if(m_mediaObject->isSeekable())
    {
         //   m_mediaObject->seek(position);
    }
}
QString AudioPlayer::getDirectoryKey()
{
    QString key;
    if (PreferencesManager::getInstance()->value("isPlayer",true).toBool())
    {
        key="MusicDirectoryPlayer";
    }
    else
    {
        key="MusicDirectoryGM";
    }
    return key;
}

void AudioPlayer::pChangeDirectory()
{

    QString key = getDirectoryKey();


    QString tmp = QFileDialog::getExistingDirectory(0 , tr("Select the songs directory"), m_preferences->value(key,QDir::homePath()).toString(),
            QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
        m_preferences->registerValue(key,tmp);

}
void AudioPlayer::selectionHasChanged()
{
     /*QList<QListWidgetItem *> selection = m_songList->selectedItems();


    if(selection.size()>0)
    {
       // switch(m_mediaObject->state())
        {
            case Phonon::PlayingState:

            break;
            case Phonon::BufferingState:
            case Phonon::PausedState:
            case Phonon::StoppedState:
            case Phonon::LoadingState:
            case Phonon::ErrorState:
                    defineSource(selection[0]);
            break;
       }
       m_deleteAction->setEnabled(true);




    }
    else
    {
        m_deleteAction->setEnabled(false);

    }*/
}

//void AudioPlayer::volumeHasChanged(qreal newVolume)
//{
//    qDebug()<< "new volume"<< newVolume;
//    m_preferences->registerValue("MusicVolume",newVolume);

//}
void AudioPlayer::processMessage(NetworkMessageReader* msg)
{
    qDebug() << "network debug";
    int id = msg->uint8();
    NetMsg::Action action = msg->action();
    switch (action) {
    case NetMsg::PlaySong:
        qDebug() << "playSong";
        m_players[id]->playSong(msg->uint64());
        break;
    case NetMsg::PauseSong:
        qDebug() << "pauseSong";
        m_players[id]->pause();
        break;
    case NetMsg::ChangePositionSong:
    {
        qDebug() << "change position";
        int pos= msg->uint64();
        m_players[id]->setPositionAt(pos);
        }
        break;
    case NetMsg::StopSong:
        qDebug() << "stop";
        m_players[id]->stop();
        break;
    case NetMsg::NewSong:
    {
        qDebug() << "new";
        QString file = msg->string32();
        m_players[id]->setSourceSong(file);
    }
        break;
    default:
        break;
    }
}
