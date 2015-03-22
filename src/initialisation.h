/*************************************************************************
 *     Copyright (C) 2007 by Romain Campioni                             *
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


#ifndef INITIALISATION_H
#define INITIALISATION_H

#include <QColor>


#define COLOR_TAB_SIZE 16
/**
 * @brief Hold session parameters
 * @note
 * This class comes from a struct. A useful constructor and destructor that read and write from/to
 * a file has been added.
 */
class Initialisation
{
private:
    /**
     * @brief Constructor
     * Get last session parameters from file or set defaults.
     */
    Initialisation();
public:


    /**
     * @brief Destructor
     * Write parameters to file.
     */
    ~Initialisation();



    void saveConfiguration();
    QString getUserName() const;
    QColor getUserColor() const;
    bool isPlayer() const;
    bool isClient() const;
    QString getIpAddress() const;
    quint16 getServerPort() const;
    QString getClientPort() const;
    QString getMusicDirectoryGM() const;
    QString getMusicDirectoryPlayer() const;
    QString getImageDirectory() const;
    QString getMapDirectory() const;
    QString getScenarioDirectory() const;
    QString getMinutesDirectory() const;
    QString getChatDirectory() const;
    QColor getCustomColorAt(int) const;
    int getVolumeLevel() const;

    void setUserName(QString);
    void setUserColor(QColor);
    void setPlayer(bool);
    void setClient(bool);
    void setIpAddress(QString);
    void setServerPort(quint16);
    void setClientPort(QString);
    void setMusicDirectoryGM(QString);
    void setMusicDirectoryPlayer(QString);
    void setImageDirectory(QString);
    void setMapDirectory(QString);
    void setScenarioDirectory(QString);
    void setMinutesDirectory(QString);
    void setChatDirectory(QString);
    void setCustomColorAt(int,QColor);
    void setVolumeLevel(int);

    QString getApplicationName() const;

    static Initialisation* getInstance();


    void createDirectories();

    bool mustCheckUpdate();
    void setCheckUpdate(bool );

private:
    static Initialisation* m_singleton;
    QString m_confdir;
    QString m_filename;
    QString m_version;
    QString m_applicationName; /// store application name.


    QString m_userName;
    QColor m_userColor;
    bool m_player;
    bool m_client;
    QString m_ipAddress;
    quint16 m_serverPort;
    QString m_clientPort;
    QString m_musicDirectoryGM;
    QString m_musicDirectoryPlayer;
    QString m_imageDirectory;
    QString m_mapDirectory;
    QString m_scenarioDirectory;
    QString m_minutesDirectory;
    QString m_chatDirectory;
    QColor m_customColor[COLOR_TAB_SIZE];
    int m_volumeLevel;
    bool m_mustCheckUpdate;
};

#endif
