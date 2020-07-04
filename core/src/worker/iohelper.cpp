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

#include "controller/media_controller/charactersheetmediacontroller.h"
#include "controller/media_controller/imagemediacontroller.h"
#include "controller/media_controller/mediamanagerbase.h"
#include "controller/media_controller/notemediacontroller.h"
#include "controller/media_controller/sharednotemediacontroller.h"
#include "controller/media_controller/vectorialmapmediacontroller.h"
#include "controller/media_controller/webpagemediacontroller.h"

#include "data/character.h"
#include "data/cleveruri.h"
#include "vmap/vmap.h"

#include "vmap/controller/sightcontroller.h"
#include "vmap/controller/visualitemcontroller.h"
#include "vmap/manager/characteritemcontrollermanager.h"
#include "vmap/manager/ellipscontrollermanager.h"
#include "vmap/manager/imagecontrollermanager.h"
#include "vmap/manager/linecontrollermanager.h"
#include "vmap/manager/pathcontrollermanager.h"
#include "vmap/manager/rectcontrollermanager.h"
#include "vmap/manager/textcontrollermanager.h"

#include "vmap/controller/characteritemcontroller.h"
#include "vmap/controller/ellipsecontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/linecontroller.h"
#include "vmap/controller/pathcontroller.h"
#include "vmap/controller/rectcontroller.h"
#include "vmap/controller/textcontroller.h"

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

bool IOHelper::loadVMap(VMap* vmap, CleverURI* uri, VectorialMapController* ctrl)
{
    /*  if(vmap == nullptr || nullptr == uri || nullptr == ctrl)
          return false;

      auto path= uri->getUri();
      QIODevice* device= nullptr;
      if(!path.isEmpty() && uri->loadingMode() == CleverURI::Linked)
          device= new QFile(path);
      else if(uri->hasData() && uri->loadingMode() == CleverURI::Internal)
      {
          auto data= uri->getData();
          device= new QBuffer(&data);
      }

      if(!device->open(QIODevice::ReadOnly))
          return false;
      QDataStream reader(device);
      reader.setVersion(QDataStream::Qt_5_7);

      int zIndex, propertyCount, itemCount;
      QString name;
      QColor bgColor;

      reader >> name;
      reader >> bgColor;
      reader >> zIndex;
      reader >> propertyCount;

      ctrl->setName(name);
      ctrl->setBackgroundColor(bgColor);

      bool b;
      reader >> b;
      ctrl->setNpcNameVisible(b);
      reader >> b;
      ctrl->setPcNameVisible(b);
      reader >> b;
      ctrl->setNpcNumberVisible(b);
      reader >> b;
      ctrl->setHealthBarVisible(b);
      reader >> b;
      ctrl->setInitScoreVisible(b);
      reader >> b;
      ctrl->setGridVisibility(b);
      int interger;
      reader >> interger;
      ctrl->setGridPattern(static_cast<Core::GridPattern>(interger));
      QColor color;
      reader >> color;
      ctrl->setGridColor(color);
      reader >> interger;
      ctrl->setGridSize(interger);
      qreal real;
      reader >> real;
      ctrl->setGridScale(real);
      reader >> interger;
      ctrl->setScaleUnit(static_cast<Core::ScaleUnit>(interger));
      reader >> b;
      ctrl->setCharacterVision(b);
      reader >> interger;
      ctrl->setPermission(static_cast<Core::PermissionMode>(interger));
      reader >> b;
      ctrl->setCollision(b);
      reader >> b;
      ctrl->setGridAbove(b);
      reader >> interger;
      ctrl->setVisibility(static_cast<Core::VisibilityMode>(interger));
      reader >> b;
      ctrl->setHealthBarVisible(b);
      reader >> interger;
      ctrl->setLayer(static_cast<Core::Layer>(interger));

    auto fogItem= vmap->getFogItem();
    reader >> *fogItem;

     reader >> itemCount;
     for(int i= 0; i < itemCount; ++i)
     {
         VisualItem* item= nullptr;
         int contentType;
         reader >> contentType;

         switch(static_cast<VisualItem::ItemType>(contentType))
         {
         case VisualItem::TEXT:
             item= new TextItem(ctrl);
             break;
         case VisualItem::CHARACTER:
             item= new CharacterItem(ctrl);
             break;
         case VisualItem::LINE:
             item= new LineItem(ctrl);

             break;
         case VisualItem::RECT:
             item= new RectItem(ctrl);
             break;
         case VisualItem::ELLISPE:
             item= new EllipsItem(ctrl);
             break;
         case VisualItem::PATH:
             item= new PathItem(ctrl);
             break;
         case VisualItem::SIGHT:
             item= vmap->getFogItem();
             break;
         case VisualItem::GRID:
             item= vmap->getGridItem();
             break;
         case VisualItem::IMAGE:
             item= new ImageItem(ctrl);
             break;
         default:
             break;
         }
         if(nullptr == item)
             continue;

         reader >> *item;

         qreal x, y;
         reader >> x;
         reader >> y;
         // addNewItem(new AddVmapItemCommand(item, false, this), false);
         vmap->addItem(item);

         item->setPos(x, y);
         item->initChildPointItem();
     }*/
    return true;
}

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

