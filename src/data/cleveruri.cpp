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


#include "preferences/preferencesmanager.h"

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
QString CleverURI::m_empty=QString("");

//enum ContentType {NONE,MAP,VMAP,CHAT,PICTURE,ONLINEPICTURE,TEXT,CHARACTERSHEET,SCENARIO,SONG,SONGLIST
QStringList CleverURI::m_typeNameList = QStringList() <<    QObject::tr("None") <<QObject::tr("Map") <<QObject::tr("Vectorial Map") <<QObject::tr("Chat")
                                                      <<    QObject::tr("Picture") <<QObject::tr("Online Picture") <<QObject::tr("Text") <<QObject::tr("Text") <<
                                                            QObject::tr("Charecter Sheet") <<QObject::tr("Scenario") <<QObject::tr("Song") <<QObject::tr("Song List");

QStringList CleverURI::m_typeToPreferenceDirectory = QStringList() <<   QString("SessionDirectory") <<QString("MapDirectory")       <<QString("MapDirectory")           <<QString("ChatDirectory")
                                                                   <<   QString("ImageDirectory")   <<QString("ImageDirectory")     <<QString("Text")                   <<QString("MinutesDirectory") <<
                                                                        QString("SessionDirectory") <<QString("SessionDirectory")   <<QString("MusicDirectoryPlayer")   <<QString("MusicDirectoryPlayer");

#ifdef WITH_PDF
QString CleverURI::m_pdfIcon=QString(":/iconpdf");
#endif

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
QString CleverURI::getFilterForType(CleverURI::ContentType type) //static
{
    PreferencesManager* preferences=PreferencesManager::getInstance();
    switch(type)
    {
    case CleverURI::CHARACTERSHEET:
       return QString();
        break;
    case CleverURI::PICTURE:
        return QObject::tr("Supported Image formats (%1)").arg(preferences->value("ImageFileFilter","*.jpg *.jpeg *.png *.bmp *.svg").toString());
        break;
    case CleverURI::MAP:
        return QString();
        break;
    case CleverURI::CHAT:
        return QString();
        break;
    case CleverURI::TEXT:
        return QString();
        break;
    case CleverURI::SCENARIO:
        return QString();
        break;
    case CleverURI::SONG:
        return QObject::tr("Supported Audio formats (%1)").arg(preferences->value("AudioFileFilter","*.wav *.mp2 *.mp3 *.ogg *.flac").toString());
        break;
    case CleverURI::ONLINEPICTURE:
        return QString();
        break;
    case CleverURI::VMAP:
        //return QObject::tr("Supported Image formats (%1)").arg(preferences->value("VMapFileFilter","j,*.jpg *.jpeg *.png *.bmp *.svg").toString());
        return QString();
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
QString CleverURI::typeToString(CleverURI::ContentType type)
{
    if(m_typeNameList.size()>(int)type)
    {
        return m_typeNameList.at((int)type);
    }
}

QString CleverURI::getPreferenceDirectoryKey(CleverURI::ContentType type)
{
    if(m_typeToPreferenceDirectory.size()>(int)type)
    {
        return m_typeToPreferenceDirectory.at((int)type);
    }
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
