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
#include "worker/vectorialmapmessagehelper.h"

#include <QByteArray>
#include <QDataStream>
#include <QPointF>

#include "network/networkmessagereader.h"
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
#include "data/character.h"
#include "data/charactervision.h"
#include "model/vmapitemmodel.h"
#include "worker/characterfinder.h"
#include "worker/iohelper.h"
#include "worker/utilshelper.h"

VectorialMapMessageHelper::VectorialMapMessageHelper() {}

void VectorialMapMessageHelper::sendOffNewItem(const std::map<QString, QVariant>& args, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_12);
    for(const auto& pair : args)
    {
        stream << pair.first << pair.second;
    }
    Q_ASSERT(!array.isEmpty());
    msg.byteArray32(array);
    msg.sendToServer();
}

void VectorialMapMessageHelper::sendOffHighLight(const QPointF& p, const qreal& penSize, const QColor& color,
                                                 const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::HighLightPosition);
    msg.string8(mapId);
    msg.real(p.x());
    msg.real(p.y());
    msg.real(penSize);
    msg.string8(color.name());

    msg.sendToServer();
}

void VectorialMapMessageHelper::sendOffRemoveItems(const QStringList ids, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::DeleteItem);
    msg.string8(mapId);
    msg.uint64(ids.size());
    for(auto const& id : ids)
    {
        msg.string8(id);
    }
    msg.sendToServer();
}

QStringList VectorialMapMessageHelper::readRemoveItems(NetworkMessageReader* msg)
{
    auto size= msg->uint64();
    QStringList res;
    for(unsigned int i= 0; i < size; ++i)
    {
        res << msg->string8();
    }
    return res;
}

void VectorialMapMessageHelper::readHighLight(VectorialMapController* ctrl, NetworkMessageReader* msg)
{
    // msg->resetToData();
    auto x= msg->real();
    auto y= msg->real();

    auto size= msg->real();
    QColor c(msg->string8());

    ctrl->showHightLighter({x, y}, size, c);
}

void saveVisualItemController(const vmap::VisualItemController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    output << ctrl->itemType();
    // output << ctrl->rotationOriginPoint();
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
    output << ctrl->initialized();
}

