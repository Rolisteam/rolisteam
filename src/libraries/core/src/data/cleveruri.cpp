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
#include "data/cleveruri.h"
#include <QDebug>
#include <QString>

#include <QDataStream>
#include <QFileInfo>
#include <set>

#include "media/mediatype.h"
#include "preferences/preferencesmanager.h"
#include "worker/utilshelper.h"

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
    return QIcon::fromTheme(helper::utils::typeToIconPath(m_type));
}

CleverURI::CleverURI(const QString& name, const QString& path, Core::ContentType type)
    : ResourcesNode(TypeResource::Cleveruri), m_path(path), m_type(type), m_state(Core::State::Displayed)
{
    setName(name);
    init();
}

CleverURI::~CleverURI()= default;

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
    if(type == m_type)
        return;
    m_type= type;
    emit contentTypeChanged();
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
    if(m_data == data)
        return;
    m_data= data;
    emit dataChanged();
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
