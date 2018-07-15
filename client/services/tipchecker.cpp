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


#include "tipchecker.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
/*****************
 * UpdateChecker *
 *****************/

TipChecker::TipChecker(QObject* obj)
    : QObject(obj)
{
    m_noErrror = true;
}

bool TipChecker::hasArticle()
{
   return m_state;
}

void TipChecker::startChecking()
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager,&QNetworkAccessManager::finished,this, &TipChecker::readJSon);
    m_manager->get(QNetworkRequest(QUrl("http://www.rolisteam.org/tips.json")));
}

QString TipChecker::getArticleContent()
{
    return m_msg;
}

QString TipChecker::getUrl()
{
    return m_url;
}

QString TipChecker::getArticleTitle()
{
    return m_title;
}
void TipChecker::readJSon(QNetworkReply* p)
{
    if(p->error()!=QNetworkReply::NoError)
    {
        m_noErrror = false;
        emit checkFinished();
        return;
    }

    QByteArray a = p->readAll();
    QJsonParseError error;
    QJsonDocument doc(QJsonDocument::fromJson(a,&error));
    if(error.error != QJsonParseError::NoError)
    {
        m_noErrror = false;
        emit checkFinished();
        return;
    }

    QJsonObject obj = doc.object();
    QString locale = QLocale::system().name();
    int pos = locale.indexOf('_');
    if(pos>=0)
    {
        locale.remove(locale.indexOf(pos,1));
    }


    if(!obj.contains(locale))
    {
        locale = "en";
    }
    if(obj.contains(locale))
    {
        QJsonObject lang = obj[locale].toObject();
        m_title = lang["title"].toString();
        m_msg = lang["msg"].toString();
        m_url = lang["url"].toString();
        m_id = lang["id"].toInt();
        if(!m_msg.isEmpty() && !m_title.isEmpty())
        {
            m_state = true;
        }

    }
    emit checkFinished();
}

int TipChecker::getId() const
{
    return m_id;
}

void TipChecker::setId(int value)
{
    m_id = value;
}

