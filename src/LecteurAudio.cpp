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


#include <QtGui>

#include "LecteurAudio.h"
#include "variablesGlobales.h"
#include "constantesGlobales.h"
#include "types.h"

#include <QDebug>

LecteurAudio * LecteurAudio::singleton = NULL;

LecteurAudio::LecteurAudio(QWidget *parent)
: QDockWidget(parent)
{
    m_endFile= false;
    m_currentPlayingMode = NEXT;
    if(!G_joueur)
            m_currentPlayingMode=UNIQUE;
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    path = new Phonon::Path();
    m_time = 0;
    if(!G_joueur)
    {
        connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(tick(qint64)));
        connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
        this, SLOT(stateChanged(Phonon::State, Phonon::State)));
        connect(mediaObject, SIGNAL(currentSourceChanged(const Phonon::MediaSource &)),
        this, SLOT(sourceChanged(const Phonon::MediaSource &)));
        connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(finDeTitreSlot()));
    }
    connect(mediaObject,SIGNAL(finished()),this,SLOT(onfinished()));
    *path = Phonon::createPath(mediaObject, audioOutput);
    setupUi();
    if(G_joueur)
        playerWidget();
    setWidget(widgetPrincipal);
}

LecteurAudio*  LecteurAudio::getInstance(QWidget *parent)
 {
        if(singleton==NULL)
            singleton = new LecteurAudio(parent);
        return singleton;
}
void LecteurAudio::onfinished()
{
    if (m_currentPlayingMode==LOOP)
    {
        mediaObject->setCurrentSource(*currentsource);
        mediaObject->play();
    }
    if(G_joueur)
        mediaObject->stop();
}

