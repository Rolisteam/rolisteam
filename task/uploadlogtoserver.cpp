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
#include "uploadlogtoserver.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <iostream>

LogUploader::LogUploader() : QObject(nullptr), m_accessManager(new QNetworkAccessManager(this))
{
    connect(m_accessManager, &QNetworkAccessManager::finished, this, &LogUploader::finished);
}

int LogUploader::appId() const
{
    return m_appId;
}

void LogUploader::setAppId(int appId)
{
    m_appId= appId;
}

QString LogUploader::version() const
{
    return m_version;
}

void LogUploader::setVersion(const QString& version)
{
    m_version= version;
}

QString LogUploader::uuid() const
{
    return m_uuid;
}

void LogUploader::setUuid(const QString& uuid)
{
    m_uuid= uuid;
}

std::vector<Log> LogUploader::logs() const
{
    return m_logs;
}

void LogUploader::setLogs(const std::vector<Log>& logs)
{
    m_logs= logs;
}

void LogUploader::uploadLog()
{
    QJsonObject obj;
    obj["uuid"]= m_uuid;
    obj["version"]= m_version;
    obj["app"]= m_appId;
    obj["conf"]= m_conf;

    QJsonArray array;
    for(auto& log : m_logs)
    {
        QJsonObject logObj;
        logObj["level"]= log.m_level;
        logObj["log"]= log.m_message;
        logObj["category"]= log.m_category;
        logObj["timestamp"]= log.m_timestamp;

        array.append(logObj);
    }
    obj["logs"]= array;

    QJsonDocument doc;
    doc.setObject(obj);

    QUrl url("http://www.rolisteam.org/php/uploadlog.php");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    m_postData.clear();
    m_postData.append("log=");
    m_postData.append(doc.toJson());
    m_accessManager->post(request, m_postData);
}

QString LogUploader::conf() const
{
    return m_conf;
}

void LogUploader::setConf(const QString& conf)
{
    m_conf= conf;
}