void readVisualItemController(vmap::VisualItemController::ItemType type, std::map<QString, QVariant>& map,
                              QDataStream& input)
{
    vmap::VisualItemController::ItemType typee;
    // QPointF originPoint;
    bool selected;
    bool visible;
    qreal opacity;
    qreal rotation;
    Core::Layer layer;
    QPointF pos;
    QString uuid;
    QColor color;
    bool locked;
    Core::SelectableTool tool;

    input >> typee;
    // input >> originPoint;
    input >> selected;
    input >> visible;
    input >> opacity;
    input >> rotation;
    input >> layer;
    input >> pos;
    input >> uuid;
    input >> color;
    input >> locked;
    input >> tool;

    bool initialized= true;
    input >> initialized;

    map.insert({Core::vmapkeys::KEY_ITEMTYPE, type});
    // map.insert({Core::vmapkeys::KEY_ORIGINPOINT, originPoint});
    map.insert({Core::vmapkeys::KEY_SELECTED, selected});
    map.insert({Core::vmapkeys::KEY_VISIBLE, visible});
    map.insert({Core::vmapkeys::KEY_OPACITY, opacity});
    map.insert({Core::vmapkeys::KEY_ROTATION, rotation});
    map.insert({Core::vmapkeys::KEY_LAYER, QVariant::fromValue(layer)});
    map.insert({Core::vmapkeys::KEY_POS, pos});
    map.insert({Core::vmapkeys::KEY_UUID, uuid});
    map.insert({Core::vmapkeys::KEY_COLOR, color});
    map.insert({Core::vmapkeys::KEY_LOCKED, locked});
    map.insert({Core::vmapkeys::KEY_TOOL, tool});
    map.insert({Core::vmapkeys::KEY_INITIALIZED, true});
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

    maps.insert({Core::vmapkeys::KEY_RECT, rect});
    maps.insert({Core::vmapkeys::KEY_FILLED, filled});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});

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

    maps.insert({Core::vmapkeys::KEY_RX, rx});
    maps.insert({Core::vmapkeys::KEY_RY, ry});
    maps.insert({Core::vmapkeys::KEY_FILLED, filled});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});

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

    maps.insert({Core::vmapkeys::KEY_PIXMAP, pixmap});
    maps.insert({Core::vmapkeys::KEY_DATA, data});
    maps.insert({Core::vmapkeys::KEY_RECT, rect});
    maps.insert({Core::vmapkeys::KEY_PATH, path});
    maps.insert({Core::vmapkeys::KEY_RATIO, ratio});

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

    maps.insert({Core::vmapkeys::KEY_STARTPOINT, startPoint});
    maps.insert({Core::vmapkeys::KEY_ENDPOINT, endPoint});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});

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

    quint64 pointCount;
    input >> pointCount;

    std::vector<QPointF> vec;
    vec.reserve(static_cast<std::size_t>(pointCount));
    for(quint64 i= 0; i < pointCount; i++)
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

    maps.insert({Core::vmapkeys::KEY_POINTCOUNT, pointCount});
    maps.insert({Core::vmapkeys::KEY_POINTS, QVariant::fromValue(vec)});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});
    maps.insert({Core::vmapkeys::KEY_CLOSED, closed});
    maps.insert({Core::vmapkeys::KEY_FILLED, filled});
    maps.insert({Core::vmapkeys::KEY_PENLINE, penLine});
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

    maps.insert({Core::vmapkeys::KEY_TEXT, text});
    maps.insert({Core::vmapkeys::KEY_TEXTRECT, textRect});
    maps.insert({Core::vmapkeys::KEY_BORDERRECT, borderRect});
    maps.insert({Core::vmapkeys::KEY_BORDER, border});
    maps.insert({Core::vmapkeys::KEY_FONT, font});
    maps.insert({Core::vmapkeys::KEY_TEXTPOS, textPos});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});

    return maps;
}

void saveCharacter(Character* c, QDataStream& output)
{
    if(!c)
        return;

    output << c->isNpc();
    output << c->uuid();
    output << c->name();
    output << c->avatar();
    output << c->getHealthPointsCurrent();
    output << c->getHealthPointsMax();
    output << c->getHealthPointsMin();

    output << c->getInitiativeScore();
    output << c->getDistancePerTurn();
    output << c->stateId();
    output << c->getLifeColor();
    output << c->initCommand();
    output << c->hasInitScore();
}

void readCharacter(std::map<QString, QVariant>& map, QDataStream& output)
{

    bool isNpc;
    QString uuid;
    QString name;
    QByteArray avatar;
    int hpCurrent;
    int hpMax;
    int hpMin;
    int initScore;
    bool hasInitScore;
    qreal distancePerTurn;
    QString stateId;
    QString colorStr;
    QString initCmd;

    output >> isNpc;
    output >> uuid;
    output >> name;
    output >> avatar;
    output >> hpCurrent;
    output >> hpMax;
    output >> hpMin;

    output >> initScore;
    output >> distancePerTurn;
    output >> stateId;
    output >> colorStr;
    output >> initCmd;
    output >> hasInitScore;

    map.insert({Core::vmapkeys::KEY_CHARAC_NPC, isNpc});
    map.insert({Core::vmapkeys::KEY_CHARAC_ID, uuid});
    map.insert({Core::vmapkeys::KEY_CHARAC_NAME, name});
    map.insert({Core::vmapkeys::KEY_CHARAC_AVATAR, avatar});
    map.insert({Core::vmapkeys::KEY_CHARAC_HPCURRENT, hpCurrent});
    map.insert({Core::vmapkeys::KEY_CHARAC_HPMAX, hpMax});
    map.insert({Core::vmapkeys::KEY_CHARAC_HPMIN, hpMin});
    map.insert({Core::vmapkeys::KEY_CHARAC_INITSCORE, initScore});
    map.insert({Core::vmapkeys::KEY_CHARAC_DISTANCEPERTURN, distancePerTurn});
    map.insert({Core::vmapkeys::KEY_CHARAC_STATEID, stateId});
    map.insert({Core::vmapkeys::KEY_CHARAC_LIFECOLOR, QColor(colorStr)});
    map.insert({Core::vmapkeys::KEY_CHARAC_INITCMD, initCmd});
    map.insert({Core::vmapkeys::KEY_CHARAC_HASINIT, hasInitScore});
}

