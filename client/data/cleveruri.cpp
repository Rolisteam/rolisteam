/***************************************************************************
*	Copyright (C) 2009 by Renaud Guezennec                             *
*   http://www.rolisteam.org/contact                   *
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
    {CleverURI::VMAP,":/vmap.png"},
    {CleverURI::CHAT,":/resources/icons/scenario.png"},
    {CleverURI::PICTURE,":/resources/icons/photo.png"},
    {CleverURI::ONLINEPICTURE,":/resources/icons/photo.png"},
    {CleverURI::TEXT,":/notes.png"},
    {CleverURI::CHARACTERSHEET,":/resources/icons/treeview.png"},
    {CleverURI::SCENARIO,":/story.png"},
    {CleverURI::SONG,":/resources/icons/audiofile.svg"},
    {CleverURI::SHAREDNOTE,":/resources/icons/sharedEditor.png"},
    {CleverURI::WEBVIEW,":/resources/icons/webPage.svg"},
    #ifdef WITH_PDF
    {CleverURI::PDF,":/resources/icons/pdfLogo.png"},
    #endif
    {CleverURI::SONGLIST,":/resources/icons/playlist.svg"}
};

//enum ContentType {NONE,MAP,VMAP,CHAT,PICTURE,ONLINEPICTURE,TEXT,CHARACTERSHEET,SCENARIO,SONG,SONGLIST
QStringList CleverURI::m_typeNameList = QStringList() << QObject::tr("None")
                                                      << QObject::tr("Map")
                                                      << QObject::tr("Vectorial Map")
                                                      << QObject::tr("Chat")
                                                      << QObject::tr("Picture")
                                                      << QObject::tr("Online Picture")
                                                      << QObject::tr("Text")
                                                      << QObject::tr("Charecter Sheet")
                                                      << QObject::tr("Scenario")
                                                      << QObject::tr("Song")
                                                      << QObject::tr("Song List")
                                                      << QObject::tr("Shared Notes");

QStringList CleverURI::m_typeToPreferenceDirectory = QStringList() <<   QString("SessionDirectory") <<QString("MapDirectory")       <<QString("MapDirectory")           <<QString("ChatDirectory")
                                                                   <<   QString("ImageDirectory")   <<QString("ImageDirectory")     <<QString("MinutesDirectory")       <<QString("CharacterSheetDirectory") <<
                                                                        QString("SessionDirectory") <<QString("MusicDirectoryPlayer")   <<QString("MusicDirectoryPlayer")<<QString("MinutesDirectory");
//CleverURIListener* CleverURI::s_listener = nullptr;

CleverURI::CleverURI()
    : m_type(NONE),m_state(Unloaded)
{
    init();
}

CleverURI::CleverURI(const CleverURI & mp)
{
    m_type=mp.getType();
    m_uri=mp.getUri();
    m_currentMode = mp.getCurrentMode();
    m_name = mp.name();
    m_state = mp.getState();
    updateListener(CleverURI::NAME);
}
QIcon CleverURI::getIcon()
{
    return QIcon(m_iconPathHash[m_type]);
}

CleverURI::CleverURI(QString name,QString uri,ContentType type)
    : m_uri(uri),m_type(type),m_state(Unloaded)
{
    m_name = name;
    updateListener(CleverURI::NAME);
    init();
}

CleverURI::~CleverURI()
{
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
    QFileInfo info(uri);
    m_name = info.baseName();
    loadData();
    updateListener(CleverURI::URI);
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
void CleverURI::setUpListener()
{
    updateListener(CleverURI::NAME);
}

void CleverURI::loadData()
{
    if(getCurrentMode() == Internal)
    {
        loadFileFromUri(m_data);
    }
    /*else
        m_data.clear();*/
}

void CleverURI::init()
{
     PreferencesManager* preferences=PreferencesManager::getInstance();
     m_currentMode = static_cast<LoadingMode>(preferences->value(QStringLiteral("DefaultLoadingMode"),static_cast<int>(Linked)).toInt());
}

CleverURI::State CleverURI::getState() const
{
    return m_state;
}

void CleverURI::setState(const State &state)
{
    m_state = state;
    updateListener(ResourcesNode::DISPLAYED);
}

bool CleverURI::hasData() const
{
    return !m_data.isEmpty();
}

void CleverURI::updateListener(CleverURI::DataValue value)
{
    for(CleverURIListener* listener : m_listenerList)
    {
        listener->cleverURIHasChanged(this,value);
    }
}


void CleverURI::setListener(CleverURIListener *value)
{
    if(!m_listenerList.contains(value))
    {
        m_listenerList.append(value);
    }
}


QByteArray CleverURI::getData() const
{
    return m_data;
}

void CleverURI::setData(const QByteArray &data)
{
    m_data = data;
}

bool CleverURI::isDisplayed() const
{
    return (m_state == Displayed);
}

void CleverURI::setDisplayed(bool displayed)
{
    if(displayed)
    {
        m_state = Displayed;
    }
    else
    {
        m_state = Hidden;
    }
    updateListener(ResourcesNode::DISPLAYED);
}

CleverURI::LoadingMode CleverURI::getCurrentMode() const
{
    return m_currentMode;
}

void CleverURI::setCurrentMode(const LoadingMode &currentMode)
{
    m_currentMode = currentMode;

    loadData();
}
bool CleverURI::exists()
{
    if(m_uri.isEmpty())
        return false;

   return QFileInfo::exists(m_uri);
}
const QString CleverURI::getAbsolueDir() const
{
    QFileInfo info(m_uri);
    return info.absolutePath();
}

