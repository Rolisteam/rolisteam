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
#include "worker/fileserializer.h"

#include "data/campaign.h"
#include "data/campaignmanager.h"
#include "data/media.h"
#include "iohelper.h"
#include "model/dicealiasmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "worker/iohelper.h"

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

    return res;
}

Core::MediaType FileSerializer::typeFromExtention(const QString& filename)
{
    static QSet<QString> image{"png", "jpg", "jpeg", "bmp", "gif", "tiff"};
    static QSet<QString> pdf{"pdf"};
    static QSet<QString> mindmap{"rmap"};
    static QSet<QString> vmap{"vmap"};
    static QSet<QString> text{"txt", "md", "tst", "json"};
    static QSet<QString> html{"rweb"};
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
    info.status= true;

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
            = utils::IOHelper::absoluteToRelative(media->path(), campaign->directory(Campaign::Place::MEDIA_ROOT));
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
                   [destination](const std::unique_ptr<campaign::NonPlayableCharacter>& npc)
                   { return IOHelper::npcToJsonObject(npc.get(), destination); });
    return array;
}

QJsonArray FileSerializer::statesToArray(const std::vector<std::unique_ptr<CharacterState>>& vec,
                                         const QString& destination)
{
    QJsonArray array;
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(array),
                   [destination](const std::unique_ptr<CharacterState>& alias)
                   { return IOHelper::stateToJSonObject(alias.get(), destination); });
    return array;
}

QJsonArray FileSerializer::dicesToArray(const std::vector<std::unique_ptr<DiceAlias>>& vec)
{
    QJsonArray array;
    std::transform(std::begin(vec), std::end(vec), std::back_inserter(array),
                   [](const std::unique_ptr<DiceAlias>& alias)
                   { return IOHelper::diceAliasToJSonObject(alias.get()); });
    return array;
}

void FileSerializer::writeStatesIntoCampaign(const QString& destination, const QJsonArray& array)
{
    QtConcurrent::run(
        [destination, array]()
        { IOHelper::writeJsonArrayIntoFile(QString("%1/%2").arg(destination, campaign::STATE_MODEL), array); });
}

void FileSerializer::writeNpcIntoCampaign(const QString& destination, const QJsonArray& array)
{
    QtConcurrent::run(
        [destination, array]()
        { IOHelper::writeJsonArrayIntoFile(QString("%1/%2").arg(destination, campaign::CHARACTER_MODEL), array); });
}

void FileSerializer::writeDiceAliasIntoCampaign(const QString& destination, const QJsonArray& array)
{
    QtConcurrent::run(
        [destination, array]()
        { IOHelper::writeJsonArrayIntoFile(QString("%1/%2").arg(destination, campaign::DICE_ALIAS_MODEL), array); });
}

void FileSerializer::writeCampaignInfo(const QString& destination, const QJsonObject& object)
{
    QtConcurrent::run(
        [destination, object]()
        { IOHelper::writeJsonObjectIntoFile(QString("%1/%2").arg(destination, campaign::MODEL_FILE), object); });
}

