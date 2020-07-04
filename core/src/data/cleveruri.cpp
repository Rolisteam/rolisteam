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
// CleverUri       {Core::SONG, ":/resources/images/audiofile.svg"},{Core::CHAT,
// ":/resources/images/scenario.png"},{CleverURI::SONGLIST, ":/resources/images/playlist.svg"}
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

QIcon CleverURI::icon() const
{
    return QIcon(CleverURI::typeToIconPath(m_type));
}

CleverURI::CleverURI(const QString& name, const QString& path, Core::ContentType type)
    : ResourcesNode(TypeResource::Cleveruri), m_path(path), m_type(type), m_state(Core::State::Displayed)
{
    setName(name);
    init();
}

CleverURI::~CleverURI()= default;

QString CleverURI::typeToIconPath(Core::ContentType type)
{
    auto hash= QHash<Core::ContentType, QString>({
        {Core::ContentType::VECTORIALMAP, ":/vmap.png"},
        {Core::ContentType::PICTURE, ":/resources/images/photo.png"},
        {Core::ContentType::ONLINEPICTURE, ":/resources/images/photo.png"},
        {Core::ContentType::NOTES, ":/notes.png"},
        {Core::ContentType::CHARACTERSHEET, ":/resources/images/treeview.png"},
        {Core::ContentType::SHAREDNOTE, ":/resources/images/sharedEditor.png"},
        {Core::ContentType::WEBVIEW, ":/resources/images/webPage.svg"},
#ifdef WITH_PDF
        {Core::ContentType::PDF, ":/resources/images/pdfLogo.png"},
#endif
    });
    return hash.value(type);
}

bool CleverURI::operator==(const CleverURI& uri) const
{
    if((uri.path() == path()) && (uri.contentType() == contentType()))
        return true;
    return false;
}

void CleverURI::setPath(const QString& path)
{
    if(path == m_path)
        return;
    m_path= path;
    emit pathChanged();
}

Core::State CleverURI::state() const
{
    return m_state;
}

void CleverURI::setContentType(Core::ContentType type)
{
    m_type= type;
}

QString CleverURI::path() const
{
    return m_path;
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
    if(m_path.isEmpty())
        m_loadingMode= Core::LoadingMode::Internal;
    else
        m_loadingMode= Core::LoadingMode::Linked;
}

void CleverURI::setState(const Core::State& state)
{
    if(state == m_state)
        return;
    m_state= state;
    emit stateChanged();
}

bool CleverURI::hasData() const
{
    return !m_data.isEmpty();
}

QByteArray CleverURI::data() const
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

Core::LoadingMode CleverURI::loadingMode() const
{
    return m_loadingMode;
}

void CleverURI::setLoadingMode(const Core::LoadingMode& currentMode)
{
    if(currentMode == m_loadingMode)
        return;
    m_loadingMode= currentMode;
    emit loadingModeChanged();
}
bool CleverURI::exists()
{
    if(m_path.isEmpty())
        return false;

    return QFileInfo::exists(m_path);
}
const QString CleverURI::getAbsolueDir() const
{
    QFileInfo info(m_path);
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
    out << static_cast<int>(m_type) << m_path << m_name << static_cast<int>(m_loadingMode) << m_data
        << static_cast<int>(m_state);
}

void CleverURI::read(QDataStream& in)
{
    int type;
    int mode;
    int state;
    in >> type >> m_path >> m_name >> mode >> m_data >> state;
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
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        filterType= QObject::tr("Pdf File (%1)").arg(preferences->value("PdfFileFilter", "*.pdf").toString());
        break;
#endif
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
#ifdef WITH_PDF
    names.insert(Core::ContentType::PDF, QObject::tr("Pdf"));
#endif
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
    QFile file(m_path);
    if(file.open(QIODevice::ReadOnly))
    {
        array= file.readAll();
    }
}

QVariant CleverURI::getData(ResourcesNode::DataValue i) const
{
    QVariant var;
    switch(i)
    {
    case ResourcesNode::NAME:
        var= m_name;
        break;
    case ResourcesNode::MODE:
        var= m_loadingMode == Core::LoadingMode::Internal ? QObject::tr("Internal") : QObject::tr("Linked");
        break;
    case ResourcesNode::DISPLAYED:
    {
        static const std::vector<QString> list(
            {QObject::tr("Closed"), QObject::tr("Hidden"), QObject::tr("Displayed")});
        var= list[m_state];
        break;
    }
    case ResourcesNode::URI:
        var= m_path;
        break;
    }
    return var;
}
