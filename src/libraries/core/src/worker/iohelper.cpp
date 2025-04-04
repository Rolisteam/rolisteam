/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "worker/iohelper.h"

#include <QBuffer>
#include <QCborValue>
#include <QClipboard>
#include <QColor>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QMimeData>
#include <QMutex>
#include <QMutexLocker>
#include <QSaveFile>
#include <QString>
#include <QStyleFactory>
#include <QVariant>
#include <map>

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "charactersheet/worker/ioworker.h"
#include "controller/audioplayercontroller.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/mindmapcontrollerbase.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"
#include "data/character.h"
#include "data/rolisteamtheme.h"
#include "diceparser/dicealias.h"
#include "media/mediatype.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"
#include "model/musicmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "utils/iohelper.h"
#include "worker/utilshelper.h"
#include "worker/vectorialmapmessagehelper.h"

constexpr char const* k_language_dir_path{":/translations"};
constexpr char const* k_rolisteam_pattern{"rolisteam"};
constexpr char const* k_qt_pattern{"qt"};
constexpr char const* k_state_id{"id"};
constexpr char const* k_state_label{"label"};
constexpr char const* k_state_color{"color"};
constexpr char const* k_state_image{"image"};

namespace
{
template <class T>
T* convertField(CharacterField* field)
{
    return dynamic_cast<T*>(field);
}
} // namespace

IOHelper::IOHelper() {}

/*bool IOHelper::loadToken(campaign::NonPlayableCharacter* character, const QString& root,
                         std::map<QString, QVariant>& params)
{
    bool ok;
    auto obj= loadJsonFileIntoObject(QString("%1/%2").arg(root, character->tokenPath()), ok);

    if(!ok)
        return false;

    params["side"]= obj[Core::JsonKey::JSON_TOKEN_SIZE].toDouble();


    character->setName(obj[Core::JsonKey::JSON_TOKEN_SIZE].toString());
    character->setColor(obj[Core::JsonKey::JSON_NPC_COLOR].toString());
    character->setHealthPointsMax(obj[Core::JsonKey::JSON_NPC_MAXHP].toInt());
    character->setHealthPointsMin(obj[Core::JsonKey::JSON_NPC_MINHP].toInt());
    character->setHealthPointsCurrent(obj[Core::JsonKey::JSON_NPC_HP].toInt());

    character->setInitiativeScore(obj[Core::JsonKey::JSON_NPC_INITVALUE].toInt());
    character->setInitCommand(obj[Core::JsonKey::JSON_NPC_INITCOMMAND].toString());
    // character->setAvatarPath(obj["avatarUri"].toString());

    auto img= loadFile(QString("%1/%2").arg(root, obj[Core::JsonKey::JSON_NPC_AVATAR].toString()));

    character->setAvatar(img);

    auto actionArray= obj[Core::JsonKey::JSON_NPC_ACTIONS].toArray();
    for(auto act : actionArray)
    {
        auto actObj= act.toObject();
        auto action= new CharacterAction();
        action->setName(actObj[Core::JsonKey::JSON_NPC_ACTION_NAME].toString());
        action->setCommand(actObj[Core::JsonKey::JSON_NPC_ACTION_COMMAND].toString());
        character->addAction(action);
    }

    auto propertyArray= obj[Core::JsonKey::JSON_NPC_PROPERTIES].toArray();
    for(auto pro : propertyArray)
    {
        auto proObj= pro.toObject();
        auto property= new CharacterProperty();
        property->setName(proObj[Core::JsonKey::JSON_NPC_PROPERTY_NAME].toString());
        property->setValue(proObj[Core::JsonKey::JSON_NPC_PROPERTY_VALUE].toString());
        character->addProperty(property);
    }

    auto shapeArray= obj[Core::JsonKey::JSON_NPC_SHAPES].toArray();
    for(auto sha : shapeArray)
    {
        auto objSha= sha.toObject();
        auto shape= new CharacterShape();
        shape->setName(objSha[Core::JsonKey::JSON_NPC_SHAPE_NAME].toString());
        auto avatarData= QByteArray::fromBase64(objSha[Core::JsonKey::JSON_NPC_SHAPE_DATAIMG].toString().toUtf8());
        QImage img= QImage::fromData(avatarData);
        shape->setImage(img);
        character->addShape(shape);
    }

    params["character"]= QVariant::fromValue(character);
    return true;
}*/

QList<QUrl> IOHelper::readM3uPlayList(const QString& filepath)
{
    QList<QUrl> res;
    QFile file(filepath);
    /// @todo make this job in thread.
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream read(&file);
        QString line;
        while(!read.atEnd())
        {
            line= read.readLine();
            if(line.startsWith("#EXTINF", Qt::CaseSensitive) || line.isEmpty())
                continue;

            auto url= QUrl::fromUserInput(line);
            if(url.isValid())
                res.append(url);
        }
    }
    return res;
}

QJsonObject IOHelper::byteArrayToJsonObj(const QByteArray& data)
{
    auto doc= QCborValue(data);
    return doc.toJsonValue().toObject();
}

QJsonObject IOHelper::textByteArrayToJsonObj(const QByteArray& data)
{
    QJsonDocument doc= QJsonDocument::fromJson(data);
    return doc.object();
}

QJsonArray IOHelper::byteArrayToJsonArray(const QByteArray& data)
{
    QJsonDocument doc= QJsonDocument::fromJson(data);
    return doc.array();
}

QStringList IOHelper::jsonArrayToStringList(const QJsonArray& data)
{
    QStringList res;
    for(auto valref : data)
    {
        res << valref.toString();
    }
    return res;
}

QByteArray IOHelper::jsonObjectToByteArray(const QJsonObject& obj)
{
    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Indented);
}

QByteArray IOHelper::jsonArrayToByteArray(const QJsonArray& obj)
{
    QJsonDocument doc;
    doc.setArray(obj);
    return doc.toJson(QJsonDocument::Indented);
}

QJsonObject IOHelper::loadJsonFileIntoObject(const QString& filename, bool& ok)
{
    QJsonDocument doc= QJsonDocument::fromJson(utils::IOHelper::loadFile(filename));
    ok= !doc.isEmpty();
    return doc.object();
}

QJsonArray IOHelper::loadJsonFileIntoArray(const QString& filename, bool& ok)
{
    QJsonDocument doc= QJsonDocument::fromJson(utils::IOHelper::loadFile(filename));
    ok= !doc.isEmpty();
    return doc.array();
}