void VectorialMapMessageHelper::fetchCharacter(const std::map<QString, QVariant>& params, Character* character)
{
    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;

    // clang-format off
    hu::setParamIfAny<bool>(cv::KEY_CHARAC_NPC, params, std::bind(&Character::setNpc, character, _1));
    hu::setParamIfAny<QString>(cv::KEY_CHARAC_ID, params, std::bind(&Character::setUuid, character, _1));
    hu::setParamIfAny<QString>(cv::KEY_CHARAC_NAME, params, std::bind(&Character::setName, character, _1));
    hu::setParamIfAny<QByteArray>(cv::KEY_CHARAC_AVATAR, params, std::bind(&Character::setAvatar, character, _1));
    hu::setParamIfAny<int>(cv::KEY_CHARAC_HPCURRENT, params, std::bind(&Character::setHealthPointsCurrent, character, _1));
    hu::setParamIfAny<int>(cv::KEY_CHARAC_HPMAX, params, std::bind(&Character::setHealthPointsMax, character, _1));
    hu::setParamIfAny<int>(cv::KEY_CHARAC_HPMIN, params, std::bind(&Character::setHealthPointsMin, character, _1));
    hu::setParamIfAny<int>(cv::KEY_CHARAC_INITSCORE, params, std::bind(&Character::setInitiativeScore, character, _1));
    hu::setParamIfAny<qreal>(cv::KEY_CHARAC_DISTANCEPERTURN, params,std::bind(&Character::setDistancePerTurn, character, _1));
    hu::setParamIfAny<QString>(cv::KEY_CHARAC_STATEID, params, std::bind(&Character::setStateId, character, _1));
    hu::setParamIfAny<QColor>(cv::KEY_CHARAC_LIFECOLOR, params, std::bind(&Character::setLifeColor, character, _1));
    hu::setParamIfAny<QString>(cv::KEY_CHARAC_INITCMD, params, std::bind(&Character::setInitCommand, character, _1));
    hu::setParamIfAny<bool>(cv::KEY_CHARAC_HASINIT, params, std::bind(&Character::setHasInitiative, character, _1));
    // clang-format on
}

void saveVMapCharacterItemController(const vmap::CharacterItemController* ctrl, QDataStream& output)
{
    if(!ctrl)
        return;

    auto c= ctrl->character();

    if(!c)
    {
        qWarning() << "character is null";
        return;
    }

    saveVisualItemController(ctrl, output);

    saveCharacter(c, output);

    output << ctrl->side();
    output << ctrl->stateColor();
    output << ctrl->number();
    output << ctrl->textRect();
    output << ctrl->font();
    output << ctrl->radius();

    auto vision= ctrl->vision();
    auto b= (ctrl->playableCharacter() && vision);

    output << b;

    if(b)
    {
        output << vision->position();
        output << vision->angle();
        output << vision->shape();
        output << vision->visible();
        output << vision->radius();
    }
}