void CleverURI::setName(const QString &name)
{
    ResourcesNode::setName(name);
    updateListener(CleverURI::NAME);
}

void CleverURI::write(QDataStream &out, bool tag, bool saveData) const
{
    if(tag)
    {
        out << QStringLiteral("CleverUri");
    }

    QByteArray data;
    if(saveData)
    {
        if(m_data.isEmpty())
        {
            loadFileFromUri(data);
        }
        else
        {
            data = m_data;
        }
    }
    out << static_cast<int>(m_type) << m_uri << m_name <<static_cast<int>(m_currentMode) << m_data << static_cast<int>(m_state);
}

void CleverURI::read(QDataStream &in)
{
    int type;
    int mode;
    int state;
    in >> type >> m_uri >> m_name >> mode >> m_data >> state;
    m_type = static_cast<CleverURI::ContentType>(type);
    m_currentMode = static_cast<CleverURI::LoadingMode>(mode);
    m_state = static_cast<CleverURI::State>(state);
    updateListener(CleverURI::NAME);
}

QString CleverURI::getFilterForType(CleverURI::ContentType type) //static
{
    PreferencesManager* preferences=PreferencesManager::getInstance();
    QString filterType;
    switch(type)
    {
    case CleverURI::CHARACTERSHEET:
        filterType =  QObject::tr("Character Sheets files  (%1)").arg(preferences->value("CharacterSheetFileFilter","*.rcs").toString());
        break;
    case CleverURI::PICTURE:
        filterType = QObject::tr("Supported Image formats (%1)").arg(preferences->value("ImageFileFilter","*.jpg *.jpeg *.png *.bmp *.svg").toString());
        break;
    case CleverURI::TEXT:
        filterType = QObject::tr("Supported Text Files (%1)").arg(preferences->value("TextFileFilter","*.odt *.htm *.html *.txt *.md").toString());
        break;
    case CleverURI::SCENARIO:
        filterType = QObject::tr("Supported Story Files (%1)").arg(preferences->value("StoryFileFilter","*.sce").toString());
        break;
    case CleverURI::SONG:
        filterType = QObject::tr("Supported Audio formats (%1)").arg(preferences->value("AudioFileFilter","*.wav *.mp2 *.mp3 *.ogg *.flac").toString());
        break;
    case CleverURI::SHAREDNOTE:
        filterType = QObject::tr("Supported Shared Note formats (%1)").arg(preferences->value("TextFileFilter","*.rsn *.txt *.html *.htm *.md").toString());
        break;
    case CleverURI::WEBVIEW:
        filterType = QObject::tr("Supported WebPage (%1)").arg(preferences->value("WebPageFilter","*.html *.xhtml *.htm").toString());
        break;
#ifdef WITH_PDF
    case CleverURI::PDF:
        filterType = QObject::tr("Pdf File (%1)").arg(preferences->value("PdfFileFilter","*.pdf").toString());
        break;
#endif
    case CleverURI::VMAP:
        filterType = QObject::tr("Vectorial Map (%1)").arg(preferences->value("VictorialFilter","*.vmap").toString());
        break;
    case CleverURI::MAP:
    case CleverURI::CHAT:
    case CleverURI::ONLINEPICTURE:
    default:
        filterType = QString();
        break;
    }
    return filterType;
}
QString CleverURI::typeToString(CleverURI::ContentType type)
{
    if(m_typeNameList.size()>static_cast<int>(type))
    {
        return m_typeNameList.at(static_cast<int>(type));
    }
    return QString();
}

QString CleverURI::getPreferenceDirectoryKey(CleverURI::ContentType type)
{
    if(m_typeToPreferenceDirectory.size()>static_cast<int>(type))
    {
        return m_typeToPreferenceDirectory.at(static_cast<int>(type));
    }
    return QString();
}
void CleverURI::loadFileFromUri(QByteArray& array) const
{
    QFile file(m_uri);
    if(file.open(QIODevice::ReadOnly))
    {
        array = file.readAll();
    }
}

void CleverURI::clearData()
{
    m_data.clear();
}
bool CleverURI::seekNode(QList<ResourcesNode*>& ,ResourcesNode* )
{
    return false;
}

ResourcesNode::TypeResource CleverURI::getResourcesType() const
{
    return ResourcesNode::Cleveruri;
}

QVariant CleverURI::getData(ResourcesNode::DataValue i)
{

    switch(i)
    {
    case ResourcesNode::NAME:
        return m_name;
    case ResourcesNode::MODE:
        return m_currentMode==Internal ? QObject::tr("Internal") : QObject::tr("Linked");
    case ResourcesNode::DISPLAYED:
    {
        static const std::vector<QString> list({QObject::tr("Closed"),QObject::tr("Hidden"),QObject::tr("Displayed")});
        return list[m_state];
    }
    case ResourcesNode::URI:
        return m_uri;
    }
    return {};
}

QDataStream& operator<<(QDataStream& out, const CleverURI& con)
{
    con.write(out,false,false);
    return out;
}

QDataStream& operator>>(QDataStream& is,CleverURI& con)
{
    con.read(is);
    return is;
}

QDataStream& operator<<(QDataStream& out, const CleverUriList& con)
{
    out << con.size();
    for(const CleverURI& uri : con)
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
