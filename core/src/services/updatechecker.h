/*************************************************************************
 *    Copyright (C) 2011 by Renaud Guezennec                             *
 *                                                                       *
 *      https://rolisteam.org/                                        *
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

#include <QNetworkAccessManager>
#include <QString>
#include <memory>
/**
 * @brief The UpdateChecker class is dedicated to check if there is new release of rolisteam.
 */
class UpdateChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool needUpdate READ needUpdate NOTIFY needUpdateChanged)
public:
    UpdateChecker(const QString& version, QObject* obj= nullptr);

    bool needUpdate();
    void startChecking();

    QString getLatestVersion();
    QString getLatestVersionDate();
signals:
    void checkFinished();
    void needUpdateChanged();

private slots:
    void readXML(QNetworkReply* p);
    void setNeedUpdate(bool b);

private:
    bool m_needUpdate= false;
    QString m_localVersion;
    QString m_remoteVersion;
    QString m_dateRemoteVersion;
    QString m_changeLog;
    std::unique_ptr<QNetworkAccessManager> m_manager;
    bool m_noErrror= false;
};

#endif // UPDATECHECKER_H
