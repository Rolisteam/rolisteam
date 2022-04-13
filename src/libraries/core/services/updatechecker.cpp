/*************************************************************************
 *    Copyright (C) 2011 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
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

#include "updatechecker.h"

#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>
/*****************
 * UpdateChecker *
 *****************/

UpdateChecker::UpdateChecker(const QString& version, QObject* obj)
    : QObject(obj), m_localVersion(version), m_manager(nullptr)
{
}

bool UpdateChecker::needUpdate()
{
    return m_needUpdate;
}

void UpdateChecker::startChecking()
{
#ifdef HAVE_QT_NETWORK
    m_manager.reset(new QNetworkAccessManager);
    connect(m_manager.get(), &QNetworkAccessManager::finished, this, &UpdateChecker::readXML);
    m_manager->get(QNetworkRequest(QUrl("https://rolisteam.org/version.xml")));
#endif
}

QString UpdateChecker::getLatestVersion()
{
    return m_remoteVersion;
}

QString UpdateChecker::getLatestVersionDate()
{
    return m_dateRemoteVersion;
}
void UpdateChecker::readXML(QNetworkReply* p)
{
    if(p->error() != QNetworkReply::NoError)
    {
        m_noErrror= false;
        emit checkFinished();
        return;
    }

    QByteArray a= p->readAll();
    QString string(a);
    auto func= [](const QString& string, const QRegularExpression& reg) -> QString {
        auto match= reg.match(string);
        if(match.hasMatch())
            return match.captured(0);
        return {};
    };

    int major= func(string, QRegularExpression("<version_major>(.*)</version_major>")).toInt();
    int middle= func(string, QRegularExpression("<version_middle>(.*)</version_middle>")).toInt();
    int minor= func(string, QRegularExpression("<version_minor>(.*)</version_minor>")).toInt();
    m_dateRemoteVersion= func(string, QRegularExpression("<date>(.*)</date>"));
    m_changeLog= func(string, QRegularExpression("<changelog>(.*)</changelog>"));

    m_remoteVersion= QString("%1.%2.%3").arg(major).arg(middle).arg(minor);

    setNeedUpdate(m_remoteVersion != m_localVersion);
    emit checkFinished();
}

void UpdateChecker::setNeedUpdate(bool b)
{
    if(m_needUpdate == b)
        return;
    m_needUpdate= b;
    emit needUpdateChanged();
}