void LecteurAudio::setupUi()
{
        setWindowTitle(tr("Musiques d'ambiance"));
        setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
        setFeatures(QDockWidget::AllDockWidgetFeatures);
        setMinimumWidth(255);
        widgetPrincipal = new QWidget();
        widgetAffichage = new QWidget();
        widgetCommande = new QWidget();
        layoutPrincipal = new QVBoxLayout(widgetPrincipal);
        layoutPrincipal->setMargin(0);
        QWidget *separateur1 = new QWidget();
        separateur1->setFixedHeight(2);
        layoutPrincipal->addWidget(separateur1);
        layoutPrincipal->addWidget(widgetAffichage);
        layoutPrincipal->addWidget(widgetCommande);
        QHBoxLayout *layoutAffichage = new QHBoxLayout(widgetAffichage);
        layoutAffichage->setMargin(0);
        afficheurTitre = new QLineEdit();
        afficheurTitre->setReadOnly(true);

        layoutAffichage->addWidget(afficheurTitre);
        qDebug() << (G_joueur ? "Joueur": "MJ");
        if (G_joueur)
        {
                QAction *actionChangerDossier = new QAction(QIcon(":/resources/icones/dossier.png"), tr("Choisir le répertoire où sont stockées les musiques"), widgetAffichage);
                QToolButton *boutonChangerDossier = new QToolButton(widgetAffichage);
                boutonChangerDossier->setDefaultAction(actionChangerDossier);
                boutonChangerDossier->setFixedSize(20, 20);
                layoutAffichage->addWidget(boutonChangerDossier);
                connect(actionChangerDossier, SIGNAL(triggered()), this, SLOT(pChangeDirectory()));
        }

        niveauVolume = new Phonon::VolumeSlider(this);
        niveauVolume->setAudioOutput(audioOutput);
        niveauVolume->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

        layoutAffichage->addWidget(niveauVolume);

        QVBoxLayout *layoutCommande = new QVBoxLayout(widgetCommande);
        layoutCommande->setMargin(0);

        QHBoxLayout *layoutTemps = new QHBoxLayout();
        layoutTemps->setMargin(0);

        positionTemps = new Phonon::SeekSlider(this);
        positionTemps->setMediaObject(mediaObject);
        positionTemps->setStyle(new QCleanlooksStyle());

        afficheurTemps = new QLCDNumber();
        afficheurTemps->setNumDigits(5);
        afficheurTemps->setSegmentStyle(QLCDNumber::Flat);
        afficheurTemps->display("0:00");
        afficheurTemps->setFixedWidth(40);

        layoutTemps->addWidget(positionTemps);
        layoutTemps->addWidget(afficheurTemps);

        layoutCommande->addLayout(layoutTemps);

        QHBoxLayout *layoutBoutons = new QHBoxLayout();
        layoutBoutons->setMargin(0);
        layoutBoutons->setSpacing(0);

        actionBoucle	= new QAction(QIcon(":/resources/icones/boucle.png"), tr("Lecture en boucle"), widgetCommande);
        actionUnique	= new QAction(QIcon(":/resources/icones/lecture unique.png"), tr("Lecture unique"), widgetCommande);
        actionAjouter 	= new QAction(tr("Ajouter"), widgetCommande);
        actionSupprimer	= new QAction(tr("Supprimer"), widgetCommande);

        actionLecture = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Lecture"), this);
        actionLecture->setShortcut(Qt::Key_Space);
        actionLecture->setDisabled(true);
        actionPause = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
        actionPause->setDisabled(true);
        actionStop = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
        actionStop->setDisabled(true);

        actionAjouter->setToolTip(tr("Ajouter un titre �  la liste"));
        actionSupprimer->setToolTip(tr("Supprimer le titre sélectionné"));
        actionLecture->setCheckable(true);
        actionPause->setCheckable(true);
        actionBoucle->setCheckable(true);
        actionUnique->setCheckable(true);

        QToolButton *boutonLecture= new QToolButton(widgetCommande);
        QToolButton *boutonPause= new QToolButton(widgetCommande);
        QToolButton *boutonStop= new QToolButton(widgetCommande);
        QToolButton *boutonBoucle= new QToolButton(widgetCommande);
        QToolButton *boutonUnique= new QToolButton(widgetCommande);
        QToolButton *boutonAjouter= new QToolButton(widgetCommande);
        QToolButton *boutonSupprimer= new QToolButton(widgetCommande);

        boutonLecture->setDefaultAction(actionLecture);
        boutonPause->setDefaultAction(actionPause);
        boutonStop->setDefaultAction(actionStop);
        boutonBoucle->setDefaultAction(actionBoucle);
        boutonUnique->setDefaultAction(actionUnique);
        boutonAjouter->setDefaultAction(actionAjouter);
        boutonSupprimer->setDefaultAction(actionSupprimer);

        boutonLecture->setAutoRaise(true);
        boutonPause->setAutoRaise(true);
        boutonStop->setAutoRaise(true);
        boutonBoucle->setAutoRaise(true);
        boutonUnique->setAutoRaise(true);

        boutonLecture->setFixedSize(20, 20);
        boutonPause->setFixedSize(20, 20);
        boutonStop->setFixedSize(20, 20);
        boutonBoucle->setFixedSize(20, 20);
        boutonUnique->setFixedSize(20, 20);
        boutonAjouter->setFixedSize(70, 20);
        boutonSupprimer->setFixedSize(70, 20);

        QWidget *separateur2 = new QWidget();

        layoutBoutons->addWidget(boutonLecture);
        layoutBoutons->addWidget(boutonPause);
        layoutBoutons->addWidget(boutonStop);
        layoutBoutons->addWidget(boutonBoucle);
        layoutBoutons->addWidget(boutonUnique);
        layoutBoutons->addWidget(separateur2);
        layoutBoutons->addWidget(boutonAjouter);
        layoutBoutons->addWidget(boutonSupprimer);

        layoutCommande->addLayout(layoutBoutons);
        listeTitres = new QListWidget();
        listeTitres->setSelectionMode(QAbstractItemView::SingleSelection);
        layoutCommande->addWidget(listeTitres);

        connect(actionLecture, SIGNAL(triggered()), mediaObject, SLOT(play()));
        connect(actionPause, SIGNAL(triggered()), mediaObject, SLOT(pause()));
        connect(actionStop, SIGNAL(triggered()), mediaObject, SLOT(stop()));
        connect(actionBoucle, SIGNAL(triggered()), this, SLOT(updatePlayingMode()));
        connect(actionUnique, SIGNAL(triggered()), this, SLOT(updatePlayingMode()));
        connect(actionAjouter, SIGNAL(triggered(bool)), this, SLOT(addFiles()));
        connect(actionSupprimer, SIGNAL(triggered(bool)), this, SLOT(removeFile()));
        connect(listeTitres, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(clickOnList(QListWidgetItem *)));
        connect(mediaObject, SIGNAL(aboutToFinish()), this, SLOT(isAboutToFinish()));

        actionLecture->setEnabled(false);
        actionPause->setEnabled(false);
        actionStop->setEnabled(false);
        actionBoucle->setEnabled(false);
        actionUnique->setEnabled(false);
        actionSupprimer->setEnabled(false);
        positionTemps->setEnabled(false);

}
void LecteurAudio::clickOnList(QListWidgetItem * p)//double click
{
             currentsource = new Phonon::MediaSource(listeChemins[listeTitres->row(p)]);
             mediaObject->setCurrentSource(*currentsource);
             emettreCommande(nouveauMorceau, p->text());
             qDebug() << "Changement de titre" << listeChemins[listeTitres->row(p)] << p->text();
             mediaObject->play();
}