QJsonArray IOHelper::fetchLanguageModel()
{
    QJsonArray array;
    QDir dir(k_language_dir_path);

    auto list= dir.entryList(QStringList() << QStringLiteral("*.qm"), QDir::Files);

    static QRegularExpression reQt(QStringLiteral("%1_(.*)\\.qm").arg(k_qt_pattern));
    QHash<QString, QString> hash;
    for(const auto& info : std::as_const(list))
    {
        auto match= reQt.match(info);
        if(match.hasMatch())
        {
            hash.insert(match.captured(1), info);
        }
    }

    static QRegularExpression reRolisteam(QStringLiteral("%1_(.*)\\.qm").arg(k_rolisteam_pattern));
    for(auto info : std::as_const(list))
    {
        auto match= reRolisteam.match(info);
        if(match.hasMatch())
        {
            auto iso= match.captured(1);
            QJsonObject obj;
            QJsonArray paths;
            paths << QString("%1/%2").arg(k_language_dir_path, info);
            if(hash.contains(iso))
                paths << QString("%1/%2").arg(k_language_dir_path, hash.value(iso));
            // else
            // qWarning() << "No Qt translation for " << iso;

            QLocale local(iso);

            obj[Core::i18n::KEY_PATH]= paths;
            obj[Core::i18n::KEY_CODE]= iso;
            obj[Core::i18n::KEY_COMMONNAME]= QLocale::languageToString(local.language());
            obj[Core::i18n::KEY_LANGNAME]= local.nativeLanguageName();
            array.append(obj);
        }
    }

    return array;
}

void IOHelper::saveBase(MediaControllerBase* base, QDataStream& output)
{
    if(!base)
        return;

    output << base->contentType();
    output << base->uuid();
    output << base->url();
    output << base->name();
    output << base->ownerId();
}

void IOHelper::saveMediaBaseIntoJSon(MediaControllerBase* base, QJsonObject& obj)
{
    if(!base)
        return;

    obj[Core::jsonctrl::base::JSON_CONTENT_TYPE]= static_cast<int>(base->contentType());
    obj[Core::jsonctrl::base::JSON_UUID]= base->uuid();
    obj[Core::jsonctrl::base::JSON_PATH]= base->url().toString();
    obj[Core::jsonctrl::base::JSON_NAME]= base->name();
    obj[Core::jsonctrl::base::JSON_OWNERID]= base->ownerId();
    obj[Core::jsonctrl::base::JSON_STATIC]= base->staticData().toString();
}

void IOHelper::readBaseFromJson(MediaControllerBase* base, const QJsonObject& data)
{
    if(!base)
        return;

    if(base->uuid().isEmpty())
        base->setUuid(data[Core::jsonctrl::base::JSON_UUID].toString());
    base->setUrl(QUrl::fromUserInput(data[Core::jsonctrl::base::JSON_PATH].toString()));
    base->setName(data[Core::jsonctrl::base::JSON_NAME].toString());
    base->setOwnerId(data[Core::jsonctrl::base::JSON_OWNERID].toString());
    base->setStaticData(QUrl::fromUserInput(data[Core::jsonctrl::base::JSON_STATIC].toString()));
}

QByteArray IOHelper::pixmapToData(const QPixmap& pix)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    return bytes;
}

QByteArray IOHelper::imageToData(const QImage& pix)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    return bytes;
}

const QMimeData* IOHelper::clipboardMineData()
{
    auto clipboard= QGuiApplication::clipboard();
    if(!clipboard)
        return {};

    return clipboard->mimeData();
}

QString IOHelper::htmlToTitle(const QMimeData& data, const QString& defaultName)
{
    QString name= defaultName;
    if(data.hasHtml())
    {
        QRegularExpression reg("src=\\\"([^\\s]+)\\\"");
        auto match= reg.match(data.html());
        if(match.hasMatch())
        {
            auto urlString= QUrl::fromUserInput(match.captured(1));
            name= urlString.fileName();
        }
    }
    return name;
}

QImage IOHelper::readImageFromURL(const QUrl& url)
{
    QImage img;
    if(url.isLocalFile())
    {
        img= readImageFromFile(url.toLocalFile());
    }
    return img;
}

QImage IOHelper::readImageFromFile(const QString& url)
{
    return QImage(url);
}

QPixmap IOHelper::dataToPixmap(const QByteArray& data)
{
    QPixmap pix;
    pix.loadFromData(data);
    return pix;
}

QByteArray saveImage(ImageController* ctrl)
{
    if(!ctrl)
        return {};

    QJsonDocument doc;
    QJsonObject obj;

    IOHelper::saveMediaBaseIntoJSon(ctrl, obj);
    obj[Core::imageMedia::zoomLevel]= ctrl->zoomLevel();
    obj[Core::imageMedia::fitwindow]= ctrl->fitWindow();
    obj[Core::imageMedia::sharing]= ctrl->sharing();
    doc.setObject(obj);
    return doc.toJson();
}

QByteArray saveNotes(NoteController* ctrl)
{
    if(!ctrl)
        return {};

    QJsonDocument doc;
    QJsonObject obj;
    IOHelper::saveMediaBaseIntoJSon(ctrl, obj);
    obj[Core::keys::KEY_TEXT]= ctrl->text();
    doc.setObject(obj);
    return doc.toJson();
}

QByteArray saveCharacterSheet(CharacterSheetController* ctrl)
{
    if(!ctrl)
        return {};

    QFile file(ctrl->url().toLocalFile());
    if(!file.open(QIODevice::ReadOnly))
        qDebug() << "error reading file" << ctrl->url().toLocalFile();

    auto jsonData= file.readAll();
    QJsonDocument doc= QJsonDocument::fromJson(jsonData);

    QJsonObject obj= doc.object();
    IOHelper::saveMediaBaseIntoJSon(ctrl, obj);
    auto model= ctrl->model();
    model->writeModel(obj);
    auto images= ctrl->imageModel();
    auto array= IOWorker::saveImageModel(images);
    obj[Core::jsonctrl::sheet::JSON_IMAGES_CONTENT]= array;

    auto sharingData= ctrl->sheetData();
    QJsonArray sharingInfos;
    for(auto const& info : sharingData)
    {
        if(info.remote)
            continue;

        QJsonObject shareInfo;
        shareInfo[Core::jsonctrl::sheet::KEY_SHEETID]= info.m_sheetId;
        shareInfo[Core::jsonctrl::sheet::KEY_CHARACTERID]= info.m_characterId;
        shareInfo[Core::jsonctrl::sheet::KEY_EVERYONE]= info.everyone;
        sharingInfos.append(shareInfo);
    }
    obj[Core::jsonctrl::sheet::KEY_SHARING_INFO]= sharingInfos;
    doc.setObject(obj);

    return doc.toJson();
}

