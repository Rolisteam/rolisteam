/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#include "fileserializer.h"

#include "data/campaign.h"
#include "data/campaignmanager.h"
#include "data/media.h"
#include "iohelper.h"
#include "model/dicealiasmodel.h"
#include "model/nonplayablecharactermodel.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtConcurrent>

namespace campaign
{

bool FileSerializer::createCampaignDirectory(const QString& campaign)
{
    bool res= false;
    QDir dir(campaign);

    res= dir.mkdir(campaign::MEDIA_ROOT);
    res&= dir.mkdir(campaign::STATE_ROOT);
    res&= dir.mkdir(campaign::TRASH_FOLDER);
    res&= dir.mkdir(campaign::CHARACTER_ROOT);

    // dir.cd(campaign::MEDIA_ROOT);

    return res;
}

Core::MediaType FileSerializer::typeFromExtention(const QString& filename)
{
    static QSet<QString> image{"png", "jpg", "jpeg", "bmp", "gif", "tiff"};
    static QSet<QString> pdf{"pdf"};
    static QSet<QString> mindmap{"rmap"};
    static QSet<QString> vmap{"vmap"};
    static QSet<QString> text{"txt", "md", "tst", "json"};
    static QSet<QString> html{"html", "htm", "link"};
    static QSet<QString> token{"rtok"};
    static QSet<QString> playlist{"m3u"};
    static QSet<QString> audio{"mp3", "ogg", "mpc", "wav"};

    QHash<Core::MediaType, QSet<QString>> hash{
        {Core::MediaType::ImageFile, image},     {Core::MediaType::PdfFile, pdf},
        {Core::MediaType::MindmapFile, mindmap}, {Core::MediaType::MapFile, vmap},
        {Core::MediaType::TextFile, text},       {Core::MediaType::WebpageFile, html},
        {Core::MediaType::TokenFile, token},     {Core::MediaType::PlayListFile, playlist},
        {Core::MediaType::AudioFile, audio}};

    QFileInfo info(filename);
    auto ext= info.suffix();

    auto keys= hash.keys();
    for(auto type : keys)
    {
        auto set= hash[type];
        if(set.contains(ext))
            return type;
    }

    return Core::MediaType::Unknown;
}

CampaignInfo FileSerializer::readCampaignDirectory(const QString& directory)
{
    CampaignInfo info;
    info.status= false;
    if(directory.isEmpty())
        return info;

    QDir dir(directory);
    if(!dir.exists() || !dir.isReadable())
        info.errors << QObject::tr("Error: %1 does not exist or it is not readable.").arg(directory);

    if(dir.isEmpty())
    {
        info.status= true;
        createCampaignDirectory(directory);
        return info;
    }

    // read model: data.json
    bool ok;
    info.asset= IOHelper::loadJsonFileIntoObject(QStringLiteral("%1/%2").arg(directory, campaign::MODEL_FILE), ok);

    // read themes.json
    info.theme= IOHelper::loadJsonFileIntoObject(QStringLiteral("%1/%2").arg(directory, campaign::THEME_FILE), ok);

    // read dices.json
    info.dices= IOHelper::loadJsonFileIntoArray(QStringLiteral("%1/%2").arg(directory, campaign::DICE_ALIAS_MODEL), ok);

    // read states.json
    info.states= IOHelper::loadJsonFileIntoArray(QStringLiteral("%1/%2").arg(directory, campaign::STATE_MODEL), ok);

    // read npcs.json
    info.npcs= IOHelper::loadJsonFileIntoArray(QStringLiteral("%1/%2").arg(directory, campaign::CHARACTER_MODEL), ok);

    // read
    auto assetRoot= QString("%1/%2").arg(directory, campaign::MEDIA_ROOT);
    auto array= info.asset[Core::JsonKey::JSON_MEDIAS].toArray();

    QSet<QString> managedFiles;
    for(auto const& item : qAsConst(array))
    {
        auto asset= item.toObject();
        auto path= asset[Core::JsonKey::JSON_MEDIA_PATH].toString();
        auto filepath= QStringLiteral("%1/%2").arg(assetRoot, path);
        QFileInfo fileInfo(filepath);
        managedFiles << filepath;
        if(!fileInfo.exists())
        {
            info.missingFiles << path;
        }
    }

    QDirIterator it(assetRoot, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        auto path= it.next();
        if(!managedFiles.contains(path))
        {
            info.unmanagedFiles << path;
        }
    }

    // check integrity of data.json and files

    return info;
}

QJsonObject FileSerializer::campaignToObject(Campaign* campaign)
{
    QJsonObject root;
    auto const& data= campaign->medias();
    QJsonArray array;
    for(auto const& media : data)
    {
        QJsonObject obj;
        obj[Core::JsonKey::JSON_MEDIA_PATH]
            = IOHelper::absoluteToRelative(media->path(), campaign->directory(Campaign::Place::MEDIA_ROOT));
        obj[Core::JsonKey::JSON_MEDIA_CREATIONTIME]= media->addedTime().toString(Qt::ISODate);
        obj[Core::JsonKey::JSON_MEDIA_ID]= media->id();
        array.append(obj);
    }
    root[Core::JsonKey::JSON_MEDIAS]= array;
    root[Core::JsonKey::JSON_NAME]= campaign->name();
    return root;
}

QJsonArray FileSerializer::npcToArray(const std::vector<std::unique_ptr<campaign::NonPlayableCharacter>>& vec,
                                      const QString& destination)
{
    QJsonArray array;
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(array),
                   [destination](const std::unique_ptr<campaign::NonPlayableCharacter>& npc) {
                       return IOHelper::npcToJsonObject(npc.get(), destination);
                   });
    return array;
}