void LecteurAudio::tick(qint64 time)
{
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
     if((!G_joueur) && ((time>m_time+(2*mediaObject->tickInterval()))||(time<m_time)))
          emettreCommande(nouvellePositionMorceau, "", time, -1);
     m_time = time;
     afficheurTemps->display(displayTime.toString("mm:ss"));
}

void LecteurAudio::sourceChanged(const Phonon::MediaSource &source)
{
     qDebug() << "sourceChanged" << source.fileName();
     afficheurTitre->setText(source.fileName().right(source.fileName().length()-source.fileName().lastIndexOf("/")-1));
     afficheurTemps->display("00:00");
}
void LecteurAudio::stateChanged(Phonon::State newState, Phonon::State oldState)
{
     switch (newState)
     {
         case Phonon::ErrorState:
            qDebug() << "error State";
             break;
         case Phonon::PlayingState:
             qDebug() << "playing State";
                if(!G_joueur)
                {
                 actionLecture->setEnabled(false);
                 actionLecture->setChecked(true);
                 actionPause->setEnabled(true);
                 actionPause->setChecked(false);
                 actionStop->setEnabled(true);
                // if(oldState == Phonon::PausedState)
                    //emettreCommande();
                // else
                    emettreCommande(lectureMorceau,"",m_time);
                }
                 break;
         case Phonon::StoppedState:
                 qDebug() << "stopped State";
                 //arreter();
                 if(!G_joueur)
                 {
                     emettreCommande(arretMorceau);
                     actionStop->setEnabled(false);
                     actionLecture->setEnabled(true);
                     actionLecture->setChecked(false);
                     actionPause->setChecked(false);
                     actionPause->setEnabled(false);
                     afficheurTemps->display("00:00");
                 }
                 break;
         case Phonon::PausedState:
                 qDebug() << "paused State";
                 /// @attention Workaround for phonon issue with some file. Pause state is sometime
                 if(!G_joueur)
                 {
                     if(!actionPause->isChecked())
                     {
                         isAboutToFinish();
                         qDebug() << "Something strange, perhaps the audio file is corrumpted";
                     }


                     emettreCommande(pauseMorceau);
                     actionStop->setEnabled(true);
                     actionLecture->setEnabled(true);
                     actionLecture->setChecked(false);
                     actionPause->setEnabled(false);
                 }
                 break;
         case Phonon::BufferingState:
                 qDebug() << "buffering State";
                 break;
         default:
                 qDebug() << "default State";
              break;
     }

}
void LecteurAudio::playerWidget()
{
                afficheurTitre->setToolTip(tr("Aucun titre"));
                widgetCommande->hide();
                QWidget *separateur3 = new QWidget();
                separateur3->setFixedHeight(2);
                layoutPrincipal->addWidget(separateur3);
                setFixedHeight(66);

}
void LecteurAudio::updatePlayingMode()
{
    QAction* tmp = static_cast<QAction*>(sender());
    if((tmp==actionBoucle)&&(actionBoucle->isChecked())&&(actionUnique->isChecked()))
    {
        actionUnique->setChecked(false);
    }
    if((tmp==actionUnique)&&(actionBoucle->isChecked())&&(actionUnique->isChecked()))
    {
        actionBoucle->setChecked(false);
    }


    if(actionBoucle->isChecked())
        m_currentPlayingMode=LOOP;
    else if(actionUnique->isChecked())
        m_currentPlayingMode=UNIQUE;
    else if((!actionUnique->isChecked())&&(!actionBoucle->isChecked()))
        m_currentPlayingMode=NEXT;


    qDebug()<< "mode playing =" <<m_currentPlayingMode;

}
void LecteurAudio::addFiles()
{

        QStringList listeFichiers = QFileDialog::getOpenFileNames(this, tr("Ajouter un titre"), G_initialisation.dossierMusiquesMj, tr("Fichiers audio (*.wav *.mp2 *.mp3 *.ogg *.flac)"));
        qDebug() << "ajouterTitre" << listeFichiers;
        if (listeFichiers.isEmpty())
                return;
        int dernierSlash = listeFichiers[0].lastIndexOf("/");
        G_initialisation.dossierMusiquesMj = listeFichiers[0].left(dernierSlash);
        while (!listeFichiers.isEmpty())
        {
                QString fichier = listeFichiers.takeFirst();
                int dernierSlash = fichier.lastIndexOf("/");
                QString titre = fichier.right(fichier.length()-dernierSlash-1);


                if (listeChemins.isEmpty())
                {
                        qDebug() << "emettre ajouter titre=" << titre;
                        emettreCommande(nouveauMorceau, titre);

                        // On active tous les boutons
                        actionLecture->setEnabled(true);
                        actionPause->setEnabled(true);
                        actionStop->setEnabled(true);
                        actionBoucle->setEnabled(true);
                        actionUnique->setEnabled(true);
                        actionSupprimer->setEnabled(true);
                        positionTemps->setEnabled(true);

                }
                QListWidgetItem *morceau = new QListWidgetItem(titre, listeTitres);
                morceau->setToolTip(fichier);
                listeChemins.append(fichier);
        }
}
void LecteurAudio::removeFile()
{
       qDebug() << "removeFile";
        QList<QListWidgetItem *> titreSelectionne = listeTitres->selectedItems();
        if (titreSelectionne.isEmpty())
                return;

        listeTitres->removeItemWidget(titreSelectionne[0]);
        listeChemins.removeAt(listeTitres->row(titreSelectionne[0]));
        if (titreSelectionne[0]->text() == m_currentFile)
        {
                /*if (titreCourant < listeChemins.size())
                {
                        emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
                        //nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);
                }
                else if (titreCourant != 0)
                {
                        titreCourant--;
                        emettreCommande(nouveauMorceau, listeTitres->item(titreCourant)->text());
                        //nouveauTitre(listeTitres->item(titreCourant)->text(), listeChemins[titreCourant]);
                }
                else
                {
                        emettreCommande(nouveauMorceau, "");
                        //arreter();
                        afficheurTitre->clear();
                        afficheurTitre->setToolTip("");
                        actionLecture->setEnabled(false);
                        actionPause->setEnabled(false);
                        actionStop->setEnabled(false);
                        actionBoucle->setEnabled(false);
                        actionUnique->setEnabled(false);
                        actionSupprimer->setEnabled(false);
                        positionTemps->setEnabled(false);
                }*/
        }
       // else if (ligne < titreCourant)
       //         titreCourant--;
}