void VectorialMapMessageHelper::fetchCharacterItem(const std::map<QString, QVariant>& params,
                                                   vmap::CharacterItemController* ctrl)
{
    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;

    // clang-format off
    hu::setParamIfAny<qreal>(cv::KEY_SIDE, params, std::bind(&vmap::CharacterItemController::setSide, ctrl, _1));
    hu::setParamIfAny<QColor>(cv::KEY_STATECOLOR, params,std::bind(&vmap::CharacterItemController::setStateColor, ctrl, _1));
    hu::setParamIfAny<int>(cv::KEY_NUMBER, params, std::bind(&vmap::CharacterItemController::setNumber, ctrl, _1));
    hu::setParamIfAny<QRectF>(cv::KEY_TEXTRECT, params, std::bind(&vmap::CharacterItemController::setTextRect, ctrl, _1));
    hu::setParamIfAny<qreal>(cv::KEY_RADIUS, params, std::bind(&vmap::CharacterItemController::setRadius, ctrl, _1));
    hu::setParamIfAny<QFont>(cv::KEY_FONT, params, std::bind(&vmap::CharacterItemController::setFont, ctrl, _1));
    // clang-format on
}

std::map<QString, QVariant> readCharacterItemController(QDataStream& input)
{
    std::map<QString, QVariant> maps;
    readVisualItemController(vmap::VisualItemController::CHARACTER, maps, input);
    readCharacter(maps, input);

    qreal side;
    QColor stateColor;
    int number;
    QRectF textRect;
    QFont font;
    qreal radius;

    input >> side;
    input >> stateColor;
    input >> number;
    input >> textRect;
    input >> font;
    input >> radius;

    maps.insert({Core::vmapkeys::KEY_SIDE, side});
    maps.insert({Core::vmapkeys::KEY_STATECOLOR, stateColor});
    maps.insert({Core::vmapkeys::KEY_NUMBER, number});
    maps.insert({Core::vmapkeys::KEY_TEXTRECT, textRect});
    maps.insert({Core::vmapkeys::KEY_RADIUS, radius});
    maps.insert({Core::vmapkeys::KEY_FONT, font});

    bool hasVision;
    input >> hasVision;

    if(hasVision)
    {
        // vision
        QPointF position;
        qreal angle;
        CharacterVision::SHAPE visionShapeV;
        bool visible;
        qreal radiusV;

        input >> position;
        input >> angle;
        input >> visionShapeV;
        input >> visible;
        input >> radiusV;

        maps.insert({Core::vmapkeys::KEY_VIS_POS, position});
        maps.insert({Core::vmapkeys::KEY_VIS_ANGLE, angle});
        maps.insert({Core::vmapkeys::KEY_VIS_SHAPE, visionShapeV});
        maps.insert({Core::vmapkeys::KEY_VIS_VISIBLE, visible});
        maps.insert({Core::vmapkeys::KEY_VIS_RADIUS, radiusV});
    }

    if(maps.end() != maps.find(Core::vmapkeys::KEY_CHARAC_ID))
    {
        CharacterFinder finder;
        auto c= finder.find(maps.at(Core::vmapkeys::KEY_CHARAC_ID).value<QString>());
        if(c)
        {
            maps.insert({Core::vmapkeys::KEY_CHARACTER, QVariant::fromValue(c)});
        }
    }

    return maps;
}

