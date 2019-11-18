/*************************************************************************
 *    Copyright (C) 2011 by Renaud Guezennec                             *
 *    Copyright (C) 2011 by Joseph Boudou                                *
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

#include "updatechecker.h"

#include <QDebug>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegExp>
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
    m_manager->get(QNetworkRequest(QUrl("http://www.rolisteam.org/version.xml")));
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

    QRegExp versionMajor("<version_major>(.*)</version_major>");
    QRegExp versionMiddle("<version_middle>(.*)</version_middle>");
    QRegExp versionMinor("<version_minor>(.*)</version_minor>");
    QRegExp date("<date>(.*)</date>");
    QRegExp changelog("<date>(.*)</date>");

    QString string(a);
    int pos= versionMajor.indexIn(string);
    int major= 0;
    int middle= 0;
    int minor= 0;
    if(pos != -1)
    {
        major= versionMajor.capturedTexts().at(1).toInt();
    }
    pos= versionMiddle.indexIn(string);
    if(pos != -1)
    {
        middle= versionMiddle.capturedTexts().at(1).toInt();
    }
    pos= versionMinor.indexIn(string);
    if(pos != -1)
    {
        minor= versionMinor.capturedTexts().at(1).toInt();
    }
    pos= date.indexIn(string);
    if(pos != -1)
    {
        m_dateRemoteVersion= date.capturedTexts().at(1);
    }
    pos= changelog.indexIn(string);
    if(pos != -1)
    {
        m_changeLog= changelog.capturedTexts().at(1);
    }
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