void LecteurAudio::isAboutToFinish()
{
        if(G_joueur)
            return;

        else if (m_currentPlayingMode==UNIQUE)
        {
                emettreCommande(arretMorceau);
                mediaObject->stop();
                qDebug() << "finDeTitreSlot" << "lecture unique";
        }
        else if (m_currentPlayingMode==NEXT)
        {

        }
}

void LecteurAudio::emettreCommande(actionMusique action, QString nomFichier, quint64 position, int numeroLiaison)
{
        qDebug() << " emettre commande " << action << nomFichier << position << numeroLiaison;
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
                        qWarning("Commande inconnue envoyee aux lecteurs audio des autres utilisateurs");
                        break;
        }
        if(donnees == NULL)
            return;

        memcpy(donnees, &uneEntete, sizeof(enteteMessage));

        if (numeroLiaison == -1)
          emettre(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage));
        else
          emettre(donnees, uneEntete.tailleDonnees + sizeof(enteteMessage), numeroLiaison);

        delete[] donnees;
}


void LecteurAudio::emettreEtat(int numeroLiaison)
{
        if(afficheurTitre->text().isEmpty())
           emettreCommande(nouveauMorceau, "", 0, numeroLiaison);
        else
        {
            emettreCommande(nouveauMorceau, afficheurTitre->text(), 0, numeroLiaison);
            switch(mediaObject->state())
            {
                case Phonon::PausedState:
                    emettreCommande(pauseMorceau,"", 0, numeroLiaison);
                    emettreCommande(nouvellePositionMorceau, "", m_time, numeroLiaison);
                    break;
                case Phonon::StoppedState:
                    emettreCommande(arretMorceau, "",0, numeroLiaison);
                    break;
            case Phonon::PlayingState :
                    emettreCommande(pauseMorceau,"", 0, numeroLiaison);
                    emettreCommande(nouvellePositionMorceau, "", m_time, numeroLiaison);
                    emettreCommande(lectureMorceau, "", 0, numeroLiaison);
                    break;
                default :
                    break;
            }
        }
}

