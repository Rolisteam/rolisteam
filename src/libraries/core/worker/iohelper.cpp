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
#include "iohelper.h"

#include <QBuffer>
#include <QCborValue>
#include <QClipboard>
#include <QColor>
#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QMimeData>
#include <QString>
#include <QStyleFactory>
#include <QVariant>
#include <map>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#include "core/data/character.h"
#include "core/model/genericmodel.h"
#include "core/network/channel.h"
#include "core/network/tcpclient.h"
#include "data/rolisteamtheme.h"
#include "media/mediatype.h"
#include "model/nonplayablecharactermodel.h"

#ifdef WITH_PDF
#include "controller/media_controller/pdfmediacontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#endif

#include "data/character.h"
#include "diceparser/include/dicealias.h"

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "worker/vectorialmapmessagehelper.h"

const QString k_language_dir_path= ":/translations";
const QString k_rolisteam_pattern= "rolisteam";
const QString k_qt_pattern= "qt";
// json key of dice
const QString k_dice_pattern{"pattern"};
const QString k_dice_command{"command"};
const QString k_dice_comment{"comment"};
const QString k_dice_enabled{"enable"};
const QString k_dice_replacement{"replace"};

const QString k_state_id{"id"};
const QString k_state_label{"label"};
const QString k_state_color{"color"};
const QString k_state_image{"image"};

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

QByteArray IOHelper::loadFile(const QString& filepath)
{
    QByteArray data;
    if(filepath.isEmpty())
        return data;
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly))
    {
        data= file.readAll();
    }
    return data;
}

bool makeSurePathExist(const QDir& dir)
{
    if(!dir.exists())
        dir.mkpath(dir.path());

    return dir.exists();
}

bool IOHelper::makeDir(const QString& dir)
{
    return makeSurePathExist(dir);
}

QString IOHelper::copyFile(const QString& source, const QString& destination)
{
    QFile src(source);
    QFileInfo srcInfo(source);
    QFileInfo dest(destination);
    if(dest.isDir())
        dest.setFile(QString("%1/%2").arg(destination, srcInfo.fileName()));

    if(makeSurePathExist(dest.dir()))
    {
        src.copy(dest.absoluteFilePath());
    }
    return dest.absoluteFilePath();
}

void IOHelper::removeFile(const QString& source)
{
    QFile::remove(source);
}

void IOHelper::writeFile(const QString& path, const QByteArray& arry, bool override)
{
    QFile file(path);
    if(file.open(override ? QIODevice::WriteOnly : QIODevice::Append))
    {
        file.write(arry);
    }
}

void IOHelper::moveFile(const QString& source, const QString& destination)
{
    QFile src(source);
    QFileInfo srcInfo(source);
    QFileInfo dest(destination);

    if(dest.isDir())
        dest.setFile(QString("%1/%2").arg(destination, srcInfo.fileName()));

    if(makeSurePathExist(dest.dir()))
    {
        src.rename(dest.absoluteFilePath());
    }
}

QString IOHelper::readTextFile(const QString& path)
{
    if(path.isEmpty())
        return {};
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return {};

    QTextStream out(&file);
    return out.readAll();
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
    auto doc= QCborValue(data);
    return doc.toJsonValue().toArray();
}

QByteArray IOHelper::jsonObjectToByteArray(const QJsonObject& obj)
{
    QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson(QJsonDocument::Indented);
}

QJsonObject IOHelper::loadJsonFileIntoObject(const QString& filename, bool& ok)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        ok= false;
        return {};
    }
    QJsonDocument doc= QJsonDocument::fromJson(file.readAll());
    ok= true;
    return doc.object();
}

QJsonArray IOHelper::loadJsonFileIntoArray(const QString& filename, bool& ok)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        ok= false;
        return {};
    }
    QJsonDocument doc= QJsonDocument::fromJson(file.readAll());
    ok= true;
    return doc.array();
}

