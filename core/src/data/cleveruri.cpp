/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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
#include <QDebug>
#include <QString>

#include <QDataStream>
#include <QFileInfo>

#include "media/mediatype.h"
#include "preferences/preferencesmanager.h"

/////////////////
// CleverUri       {Core::SONG, ":/resources/icons/audiofile.svg"},{Core::CHAT,
// ":/resources/icons/scenario.png"},{CleverURI::SONGLIST, ":/resources/icons/playlist.svg"}
/////////////////

QStringList CleverURI::m_typeToPreferenceDirectory
    = QStringList() << QString("SessionDirectory") << QString("MapDirectory") << QString("ChatDirectory")
                    << QString("ImageDirectory") << QString("ImageDirectory") << QString("MinutesDirectory")
                    << QString("CharacterSheetDirectory") << QString("MusicDirectoryPlayer")
                    << QString("MusicDirectoryPlayer") << QString("MinutesDirectory");

CleverURI::CleverURI(Core::ContentType type)
    : ResourcesNode(TypeResource::Cleveruri), m_type(type), m_state(Core::State::Unloaded)
{
    init();
}

QIcon CleverURI::getIcon() const
{
    return QIcon(CleverURI::typeToIconPath(m_type));
}

CleverURI::CleverURI(QString name, QString uri, const QString& ownerId, Core::ContentType type)
    : ResourcesNode(TypeResource::Cleveruri)
    , m_uri(uri)
    , m_type(type)
    , m_state(Core::State::Unloaded)
    , m_ownerId(ownerId)
{
    setName(name);
    init();
}

CleverURI::~CleverURI()= default;

QString CleverURI::typeToIconPath(Core::ContentType type)
{
    auto hash= QHash<Core::ContentType, QString>({
        {Core::ContentType::VECTORIALMAP, ":/vmap.png"},
        {Core::ContentType::PICTURE, ":/resources/icons/photo.png"},
        {Core::ContentType::ONLINEPICTURE, ":/resources/icons/photo.png"},
        {Core::ContentType::NOTES, ":/notes.png"},
        {Core::ContentType::CHARACTERSHEET, ":/resources/icons/treeview.png"},
        {Core::ContentType::SHAREDNOTE, ":/resources/icons/sharedEditor.png"},
        {Core::ContentType::WEBVIEW, ":/resources/icons/webPage.svg"},
        {Core::ContentType::PDF, ":/resources/icons/pdfLogo.png"},
    });
    return hash.value(type);
}

bool CleverURI::operator==(const CleverURI& uri) const
{
    if((uri.getUri() == getUri()) && (uri.contentType() == contentType()))
        return true;
    return false;
}

void CleverURI::setUri(QString& uri)
{
    m_uri= uri;
    QFileInfo info(uri);
    m_name= info.baseName();
    loadData();
}

void CleverURI::setContentType(Core::ContentType type)
{
    m_type= type;
}

QString CleverURI::ownerId() const
{
    return m_ownerId;
}

const QString CleverURI::getUri() const
{
    return m_uri;
}

Core::ContentType CleverURI::contentType() const
{
    return m_type;
}
bool CleverURI::hasChildren() const
{
    return false;
}

void CleverURI::loadData()
{
    if(loadingMode() == Core::LoadingMode::Internal)
    {
        loadFileFromUri(m_data);
    }
}

void CleverURI::init()
{
    PreferencesManager* preferences= PreferencesManager::getInstance();
    m_loadingMode= static_cast<Core::LoadingMode>(
        preferences->value(QStringLiteral("DefaultLoadingMode"), static_cast<int>(Core::LoadingMode::Linked)).toInt());
}

Core::State CleverURI::getState() const
{
    return m_state;
}

void CleverURI::setState(const Core::State& state)
{
    m_state= state;
}

bool CleverURI::hasData() const
{
    return !m_data.isEmpty();
}

QByteArray CleverURI::getData() const
{
    return m_data;
}

void CleverURI::setData(const QByteArray& data)
{
    m_data= data;
}

bool CleverURI::isDisplayed() const
{
    return (m_state == Core::State::Displayed);
}

void CleverURI::setDisplayed(bool displayed)
{
    if(displayed)
    {
        m_state= Core::State::Displayed;
    }
    else
    {
        m_state= Core::State::Hidden;
    }
}

Core::LoadingMode CleverURI::loadingMode() const
{
    return m_loadingMode;
}

void CleverURI::setLoadingMode(const Core::LoadingMode& currentMode)
{
    m_loadingMode= currentMode;

    if(m_loadingMode == Core::LoadingMode::Linked)
        loadData();
    else if(m_loadingMode == Core::LoadingMode::Internal)
        m_data.clear();
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

void CleverURI::write(QDataStream& out, bool tag, bool saveData) const
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
            data= m_data;
        }
    }
    out << static_cast<int>(m_type) << m_uri << m_name << static_cast<int>(m_loadingMode) << m_data
        << static_cast<int>(m_state);
}

