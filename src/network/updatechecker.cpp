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
#include "updatechecker.h"
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>


UpdateChecker::UpdateChecker()
{
    m_state = false;
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
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(readXML(QNetworkReply*)));
    m_manager->get(QNetworkRequest(QUrl("http://www.rolisteam.org/version.xml")));
#endif
#endif
#endif
}

QString& UpdateChecker::getLatestVersion()
{
    return m_version;
}

QString& UpdateChecker::getLatestVersionDate()
{
    return m_versionDate;
}
void UpdateChecker::readXML(QNetworkReply* p)
{
#ifdef VERSION
    QByteArray a= p->readAll();
    
    QRegExp version("<version>(.*)</version>");
    QRegExp date("<date>(.*)</date>");
    
    QString string(a);
    int pos = version.indexIn(string);
    if(pos!=-1)
    {
        m_version = version.capturedTexts().at(1);
    }
    pos = date.indexIn(string);
    if(pos!=-1)
    {
        m_versionDate = date.capturedTexts().at(1);
    }
    
    if(VERSION!=m_version)
        m_state=true;
    
    emit checkFinished();
#endif
}
