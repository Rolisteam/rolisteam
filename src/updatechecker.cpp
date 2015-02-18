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

#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>

/*****************
 * VersionParser *
 *****************/

class VersionParser
{
    public:
        VersionParser(const QString & version)
            : m_version(version), m_pos(0)
        {};

        int getNumber()
        {
            int ret = 0;
            int size = m_version.size();
            QChar current;

            while (m_pos < size && (current = m_version.at(m_pos++)).isDigit())
                ret = ret * 10 + current.digitValue();

            while (current == QChar('-') && m_pos < size)
                current = m_version.at(m_pos++);

            m_pos -= 1;
            return ret;
        }

        QChar getChar()
        {
            QChar ret;
            int size = m_version.size();

            if (m_pos >= size)
                return ret;

            ret = m_version.at(m_pos);

            if (ret.isDigit())
                return QChar();

            if (ret == QChar('.'))
            {
                while (m_version.at(++m_pos) == QChar('.')) {}
                return QChar();
            }

            m_pos += 1;
            return ret;
        }

        bool hasFinished()
        {   return m_pos >= m_version.size(); }

    private:
        QString m_version;
        int     m_pos;
};

bool inferiorVersion(const QString & version, const QString & than)
{
    VersionParser a(version);
    VersionParser b(than);
    int na, nb;
    QChar ca, cb;

    while (true)
    {
        na = a.getNumber();
        nb = b.getNumber();
        if (na != nb)
            return na < nb;

        do
        {
            ca = a.getChar();
            cb = b.getChar();
            if (ca != cb)
                return ca < cb;
        } while (!ca.isNull() && !cb.isNull());

        if (b.hasFinished())
            return false;
        if (a.hasFinished())
            return true;
    }
}


/*****************
 * UpdateChecker *
 *****************/

UpdateChecker::UpdateChecker()
    : m_state(false)
{
}

bool UpdateChecker::mustBeUpdated()
{
   return m_state;
}

void UpdateChecker::startChecking()
{
#ifdef VERSION
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(readXML(QNetworkReply*)));

     m_manager->get(QNetworkRequest(QUrl("http://www.rolisteam.org/version.xml")));
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

     m_state = inferiorVersion(VERSION, m_version);

    emit checkFinished();
#endif
}