void CleverURI::read(QDataStream& in)
{
    int type;
    int mode;
    int state;
    in >> type >> m_uri >> m_name >> mode >> m_data >> state;
    m_type= static_cast<Core::ContentType>(type);
    m_loadingMode= static_cast<Core::LoadingMode>(mode);
    m_state= static_cast<Core::State>(state);
    // updateListener(CleverURI::NAME);
}

QString CleverURI::getFilterForType(Core::ContentType type) // static
{
    PreferencesManager* preferences= PreferencesManager::getInstance();
    QString filterType;
    switch(type)
    {
    case Core::ContentType::CHARACTERSHEET:
        filterType= QObject::tr("Character Sheets files  (%1)")
                        .arg(preferences->value("CharacterSheetFileFilter", "*.rcs").toString());
        break;
    case Core::ContentType::PICTURE:
        filterType= QObject::tr("Supported Image formats (%1)")
                        .arg(preferences->value("ImageFileFilter", "*.jpg *.jpeg *.png *.bmp *.svg *.gif").toString());
        break;
    case Core::ContentType::NOTES:
        filterType= QObject::tr("Supported Text Files (%1)")
                        .arg(preferences->value("TextFileFilter", "*.odt *.htm *.html *.txt *.md").toString());
        break;
        /* case Core::ContentType:: ::SONG:
             filterType= QObject::tr("Supported Audio formats (%1)")
                             .arg(preferences->value("AudioFileFilter", "*.wav *.mp2 *.mp3 *.ogg *.flac").toString());
             break;*/
    case Core::ContentType::SHAREDNOTE:
        filterType= QObject::tr("Supported Shared Note formats (%1)")
                        .arg(preferences->value("TextFileFilter", "*.rsn *.txt *.html *.htm *.md").toString());
        break;
    case Core::ContentType::WEBVIEW:
        filterType= QObject::tr("Supported WebPage (%1)")
                        .arg(preferences->value("WebPageFilter", "*.html *.xhtml *.htm").toString());
        break;
    case Core::ContentType::PDF:
        filterType= QObject::tr("Pdf File (%1)").arg(preferences->value("PdfFileFilter", "*.pdf").toString());
        break;
    case Core::ContentType::VECTORIALMAP:
        filterType= QObject::tr("Vectorial Map (%1)").arg(preferences->value("VictorialFilter", "*.vmap").toString());
        break;
    case Core::ContentType::ONLINEPICTURE:
    default:
        filterType= QString();
        break;
    }
    return filterType;
}

QString CleverURI::typeToString(Core::ContentType type)
{
    QHash<Core::ContentType, QString> names;

    names.insert(Core::ContentType::VECTORIALMAP, QObject::tr("Vectorial Map"));
    // names.insert(Core::ContentType::CHAT, QObject::tr("Chat"));
    names.insert(Core::ContentType::PICTURE, QObject::tr("Picture"));
    names.insert(Core::ContentType::ONLINEPICTURE, QObject::tr("Online Picture"));
    names.insert(Core::ContentType::NOTES, QObject::tr("Minutes"));
    names.insert(Core::ContentType::CHARACTERSHEET, QObject::tr("Charecter Sheet"));
    // names.insert(Core::ContentType::SONG, QObject::tr("Song"));
    // names.insert(Core::ContentType::SONGLIST, QObject::tr("Song List"));
    names.insert(Core::ContentType::SHAREDNOTE, QObject::tr("Shared Notes"));
    // names.insert(Core::ContentType::TOKEN, QObject::tr("NPC Token"));
    names.insert(Core::ContentType::PDF, QObject::tr("Pdf"));
    names.insert(Core::ContentType::WEBVIEW, QObject::tr("Webview"));

    return names.value(type);
}

QString CleverURI::getPreferenceDirectoryKey(Core::ContentType type)
{
    if(m_typeToPreferenceDirectory.size() > static_cast<int>(type))
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
        array= file.readAll();
    }
}

void CleverURI::clearData()
{
    m_data.clear();
}
bool CleverURI::seekNode(QList<ResourcesNode*>&, ResourcesNode*)
{
    return false;
}

QVariant CleverURI::getData(ResourcesNode::DataValue i) const
{
    switch(i)
    {
    case ResourcesNode::NAME:
        return m_name;
    case ResourcesNode::MODE:
        return m_loadingMode == Core::LoadingMode::Internal ? QObject::tr("Internal") : QObject::tr("Linked");
    case ResourcesNode::DISPLAYED:
    {
        static const std::vector<QString> list(
            {QObject::tr("Closed"), QObject::tr("Hidden"), QObject::tr("Displayed")});
        return list[m_state];
    }
    case ResourcesNode::URI:
        return m_uri;
    }
    return {};
}

/*QDataStream& operator<<(QDataStream& out, const CleverURI& con)
{
    con.write(out, false, false);
    return out;
}

QDataStream& operator>>(QDataStream& is, CleverURI& con)
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

QDataStream& operator>>(QDataStream& is, CleverUriList& con)
{
    int count;
    is >> count;
    for(int i= 0; i < count; ++i)
    {
        CleverURI* uri= new CleverURI();
        is >> *uri;
        con.append(*uri);
    }
    return is;
}*/
