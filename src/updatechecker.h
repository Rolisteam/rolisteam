/*************************************************************************
 *    Copyright (C) 2011 by Renaud Guezennec                             *
 *                                                                       *
 *      http://www.rolisteam.org/                                        *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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

#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QString>
#include <QTcpSocket>
#include <QNetworkAccessManager>
/**
 * @brief The UpdateChecker class
 */
class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    UpdateChecker();

    bool mustBeUpdated();
    void startChecking();

    QString& getLatestVersion();
    QString& getLatestVersionDate();
signals:
    void checkFinished();

private slots:
    void readXML(QNetworkReply* p);

private:
 bool inferiorVersion();

private:
    QString m_version;
    int m_versionMinor;
    int m_versionMajor;
    int m_versionMiddle;
    QString m_versionDate;
    QString m_versionChangelog;
    QTcpSocket m_socket;
    QNetworkAccessManager* m_manager;
    bool m_state;
    bool m_noErrror;
};

#endif // UPDATECHECKER_H