QJsonArray FileSerializer::statesToArray(const std::vector<std::unique_ptr<CharacterState>>& vec,
                                         const QString& destination)
{
    QJsonArray array;
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(array),
                   [destination](const std::unique_ptr<CharacterState>& alias) {
                       return IOHelper::stateToJSonObject(alias.get(), destination);
                   });
    return array;
}

QJsonArray FileSerializer::dicesToArray(const std::vector<std::unique_ptr<DiceAlias>>& vec)
{
    QJsonArray array;
    std::transform(
        std::begin(vec), std::end(vec), std::back_inserter(array),
        [](const std::unique_ptr<DiceAlias>& alias) { return IOHelper::diceAliasToJSonObject(alias.get()); });
    return array;
}

void FileSerializer::writeStatesIntoCampaign(const QString& destination, const QJsonArray& array)
{
    QtConcurrent::run([destination, array]() {
        IOHelper::writeJsonArrayIntoFile(QString("%1/%2").arg(destination, campaign::STATE_MODEL), array);
    });
}

void FileSerializer::writeNpcIntoCampaign(const QString& destination, const QJsonArray& array)
{
    QtConcurrent::run([destination, array]() {
        IOHelper::writeJsonArrayIntoFile(QString("%1/%2").arg(destination, campaign::CHARACTER_MODEL), array);
    });
}

void FileSerializer::writeDiceAliasIntoCampaign(const QString& destination, const QJsonArray& array)
{
    QtConcurrent::run([destination, array]() {
        IOHelper::writeJsonArrayIntoFile(QString("%1/%2").arg(destination, campaign::DICE_ALIAS_MODEL), array);
    });
}

void FileSerializer::writeCampaignInfo(const QString& destination, const QJsonObject& object)
{
    QtConcurrent::run([destination, object]() {
        IOHelper::writeJsonObjectIntoFile(QString("%1/%2").arg(destination, campaign::MODEL_FILE), object);
    });
}

QString FileSerializer::contentTypeToDefaultExtension(Core::ContentType type)
{
    using cc= Core::ContentType;

    QString res;
    switch(type)
    {
    case cc::VECTORIALMAP:
        res= Core::extentions::EXT_MAP;
        break;
    case cc::PICTURE:
    case cc::ONLINEPICTURE:
        res= Core::extentions::EXT_IMG;
        break;
    case cc::NOTES:
        res= Core::extentions::EXT_TEXT;
        break;
    case cc::CHARACTERSHEET:
        res= Core::extentions::EXT_SHEET;
        break;
    case cc::SHAREDNOTE:
        res= Core::extentions::EXT_SHAREDNOTE;
        break;
    case cc::PDF:
        res= Core::extentions::EXT_PDF;
        break;
    case cc::WEBVIEW:
        res= Core::extentions::EXT_WEBVIEW;
        break;
    case cc::INSTANTMESSAGING:
        res= Core::extentions::EXT_TEXT;
        break;
    case cc::MINDMAP:
        res= Core::extentions::EXT_MINDMAP;
        break;
    case cc::UNKNOWN:
        break;
    }
    return res;
}

QString FileSerializer::addExtention(const QString& name, Core::ContentType type)
{
    auto ext= contentTypeToDefaultExtension(type);
    QString ret= name;
    if(!name.endsWith(ext))
        ret= QString("%1%2").arg(name, ext);
    return ret;
}
} // namespace campaign