void readVisualItemController(std::map<QString, QVariant>& map, QDataStream& input)
{
    vmap::VisualItemController::ItemType type;
    input >> type;

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

void saveAllRects(const RectControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::RectController* ctrl) { saveVMapRectItemController(ctrl, output); });
}

std::map<QString, QVariant> readRectController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(maps, input);

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

void saveAllEllipses(const EllipsControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::EllipseController* ctrl) { saveVMapEllipseItemController(ctrl, output); });
}

std::map<QString, QVariant> readEllipseController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(maps, input);

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

void saveAllImages(const ImageControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::ImageController* ctrl) { saveVMapImageItemController(ctrl, output); });
}

std::map<QString, QVariant> readImageController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(maps, input);

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

void saveAllLines(const LineControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::LineController* ctrl) { saveVMapLineItemController(ctrl, output); });
}

std::map<QString, QVariant> readLineController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(maps, input);

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
    readVisualItemController(maps, input);

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

void saveAllPaths(const PathControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::PathController* ctrl) { saveVMapPathItemController(ctrl, output); });
}

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
    readVisualItemController(maps, input);

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
    readVisualItemController(maps, input);

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

void saveAllTexts(const TextControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::TextController* ctrl) { saveVMapTextItemController(ctrl, output); });
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
    readVisualItemController(maps, input);

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

void saveAllCharacters(const CharacterItemControllerManager* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto ctrls= ctrl->controllers();
    output << static_cast<int>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](vmap::CharacterItemController* ctrl) { saveVMapCharacterItemController(ctrl, output); });
}

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
    auto list= ctrl->orderList();
    output << static_cast<int>(list.size());
    std::for_each(list.begin(), list.end(), [&output](const QString& id) { output << id; });

    // fog singularies
    auto sight= ctrl->sightController();
    saveVmapSightController(sight, output);

    // Rects
    auto rect= ctrl->rectManager();
    saveAllRects(rect, output);

    // Ellipse
    auto ellipse= ctrl->ellipseManager();
    saveAllEllipses(ellipse, output);

    // line
    auto line= ctrl->lineManager();
    saveAllLines(line, output);

    // image
    auto images= ctrl->imageManager();
    saveAllImages(images, output);

    // path
    auto path= ctrl->pathManager();
    saveAllPaths(path, output);

    // text
    auto text= ctrl->textManager();
    saveAllTexts(text, output);

    // Character
    auto character= ctrl->characterManager();
    saveAllCharacters(character, output);
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

void saveAllVectorialMaps(VectorialMapMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();

    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](VectorialMapController* ctrl) { output << IOHelper::saveController(ctrl); });
}

void saveAllImageControllers(ImageMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();

    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](ImageController* ctrl) { output << IOHelper::saveController(ctrl); });
}

void saveAllNoteControllers(NoteMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();

    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](NoteController* ctrl) { output << IOHelper::saveController(ctrl); });
}
void saveAllCharacterSheetControllers(CharacterSheetMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();
    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](CharacterSheetController* ctrl) { output << IOHelper::saveController(ctrl); });
}

void saveAllSharedNoteControllers(SharedNoteMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();
    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](SharedNoteController* ctrl) { output << IOHelper::saveController(ctrl); });
}