qreal LecteurAudio::volume()
{
        return audioOutput->volume();
}



void LecteurAudio::pplay()
{
    qDebug() << " pplay ";
    mediaObject->play();
}

void LecteurAudio::ppause()
{
    mediaObject->pause();
}

void LecteurAudio::pstop()
{
    mediaObject->stop();
}
void LecteurAudio::pselectNewFile(QString file)
{
        m_currentFile = file;
        qDebug() << " file = " << m_currentFile;

    if(m_currentFile.isEmpty())
    {
        qDebug() << " file vide= " << m_currentFile;
        afficheurTitre->clear();
        afficheurTitre->setToolTip(tr("Aucun titre"));
        mediaObject->stop();
    }
    else
    {


        QString path(tr("%1/%2").arg(G_initialisation.dossierMusiquesMj).arg(m_currentFile));
        QFileInfo fileInfo(path);
        if (!fileInfo.exists())
        {
                    qDebug() << " file existe pas = " << path;
            QPalette palette(afficheurTitre->palette());
            palette.setColor(QPalette::Normal, QPalette::Text, Qt::red);
            afficheurTitre->setPalette(palette);
            afficheurTitre->setText(tr("%1 : fichier introuvable ou impossible  ouvrir").arg(file));
            afficheurTitre->setToolTip(tr("Fichier introuvable ou impossible  ouvrir : %1").arg(path));
        }
        else
        {
            qDebug() << " file existe = " << path;
            currentsource = new Phonon::MediaSource(m_currentFile);
            mediaObject->setCurrentSource(*currentsource);
            afficheurTitre->setEchoMode(QLineEdit::Password);
            QPalette palette(afficheurTitre->palette());
            palette.setColor(QPalette::Normal, QPalette::Text, Qt::black);
            afficheurTitre->setPalette(palette);
            afficheurTitre->setText(m_currentFile);
        }



    }

}
void LecteurAudio::pseek(quint32 position)
{
    if(mediaObject->isSeekable())
            mediaObject->seek(position);
}

void LecteurAudio::pChangeDirectory()
{

        G_initialisation.dossierMusiquesMj = QFileDialog::getExistingDirectory(0 , tr("Choix du r�pertoire des musiques"), G_initialisation.dossierMusiquesMj,
        QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
}
