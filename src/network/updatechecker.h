/***************************************************************************
 *	Copyright (C) 2011 by Renaud Guezennec                             *
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
#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H
#include <QString>
#include <QTcpSocket>
#include <QNetworkAccessManager>
/**
  * @brief read an xml file on the project's server. If latest version has been released. Users are notified
  *
  *
  */
class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    /**
      * @brief default constructor
      */
    UpdateChecker();
    /**
      * @brief gives the final answer, should not be called until receiving checkFinished signal.
      * @return bool true: new release available, false otherwise.
      */
    bool mustBeUpdated();
    /**
      * @brief start checking will get the appropriate file and extract informations from it.
      */
    void startChecking();

    /**
      * @brief accessor to get the version number.
      * @return the version number (must look like that: 1.0.0)
      */
    QString& getLatestVersion();
    /**
      * @brief accessor to get the release date.
      * @return the date in QString
      */
    QString& getLatestVersionDate();
signals:
    /**
      * @brief emited when it's done
      */
    void checkFinished();

private slots:
    /**
      * @brief internal member to extract information of xml file.
      */
    void readXML(QNetworkReply* p);

private:
    /**
      * @brief version number
      */
    QString m_version;
    /**
      * @brief version date
      */
    QString m_versionDate;
    /**
      * @brief socket to reach/read the file.
      */
    QTcpSocket m_socket;
    /**
      * @brief easier way to use http connection.
      */
    QNetworkAccessManager* m_manager;
    /**
      * @brief stored the final answer.
      */
    bool m_state;
};

#endif // UPDATECHECKER_H
