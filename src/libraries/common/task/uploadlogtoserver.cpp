/***************************************************************************
 *     Copyright (C) 2018 by Renaud Guezennec                              *
 *     https://rolisteam.org/                                           *
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
#include "common/uploadlogtoserver.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <iostream>

namespace logger
{
constexpr auto KEY_UUID{"uuid"};
constexpr auto KEY_VERSION{"version"};
constexpr auto KEY_APP{"app"};
constexpr auto KEY_CONF{"conf"};
constexpr auto KEY_LEVEL{"level"};
constexpr auto KEY_LOG{"log"};
constexpr auto KEY_CATEGORY{"category"};
constexpr auto KEY_TIME{"timestamp"};
constexpr auto KEY_LOGS{"logs"};
constexpr auto KEY_URL{"https://rolisteam.org/php/uploadlog.php"};
constexpr auto KEY_MIME{"application/x-www-form-urlencoded"};
} // namespace logger

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

std::vector<common::Log> LogUploader::logs() const
{
    return m_logs;
}

void LogUploader::setLogs(const std::vector<common::Log>& logs)
{
    m_logs= logs;
}

void LogUploader::uploadLog()
{
    QJsonObject obj;

    obj[logger::KEY_UUID]= m_uuid;
    obj[logger::KEY_VERSION]= m_version;
    obj[logger::KEY_APP]= m_appId;
    obj[logger::KEY_CONF]= m_conf;

    QJsonArray array;
    for(auto& log : m_logs)
    {
        QJsonObject logObj;
        logObj[logger::KEY_LEVEL]= log.m_level;
        logObj[logger::KEY_LOG]= log.m_message;
        logObj[logger::KEY_CATEGORY]= log.m_category;
        logObj[logger::KEY_TIME]= log.m_timestamp;

        array.append(logObj);
    }
    obj[logger::KEY_LOGS]= array;

    QJsonDocument doc;
    doc.setObject(obj);

    QUrl url(logger::KEY_URL);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, logger::KEY_MIME);
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
