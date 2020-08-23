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

#include <QColor>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <QVariant>
#include <map>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#ifdef WITH_PDF
#include "controller/media_controller/pdfmediacontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#endif

#include "data/character.h"
#include "data/cleveruri.h"
#include "model/vmapitemmodel.h"
#include "vmap/vmap.h"

#include "vmap/controller/characteritemcontroller.h"
#include "vmap/controller/ellipsecontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/linecontroller.h"
#include "vmap/controller/pathcontroller.h"
#include "vmap/controller/rectcontroller.h"
#include "vmap/controller/sightcontroller.h"
#include "vmap/controller/textcontroller.h"
#include "vmap/controller/visualitemcontroller.h"

#include "vmap/items/characteritem.h"
#include "vmap/items/ellipsitem.h"
#include "vmap/items/imageitem.h"
#include "vmap/items/lineitem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/rectitem.h"
#include "vmap/items/textitem.h"

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"

IOHelper::IOHelper() {}

bool IOHelper::loadToken(const QString& filename, std::map<QString, QVariant>& params)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QJsonDocument doc= QJsonDocument::fromJson(file.readAll());
    QJsonObject obj= doc.object();
    params["side"]= obj["size"].toDouble();
    auto character= new Character();
    character->setNpc(true);
    character->setName(obj["name"].toString());
    character->setColor(obj["color"].toString());
    character->setHealthPointsMax(obj["lifeMax"].toInt());
    character->setHealthPointsMin(obj["lifeMin"].toInt());
    character->setHealthPointsCurrent(obj["lifeCurrent"].toInt());
    character->setInitiativeScore(obj["initValue"].toInt());
    character->setInitCommand(obj["initCommand"].toString());
    character->setAvatarPath(obj["avatarUri"].toString());

    auto array= QByteArray::fromBase64(obj["avatarImg"].toString().toUtf8());
    character->setAvatar(QImage::fromData(array));

    auto actionArray= obj["actions"].toArray();
    for(auto act : actionArray)
    {
        auto actObj= act.toObject();
        auto action= new CharacterAction();
        action->setName(actObj["name"].toString());
        action->setCommand(actObj["command"].toString());
        character->addAction(action);
    }

    auto propertyArray= obj["properties"].toArray();
    for(auto pro : propertyArray)
    {
        auto proObj= pro.toObject();
        auto property= new CharacterProperty();
        property->setName(proObj["name"].toString());
        property->setValue(proObj["value"].toString());
        character->addProperty(property);
    }

    auto shapeArray= obj["shapes"].toArray();
    for(auto sha : shapeArray)
    {
        auto objSha= sha.toObject();
        auto shape= new CharacterShape();
        shape->setName(objSha["name"].toString());
        shape->setUri(objSha["uri"].toString());
        auto avatarData= QByteArray::fromBase64(objSha["dataImg"].toString().toUtf8());
        QImage img= QImage::fromData(avatarData);
        shape->setImage(img);
        character->addShape(shape);
    }

    params["character"]= QVariant::fromValue(character);
    return true;
}

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
    auto doc= QJsonDocument::fromBinaryData(data);
    return doc.object();
}

QJsonArray IOHelper::byteArrayToJsonArray(const QByteArray& data)
{
    auto doc= QJsonDocument::fromBinaryData(data);
    return doc.array();
}

void saveBase(MediaControllerBase* base, QDataStream& output)
{
    if(!base)
        return;

    output << base->contentType();
    output << base->uuid();
    output << base->path();
    output << base->name();
    output << base->ownerId();
}

void saveImage(ImageController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveBase(ctrl, output);

    output << ctrl->fitWindow();
    output << ctrl->data();
    output << ctrl->zoomLevel();
}
void saveVisualItemController(const vmap::VisualItemController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    output << ctrl->itemType();
    output << ctrl->selected();
    output << ctrl->visible();
    output << ctrl->opacity();
    output << ctrl->rotation();
    output << ctrl->layer();
    output << ctrl->pos();
    output << ctrl->uuid();
    output << ctrl->color();
    output << ctrl->locked();
    output << ctrl->tool();
}

