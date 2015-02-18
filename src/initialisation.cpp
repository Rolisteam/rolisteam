/*************************************************************************
 *	   Copyright (C) 2007 by Romain Campioni                             *
 *	   Copyright (C) 2009 by Renaud Guezennec                            *
 *     Copyright (C) 2010 by Joseph Boudou                               *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "initialisation.h"

#include <QDir>
#include <QApplication>

Initialisation* Initialisation::m_singleton = NULL;

Initialisation::Initialisation()
{
    m_applicationName = "rolisteam";//qApp->arguments().at(0);
    m_confdir = QString("%1/.%2").arg(QDir::homePath(), m_applicationName);
    m_filename = QString("%1/%2.ini").arg(m_confdir, m_applicationName);

    QFile file(m_filename);
    m_version = QObject::tr("Unknown");
    #ifdef VERSION_MINOR
        #ifdef VERSION_MAJOR
            #ifdef VERSION_MIDDLE
                m_version = QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MIDDLE).arg(VERSION_MINOR);
            #endif
        #endif
    #endif
    
    // Si le fichier d'initialisation existe, on le charge
    if (file.exists() && file.open(QIODevice::ReadOnly))
    {
        // On cree un flux de donnees rattache au fichier
        QDataStream fluxFichier(&file);
        
        // On recupere la version de l'application
        // This field is ignored actually.
        fluxFichier >> m_version;
        // ...le nom de l'utilisateur
        fluxFichier >> m_userName;
        // ...la couleur de l'utilisateur
        fluxFichier >> m_userColor;
        // ...la nature de l'utilisateur (joueur ou MJ)
        fluxFichier >> m_player;
        // ...la nature de l'ordinateur local
        fluxFichier >> m_client;
        // ...l'adresse IP du serveur
        fluxFichier >> m_ipAddress;
        // ...le port du serveur
        fluxFichier >> m_serverPort;
        if (m_serverPort < 1024)
            m_serverPort = 6660;
        // ...le port de connexion pour les clients
        fluxFichier >> m_clientPort;
        if (m_clientPort.toInt() < 1024)
            m_clientPort = QString("6660");
        // ...le chemin pour les musiques
        fluxFichier >> m_musicDirectoryGM;
        // ...le chemin pour les musiques des joueurs
        fluxFichier >> m_musicDirectoryPlayer;
        // ...le chemin pour les images
        fluxFichier >> m_imageDirectory;
        // ...le chemin pour les plans
        fluxFichier >> m_mapDirectory;
        // ...le chemin pour les scenarii
        fluxFichier >> m_scenarioDirectory;
        // ...le chemin pour les notes
        fluxFichier >> m_minutesDirectory;
        // ...le chemin pour les tchats
        fluxFichier >> m_chatDirectory;
        fluxFichier >> m_mustCheckUpdate;
        // ...les couleurs personnelles
        for (int i=0; i<COLOR_TAB_SIZE; i++)
            fluxFichier >> m_customColor[i];
        // ...le volume du lecteur audio
        fluxFichier >> m_volumeLevel;
        // Fermeture du fichier
        file.close();
    }

    // If we don't have the file, we set default values
    else
    {

        m_userName        = QString();
        m_userColor    = QColor();
        m_userColor.setHsv(qrand()%360, qrand()%200 + 56, qrand()%190 + 50);
        m_player                = true;
        m_client                = true;
        m_ipAddress             = QString();
        m_serverPort           = 6660;
        m_clientPort            = QString().setNum(m_serverPort);
        m_musicDirectoryGM     = m_confdir + QObject::tr("/audio");
        m_musicDirectoryPlayer = m_musicDirectoryGM;
        m_imageDirectory         = m_confdir + QObject::tr("/images");
        m_mapDirectory          = m_confdir + QObject::tr("/maps");
        m_scenarioDirectory       = m_confdir + QObject::tr("/scenario");
        m_minutesDirectory          = m_confdir + QObject::tr("/minutes");
        m_chatDirectory         = m_confdir + QObject::tr("/chat");
        m_mustCheckUpdate = true;
        for (int i=0; i<COLOR_TAB_SIZE; i++)
        {
            m_customColor[i] = QColor(i*COLOR_TAB_SIZE, i*COLOR_TAB_SIZE, i*COLOR_TAB_SIZE);
        }
        m_volumeLevel          = 100;

    }
    createDirectories();
}
Initialisation::~Initialisation()
{

}

void Initialisation::saveConfiguration()
{
    createDirectories();
    QFile file(m_filename);

    // Ouverture du fichier en ecriture seule
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning("Unable to open parameters file for writing. No parameters will be saved.");
        return;
    }

    // On cree un flux de donnees rattache au fichier
    QDataStream fluxFichier(&file);

    fluxFichier << m_version;
    fluxFichier << m_userName;
    fluxFichier << m_userColor;
    fluxFichier << m_player;
    fluxFichier << m_client;
    fluxFichier << m_ipAddress;
    fluxFichier << m_serverPort;
    fluxFichier << m_clientPort;
    fluxFichier << m_musicDirectoryGM;
    fluxFichier << m_musicDirectoryPlayer;
    fluxFichier << m_imageDirectory;
    fluxFichier << m_mapDirectory;
    fluxFichier << m_scenarioDirectory;
    fluxFichier << m_minutesDirectory;
    fluxFichier << m_chatDirectory;
    fluxFichier << m_mustCheckUpdate;
    for (int i=0; i<COLOR_TAB_SIZE; i++)
            fluxFichier << m_customColor[i];
    fluxFichier << m_volumeLevel;
    file.close();
}   
Initialisation* Initialisation::getInstance()
{
    if(m_singleton==NULL)
        m_singleton = new Initialisation();

    return m_singleton;
}
QString Initialisation::getApplicationName() const
{
    return m_applicationName;
}

void Initialisation::createDirectories()
{
    QDir  dir(m_confdir);
    if (!dir.exists())
        dir.mkpath(m_confdir);

    dir.setPath(m_musicDirectoryGM);
    if (!dir.exists())
        dir.mkpath(m_musicDirectoryGM);

     dir.setPath(m_musicDirectoryPlayer);
    if (!dir.exists())
        dir.mkpath(m_musicDirectoryPlayer);

     dir.setPath(m_imageDirectory);
    if (!dir.exists())
        dir.mkpath(m_imageDirectory);

      dir.setPath(m_mapDirectory);
    if (!dir.exists())
        dir.mkpath(m_mapDirectory);

     dir.setPath(m_scenarioDirectory);
    if (!dir.exists())
        dir.mkpath(m_scenarioDirectory);

     dir.setPath(m_minutesDirectory);
    if (!dir.exists())
        dir.mkpath(m_minutesDirectory);


     dir.setPath(m_chatDirectory);
    if (!dir.exists())
        dir.mkpath(m_chatDirectory);

}



QString Initialisation::getUserName() const
{
return m_userName;
}

QColor Initialisation::getUserColor() const
{
    return m_userColor;
}

bool Initialisation::isPlayer() const
{
return m_player;
}

bool Initialisation::isClient() const
{
    return m_client;
}

QString Initialisation::getIpAddress() const
{
return m_ipAddress;
}

quint16 Initialisation::getServerPort() const
{
return m_serverPort;
}

QString Initialisation::getClientPort() const
{
    return m_clientPort;
}

QString Initialisation::getMusicDirectoryGM() const
{
return m_musicDirectoryGM;
}

QString Initialisation::getMusicDirectoryPlayer() const
{
 return m_musicDirectoryPlayer;
}

QString Initialisation::getImageDirectory() const
{
return m_imageDirectory;
}

QString Initialisation::getMapDirectory() const
{
return m_mapDirectory;
}

QString Initialisation::getScenarioDirectory() const
{
return m_scenarioDirectory;
}

QString Initialisation::getMinutesDirectory() const
{
    return m_minutesDirectory;
}

QString Initialisation::getChatDirectory() const
{
    return m_chatDirectory;
}

QColor Initialisation::getCustomColorAt(int i) const
{
    Q_ASSERT(i < COLOR_TAB_SIZE);
    return m_customColor[i];

}

int Initialisation::getVolumeLevel() const
{
    return m_volumeLevel;
}

void Initialisation::setUserName(QString value)
{
    m_userName = value;
}

void Initialisation::setUserColor(QColor  value)
{
    m_userColor = value;
}
void Initialisation::setPlayer(bool value)
{
    m_player = value;
}
void Initialisation::setClient(bool value)
{
    m_client = value;
}
void Initialisation::setIpAddress(QString value)
{
    m_ipAddress = value;
}
void Initialisation::setServerPort(quint16 value)
{
    m_serverPort = value;
}
void Initialisation::setClientPort(QString value)
{
    m_clientPort = value;
}
void Initialisation::setMusicDirectoryGM(QString value)
{
    m_musicDirectoryGM = value;
}
void Initialisation::setMusicDirectoryPlayer(QString value)
{
    m_musicDirectoryPlayer = value;
}
void Initialisation::setImageDirectory(QString value)
{
m_imageDirectory = value;
}
void Initialisation::setMapDirectory(QString value)
{
    m_mapDirectory = value;
}
void Initialisation::setScenarioDirectory(QString value)
{
    m_scenarioDirectory = value;
}
void Initialisation::setMinutesDirectory(QString value)
{
    m_minutesDirectory = value;
}
void Initialisation::setChatDirectory(QString value)
{
    m_chatDirectory = value;
}
void Initialisation::setCustomColorAt(int index,QColor value)
{
    Q_ASSERT(index < COLOR_TAB_SIZE);
    m_customColor[index] = value;
}
void Initialisation::setVolumeLevel(int  value)
{
    m_volumeLevel = value;
}
bool Initialisation::mustCheckUpdate()
{
    return m_mustCheckUpdate;
}

void Initialisation::setCheckUpdate(bool value)
{
    m_mustCheckUpdate = value;
}
