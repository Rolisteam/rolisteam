/***************************************************************************
*	Copyright (C) 2009 by Renaud Guezennec                             *
*   http://renaudguezennec.homelinux.org/accueil,3.html                   *
*                                                                         *
*   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "cleveruri.h"
#include <QString>
#include <QDebug>


#include <QDataStream>
#include <QFileInfo>


/////////////////
// CleverUri
/////////////////
QString CleverURI::m_textIcon=QString(":/resources/icons/notes.png");
QString CleverURI::m_mapIcon=QString(":/map.png");
QString CleverURI::m_pictureIcon=QString(":/picture.png");
QString CleverURI::m_charactersheetIcon=QString(":/resources/icons/treeview.png");
QString CleverURI::m_scenarioIcon=QString(":/story.png");
QString CleverURI::m_chatIcon=QString(":/resources/icons/scenario.png");
QString CleverURI::m_musicIcon=QString(":/resources/icons/music.svg");
#ifdef WITH_PDF
QString CleverURI::m_pdfIcon=QString(":/iconpdf");
#endif
QString CleverURI::m_empty=QString("");

CleverURI::CleverURI()
{

}

CleverURI::CleverURI(const CleverURI & mp)
{
    m_type=mp.getType();
    m_uri=mp.getUri();
    defineShortName();
}
QString& CleverURI::getIcon()
{
    switch(m_type)
    {
    case CleverURI::CHARACTERSHEET:
        return m_charactersheetIcon;
        break;
    case CleverURI::PICTURE:
        return m_pictureIcon;
        break;
    case CleverURI::MAP:
        return m_mapIcon;
        break;
    case CleverURI::CHAT:
        return m_chatIcon;
        break;
    case CleverURI::TEXT:
        return m_textIcon;
        break;
    case CleverURI::SCENARIO:
        return m_scenarioIcon;
        break;
    case CleverURI::SONG:
        return m_musicIcon;
        break;
    case CleverURI::ONLINEPICTURE:
        return m_pictureIcon;
        break;
#ifdef WITH_PDF
    case CleverURI::PDF:
        return m_pdfIcon;
        break;
#endif
    default:
        return m_empty;
        break;
    }
}

CleverURI::CleverURI(QString uri,ContentType type)
    : m_uri(uri),m_type(type)
{
    defineShortName();
}

CleverURI::~CleverURI()
{
    // qDebug() << "Destructor of CleverURI";
}
bool CleverURI::operator==(const CleverURI& uri) const
{
    if((uri.getUri()==getUri())&&(uri.getType()==getType()))
        return true;
    return false;
}

void CleverURI::setUri(QString& uri)
{
    m_uri=uri;
    defineShortName();
}

void CleverURI::setType(CleverURI::ContentType type)
{
    m_type=type;
}

const QString CleverURI::getUri() const
{
    return m_uri;
}

CleverURI::ContentType CleverURI::getType() const
{
    return m_type;
}
bool CleverURI::hasChildren() const
{
    return false;
}
void CleverURI::defineShortName()
{
    QFileInfo info(m_uri);

    m_shortname = info.baseName();
}
const QString CleverURI::getAbsolueDir() const
{
    QFileInfo info(m_uri);

    return info.absolutePath();
}
const QString& CleverURI::getShortName() const
{
    return m_shortname;
}
void CleverURI::setShortName(QString& name)
{
    m_shortname = name;
}

QDataStream& operator<<(QDataStream& out, const CleverURI& con)
{
    out << con.getUri();
    out << (int)con.getType();
    return out;
}

QDataStream& operator>>(QDataStream& is,CleverURI& con)
{
    is >>(con.m_uri);
    int type;
    is >> type;
    con.m_type=(CleverURI::ContentType)type;
    con.defineShortName();
    return is;
}