void FileSerializer::writeFileIntoCampaign(const QString& destination, const QByteArray& array)
{
    qDebug() << "fileserializer" << destination << array.size();
    QtConcurrent::run([destination, array]() { utils::IOHelper::writeFile(destination, array); });
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
        // case cc::ONLINEPICTURE:
        res= Core::extentions::EXT_IMG_PNG;
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

bool FileSerializer::isValidCampaignDirectory(const QString& path, bool acceptEmpty)
{
    if(path.isEmpty())
        return false;

    QDir direct(path);
    auto entrylist= direct.entryList(QDir::NoDotAndDotDot);

    if(acceptEmpty && entrylist.isEmpty()) // empty directory is valid.
        return true;

    QList<QString> list{campaign::MEDIA_ROOT, campaign::STATE_ROOT, campaign::TRASH_FOLDER, campaign::CHARACTER_ROOT,
                        campaign::MODEL_FILE};
    return std::all_of(std::begin(list), std::end(list),
                       [path](const QString& subpath)
                       { return QFileInfo::exists(QString("%1/%2").arg(path, subpath)); });
}

bool FileSerializer::hasContent(const QString& path, Core::CampaignDataCategory category)
{
    if(path.isEmpty())
        return false;

    QDir direct(path);
    auto entrylist= direct.entryList(QDir::NoDotAndDotDot);

    if(entrylist.isEmpty()) // empty dir has no content
        return false;

    QSet<Core::CampaignDataCategory> mediaCategories{
        Core::CampaignDataCategory::Images, Core::CampaignDataCategory::Maps, Core::CampaignDataCategory::MindMaps,
        Core::CampaignDataCategory::CharacterSheets, Core::CampaignDataCategory::Notes};

    QList<QString> list;
    switch(category)
    {
    case Core::CampaignDataCategory::AudioPlayer1:
        list.append(campaign::FIRST_AUDIO_PLAYER_FILE);
        break;
    case Core::CampaignDataCategory::AudioPlayer2:
        list.append(campaign::SECOND_AUDIO_PLAYER_FILE);
        break;
    case Core::CampaignDataCategory::AudioPlayer3:
        list.append(campaign::THIRD_AUDIO_PLAYER_FILE);
        break;
    case Core::CampaignDataCategory::Images:
    case Core::CampaignDataCategory::Maps:
    case Core::CampaignDataCategory::MindMaps:
    case Core::CampaignDataCategory::CharacterSheets:
    case Core::CampaignDataCategory::Notes:
    case Core::CampaignDataCategory::WebLink:
    case Core::CampaignDataCategory::PDFDoc:
        list.append(campaign::MEDIA_ROOT);
        list.append(campaign::MODEL_FILE);
        break;
    case Core::CampaignDataCategory::DiceAlias:
        list.append(campaign::DICE_ALIAS_MODEL);
        break;
    case Core::CampaignDataCategory::CharacterStates:
        list.append(campaign::STATE_MODEL);
        break;
    case Core::CampaignDataCategory::Themes:
        list.append(campaign::THEME_FILE);
        break;
    case Core::CampaignDataCategory::AntagonistList:
        list.append(campaign::CHARACTER_ROOT);
        list.append(campaign::CHARACTER_MODEL);
        break;
    }

    auto res= std::all_of(std::begin(list), std::end(list),
                          [path](const QString& subpath)
                          { return QFileInfo::exists(QString("%1/%2").arg(path, subpath)); });

    if(res && mediaCategories.contains(category))
    {
        direct.cd(campaign::MEDIA_ROOT);
        auto subentrylist= direct.entryList(QDir::NoDotAndDotDot);
        if(category == Core::CampaignDataCategory::Images)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> imgExt{"png", "jpg", "bmp", "gif", "svg"};
                                  return std::any_of(std::begin(imgExt), std::end(imgExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
        else if(category == Core::CampaignDataCategory::Maps)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> mapExt{"vmap"};
                                  return std::any_of(std::begin(mapExt), std::end(mapExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
        else if(category == Core::CampaignDataCategory::MindMaps)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> mindmapExt{"rmap"};
                                  return std::any_of(std::begin(mindmapExt), std::end(mindmapExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
        else if(category == Core::CampaignDataCategory::CharacterSheets)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> sheetExt{"rcs"};
                                  return std::any_of(std::begin(sheetExt), std::end(sheetExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
        else if(category == Core::CampaignDataCategory::Notes)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> noteExt{"txt", "md", "html"};
                                  return std::any_of(std::begin(noteExt), std::end(noteExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
        else if(category == Core::CampaignDataCategory::WebLink)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> noteExt{"txt", "md", "html"};
                                  return std::any_of(std::begin(noteExt), std::end(noteExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
        else if(category == Core::CampaignDataCategory::PDFDoc)
        {
            res|= std::any_of(std::begin(subentrylist), std::end(subentrylist),
                              [](const QString& item)
                              {
                                  QSet<QString> noteExt{"txt", "md", "html"};
                                  return std::any_of(std::begin(noteExt), std::end(noteExt),
                                                     [item](const QString& ext) { return item.endsWith(ext); });
                              });
        }
    }

    return res;
}
} // namespace campaign