QByteArray saveSharedNote(SharedNoteController* ctrl)
{
    if(!ctrl)
        return {};

    QJsonObject objCtrl;
    IOHelper::saveMediaBaseIntoJSon(ctrl, objCtrl);

    objCtrl[Core::keys::KEY_TEXT]= ctrl->text();
    objCtrl[Core::keys::KEY_SYNTAXE]= static_cast<int>(ctrl->highligthedSyntax());
    objCtrl[Core::keys::KEY_PREVIEW_VISIBLE]= ctrl->markdownVisible();
    return IOHelper::jsonObjectToByteArray(objCtrl);
}

QByteArray saveWebView(WebpageController* ctrl)
{
    if(!ctrl)
        return {};
    QJsonObject objCtrl;
    IOHelper::saveMediaBaseIntoJSon(ctrl, objCtrl);

    objCtrl[Core::webview::JSON_HIDE_URL]= ctrl->hideUrl();
    objCtrl[Core::webview::JSON_KEEP_SHARING]= ctrl->keepSharing();
    objCtrl[Core::webview::JSON_HTML_CODE]= ctrl->html();
    objCtrl[Core::webview::JSON_PAGE_URL]= ctrl->pageUrl().toDisplayString();
    objCtrl[Core::webview::JSON_STATE]= ctrl->state();
    objCtrl[Core::webview::JSON_SHARING_MODE]= ctrl->sharingMode();

    /*output << ctrl->hideUrl();
    output << ctrl->keepSharing();
    output << ctrl->htmlSharing();
    output << ctrl->urlSharing();
    output << ctrl->html();
    output << ctrl->url();
    output << ctrl->pageUrl();
    output << ctrl->state();
    output << ctrl->sharingMode();*/
    return IOHelper::jsonObjectToByteArray(objCtrl);
}

QByteArray saveMindmap(mindmap::MindMapControllerBase* ctrl)
{
    if(!ctrl)
        return {};

    QJsonObject objCtrl;
    IOHelper::saveMediaBaseIntoJSon(ctrl, objCtrl);

    objCtrl[Core::jsonctrl::Mindmap::JSON_CTRL_DEFAULT_INDEX_STYLE]= ctrl->defaultStyleIndex();
    objCtrl[Core::jsonctrl::Mindmap::JSON_CTRL_SPACING]= ctrl->spacing();
    objCtrl[Core::jsonctrl::Mindmap::JSON_CTRL_LINK_LABEL_VISIBILITY]= ctrl->linkLabelVisibility();

    auto updateMindItem= [](QJsonObject& obj, mindmap::MindItem* item)
    {
        namespace ujm= Core::jsonctrl::Mindmap;
        obj[ujm::JSON_MINDITEM_ID]= item->id();
        obj[ujm::JSON_MINDITEM_TEXT]= item->text();
        obj[ujm::JSON_MINDITEM_VISIBLE]= item->isVisible();
        obj[ujm::JSON_MINDITEM_SELECTED]= item->selected();
        obj[ujm::JSON_MINDITEM_TYPE]= item->type();
    };

    auto updatePositionItem= [updateMindItem](QJsonObject& obj, mindmap::PositionedItem* pitem)
    {
        namespace ujm= Core::jsonctrl::Mindmap;
        updateMindItem(obj, pitem);
        obj[ujm::JSON_POSITIONED_POSITIONX]= pitem->position().x();
        obj[ujm::JSON_POSITIONED_POSITIONY]= pitem->position().y();
        obj[ujm::JSON_POSITIONED_CENTERX]= pitem->centerPoint().x();
        obj[ujm::JSON_POSITIONED_CENTERY]= pitem->centerPoint().y();
        obj[ujm::JSON_POSITIONED_WIDTH]= pitem->width();
        obj[ujm::JSON_POSITIONED_HEIGHT]= pitem->height();
        obj[ujm::JSON_POSITIONED_DRAGGED]= pitem->isDragged();
        obj[ujm::JSON_POSITIONED_OPEN]= pitem->open();
        obj[ujm::JSON_POSITIONED_LOCKED]= pitem->isLocked();
        obj[ujm::JSON_POSITIONED_MASS]= pitem->mass();
    };

    auto model= dynamic_cast<mindmap::MindItemModel*>(ctrl->itemModel());
    auto nodes= model->items(mindmap::MindItem::NodeType);
    QJsonArray nodeArray;
    for(auto const& i : nodes)
    {
        auto node= dynamic_cast<mindmap::MindNode*>(i);
        if(!node)
            continue;
        QJsonObject obj;
        updatePositionItem(obj, node);
        // obj[Core::jsonctrl::Mindmap::JSON_NODE_IMAGE]= node->avatarUrl().toString();
        obj[Core::jsonctrl::Mindmap::JSON_NODE_STYLE]= node->styleIndex();
        obj[Core::jsonctrl::Mindmap::JSON_NODE_DESC]= node->description();
        obj[Core::jsonctrl::Mindmap::JSON_NODE_TAGS]= QJsonArray::fromStringList(node->tags());

        nodeArray.append(obj);
    }

    objCtrl[Core::jsonctrl::Mindmap::JSON_NODES]= nodeArray;

    QJsonArray packagesArray;
    auto packagesItem= model->items(mindmap::MindItem::PackageType);
    for(auto const& i : packagesItem)
    {

        auto pack= dynamic_cast<mindmap::PackageNode*>(i);
        if(!pack)
            continue;
        QJsonObject obj;
        updatePositionItem(obj, pack);
        obj[Core::jsonctrl::Mindmap::JSON_PACK_TITLE]= pack->title();
        obj[Core::jsonctrl::Mindmap::JSON_PACK_MINMARGE]= pack->minimumMargin();
        obj[Core::jsonctrl::Mindmap::JSON_PACK_INTERNAL_CHILDREN]= QJsonArray::fromStringList(pack->childrenId());
        packagesArray.append(obj);
    }
    objCtrl[Core::jsonctrl::Mindmap::JSON_PACK_PACKAGES]= packagesArray;

    QJsonArray linkArray;
    auto links= model->items(mindmap::MindItem::LinkType);
    for(auto const& i : links)
    {

        auto link= dynamic_cast<mindmap::LinkController*>(i);
        if(!link)
            continue;

        QJsonObject obj;
        updateMindItem(obj, link);
        auto start= link->start();
        obj[Core::jsonctrl::Mindmap::JSON_LINK_IDSTART]= start ? start->id() : QString();
        auto end= link->end();
        obj[Core::jsonctrl::Mindmap::JSON_LINK_IDEND]= end ? end->id() : QString();
        obj[Core::jsonctrl::Mindmap::JSON_LINK_DIRECTION]= static_cast<int>(link->direction());
        linkArray.append(obj);
    }
    objCtrl[Core::jsonctrl::Mindmap::JSON_LINKS]= linkArray;

    auto imgModel= ctrl->imgModel();
    auto imgs= imgModel->imageInfos();
    QJsonArray imgArray;
    for(const auto& img : imgs)
    {
        QJsonObject obj;
        obj[Core::jsonctrl::Mindmap::JSON_IMG_ID]= img.m_id;
        obj[Core::jsonctrl::Mindmap::JSON_IMG_URL]= img.m_url.toString();
        imgArray.append(obj);
    }
    objCtrl[Core::jsonctrl::Mindmap::JSON_IMGS]= imgArray;

    return IOHelper::jsonObjectToByteArray(objCtrl);
}