void readVisualItemController(vmap::VisualItemController::ItemType type, std::map<QString, QVariant>& map,
                              QDataStream& input)
{
    /*vmap::VisualItemController::ItemType type;
    input >> type;*/

    bool selected;
    input >> selected;

    bool visible;
    input >> visible;

    qreal opacity;
    input >> opacity;

    qreal rotation;
    input >> rotation;

    Core::Layer layer;
    input >> layer;

    QPointF pos;
    input >> pos;

    QString uuid;
    input >> uuid;

    QColor color;
    input >> color;

    bool locked;
    input >> locked;

    Core::SelectableTool tool;
    input >> tool;

    map.insert({"type", type});
    map.insert({"selected", selected});
    map.insert({"visible", visible});
    map.insert({"opacity", opacity});
    map.insert({"rotation", rotation});
    map.insert({"layer", QVariant::fromValue(layer)});
    map.insert({"position", pos});
    map.insert({"uuid", uuid});
    map.insert({"color", color});
    map.insert({"locked", locked});
    map.insert({"tool", tool});
}

void saveVmapSightController(const vmap::SightController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->characterSight();
    output << ctrl->fowPath();
    output << ctrl->rect();
    output << ctrl->characterCount();

    auto singus= ctrl->singularityList();
    output << static_cast<int>(singus.size());
    std::for_each(singus.begin(), singus.end(), [&output](const std::pair<QPolygonF, bool>& singu) {
        output << singu.first;
        output << singu.second;
    });
}

void saveVMapRectItemController(const vmap::RectController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->rect();
    output << ctrl->filled();
    output << ctrl->penWidth();
}

std::map<QString, QVariant> readRectController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::RECT, maps, input);

    QRectF rect;
    input >> rect;
    bool filled;
    input >> filled;

    quint16 penWidth;
    input >> penWidth;

    maps.insert({"rect", rect});
    maps.insert({"filled", filled});
    maps.insert({"penWidth", penWidth});

    return maps;
}

void saveVMapEllipseItemController(const vmap::EllipseController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->rx();
    output << ctrl->ry();
    output << ctrl->filled();
    output << ctrl->penWidth();
}

std::map<QString, QVariant> readEllipseController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::ELLIPSE, maps, input);

    qreal rx;
    input >> rx;

    qreal ry;
    input >> ry;

    bool filled;
    input >> filled;

    quint16 penWidth;
    input >> penWidth;

    maps.insert({"rx", rx});
    maps.insert({"ry", ry});
    maps.insert({"filled", filled});
    maps.insert({"penWidth", penWidth});

    return maps;
}

void saveVMapImageItemController(const vmap::ImageController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->pixmap();
    output << ctrl->data();
    output << ctrl->rect();
    output << ctrl->path();
    output << ctrl->ratio();
}
std::map<QString, QVariant> readImageController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::IMAGE, maps, input);

    QPixmap pixmap;
    input >> pixmap;

    QByteArray data;
    input >> data;

    QRectF rect;
    input >> rect;

    QString path;
    input >> path;

    qreal ratio;
    input >> ratio;

    maps.insert({"pixmap", pixmap});
    maps.insert({"data", data});
    maps.insert({"rect", rect});
    maps.insert({"path", path});
    maps.insert({"ratio", ratio});

    return maps;
}

void saveVMapLineItemController(const vmap::LineController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->startPoint();
    output << ctrl->endPoint();
    output << ctrl->penWidth();
}

std::map<QString, QVariant> readLineController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::LINE, maps, input);

    QPointF startPoint;
    input >> startPoint;

    QPointF endPoint;
    input >> endPoint;

    quint16 penWidth;
    input >> penWidth;

    maps.insert({"startPoint", startPoint});
    maps.insert({"endPoint", endPoint});
    maps.insert({"penWidth", penWidth});

    return maps;
}

