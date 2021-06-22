/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "vectorialmapmessagehelper.h"

#include <QByteArray>
#include <QDataStream>

#include "network/networkmessagewriter.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/item_controllers/imagecontroller.h"
#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/sightcontroller.h"
#include "controller/item_controllers/textcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "model/vmapitemmodel.h"

VectorialMapMessageHelper::VectorialMapMessageHelper() {}

void VectorialMapMessageHelper::sendOffNewItem(const std::map<QString, QVariant>& args, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    for(auto pair : args)
    {
        stream << pair.first << pair.second;
    }
    Q_ASSERT(!array.isEmpty());
    msg.byteArray32(array);
    msg.sendToServer();
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
    vmap::VisualItemController::ItemType typee;
    input >> typee;

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
    vec.reserve(static_cast<std::size_t>(pointCount));
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
#include "worker/iohelper.h"
QByteArray VectorialMapMessageHelper::saveVectorialMap(VectorialMapController* ctrl)
{
    QByteArray data;
    if(!ctrl)
        return data;

    QDataStream output(&data, QIODevice::WriteOnly);

    IOHelper::saveBase(ctrl, output);

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
        output << itemCtrl->itemType();
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

    return data;
}

void VectorialMapMessageHelper::readVectorialMapController(VectorialMapController* ctrl, const QByteArray& array)
{
    if(!ctrl)
        return;
    auto data= array;

    QDataStream input(&data, QIODevice::ReadOnly);

    IOHelper::readBase(ctrl, input);

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