QByteArray savePdfView(PdfController* ctrl)
{
    if(!ctrl)
        return {};
    QJsonObject objCtrl;
    IOHelper::saveMediaBaseIntoJSon(ctrl, objCtrl);

    objCtrl[Core::pdfMedia::zoomFactor]= ctrl->zoomFactor();

    return IOHelper::jsonObjectToByteArray(objCtrl);
}

QByteArray IOHelper::saveController(MediaControllerBase* media)
{
    QByteArray data;

    if(!media)
        return data;

    auto uri= media->contentType();
    switch(uri)
    {
    case Core::ContentType::VECTORIALMAP:
        data= VectorialMapMessageHelper::saveVectorialMap(dynamic_cast<VectorialMapController*>(media));
        break;
    case Core::ContentType::PICTURE:
        data= saveImage(dynamic_cast<ImageController*>(media));
        break;
    case Core::ContentType::NOTES:
        data= saveNotes(dynamic_cast<NoteController*>(media));
        break;
    case Core::ContentType::CHARACTERSHEET:
        data= saveCharacterSheet(dynamic_cast<CharacterSheetController*>(media));
        break;
    case Core::ContentType::SHAREDNOTE:
        data= saveSharedNote(dynamic_cast<SharedNoteController*>(media));
        break;
    case Core::ContentType::WEBVIEW:
        data= saveWebView(dynamic_cast<WebpageController*>(media));
        break;
    case Core::ContentType::MINDMAP:
        data= saveMindmap(dynamic_cast<mindmap::MindMapControllerBase*>(media));
        break;
    case Core::ContentType::PDF:
        data= savePdfView(dynamic_cast<PdfController*>(media));
        break;
    default:
        break;
    }

    return data;
}

void IOHelper::writePlaylist(const QString& path, const QList<QUrl>& urls)
{
    QByteArray array;
    QTextStream stream(array);
    for(auto const& url : urls)
    {
        stream << url.toString();
    }

    utils::IOHelper::writeFile(path, array);
}

QJsonObject IOHelper::saveAudioPlayerController(AudioPlayerController* controller)
{
    QJsonObject obj;
    if(!controller)
        return obj;
    obj[Core::jsonctrl::Audio::JSON_AUDIO_VOLUME]= static_cast<int>(controller->volume());
    obj[Core::jsonctrl::Audio::JSON_AUDIO_VISIBLE]= controller->visible();
    obj[Core::jsonctrl::Audio::JSON_PLAYING_MODE]= controller->mode();

    auto m= controller->model()->urls();
    QJsonArray array;
    std::transform(std::begin(m), std::end(m), std::back_inserter(array),
                   [](const QUrl& url) { return url.toString(); });
    obj[Core::jsonctrl::Audio::JSON_AUDIO_URLS]= array;

    return obj;
}

void IOHelper::fetchAudioPlayerController(AudioPlayerController* controller, const QJsonObject& obj)
{
    if(!controller || obj.isEmpty())
        return;

    controller->setVolume(static_cast<uint>(obj[Core::jsonctrl::Audio::JSON_AUDIO_VOLUME].toInt()));
    controller->setVisible(obj[Core::jsonctrl::Audio::JSON_AUDIO_VISIBLE].toBool());
    controller->setPlayingMode(
        static_cast<AudioPlayerController::PlayingMode>(obj[Core::jsonctrl::Audio::JSON_PLAYING_MODE].toInt()));

    auto urlArray= obj[Core::jsonctrl::Audio::JSON_AUDIO_URLS].toArray();
    QList<QUrl> urls;
    std::transform(std::begin(urlArray), std::end(urlArray), std::back_inserter(urls),
                   [](const QJsonValueRef& ref) { return QUrl::fromUserInput(ref.toString()); });

    controller->clear();
    controller->addSong(urls);
}

void IOHelper::readBase(MediaControllerBase* base, QDataStream& input)
{
    if(!base)
        return;

    Core::ContentType type;
    input >> type;
    QString uuid;
    input >> uuid;
    QUrl path;
    input >> path;

    QString name;
    input >> name;

    QString ownerId;
    input >> ownerId;

    base->setUuid(uuid);
    base->setName(name);
    base->setUrl(path);
    base->setOwnerId(ownerId);
}