void saveVMapPathItemController(const vmap::PathController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->pointCount();
    auto ps= ctrl->points();
    std::for_each(ps.begin(), ps.end(), [&output](const QPointF& p) { output << p; });
    output << ctrl->penWidth();
    output << ctrl->closed();
    output << ctrl->filled();
    output << ctrl->penLine();
}

std::map<QString, QVariant> readPathController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::PATH, maps, input);

    int pointCount;
    input >> pointCount;

    std::vector<QPointF> vec;
    vec.reserve(pointCount);
    for(int i= 0; i < pointCount; i++)
    {
        QPointF p;
        input >> p;
        vec.push_back(p);
    }

    quint16 penWidth;
    input >> penWidth;

    bool closed;
    input >> closed;

    bool filled;
    input >> filled;

    bool penLine;
    input >> penLine;

    maps.insert({"pointCount", pointCount});
    maps.insert({"points", QVariant::fromValue(vec)});
    maps.insert({"penWidth", penWidth});
    maps.insert({"closed", closed});
    maps.insert({"filled", filled});
    maps.insert({"penLine", penLine});
    return maps;
}

/*void saveAllPaths(const PathControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::PathController* ctrl) { saveVMapPathItemController(ctrl, output); });
}*/

void saveVMapTextItemController(const vmap::TextController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->text();
    output << ctrl->textRect();
    output << ctrl->borderRect();
    output << ctrl->border();
    output << ctrl->font();
    output << ctrl->penWidth();
    output << ctrl->textPos();
}

void readVmapSightController(vmap::SightController* sight, QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::SIGHT, maps, input);

    bool characterSight;
    input >> characterSight;
    sight->setCharacterSight(characterSight);

    QPainterPath fowPath;
    input >> fowPath;
    // sight->setFowPath(fowPath);

    QRectF rect;
    input >> rect;
    sight->setRect(rect);

    int characterCount;
    input >> characterCount;

    int size;
    input >> size;

    for(int i= 0; i < size; ++i)
    {
        QPolygonF poly;
        input >> poly;

        bool adding;
        input >> adding;

        sight->addPolygon(poly, adding);
    }
}

std::map<QString, QVariant> readTextController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::TEXT, maps, input);

    QString text;
    input >> text;

    QRectF textRect;
    input >> textRect;

    QRectF borderRect;
    input >> borderRect;

    bool border;
    input >> border;

    QFont font;
    input >> font;

    quint16 penWidth;
    input >> penWidth;

    QPointF textPos;
    input >> textPos;

    maps.insert({"text", text});
    maps.insert({"textRect", textRect});
    maps.insert({"borderRect", borderRect});
    maps.insert({"border", border});
    maps.insert({"font", font});
    maps.insert({"textPos", textPos});
    maps.insert({"penWidth", penWidth});

    return maps;
}

void saveVMapCharacterItemController(const vmap::CharacterItemController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveVisualItemController(ctrl, output);

    output << ctrl->side();
    output << ctrl->stateColor();
    output << ctrl->number();
    output << ctrl->playableCharacter();
    output << ctrl->thumnailRect();
    output << ctrl->visionShape();
    output << ctrl->textRect();
    output << ctrl->text();
    output << *ctrl->avatar();
    output << ctrl->font();
    output << ctrl->radius();
    auto vision= ctrl->vision();
    output << vision->position();
    output << vision->angle();
    output << vision->shape();
    output << vision->visible();
    output << vision->radius();
}

