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
#define FACTOR_WAIT 4

AudioPlayer * AudioPlayer::m_singleton = NULL;


AudioPlayer::AudioPlayer(QWidget *parent)
    : QDockWidget(parent)//,m_currentSource(NULL)
{
    m_preferences = PreferencesManager::getInstance();
    setObjectName("MusicPlayer");

    m_endFile= false;
    m_currentPlayingMode = NEXT;

    if(G_joueur)/// fully defined by the GM
    {
            m_currentPlayingMode=UNIQUE;
    }
    //m_mediaPlayer = new QMediaPlayer(this);

    m_playOnFirstAction = new QAction(tr("Play on First Player"),this);
    m_playOnSecondAction = new QAction(tr("Play on second Player"),this);
    m_playOnThirdAction = new QAction(tr("Play on third Player"),this);



    connect(m_playOnFirstAction,SIGNAL(triggered()),this,SLOT(startSongOnSpecificPlayer()));
    connect(m_playOnSecondAction,SIGNAL(triggered()),this,SLOT(startSongOnSpecificPlayer()));
    connect(m_playOnThirdAction,SIGNAL(triggered()),this,SLOT(startSongOnSpecificPlayer()));


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

void AudioPlayer::startSongOnSpecificPlayer()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if(NULL!=act)
    {
        PlayerWidget* destination = NULL;

        if(act == m_playOnFirstAction)
         {
           destination= m_mainPlayer ;
        }
        else if(act == m_playOnSecondAction)
        {
           destination=m_secondPlayer;
        }
        else
        {
            destination=m_thirdPlayer;
        }
        destination->startMedia(m_model->getMediaByModelIndex(m_songList->currentIndex()));
        //m_mainPlayer->startMedia(m_model->getMediaByModelIndex(p));
    }

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
        QString title(tr("Player %1"));

        m_mainPlayer = new PlayerWidget(title.arg(1));
        m_secondPlayer = new PlayerWidget(title.arg(2));
        m_thirdPlayer = new PlayerWidget(title.arg(3));


        m_mainLayout->addWidget(m_mainPlayer);
        m_mainLayout->addWidget(m_secondPlayer);
        m_mainLayout->addWidget(m_thirdPlayer);


        QHBoxLayout* buttonLayout = new QHBoxLayout();
        m_addAction 	= new QAction(tr("Add"), this);
        m_deleteAction	= new QAction(tr("Remove"), this);
        m_addAction->setToolTip(tr("Add song to the list"));
        m_deleteAction->setToolTip(tr("Remove selected file"));

        QToolButton *delButton= new QToolButton(this);
        QToolButton *addButton= new QToolButton(this);

        delButton->setDefaultAction(m_deleteAction);
        addButton->setDefaultAction(m_addAction);

        buttonLayout->addWidget(addButton);
        buttonLayout->addStretch();
        buttonLayout->addWidget(delButton);

        m_mainLayout->addLayout(buttonLayout);

        m_songList = new QListView(this);
        m_songList->installEventFilter(this);
        m_model = new MusicModel(this);
        m_songList->setSelectionMode(QAbstractItemView::SingleSelection);
        m_songList->setModel(m_model);

        m_mainLayout->addWidget(m_songList);

        m_mainWidget->setLayout(m_mainLayout);


        connect(m_addAction, SIGNAL(triggered(bool)), this, SLOT(addFiles()));
        connect(m_deleteAction, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
        connect(m_songList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(clickOnList(QModelIndex)));
        //connect(m_songList,SIGNAL(itemSelectionChanged()),this,SLOT(selectionHasChanged()),Qt::QueuedConnection);

}
void AudioPlayer::clickOnList(QModelIndex p)//double click
{
        if(NULL!=m_mainPlayer)
        {
            m_mainPlayer->startMedia(m_model->getMediaByModelIndex(p));
        }
          //  m_mediaObject->play();
}
bool AudioPlayer::eventFilter(QObject *obj, QEvent *event)
 {
     if (event->type() == QEvent::ContextMenu)
     {
         if(obj == m_songList)
         {
             QContextMenuEvent *menuEvent = static_cast<QContextMenuEvent *>(event);
             showContextMenu(menuEvent);

         }
         return true;
     }
     else
     {
         // standard event processing
         return QObject::eventFilter(obj, event);
     }
 }
void AudioPlayer::showContextMenu(QContextMenuEvent* ev)
{
    QMenu menu;


    menu.addAction(m_playOnFirstAction);
    menu.addAction(m_playOnSecondAction);
    menu.addAction(m_playOnThirdAction);



    menu.exec(ev->globalPos());
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
    if(!G_joueur)
    {
     //   connect(m_mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
       // connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
      //  this, SLOT(stateChanged(Phonon::State, Phonon::State)));
     //   connect(m_mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
     //   this, SLOT(sourceChanged(const Phonon::MediaSource &)));
      //  connect(MainWindow::getInstance()->getNetWorkManager(), SIGNAL(linkAdded(Liaison *)), this, SLOT(emettreEtat(Liaison *)));
    }
    else
    {
           playerWidget();
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
void AudioPlayer::playerWidget()
{
    /*m_titleDisplay->setToolTip(tr("No songs"));
    m_commandWidget->hide();*/
    QWidget *separateur3 = new QWidget();
    separateur3->setFixedHeight(2);
    m_mainLayout->addWidget(separateur3);
    setFixedHeight(66);

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
        m_currentPlayingMode=LOOP;
    }
    else if(m_uniqueAction->isChecked())
    {
        m_currentPlayingMode=UNIQUE;
    }
    else if((!m_uniqueAction->isChecked())&&(!m_loopAction->isChecked()))
    {
        m_currentPlayingMode=NEXT;
    }
}
void AudioPlayer::addFiles()
{

    QStringList fileList = QFileDialog::getOpenFileNames(this, tr("Add song"), m_preferences->value("MusicDirectoryGM",QDir::homePath()).toString(), tr("Audio files (*.wav *.mp2 *.mp3 *.ogg *.flac)\n PlayList (*.m3u)"));

        if (fileList.isEmpty())
                return;
        QFileInfo fileinfo(fileList[0]);
        m_preferences->registerValue("MusicDirectoryGM",fileinfo.absolutePath());

        m_model->addSong(fileList);

        while (!fileList.isEmpty())
        {
                QString fichier = fileList.takeFirst();

                QFileInfo fi(fichier);
                QString titre = fi.fileName();
                if (m_pathList.isEmpty())
                {

                        emettreCommande(nouveauMorceau, titre);
                        // On active tous les boutons



                }
                //morceau->setToolTip(fichier);
                m_pathList.append(fichier);
        }
}
void AudioPlayer::removeFile()
{

/// @todo test to perform with several computers. The sound must stop on both side.
        /*QList<QListWidgetItem *> titreSelectionne = m_songList->selectedItems();
        if (titreSelectionne.isEmpty())
        {
                return;
        }
        m_mutex.lock();
        foreach(QListWidgetItem * tmp, titreSelectionne)
        {
            m_pathList.removeAt(m_songList->row(tmp));
            if(m_currentItemFile == tmp)
            {
                //delete m_currentSource;
                //m_currentSource = NULL;
                delete tmp;
                tmp = NULL;
                m_currentItemFile = NULL;
             //   m_mediaObject->clear();
                isAboutToFinish();
            }
            if(tmp!=NULL)
            {
                delete tmp;
            }
        }
        m_mutex.unlock();*/
}

void AudioPlayer::isAboutToFinish()
{
   /* qDebug() << m_currentPlayingMode << "joueur" << G_joueur;
        if(G_joueur)
        {
            return;
        }
        else if (m_currentPlayingMode==UNIQUE)
        {
                emettreCommande(arretMorceau);
                //m_mediaObject->stop();
                qDebug() << "finDeTitreSlot" << "lecture unique";
        }
        else if (m_currentPlayingMode==NEXT)
        {
            int position= m_songList->row(m_currentItemFile);
            if(position<0)
            {
                //m_mediaObject->stop();
                return;
            }

            position++;
            if(position>=m_pathList.size())
            {
                position =0;
            }
           // m_mediaObject->stop();
            m_formerItemFile = m_currentItemFile;
            m_currentItemFile = m_songList->item(position);

            // m_currentSource = new Phonon::MediaSource();
            //setSource(m_pathList[position]);
//            m_mediaObject->clear();
          // m_mediaObject->setCurrentSource(*m_currentSource);
            emettreCommande(nouveauMorceau, m_currentItemFile->text());

            qDebug() << "Changement de titre 2" << m_pathList[position] << m_currentItemFile->text();
            //m_mediaObject->play();

        }
        else if(m_currentPlayingMode==LOOP)
        {
         //   m_mediaObject->setCurrentSource(*m_currentSource);
         //   m_mediaObject->play();
        }*/
}

void AudioPlayer::emettreCommande(actionMusique action, QString nomFichier, quint64 position, Liaison * link)
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


void AudioPlayer::emettreEtat(Liaison * link)
{
        //if(m_titleDisplay->text().isEmpty())
    //    {
     //      emettreCommande(nouveauMorceau, "", 0, link);
    //    }
   //     else
   //     {
     //       emettreCommande(nouveauMorceau, m_titleDisplay->text(), 0, link);
         //   switch(m_mediaObject->state())
            {
                /*case Phonon::PausedState:
                    emettreCommande(pauseMorceau,"", 0, link);
                    emettreCommande(nouvellePositionMorceau, "", m_time, link);
                    break;
                case Phonon::StoppedState:
                    emettreCommande(arretMorceau, "",0, link);
                    break;
            case Phonon::PlayingState :
                    emettreCommande(pauseMorceau,"", 0, link);
                    emettreCommande(nouvellePositionMorceau, "", m_time, link);
                    emettreCommande(lectureMorceau, "", 0, link);
                    break;
                default :
                    break;*/
            }
     //   }
}

qreal AudioPlayer::volume()
{
        return 0;//return audioOutput->volume();
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

void AudioPlayer::volumeHasChanged(qreal newVolume)
{
    qDebug()<< "new volume"<< newVolume;
    m_preferences->registerValue("MusicVolume",newVolume);

}