void IOHelper::readCharacterSheetController(CharacterSheetController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    auto data= IOHelper::textByteArrayToJsonObj(array);
    IOHelper::readBaseFromJson(ctrl, data);

    auto images= data[Core::jsonctrl::sheet::JSON_IMAGES_CONTENT].toArray();
    ctrl->setQmlCode(data[Core::jsonctrl::sheet::JSON_QML_CONTENT].toString());

    auto model= ctrl->model();
    model->readModel(data, true);
    auto imagesModel= ctrl->imageModel();
    IOWorker::fetchImageModel(imagesModel, images);

    auto sharing= data[Core::jsonctrl::sheet::KEY_SHARING_INFO].toArray();
    for(auto const& ref : sharing)
    {
        auto info= ref.toObject();
        auto sheerId= info[Core::jsonctrl::sheet::KEY_SHEETID].toString();
        auto characterId= info[Core::jsonctrl::sheet::KEY_CHARACTERID].toString();
        auto everyone= info[Core::jsonctrl::sheet::KEY_EVERYONE].toBool();
        ctrl->addSheetData({sheerId, characterId, everyone, false});
    }
}

QString IOHelper::copyImageFileIntoCampaign(const QString& path, const QString& dest)
{
    if(path.startsWith(dest))
        return path;
    else
        return utils::IOHelper::copyFile(path, dest, true);
}

Character* IOHelper::dupplicateCharacter(const Character* obj)
{
    auto res= new Character();

    res->setHealthPointsCurrent(obj->getHealthPointsCurrent());
    res->setHealthPointsMax(obj->getHealthPointsMax());
    res->setHealthPointsMin(obj->getHealthPointsMin());
    res->setNpc(obj->isNpc());
    res->setInitCommand(obj->initCommand());
    res->setInitiativeScore(obj->getInitiativeScore());
    res->setImageProvider(obj->getImageProvider());
    res->setCurrentShape(obj->currentShape());
    res->setLifeColor(obj->getLifeColor());
    res->setStateId(obj->stateId());
    res->setDistancePerTurn(obj->getDistancePerTurn());
    res->setNumber(obj->number());
    res->setName(obj->name());
    res->setUuid(obj->uuid());
    res->setColor(obj->getColor());
    res->setSheet(obj->getSheet());
    res->defineActionList(obj->actionList());
    res->defineShapeList(obj->shapeList());
    res->definePropertiesList(obj->propertiesList());
    res->setAvatar(obj->avatar());

    return res;
}

void IOHelper::readPdfController(PdfController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    auto objCtrl= IOHelper::textByteArrayToJsonObj(array);

    IOHelper::readBaseFromJson(ctrl, objCtrl);
    ctrl->setZoomFactor(objCtrl[Core::pdfMedia::zoomFactor].toDouble());
}

void IOHelper::readImageController(ImageController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    if(!array.isEmpty())
        ctrl->setData(array);

    auto objCtrl= IOHelper::textByteArrayToJsonObj(array);

    IOHelper::readBaseFromJson(ctrl, objCtrl);
    ctrl->setZoomLevel(objCtrl[Core::imageMedia::zoomLevel].toDouble());
    ctrl->setFitWindow(objCtrl[Core::imageMedia::fitwindow].toBool());
    // ctrl->setData(QByteArray::fromBase64(objCtrl[Core::imageMedia::data].toString().toUtf8()));
    ctrl->setSharing(objCtrl[Core::imageMedia::sharing].toBool());
    auto url= ctrl->staticData();
    auto data= utils::IOHelper::loadFile(url.toLocalFile());
    ctrl->setData(data);
}