std::map<QString, QVariant> readCharacterController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::CHARACTER, maps, input);

    qreal side;
    input >> side;

    QColor stateColor;
    input >> stateColor;

    int number;
    input >> number;

    bool playableCharacter;
    input >> playableCharacter;

    QRectF thumnailRect;
    input >> thumnailRect;

    CharacterVision::SHAPE visionShape;
    input >> visionShape;

    QRectF textRect;
    input >> textRect;

    QString text;
    input >> text;

    bool hasAvatar;
    input >> hasAvatar;

    QImage image;
    input >> image;

    QFont font;
    input >> font;

    qreal radius;
    input >> radius;

    // vision

    QPointF position;
    input >> position;

    qreal angle;
    input >> angle;

    CharacterVision::SHAPE visionShapeV;
    input >> visionShapeV;
    bool visible;
    input >> visible;
    qreal radiusV;
    input >> radiusV;

    return maps;
}

/*void saveAllCharacters(const CharacterItemControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::CharacterItemController* ctrl) { saveVMapCharacterItemController(ctrl, output); });
}*/

void saveVectorialMap(VectorialMapController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;
    saveBase(ctrl, output);

    // properties
    output << ctrl->npcNameVisible();
    output << ctrl->pcNameVisible();
    output << ctrl->npcNumberVisible();
    output << ctrl->healthBarVisible();
    output << ctrl->initScoreVisible();
    output << ctrl->stateLabelVisible();
    output << ctrl->collision();
    output << ctrl->characterVision();
    output << ctrl->gridColor();
    output << ctrl->gridScale();
    output << ctrl->gridSize();
    output << ctrl->gridVisibility();
    output << ctrl->gridAbove();
    output << ctrl->scaleUnit();
    output << ctrl->npcName();
    output << ctrl->permission();
    output << ctrl->gridPattern();
    output << ctrl->visibility();
    output << ctrl->backgroundColor();
    output << ctrl->toolColor();
    output << ctrl->penSize();
    output << ctrl->npcNumber();
    output << ctrl->zoomLevel();
    output << ctrl->layer();
    output << ctrl->opacity();
    output << ctrl->visualRect();
    output << ctrl->idle();
    output << ctrl->zIndex();

    auto sight= ctrl->sightController();
    saveVmapSightController(sight, output);

    auto model= ctrl->model();

    auto vec= model->items();

    output << static_cast<quint64>(vec.size());

    using vv= vmap::VisualItemController;
    for(auto itemCtrl : vec)
    {
        switch(itemCtrl->itemType())
        {
        case vv::RECT:
            saveVMapRectItemController(dynamic_cast<vmap::RectController*>(itemCtrl), output);
            break;
        case vv::PATH:
            saveVMapPathItemController(dynamic_cast<vmap::PathController*>(itemCtrl), output);
            break;
        case vv::LINE:
            saveVMapLineItemController(dynamic_cast<vmap::LineController*>(itemCtrl), output);
            break;
        case vv::ELLIPSE:
            saveVMapEllipseItemController(dynamic_cast<vmap::EllipseController*>(itemCtrl), output);
            break;
        case vv::CHARACTER:
            saveVMapCharacterItemController(dynamic_cast<vmap::CharacterItemController*>(itemCtrl), output);
            break;
        case vv::TEXT:
            saveVMapTextItemController(dynamic_cast<vmap::TextController*>(itemCtrl), output);
            break;
        case vv::IMAGE:
            saveVMapImageItemController(dynamic_cast<vmap::ImageController*>(itemCtrl), output);
            break;
        case vv::RULE:
        case vv::SIGHT:
        case vv::ANCHOR:
        case vv::GRID:
        case vv::HIGHLIGHTER:
            break;
        }
    }
}

void saveNotes(NoteController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;
    saveBase(ctrl, output);

    output << ctrl->text();
}

void saveCharacterSheet(CharacterSheetController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;
    saveBase(ctrl, output);

    QJsonDocument doc;
    QJsonObject obj= ctrl->rootObject();
    auto model= ctrl->model();
    QJsonObject data;
    model->writeModel(data);

    obj["character"]= data;

    auto images= ctrl->imageModel();
    QJsonArray array;
    images->save(array);

    obj["images"]= array;

    doc.setObject(obj);

    output << doc.toBinaryData();
}