QJsonArray IOHelper::fetchLanguageModel()
{
    QJsonArray array;
    QDir dir(k_language_dir_path);

    auto list= dir.entryList(QStringList() << QStringLiteral("*.qm"), QDir::Files);

    QRegularExpression reQt(QStringLiteral("%1_(.*)\\.qm").arg(k_qt_pattern));
    QHash<QString, QString> hash;
    for(const auto& info : qAsConst(list))
    {
        auto match= reQt.match(info);
        if(match.hasMatch())
        {
            hash.insert(match.captured(1), info);
        }
    }

    QRegularExpression reRolisteam(QStringLiteral("%1_(.*)\\.qm").arg(k_rolisteam_pattern));
    for(auto info : qAsConst(list))
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
            else
                qWarning() << "No Qt translation for " << iso;

            QLocale local(iso);

            obj["path"]= paths;
            obj["code"]= iso;
            obj["commonName"]= QLocale::languageToString(local.language());
            obj["langname"]= local.nativeLanguageName();
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
    output << base->path();
    output << base->name();
    output << base->ownerId();
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

QPixmap IOHelper::readPixmapFromURL(const QUrl& url)
{
    QPixmap map;
    if(url.isLocalFile())
    {
        map= readPixmapFromFile(url.toLocalFile());
    }
    return map;
}

QPixmap IOHelper::readPixmapFromFile(const QString& uri)
{
    QPixmap map(uri);
    return map;
}

QPixmap IOHelper::dataToPixmap(const QByteArray& data)
{
    QPixmap pix;
    pix.loadFromData(data);
    return pix;
}

QImage IOHelper::dataToImage(const QByteArray& data)
{
    QImage img;
    img.loadFromData(data);
    return img;
}

QByteArray saveImage(ImageController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;

    QDataStream output(&data, QIODevice::WriteOnly);

    IOHelper::saveBase(ctrl, output);

    output << ctrl->fitWindow();
    output << ctrl->data();
    output << ctrl->zoomLevel();

    return data;
}

QByteArray saveNotes(NoteController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;
    QDataStream output(&data, QIODevice::WriteOnly);
    IOHelper::saveBase(ctrl, output);

    output << ctrl->text();
    return data;
}

QByteArray saveCharacterSheet(CharacterSheetController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;
    QDataStream output(&data, QIODevice::WriteOnly);
    IOHelper::saveBase(ctrl, output);

    QJsonDocument doc;
    QJsonObject obj= ctrl->rootObject();
    auto model= ctrl->model();
    QJsonObject dataObj;
    model->writeModel(dataObj);

    obj["character"]= dataObj;

    auto images= ctrl->imageModel();
    QJsonArray array;
    images->save(array);

    obj["images"]= array;

    doc.setObject(obj);

    QCborValue val(doc.toJson());
    output << val;

    return data;
}

QByteArray saveSharedNote(SharedNoteController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;
    QDataStream output(&data, QIODevice::WriteOnly);

    IOHelper::saveBase(ctrl, output);

    // output << ctrl->permission();
    output << ctrl->text();
    output << ctrl->highligthedSyntax();
    output << ctrl->markdownVisible();
    return data;
}

QByteArray saveWebView(WebpageController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;
    QDataStream output(&data, QIODevice::WriteOnly);

    IOHelper::saveBase(ctrl, output);

    output << ctrl->hideUrl();
    output << ctrl->keepSharing();
    output << ctrl->htmlSharing();
    output << ctrl->urlSharing();
    output << ctrl->html();
    output << ctrl->state();
    output << ctrl->sharingMode();
    return data;
}

#ifdef WITH_PDF
QByteArray savePdfView(PdfController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;
    QDataStream output(&data, QIODevice::WriteOnly);

    IOHelper::saveBase(ctrl, output);

    output << ctrl->data();
    return data;
}
#endif

QByteArray IOHelper::saveController(MediaControllerBase* media)
{
    QByteArray data;

    if(!media)
        return data;

    //

    auto uri= media->contentType();
    switch(uri)
    {
    case Core::ContentType::VECTORIALMAP:
        data= VectorialMapMessageHelper::saveVectorialMap(dynamic_cast<VectorialMapController*>(media));
        break;
    case Core::ContentType::PICTURE:
        data= saveImage(dynamic_cast<ImageController*>(media));
        break;
    case Core::ContentType::ONLINEPICTURE:
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
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        data= savePdfView(dynamic_cast<PdfController*>(media));
        break;
#endif
    default:
        break;
    }

    return data;
}

MediaControllerBase* IOHelper::loadController(const QByteArray& data)
{
    QDataStream input(data);
    Core::ContentType type;
    input >> type;

    MediaControllerBase* value= nullptr;
    switch(type)
    {
    case Core::ContentType::VECTORIALMAP:
    {
        auto ctrl= new VectorialMapController("");
        value= ctrl;
        VectorialMapMessageHelper::readVectorialMapController(ctrl, data);
    }
    break;
    case Core::ContentType::PICTURE:
    case Core::ContentType::ONLINEPICTURE:
    {
        auto ctrl= new ImageController("", "", "");
        value= ctrl;
        readImageController(ctrl, data);
    }
    break;
    case Core::ContentType::NOTES:
    {
        auto ctrl= new NoteController("");
        value= ctrl;
        readNoteController(ctrl, data);
    }
    break;
    case Core::ContentType::CHARACTERSHEET:
    {
        auto ctrl= new CharacterSheetController("", "");
        value= ctrl;
        readCharacterSheetController(ctrl, data);
    }
    break;
    case Core::ContentType::SHAREDNOTE:
    {
        auto ctrl= new SharedNoteController("", "", "");
        value= ctrl;
        readSharedNoteController(ctrl, data);
    }
    break;
    case Core::ContentType::WEBVIEW:
    {
        auto ctrl= new WebpageController("");
        value= ctrl;
        readWebpageController(ctrl, data);
    }
    break;
#ifdef WITH_PDF
    case Core::ContentType::PDF:
    {
        auto ctrl= new PdfController("", "");
        value= ctrl;
        readPdfController(ctrl, data);
    }
    break;
#endif
    default:
        Q_ASSERT(false); // No valid contentType
        break;
    }

    return value;
}

void IOHelper::readBase(MediaControllerBase* base, QDataStream& input)
{
    if(!base)
        return;

    Core::ContentType type;
    input >> type;
    QString uuid;
    input >> uuid;
    QString path;
    input >> path;

    QString name;
    input >> name;

    QString ownerId;
    input >> ownerId;

    base->setUuid(uuid);
    base->setName(name);
    base->setPath(path);
    base->setOwnerId(ownerId);
}

void IOHelper::readCharacterSheetController(CharacterSheetController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    QByteArray charactersheetData;
    input >> charactersheetData;

    QCborValue value(charactersheetData);

    QJsonDocument doc= QJsonDocument::fromJson(value.toByteArray());
    auto obj= doc.object();
    auto charactersData= obj["character"].toObject();
    auto images= obj["images"].toArray();

    auto model= ctrl->model();
    model->readModel(charactersData, true);
    auto imagesModel= ctrl->imageModel();
    imagesModel->load(images);
}

#ifdef WITH_PDF
void IOHelper::readPdfController(PdfController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    QByteArray pdfData;
    input >> pdfData;
    ctrl->setData(pdfData);
}
#endif

void IOHelper::readImageController(ImageController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;

    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    bool b;
    input >> b;

    QByteArray dataByte;
    input >> dataByte;

    qreal zoom;
    input >> zoom;

    ctrl->setFitWindow(b);
    ctrl->setData(dataByte);
    ctrl->setZoomLevel(zoom);
}

void IOHelper::readWebpageController(WebpageController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    bool hide;
    input >> hide;

    bool keepSharing;
    input >> keepSharing;

    bool htmlSharing;
    input >> htmlSharing;

    bool urlSharing;
    input >> urlSharing;

    QString html;
    input >> html;

    WebpageController::State state;
    input >> state;

    WebpageController::SharingMode mode;
    input >> mode;

    ctrl->setHideUrl(hide);
    ctrl->setKeepSharing(keepSharing);
    ctrl->setHtmlSharing(htmlSharing);
    ctrl->setUrlSharing(urlSharing);
    ctrl->setState(state);
    ctrl->setSharingMode(mode);
}
void IOHelper::readMindmapController(MindMapController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    // TODO read data to define mindmapcontroller.
}

QString IOHelper::shortNameFromPath(const QString& path)
{
    QFileInfo info(path);
    return info.baseName();
}

QString IOHelper::absoluteToRelative(const QString& absolute, const QString& root)
{
    QDir dir(root);
    QFileInfo path(absolute);
    if(!path.isAbsolute())
        return absolute;

    return dir.relativeFilePath(absolute);
}

QJsonObject IOHelper::stateToJSonObject(CharacterState* state, const QString& root)
{
    QJsonObject stateJson;

    stateJson[k_state_id]= state->id();
    stateJson[k_state_label]= state->label();
    stateJson[k_state_color]= state->color().name();
    auto pathImg= state->imagePath();
    pathImg= absoluteToRelative(pathImg, root);
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
    obj[Core::JsonKey::JSON_NPC_AVATAR]= absoluteToRelative(npc->avatarPath(), destination);
    obj[Core::JsonKey::JSON_NPC_TAGS]= QJsonArray::fromStringList(npc->tags());
    obj[Core::JsonKey::JSON_NPC_DESCRIPTION]= npc->description();

    obj[Core::JsonKey::JSON_NPC_INITCOMMAND]= npc->getInitCommand();
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
    auto tags= obj[Core::JsonKey::JSON_NPC_TAGS].toArray();
    auto tokenPath= obj[Core::JsonKey::JSON_NPC_TOKEN].toString();
    QStringList list;
    std::transform(std::begin(tags), std::end(tags), std::back_inserter(list),
                   [](const QJsonValue& val) { return val.toString(); });

    npc->setUuid(uuid);
    npc->setName(name);
    npc->setDescription(desc);
    npc->setAvatar(IOHelper::loadFile(QString("%1/%2").arg(rootDir, avatar)));
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
    aliasJson[k_dice_command]= alias->command();
    aliasJson[k_dice_comment]= alias->comment();
    aliasJson[k_dice_pattern]= alias->pattern();
    aliasJson[k_dice_enabled]= alias->isEnable();
    aliasJson[k_dice_replacement]= alias->isReplace();
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
    QColor color;
    color.setNamedColor(bgColorName);
    theme->setBackgroundColor(color);

    theme->setBackgroundImage(json["bgPath"].toString());
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
    int i= 0;
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

void IOHelper::writeJsonArrayIntoFile(const QString& destination, const QJsonArray& array)
{
    QJsonDocument doc;
    doc.setArray(array);

    QFile file(destination);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
    }
}

void IOHelper::writeJsonObjectIntoFile(const QString& destination, const QJsonObject& obj)
{
    QJsonDocument doc;
    doc.setObject(obj);

    QFile file(destination);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson(QJsonDocument::Indented));
    }
}

void IOHelper::readNoteController(NoteController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    QString text;
    input >> text;
    ctrl->setText(text);
}

void IOHelper::readSharedNoteController(SharedNoteController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

    /*ParticipantModel::Permission perm;
    input >> perm;*/

    QString text;
    input >> text;

    SharedNoteController::HighlightedSyntax syntax;
    input >> syntax;

    bool b;
    input >> b;

    ctrl->setMarkdownVisible(b);
    ctrl->setText(text);
    ctrl->setHighligthedSyntax(syntax);
    // ctrl->setPermission(perm);
}

// QHash<QString, std::map<QString, QVariant>>