void IOHelper::readWebpageController(WebpageController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    auto objCtrl= IOHelper::textByteArrayToJsonObj(array);

    IOHelper::readBaseFromJson(ctrl, objCtrl);

    ctrl->setHideUrl(objCtrl[Core::webview::JSON_HIDE_URL].toBool());
    ctrl->setKeepSharing(objCtrl[Core::webview::JSON_KEEP_SHARING].toBool());
    ctrl->setHtml(objCtrl[Core::webview::JSON_HTML_CODE].toString());
    ctrl->setPageUrl(QUrl::fromUserInput(objCtrl[Core::webview::JSON_PAGE_URL].toString()));
    ctrl->setState(static_cast<WebpageController::State>(objCtrl[Core::webview::JSON_STATE].toInt()));
    ctrl->setSharingMode(
        static_cast<WebpageController::SharingMode>(objCtrl[Core::webview::JSON_SHARING_MODE].toInt()));
}
QJsonObject IOHelper::readMindmapController(MindMapController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return {};

    auto objCtrl= IOHelper::textByteArrayToJsonObj(array);

    IOHelper::readMindmapControllerBase(ctrl, objCtrl);

    return objCtrl;
}
void IOHelper::readMindmapControllerBase(mindmap::MindMapControllerBase* ctrl, const QJsonObject& objCtrl)
{
    if(!ctrl)
        return;

    IOHelper::readBaseFromJson(ctrl, objCtrl);

    ctrl->setDefaultStyleIndex(objCtrl[Core::jsonctrl::Mindmap::JSON_CTRL_DEFAULT_INDEX_STYLE].toInt());
    ctrl->setSpacing(objCtrl[Core::jsonctrl::Mindmap::JSON_CTRL_SPACING].toBool());
    ctrl->setLinkLabelVisibility(objCtrl[Core::jsonctrl::Mindmap::JSON_CTRL_LINK_LABEL_VISIBILITY].toBool());

    auto imgs= objCtrl[Core::jsonctrl::Mindmap::JSON_IMGS].toArray();
    auto imgModel= ctrl->imgModel();
    for(auto const& imgRef : std::as_const(imgs))
    {
        auto img= imgRef.toObject();
        auto id= img[Core::jsonctrl::Mindmap::JSON_IMG_ID].toString();
        auto url= QUrl(img[Core::jsonctrl::Mindmap::JSON_IMG_URL].toString());
        QPixmap map= utils::IOHelper::readPixmapFromURL(url);
        imgModel->insertPixmap(id, map, url);
    }

    auto updateMindItem= [](const QJsonObject& obj, mindmap::MindItem* item)
    {
        namespace ujm= Core::jsonctrl::Mindmap;
        item->setId(obj[ujm::JSON_MINDITEM_ID].toString());
        item->setText(obj[ujm::JSON_MINDITEM_TEXT].toString());
        item->setVisible(obj[ujm::JSON_MINDITEM_VISIBLE].toBool());
        item->setSelected(false); // obj[ujm::JSON_MINDITEM_SELECTED].toBool()
    };

    auto updatePositionItem= [updateMindItem](const QJsonObject& obj, mindmap::PositionedItem* pitem)
    {
        namespace ujm= Core::jsonctrl::Mindmap;
        updateMindItem(obj, pitem);
        pitem->setPosition(
            {obj[ujm::JSON_POSITIONED_POSITIONX].toDouble(), obj[ujm::JSON_POSITIONED_POSITIONY].toDouble()});

        qDebug() << "position" << pitem->position();

        pitem->setWidth(obj[ujm::JSON_POSITIONED_WIDTH].toDouble());
        pitem->setHeight(obj[ujm::JSON_POSITIONED_HEIGHT].toDouble());
        pitem->setDragged(obj[ujm::JSON_POSITIONED_DRAGGED].toBool());
        pitem->setOpen(obj[ujm::JSON_POSITIONED_OPEN].toBool());
        pitem->setLocked(obj[ujm::JSON_POSITIONED_LOCKED].toBool());
        pitem->setMass(obj[ujm::JSON_POSITIONED_MASS].toInt());
    };

    auto nodes= objCtrl[Core::jsonctrl::Mindmap::JSON_NODES].toArray();
    auto model= dynamic_cast<mindmap::MindItemModel*>(ctrl->itemModel());
    for(auto const& nodeRef : nodes)
    {
        auto nodeJson= nodeRef.toObject();
        auto node= new mindmap::MindNode();
        updatePositionItem(nodeJson, node);

        // node->setAvatarUrl(QUrl(nodeJson[Core::jsonctrl::Mindmap::JSON_NODE_IMAGE].toString()));
        node->setStyleIndex(nodeJson[Core::jsonctrl::Mindmap::JSON_NODE_STYLE].toInt());
        node->setDescription(nodeJson[Core::jsonctrl::Mindmap::JSON_NODE_DESC].toString());
        auto tagArray= nodeJson[Core::jsonctrl::Mindmap::JSON_NODE_TAGS].toArray();
        QStringList tags;
        std::transform(std::begin(tagArray), std::end(tagArray), std::back_inserter(tags),
                       [](const QJsonValue& val) { return val.toString(); });
        node->setTags(tags);

        model->appendItem({node});
    }

    auto packages= objCtrl[Core::jsonctrl::Mindmap::JSON_PACK_PACKAGES].toArray();
    for(auto const& packRef : packages)
    {
        auto pack= packRef.toObject();
        auto node= new mindmap::PackageNode();
        updatePositionItem(pack, node);

        node->setTitle(pack[Core::jsonctrl::Mindmap::JSON_PACK_TITLE].toString());
        node->setMinimumMargin(pack[Core::jsonctrl::Mindmap::JSON_PACK_MINMARGE].toInt());

        auto childArray= pack[Core::jsonctrl::Mindmap::JSON_PACK_INTERNAL_CHILDREN].toArray();
        std::for_each(std::begin(childArray), std::end(childArray),
                      [node, model](const QJsonValue& val) { node->addChild(model->positionItem(val.toString())); });

        model->appendItem({node});
    }

    auto linkArrays= objCtrl[Core::jsonctrl::Mindmap::JSON_LINKS].toArray();
    for(auto const& linkRef : linkArrays)
    {
        auto obj= linkRef.toObject();
        auto link= new mindmap::LinkController();
        updateMindItem(obj, link);

        auto startId= obj[Core::jsonctrl::Mindmap::JSON_LINK_IDSTART].toString();
        auto endId= obj[Core::jsonctrl::Mindmap::JSON_LINK_IDEND].toString();
        auto start= model->positionItem(startId);
        auto end= model->positionItem(endId);

        link->setStart(start);
        link->setEnd(end);
        link->setDirection(
            static_cast<mindmap::LinkController::Direction>(obj[Core::jsonctrl::Mindmap::JSON_LINK_DIRECTION].toInt()));
        model->appendItem({link});
    }
}

bool IOHelper::mergePlayList(const QString& source, const QString& dest)
{
    bool ok;
    auto sourceJson= loadJsonFileIntoObject(source, ok);
    if(!ok)
        return ok;
    auto destJson= loadJsonFileIntoObject(dest, ok);

    if(!ok)
    {
        utils::IOHelper::copyFile(source, dest, true);
        return true;
    }

    auto arraySrc= sourceJson[Core::jsonctrl::Audio::JSON_AUDIO_URLS].toArray();
    auto arrayDst= destJson[Core::jsonctrl::Audio::JSON_AUDIO_URLS].toArray();

    arrayDst.append(arraySrc);
    destJson[Core::jsonctrl::Audio::JSON_AUDIO_URLS]= arrayDst;

    writeJsonObjectIntoFile(dest, destJson);
    return true;
}

QStringList IOHelper::mediaList(const QString& source, Core::MediaType type)
{
    QDir dir(source);
    return dir.entryList(helper::utils::extentionPerType(helper::utils::mediaTypeToContentType(type), false, true),
                         QDir::Files | QDir::Readable);
}

bool IOHelper::copyArrayModelAndFile(const QString& source, const QString& sourceDir, const QString& dest,
                                     const QString& destDir)
{
    bool ok;
    auto sourceJson= loadJsonFileIntoArray(source, ok);
    if(!ok)
        return false; // nothing to do
    auto destJson= loadJsonFileIntoArray(dest, ok);

    if(!ok)
    {
        utils::IOHelper::copyFile(source, dest, true);
    }

    destJson.append(sourceJson);
    writeJsonArrayIntoFile(dest, destJson);

    // Copy npcs files
    QDir dir(sourceDir);
    auto fileList= dir.entryList(QDir::Files | QDir::Readable);
    for(auto const& file : std::as_const(fileList))
    {
        utils::IOHelper::copyFile(QString("%1/%2").arg(sourceDir, file), QString("%1/%2").arg(destDir, file), true);
    }
    return true;
}

/*bool IOHelper::copyMedia(const QString &source, const QString &dest, Core::MediaType type)
{

}*/

/*QString IOHelper::shortNameFromPath(const QString& path)
{
    QFileInfo info(path);
    return info.baseName();
}

QString IOHelper::shortNameFromUrl(const QUrl& url)
{
    return shortNameFromPath(url.fileName());
}

QString IOHelper::absoluteToRelative(const QString& absolute, const QString& root)
{
    QDir dir(root);
    QFileInfo path(absolute);
    if(!path.isAbsolute())
        return absolute;

    return dir.relativeFilePath(absolute);
}*/

