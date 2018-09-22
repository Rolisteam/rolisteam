/***************************************************************************
 *     Copyright (C) 2018 by Renaud Guezennec                              *
 *     http://www.rolisteam.org/                                           *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef UPLOADTOSERVER_H
#define UPLOADTOSERVER_H

#include <QObject>
#include <vector>

struct Log {
   QString m_category;
   QString m_message;
   QString m_timestamp;
   QString m_level;
};
class QNetworkAccessManager;
/**
 * @brief The LogUploader class dedicated to be call from a thread to send its log messages.
 */
class LogUploader : public QObject
{
    Q_OBJECT
public:
    LogUploader();

    int appId() const;
    void setAppId(int appId);

    QString version() const;
    void setVersion(const QString &version);

    QString uuid() const;
    void setUuid(const QString &uuid);

    std::vector<Log> logs() const;
    void setLogs(const std::vector<Log> &logs);

    QString conf() const;
    void setConf(const QString &conf);

public slots:
    void uploadLog();
signals:
    void finished();
private:
    std::vector<Log> m_logs;
    int m_appId = 0;
    QString m_version;
    QString m_uuid;
    QString m_conf;
    QNetworkAccessManager* m_accessManager = nullptr;
    QByteArray m_postData;
};


#endif // UPLOADTOSERVER_H
