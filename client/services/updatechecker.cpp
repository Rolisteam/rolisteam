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

UpdateChecker::UpdateChecker(QObject* obj)
    : QObject(obj), m_state(false), m_versionMinor(0), m_versionMajor(0), m_versionMiddle(0), m_manager(nullptr)
{
    m_noErrror= true;
#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    m_versionMinor= VERSION_MINOR;
    m_versionMajor= VERSION_MAJOR;
    m_versionMiddle= VERSION_MIDDLE;
#endif
#endif
#endif
}

bool UpdateChecker::mustBeUpdated()
{
    return m_state;
}

void UpdateChecker::startChecking()
{
#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    m_manager= new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(readXML(QNetworkReply*)));

    m_manager->get(QNetworkRequest(QUrl("http://www.rolisteam.org/version.xml")));
#endif
#endif
#endif
}

QString UpdateChecker::getLatestVersion()
{
    return m_version;
}

QString UpdateChecker::getLatestVersionDate()
{
    return m_versionDate;
}
void UpdateChecker::readXML(QNetworkReply* p)
{
    if(p->error() != QNetworkReply::NoError)
    {
        m_noErrror= false;
        return;
    }
#ifdef VERSION_MINOR
#ifdef VERSION_MAJOR
#ifdef VERSION_MIDDLE
    QByteArray a= p->readAll();

    QRegExp versionMajor("<version_major>(.*)</version_major>");
    QRegExp versionMiddle("<version_middle>(.*)</version_middle>");
    QRegExp versionMinor("<version_minor>(.*)</version_minor>");
    QRegExp date("<date>(.*)</date>");
    QRegExp changelog("<date>(.*)</date>");

    QString string(a);
    int pos= versionMajor.indexIn(string);
    if(pos != -1)
    {
        m_versionMajor= versionMajor.capturedTexts().at(1).toInt();
    }
    pos= versionMiddle.indexIn(string);
    if(pos != -1)
    {
        m_versionMiddle= versionMiddle.capturedTexts().at(1).toInt();
    }
    pos= versionMinor.indexIn(string);
    if(pos != -1)
    {
        m_versionMinor= versionMinor.capturedTexts().at(1).toInt();
    }
    pos= date.indexIn(string);
    if(pos != -1)
    {
        m_versionDate= date.capturedTexts().at(1);
    }
    pos= changelog.indexIn(string);
    if(pos != -1)
    {
        m_versionChangelog= changelog.capturedTexts().at(1);
    }
    m_state= inferiorVersion();
    m_version= QString("%1.%2.%3").arg(m_versionMajor).arg(m_versionMiddle).arg(m_versionMinor);
    emit checkFinished();
    m_manager->deleteLater();
#endif
#endif
#endif
}
bool UpdateChecker::inferiorVersion()
{
    if(m_versionMajor > VERSION_MAJOR)
        return true;
    else if((m_versionMajor == VERSION_MAJOR) && (m_versionMiddle > VERSION_MIDDLE))
        return true;
    else if((m_versionMajor == VERSION_MAJOR) && (m_versionMiddle == VERSION_MIDDLE)
            && (m_versionMinor > VERSION_MINOR))
        return true;
    else
        return false;
}