QJsonObject IOHelper::stateToJSonObject(CharacterState* state, const QString& root)
{
    QJsonObject stateJson;

    stateJson[k_state_id]= state->id();
    stateJson[k_state_label]= state->label();
    stateJson[k_state_color]= state->color().name();
    auto pathImg= state->imagePath();
    pathImg= utils::IOHelper::absoluteToRelative(pathImg, root);
    stateJson[k_state_image]= pathImg;
    return stateJson;
}

QJsonObject IOHelper::npcToJsonObject(const campaign::NonPlayableCharacter* npc, const QString& destination)
{
    if(nullptr == npc)
        return {};
    QJsonObject obj;

    obj[Core::JsonKey::JSON_NPC_ID]= npc->uuid();
    obj[Core::JsonKey::JSON_NPC_NAME]= npc->name();
    obj[Core::JsonKey::JSON_NPC_AVATAR]= utils::IOHelper::absoluteToRelative(npc->avatarPath(), destination);
    obj[Core::JsonKey::JSON_NPC_TAGS]= QJsonArray::fromStringList(npc->tags());
    obj[Core::JsonKey::JSON_NPC_DESCRIPTION]= npc->description();

    obj[Core::JsonKey::JSON_NPC_INITCOMMAND]= npc->initCommand();
    obj[Core::JsonKey::JSON_NPC_INITVALUE]= npc->getInitiativeScore();
    obj[Core::JsonKey::JSON_NPC_COLOR]= npc->getColor().name();
    obj[Core::JsonKey::JSON_NPC_HP]= npc->getHealthPointsCurrent();
    obj[Core::JsonKey::JSON_NPC_MAXHP]= npc->getHealthPointsMax();
    obj[Core::JsonKey::JSON_NPC_MINHP]= npc->getHealthPointsMin();
    obj[Core::JsonKey::JSON_NPC_DIST_PER_TURN]= npc->getDistancePerTurn();
    obj[Core::JsonKey::JSON_NPC_STATEID]= npc->stateId();
    obj[Core::JsonKey::JSON_NPC_LIFECOLOR]= npc->getLifeColor().name();
    obj[Core::JsonKey::JSON_TOKEN_SIZE]= npc->size();

    QJsonArray actionArray;
    auto actionList= npc->actionList();
    for(auto act : actionList)
    {
        if(act == nullptr)
            continue;
        QJsonObject actJson;
        actJson[Core::JsonKey::JSON_NPC_ACTION_NAME]= act->name();
        actJson[Core::JsonKey::JSON_NPC_ACTION_COMMAND]= act->command();
        actionArray.append(actJson);
    }
    obj[Core::JsonKey::JSON_NPC_ACTIONS]= actionArray;

    QJsonArray propertyArray;
    auto properties= npc->propertiesList();
    for(auto property : properties)
    {
        if(property == nullptr)
            continue;
        QJsonObject actJson;
        actJson[Core::JsonKey::JSON_NPC_PROPERTY_NAME]= property->name();
        actJson[Core::JsonKey::JSON_NPC_PROPERTY_VALUE]= property->value();
        propertyArray.append(actJson);
    }
    obj[Core::JsonKey::JSON_NPC_PROPERTIES]= propertyArray;

    QJsonArray shapeArray;
    auto shapes= npc->shapeList();
    for(auto shape : shapes)
    {
        if(shape == nullptr)
            continue;
        QJsonObject actJson;
        actJson[Core::JsonKey::JSON_NPC_SHAPE_NAME]= shape->name();

        auto data= IOHelper::imageToData(shape->image());

        actJson[Core::JsonKey::JSON_NPC_SHAPE_DATAIMG]= QString::fromUtf8(data.toBase64());
        shapeArray.append(actJson);
    }
    obj[Core::JsonKey::JSON_NPC_SHAPES]= shapeArray;

    return obj;
}

campaign::NonPlayableCharacter* IOHelper::jsonObjectToNpc(const QJsonObject& obj, const QString& rootDir)
{
    auto npc= new campaign::NonPlayableCharacter();
    auto uuid= obj[Core::JsonKey::JSON_NPC_ID].toString();
    auto desc= obj[Core::JsonKey::JSON_NPC_DESCRIPTION].toString();
    auto name= obj[Core::JsonKey::JSON_NPC_NAME].toString();
    auto avatar= obj[Core::JsonKey::JSON_NPC_AVATAR].toString();
    auto stateid= obj[Core::JsonKey::JSON_NPC_STATEID].toString();
    auto tags= obj[Core::JsonKey::JSON_NPC_TAGS].toArray();
    auto tokenPath= obj[Core::JsonKey::JSON_NPC_TOKEN].toString();
    QStringList list;
    std::transform(std::begin(tags), std::end(tags), std::back_inserter(list),
                   [](const QJsonValue& val) { return val.toString(); });

    npc->setUuid(uuid);
    npc->setName(name);
    npc->setDescription(desc);
    npc->setStateId(stateid);
    npc->setAvatar(utils::IOHelper::loadFile(QString("%1/%2").arg(rootDir, avatar)));
    npc->setAvatarPath(avatar);
    npc->setTags(list);

    npc->setSize(obj[Core::JsonKey::JSON_TOKEN_SIZE].toInt());
    npc->setColor(obj[Core::JsonKey::JSON_NPC_COLOR].toString());
    npc->setHealthPointsMax(obj[Core::JsonKey::JSON_NPC_MAXHP].toInt());
    npc->setHealthPointsMin(obj[Core::JsonKey::JSON_NPC_MINHP].toInt());
    npc->setHealthPointsCurrent(obj[Core::JsonKey::JSON_NPC_HP].toInt());

    npc->setInitiativeScore(obj[Core::JsonKey::JSON_NPC_INITVALUE].toInt());
    npc->setInitCommand(obj[Core::JsonKey::JSON_NPC_INITCOMMAND].toString());

    auto actionArray= obj[Core::JsonKey::JSON_NPC_ACTIONS].toArray();
    for(auto act : actionArray)
    {
        auto actObj= act.toObject();
        auto action= new CharacterAction();
        action->setName(actObj[Core::JsonKey::JSON_NPC_ACTION_NAME].toString());
        action->setCommand(actObj[Core::JsonKey::JSON_NPC_ACTION_COMMAND].toString());
        npc->addAction(action);
    }

    auto propertyArray= obj[Core::JsonKey::JSON_NPC_PROPERTIES].toArray();
    for(auto pro : propertyArray)
    {
        auto proObj= pro.toObject();
        auto property= new CharacterProperty();
        property->setName(proObj[Core::JsonKey::JSON_NPC_PROPERTY_NAME].toString());
        property->setValue(proObj[Core::JsonKey::JSON_NPC_PROPERTY_VALUE].toString());
        npc->addProperty(property);
    }

    auto shapeArray= obj[Core::JsonKey::JSON_NPC_SHAPES].toArray();
    for(auto sha : shapeArray)
    {
        auto objSha= sha.toObject();
        auto shape= new CharacterShape();
        shape->setName(objSha[Core::JsonKey::JSON_NPC_SHAPE_NAME].toString());
        auto avatarData= QByteArray::fromBase64(objSha[Core::JsonKey::JSON_NPC_SHAPE_DATAIMG].toString().toUtf8());
        QImage img= QImage::fromData(avatarData);
        shape->setImage(img);
        npc->addShape(shape);
    }

    return npc;
}