void saveAllWebpageControllers(WebpageMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();
    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](WebpageController* ctrl) { output << IOHelper::saveController(ctrl); });
}
#ifdef WITH_PDF
void saveAllPdfControllers(PdfMediaController* manager, QDataStream& output)
{
    if(!manager)
        return;

    auto ctrls= manager->controllers();
    output << static_cast<quint64>(ctrls.size());

    std::for_each(ctrls.begin(), ctrls.end(),
                  [&output](PdfController* ctrl) { output << IOHelper::saveController(ctrl); });
}
#endif
QByteArray IOHelper::saveManager(MediaManagerBase* manager)
{
    QByteArray data;

    if(!manager)
        return data;

    QDataStream output(&data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_5_7);

    auto uri= manager->type();
    switch(uri)
    {
    case Core::ContentType::VECTORIALMAP:
        saveAllVectorialMaps(dynamic_cast<VectorialMapMediaController*>(manager), output);
        break;
    case Core::ContentType::PICTURE:
        saveAllImageControllers(dynamic_cast<ImageMediaController*>(manager), output);
        break;
    case Core::ContentType::NOTES:
        saveAllNoteControllers(dynamic_cast<NoteMediaController*>(manager), output);
        break;
    case Core::ContentType::CHARACTERSHEET:
        saveAllCharacterSheetControllers(dynamic_cast<CharacterSheetMediaController*>(manager), output);
        break;
    case Core::ContentType::SHAREDNOTE:
        saveAllSharedNoteControllers(dynamic_cast<SharedNoteMediaController*>(manager), output);
        break;
    case Core::ContentType::WEBVIEW:
        saveAllWebpageControllers(dynamic_cast<WebpageMediaController*>(manager), output);
        break;
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        saveAllPdfControllers(dynamic_cast<PdfMediaController*>(manager), output);
        break;
#endif
    default:
        Q_ASSERT(false);
        break;
    }

    return data;
}

bool IOHelper::loadManager(MediaManagerBase* manager, QDataStream& input)
{
    // auto type= manager->type();

    QByteArray array;
    input >> array;

    QDataStream input2(&array, QIODevice::ReadOnly);
    input2.setVersion(QDataStream::Qt_5_7);

    quint64 size;
    input2 >> size;

    for(quint64 i= 0; i < size; ++i)
    {
        QByteArray array;
        input2 >> array;
        std::map<QString, QVariant> map({{"serializedData", array}});
        manager->openMedia(QString(), map);
    }
    return true;
}

QByteArray IOHelper::saveController(MediaControllerBase* media)
{
    QByteArray data;

    if(!media)
        return data;

    QDataStream output(&data, QIODevice::WriteOnly);

    auto uri= media->contentType();
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
    }

    return data;
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

    SharedNoteController::Permission perm;
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
// QHash<QString, std::map<QString, QVariant>>
void readAllController(VectorialMapController* ctrl, QDataStream& input, vmap::VisualItemController::ItemType type)
{
    QHash<QString, std::map<QString, QVariant>> rects;
    int size;
    input >> size;
    for(int i= 0; i < size; ++i)
    {
        std::map<QString, QVariant> map;
        VisualItemControllerManager* manager= nullptr;
        switch(type)
        {
        case vmap::VisualItemController::RECT:
            map= readRectController(input);
            manager= ctrl->rectManager();
            break;
        case vmap::VisualItemController::LINE:
            map= readLineController(input);
            manager= ctrl->lineManager();
            break;
        case vmap::VisualItemController::ELLIPSE:
            map= readEllipseController(input);
            manager= ctrl->ellipseManager();
            break;
        case vmap::VisualItemController::IMAGE:
            map= readImageController(input);
            manager= ctrl->imageManager();
            break;
        case vmap::VisualItemController::TEXT:
            map= readTextController(input);
            manager= ctrl->textManager();
            break;
        case vmap::VisualItemController::CHARACTER:
            map= readCharacterController(input);
            manager= ctrl->characterManager();
            break;
        case vmap::VisualItemController::PATH:
            map= readPathController(input);
            manager= ctrl->pathManager();
            break;
        default:
            break;
        }
        if(map.empty() || !manager)
            continue;

        manager->addItem(map);
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

    int size;
    input >> size;

    std::vector<QString> orderList;
    orderList.reserve(size);
    for(int i= 0; i < size; ++i)
    {
        QString id;
        input >> id;
        orderList.push_back(id);
    }

    // fog singularies
    auto sight= ctrl->sightController();
    readVmapSightController(sight, input);

    using v= vmap::VisualItemController;

    // Rects
    readAllController(ctrl, input, v::RECT);
    // auto rectManager = ctrl->rectManager();
    // rectManager->addItem();

    // Ellipse
    readAllController(ctrl, input, v::ELLIPSE);

    // line
    readAllController(ctrl, input, v::LINE);

    // image
    readAllController(ctrl, input, v::IMAGE);

    // path
    readAllController(ctrl, input, v::PATH);

    // text
    readAllController(ctrl, input, v::TEXT);

    // Character
    readAllController(ctrl, input, v::CHARACTER);
}
