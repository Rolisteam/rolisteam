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
QHash<CleverURI::ContentType,QString> CleverURI::m_iconPathHash={
    {CleverURI::NONE,""},
    {CleverURI::MAP,":/map.png"},
    {CleverURI::VMAP,":/map.png"},
    {CleverURI::CHAT,":/resources/icons/scenario.png"},
    {CleverURI::PICTURE,":/picture.png"},
    {CleverURI::ONLINEPICTURE,":/picture.png"},
    {CleverURI::TEXT,":/notes.png"},
    {CleverURI::CHARACTERSHEET,":/resources/icons/treeview.png"},
    {CleverURI::SCENARIO,":/story.png"},
    {CleverURI::SONG,":/resources/icons/audiofile.svg"},
    #ifdef WITH_PDF
    {CleverURI::PDF,":/iconpdf"},
    #endif
    {CleverURI::SONGLIST,":/resources/icons/playlist.svg"}
};

//enum ContentType {NONE,MAP,VMAP,CHAT,PICTURE,ONLINEPICTURE,TEXT,CHARACTERSHEET,SCENARIO,SONG,SONGLIST
QStringList CleverURI::m_typeNameList = QStringList() <<    QObject::tr("None") <<QObject::tr("Map") <<QObject::tr("Vectorial Map") <<QObject::tr("Chat")
                                                      <<    QObject::tr("Picture") <<QObject::tr("Online Picture") <<QObject::tr("Text") <<QObject::tr("Text") <<
                                                            QObject::tr("Charecter Sheet") <<QObject::tr("Scenario") <<QObject::tr("Song") <<QObject::tr("Song List");

QStringList CleverURI::m_typeToPreferenceDirectory = QStringList() <<   QString("SessionDirectory") <<QString("MapDirectory")       <<QString("MapDirectory")           <<QString("ChatDirectory")
                                                                   <<   QString("ImageDirectory")   <<QString("ImageDirectory")     <<QString("Text")                   <<QString("MinutesDirectory") <<
                                                                        QString("SessionDirectory") <<QString("SessionDirectory")   <<QString("MusicDirectoryPlayer")   <<QString("MusicDirectoryPlayer");
CleverURI::CleverURI()
    : m_type(NONE)
{

}

CleverURI::CleverURI(const CleverURI & mp)
{
    m_type=mp.getType();
    m_uri=mp.getUri();
    defineShortName();
}
QString CleverURI::getIcon()
{
    return m_iconPathHash[m_type];
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

QByteArray CleverURI::getData() const
{
    return m_data;
}

void CleverURI::setData(const QByteArray &data)
{
    m_data = data;
}

bool CleverURI::getDisplayed() const
{
    return m_displayed;
}

void CleverURI::setDisplayed(bool displayed)
{
    m_displayed = displayed;
}

CleverURI::LoadingMode CleverURI::getCurrentMode() const
{
    return m_currentMode;
}

void CleverURI::setCurrentMode(const LoadingMode &currentMode)
{
    m_currentMode = currentMode;
}
const QString CleverURI::getAbsolueDir() const
{
    QFileInfo info(m_uri);
    
    return info.absolutePath();
}

void CleverURI::write(QDataStream &out) const
{

}

void CleverURI::read(QDataStream &in)
{

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
        return QString();
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
void CleverURI::loadFileFromUri()
{
    QFile file(m_uri);
    if(file.open(QIODevice::ReadOnly))
    {
        m_data = file.readAll();
    }
}

void CleverURI::clearData()
{
    m_data.clear();
}

QDataStream& operator<<(QDataStream& out, const CleverURI& con)
{
    out << con.getUri();
    out << (int)con.getType();
    return out;
}

QDataStream& operator>>(QDataStream& is,CleverURI& con)
{
    QString str;
    is >> str;

    int type;
    is >> type;


    con.setType((CleverURI::ContentType)type);
    con.setUri(str);
    return is;
}

QDataStream& operator<<(QDataStream& out, const CleverUriList& con)
{
    out << con.size();
    foreach(CleverURI uri, con)
    {
        out << uri;
    }

    return out;
}

QDataStream& operator>>(QDataStream& is,CleverUriList& con)
{
    int count;
    is >> count;
    for(int i = 0;i<count;++i)
    {
        CleverURI* uri = new CleverURI();
        is >> *uri;
        con.append(*uri);
    }
    return is;
}