void VectorialMapMessageHelper::fetchCharacterVision(const std::map<QString, QVariant>& params, CharacterVision* vision)
{
    namespace hu= helper::utils;
    namespace cv= Core::vmapkeys;
    using std::placeholders::_1;

    // clang-format off
    hu::setParamIfAny<QPointF>(cv::KEY_VIS_POS, params, std::bind(&CharacterVision::setPosition, vision, _1));
    hu::setParamIfAny<qreal>(cv::KEY_VIS_ANGLE, params, std::bind(&CharacterVision::setAngle, vision, _1));
    hu::setParamIfAny<CharacterVision::SHAPE>(cv::KEY_VIS_SHAPE, params, std::bind(&CharacterVision::setShape, vision, _1));
    hu::setParamIfAny<bool>(cv::KEY_VIS_VISIBLE, params, std::bind(&CharacterVision::setVisible, vision, _1));
    hu::setParamIfAny<qreal>(cv::KEY_VIS_RADIUS, params, std::bind(&CharacterVision::setRadius, vision, _1));
    // clang-format on
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
            params= readCharacterItemController(input);
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

void VectorialMapMessageHelper::fetchModelFromMap(const QHash<QString, QVariant>& params, VectorialMapController* ctrl)
{
    auto s= params.size();
    for(int i= 0; i < s; ++i)
    {
        auto const& item= params.value(QString("Item_%1").arg(i));
        auto const maps= item.toMap().toStdMap();
        ctrl->addItemController(maps);
    }
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
    bool pcNameVisible;
    bool npcNumberVisible;
    bool healthBarVisible;
    bool initScoreVisible;
    bool stateLabelVisible;
    bool collision;
    bool characterVision;
    QColor gridColor;
    qreal gridScale;
    int gridSize;
    bool gridVisibility;
    bool gridAbove;
    Core::ScaleUnit scaleUnit;
    QString npcName;
    Core::PermissionMode permission;
    Core::GridPattern gridPattern;
    Core::VisibilityMode visibility;
    QColor backgroundColor;
    QColor toolColor;
    quint16 penSize;
    int npcNumber;
    qreal zoomLevel;
    Core::Layer layer;
    qreal opacity;
    QRectF visualRect;
    bool idle;
    int zIndex;

    input >> npcNameVisible;
    input >> pcNameVisible;
    input >> npcNumberVisible;
    input >> healthBarVisible;
    input >> initScoreVisible;
    input >> stateLabelVisible;
    input >> collision;
    input >> characterVision;
    input >> gridColor;
    input >> gridScale;
    input >> gridSize;
    input >> gridVisibility;
    input >> gridAbove;
    input >> scaleUnit;
    input >> npcName;
    input >> permission;
    input >> gridPattern;
    input >> visibility;
    input >> backgroundColor;
    input >> toolColor;
    input >> penSize;
    input >> npcNumber;
    input >> zoomLevel;
    input >> layer;
    input >> opacity;
    input >> visualRect;
    input >> idle;
    input >> zIndex;

    ctrl->setNpcNameVisible(npcNameVisible);
    ctrl->setPcNameVisible(pcNameVisible);
    ctrl->setNpcNumberVisible(npcNumberVisible);
    ctrl->setHealthBarVisible(healthBarVisible);
    ctrl->setInitScoreVisible(initScoreVisible);
    ctrl->setStateLabelVisible(stateLabelVisible);
    ctrl->setCollision(collision);
    ctrl->setCharacterVision(characterVision);
    ctrl->setGridColor(gridColor);
    ctrl->setGridScale(gridScale);
    ctrl->setGridSize(gridSize);
    ctrl->setGridVisibility(gridVisibility);
    ctrl->setGridAbove(gridAbove);
    ctrl->setScaleUnit(scaleUnit);
    ctrl->setNpcName(npcName);
    ctrl->setPermission(permission);
    ctrl->setGridPattern(gridPattern);
    ctrl->setVisibility(visibility);
    ctrl->setBackgroundColor(backgroundColor);
    ctrl->setToolColor(toolColor);
    ctrl->setPenSize(penSize);
    ctrl->setNpcNumber(npcNumber);
    ctrl->setZoomLevel(zoomLevel);
    ctrl->setLayer(layer);
    ctrl->setOpacity(opacity);
    ctrl->setVisualRect(visualRect);
    ctrl->setIdle(idle);
    ctrl->setZindex(zIndex);

    // fog singularies
    auto sight= ctrl->sightController();
    readVmapSightController(sight, input);

    readModel(ctrl, input);
}