void saveSharedNote(SharedNoteController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveBase(ctrl, output);

    output << ctrl->permission();
    output << ctrl->text();
    output << ctrl->highligthedSyntax();
    output << ctrl->markdownVisible();
}

void saveWebView(WebpageController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveBase(ctrl, output);

    output << ctrl->hideUrl();
    output << ctrl->keepSharing();
    output << ctrl->htmlSharing();
    output << ctrl->urlSharing();
    output << ctrl->html();
    output << ctrl->state();
    output << ctrl->sharingMode();
}
#ifdef WITH_PDF
void savePdfView(PdfController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    saveBase(ctrl, output);

    output << ctrl->data();
}
#endif

QByteArray IOHelper::saveController(MediaControllerBase* media)
{
    QByteArray data;

    if(!media)
        return data;

    QDataStream output(&data, QIODevice::WriteOnly);

    auto uri= media->contentType();
    output << uri;
    switch(uri)
    {
    case Core::ContentType::VECTORIALMAP:
        saveVectorialMap(dynamic_cast<VectorialMapController*>(media), output);
        break;
    case Core::ContentType::PICTURE:
        saveImage(dynamic_cast<ImageController*>(media), output);
        break;
    case Core::ContentType::ONLINEPICTURE:
        saveImage(dynamic_cast<ImageController*>(media), output);
        break;
    case Core::ContentType::NOTES:
        saveNotes(dynamic_cast<NoteController*>(media), output);
        break;
    case Core::ContentType::CHARACTERSHEET:
        saveCharacterSheet(dynamic_cast<CharacterSheetController*>(media), output);
        break;
    case Core::ContentType::SHAREDNOTE:
        saveSharedNote(dynamic_cast<SharedNoteController*>(media), output);
        break;
    case Core::ContentType::WEBVIEW:
        saveWebView(dynamic_cast<WebpageController*>(media), output);
        break;
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        savePdfView(dynamic_cast<PdfController*>(media), output);
        break;
#endif
    default:
        break;
    }

    return data;
}