QJsonObject IOHelper::diceAliasToJSonObject(DiceAlias* alias)
{
    QJsonObject aliasJson;
    aliasJson[Core::DiceAlias::k_dice_command]= alias->command();
    aliasJson[Core::DiceAlias::k_dice_comment]= alias->comment();
    aliasJson[Core::DiceAlias::k_dice_pattern]= alias->pattern();
    aliasJson[Core::DiceAlias::k_dice_enabled]= alias->isEnable();
    aliasJson[Core::DiceAlias::k_dice_replacement]= alias->isReplace();
    return aliasJson;
}

RolisteamTheme* IOHelper::jsonToTheme(const QJsonObject& json)
{
    auto theme= new RolisteamTheme();
    theme->setName(json["name"].toString());
    theme->setRemovable(json["removable"].toBool());
    theme->setCss(json["css"].toString());
    theme->setBackgroundPosition(json["position"].toInt());
    QString bgColorName= json["bgColor"].toString();
    QColor color = QColor::fromString(bgColorName);
    theme->setBackgroundColor(color);

    theme->setBackgroundImage(json["bgPath"].toString(":/resources/rolistheme/workspacebackground.jpg"));
    theme->setStyle(QStyleFactory::create(json["stylename"].toString()));
    QColor diceColor;
    diceColor.setNamedColor(json["diceHighlight"].toString());
    theme->setDiceHighlightColor(diceColor);
    QJsonArray colors= json["colors"].toArray();
    int i= 0;
    auto model= theme->paletteModel();
    for(auto const& ref : colors)
    {
        QJsonObject paletteObject= ref.toObject();
        QColor color;
        color.setNamedColor(paletteObject["color"].toString());
        model->setColor(i, color);
        ++i;
    }

    return theme;
}

QJsonObject IOHelper::themeToObject(const RolisteamTheme* theme)
{
    QJsonObject json;
    json["name"]= theme->getName();
    json["removable"]= theme->isRemovable();
    json["css"]= theme->getCss();
    json["position"]= theme->getBackgroundPosition();
    json["bgColor"]= theme->getBackgroundColor().name();
    json["diceHighlight"]= theme->getDiceHighlightColor().name();
    json["bgPath"]= theme->getBackgroundImage();
    json["stylename"]= theme->getStyleName();

    QJsonArray colors;
    auto const& data= theme->paletteModel()->data();
    for(auto const& tmp : data)
    {
        QJsonObject paletteObject;
        tmp->writeTo(paletteObject);
        /*json["role"]= static_cast<int>(tmp->getRole());
        json["group"]= static_cast<int>(tmp->getGroup());
        json["name"]= tmp->getName();*/
        json["color"]= tmp->getColor().name();
        colors.append(paletteObject);
    }
    json["colors"]= colors;
    return json;
}

bool IOHelper::writeByteArrayIntoFile(const QString& destination, const QByteArray& array)
{
    QSaveFile file(destination);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    file.write(array);
    file.commit();
    return true;
}

void IOHelper::writeJsonArrayIntoFile(const QString& destination, const QJsonArray& array)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QJsonDocument doc;
    doc.setArray(array);

    QSaveFile file(destination);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.commit();
    }
}

void IOHelper::writeJsonObjectIntoFile(const QString& destination, const QJsonObject& obj)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    QJsonDocument doc;
    doc.setObject(obj);

    QSaveFile file(destination);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.commit();
    }
}

void IOHelper::readNoteController(NoteController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    auto obj= IOHelper::textByteArrayToJsonObj(array);
    IOHelper::readBaseFromJson(ctrl, obj);

    ctrl->setText(obj[Core::keys::KEY_TEXT].toString());
}

void IOHelper::readSharedNoteController(SharedNoteController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    auto obj= IOHelper::textByteArrayToJsonObj(array);
    IOHelper::readBaseFromJson(ctrl, obj);
    /*
     QJsonObject objCtrl;
     IOHelper::saveMediaBaseIntoJSon(ctrl, objCtrl);

      objCtrl[Core::keys::KEY_TEXT]= ctrl->text();
      objCtrl[Core::keys::KEY_SYNTAXE]= static_cast<int>(ctrl->highligthedSyntax());
      objCtrl[Core::keys::KEY_PREVIEW_VISIBLE]= ctrl->markdownVisible();
      return IOHelper::jsonObjectToByteArray(objCtrl);*/

    ctrl->setText(obj[Core::keys::KEY_TEXT].toString());

    ctrl->setMarkdownVisible(obj[Core::keys::KEY_PREVIEW_VISIBLE].toBool());
    ctrl->setHighligthedSyntax(
        static_cast<SharedNoteController::HighlightedSyntax>(obj[Core::keys::KEY_SYNTAXE].toInt()));
}

QUrl IOHelper::findSong(const QString& name, QStringList list)
{
    QUrl url(name, QUrl::StrictMode);
    if(!url.scheme().isEmpty())
    {
        return url;
    }
    else
    {
        for(auto const& path : list)
        {
            QFileInfo info(QString("%1/%2").arg(path, name));
            if(info.exists())
                return QUrl::fromLocalFile(info.absoluteFilePath());
        }
    }

    return {};
}
// QHash<QString, std::map<QString, QVariant>>