MediaControllerBase* IOHelper::loadController(QDataStream& input)
{
    Core::ContentType type;
    input >> type;

    QByteArray data;
    input >> data;

    MediaControllerBase* value= nullptr;
    switch(type)
    {
    case Core::ContentType::VECTORIALMAP:
    {
        auto ctrl= new VectorialMapController("");
        value= ctrl;
        readVectorialMapController(ctrl, data);
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
        break;
    }

    return value;
}

void readBase(MediaControllerBase* base, QDataStream& input)
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

    readBase(ctrl, input);

    QByteArray json;
    input >> json;

    QJsonDocument doc= QJsonDocument::fromBinaryData(json);
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

    readBase(ctrl, input);

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

    readBase(ctrl, input);

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

    readBase(ctrl, input);

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

void IOHelper::readNoteController(NoteController* ctrl, const QByteArray& array)
{
    if(!ctrl || array.isEmpty())
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    readBase(ctrl, input);

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

    readBase(ctrl, input);

    ParticipantModel::Permission perm;
    input >> perm;

    QString text;
    input >> text;

    SharedNoteController::HighlightedSyntax syntax;
    input >> syntax;

    bool b;
    input >> b;

    ctrl->setMarkdownVisible(b);
    ctrl->setText(text);
    ctrl->setHighligthedSyntax(syntax);
    ctrl->setPermission(perm);
}

// QHash<QString, std::map<QString, QVariant>>
void readModel(VectorialMapController* ctrl, QDataStream& input)
{
    quint64 size;
    input >> size;

    for(quint64 i= 0; i < size; ++i)
    {
        vmap::VisualItemController::ItemType type;
        input >> type;

        std::map<QString, QVariant> params;
        switch(type)
        {
        case vmap::VisualItemController::ELLIPSE:
            params= readEllipseController(input);
            break;
        case vmap::VisualItemController::PATH:
            params= readPathController(input);
            break;
        case vmap::VisualItemController::RECT:
            params= readRectController(input);
            break;
        case vmap::VisualItemController::TEXT:
            params= readTextController(input);
            break;
        case vmap::VisualItemController::CHARACTER:
            params= readCharacterController(input);
            break;
        case vmap::VisualItemController::LINE:
            params= readLineController(input);
            break;
        case vmap::VisualItemController::IMAGE:
            params= readImageController(input);
            break;
        default:
            break;
        }

        ctrl->addItemController(params);
    }
}

void IOHelper::readVectorialMapController(VectorialMapController* ctrl, const QByteArray& array)
{
    if(!ctrl)
        return;
    auto data= array;
    QDataStream input(&data, QIODevice::ReadOnly);

    readBase(ctrl, input);

    // properties
    bool npcNameVisible;
    input >> npcNameVisible;
    ctrl->setNpcNameVisible(npcNameVisible);

    bool pcNameVisible;
    input >> pcNameVisible;
    ctrl->setPcNameVisible(pcNameVisible);

    bool npcNumberVisible;
    input >> npcNumberVisible;
    ctrl->setNpcNumberVisible(npcNumberVisible);

    bool healthBarVisible;
    input >> healthBarVisible;
    ctrl->setHealthBarVisible(healthBarVisible);

    bool initScoreVisible;
    input >> initScoreVisible;
    ctrl->setInitScoreVisible(initScoreVisible);

    bool stateLabelVisible;
    input >> stateLabelVisible;
    ctrl->setStateLabelVisible(stateLabelVisible);

    bool collision;
    input >> collision;
    ctrl->setCollision(collision);

    bool characterVision;
    input >> characterVision;
    ctrl->setCharacterVision(characterVision);

    QColor gridColor;
    input >> gridColor;
    ctrl->setGridColor(gridColor);

    qreal gridScale;
    input >> gridScale;
    ctrl->setGridScale(gridScale);

    int gridSize;
    input >> gridSize;
    ctrl->setGridSize(gridSize);

    bool gridVisibility;
    input >> gridVisibility;
    ctrl->setGridVisibility(gridVisibility);

    bool gridAbove;
    input >> gridAbove;
    ctrl->setGridAbove(gridAbove);

    Core::ScaleUnit scaleUnit;
    input >> scaleUnit;
    ctrl->setScaleUnit(scaleUnit);

    QString npcName;
    input >> npcName;
    ctrl->setNpcName(npcName);

    Core::PermissionMode permission;
    input >> permission;
    ctrl->setPermission(permission);

    Core::GridPattern gridPattern;
    input >> gridPattern;
    ctrl->setGridPattern(gridPattern);

    Core::VisibilityMode visibility;
    input >> visibility;
    ctrl->setVisibility(visibility);

    QColor backgroundColor;
    input >> backgroundColor;
    ctrl->setBackgroundColor(backgroundColor);

    QColor toolColor;
    input >> toolColor;
    ctrl->setToolColor(toolColor);

    quint16 penSize;
    input >> penSize;
    ctrl->setPenSize(penSize);

    int npcNumber;
    input >> npcNumber;
    ctrl->setNpcNumber(npcNumber);

    qreal zoomLevel;
    input >> zoomLevel;
    ctrl->setZoomLevel(zoomLevel);

    Core::Layer layer;
    input >> layer;
    ctrl->setLayer(layer);

    qreal opacity;
    input >> opacity;
    ctrl->setOpacity(opacity);

    QRectF visualRect;
    input >> visualRect;
    ctrl->setVisualRect(visualRect);

    bool idle;
    input >> idle;
    ctrl->setIdle(idle);

    int zIndex;
    input >> zIndex;
    ctrl->setZindex(zIndex);

    // fog singularies
    auto sight= ctrl->sightController();
    readVmapSightController(sight, input);

    readModel(ctrl, input);
}
