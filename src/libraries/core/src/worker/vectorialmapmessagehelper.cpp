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
#include <QRectF>
#include <QJsonArray>
#include <QJsonObject>

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/item_controllers/imageitemcontroller.h"
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

void saveVisualItemController(const vmap::VisualItemController* ctrl, QJsonObject& obj)
{
    if(!ctrl)
        return;

    obj[Core::jsonctrl::vmap::vitem::JSON_ITEM_TYPE]= ctrl->itemType();
    obj[Core::jsonctrl::vmap::vitem::JSON_SELECTED]= ctrl->selected();
    obj[Core::jsonctrl::vmap::vitem::JSON_VISIBLE]= ctrl->visible();
    obj[Core::jsonctrl::vmap::vitem::JSON_OPACITY]= ctrl->opacity();
    obj[Core::jsonctrl::vmap::vitem::JSON_ROTATION]= ctrl->rotation();
    obj[Core::jsonctrl::vmap::vitem::JSON_LAYER]= static_cast<quint8>(ctrl->layer());
    obj[Core::jsonctrl::vmap::vitem::JSON_POSITION_X]= ctrl->pos().x();
    obj[Core::jsonctrl::vmap::vitem::JSON_POSITION_Y]= ctrl->pos().y();
    obj[Core::jsonctrl::vmap::vitem::JSON_UUID]= ctrl->uuid();
    obj[Core::jsonctrl::vmap::vitem::JSON_COLOR]= ctrl->color().name();
    obj[Core::jsonctrl::vmap::vitem::JSON_LOCKED]= ctrl->locked();
    obj[Core::jsonctrl::vmap::vitem::JSON_TOOL]= ctrl->tool();
    obj[Core::jsonctrl::vmap::vitem::JSON_INITIALIZED]= ctrl->initialized();
}

vmap::VisualItemController::ItemType readVisualItemController(std::map<QString, QVariant>& map,
                              const QJsonObject& obj)
{
    auto res = static_cast<vmap::VisualItemController::ItemType>(obj[Core::jsonctrl::vmap::vitem::JSON_ITEM_TYPE].toInt());
    map.insert({Core::vmapkeys::KEY_ITEMTYPE, obj[Core::jsonctrl::vmap::vitem::JSON_ITEM_TYPE].toInt()});
    map.insert({Core::vmapkeys::KEY_SELECTED, obj[Core::jsonctrl::vmap::vitem::JSON_SELECTED].toBool()});
    map.insert({Core::vmapkeys::KEY_VISIBLE, obj[Core::jsonctrl::vmap::vitem::JSON_VISIBLE].toBool()});
    map.insert({Core::vmapkeys::KEY_OPACITY, obj[Core::jsonctrl::vmap::vitem::JSON_OPACITY].toDouble()});
    map.insert({Core::vmapkeys::KEY_ROTATION, obj[Core::jsonctrl::vmap::vitem::JSON_ROTATION].toDouble()});
    map.insert({Core::vmapkeys::KEY_LAYER, QVariant::fromValue(static_cast<Core::Layer>(obj[Core::jsonctrl::vmap::vitem::JSON_LAYER].toInt()))});
    map.insert({Core::vmapkeys::KEY_POS, QPointF{obj[Core::jsonctrl::vmap::vitem::JSON_POSITION_X].toDouble(),obj[Core::jsonctrl::vmap::vitem::JSON_POSITION_Y].toDouble()}});
    map.insert({Core::vmapkeys::KEY_UUID, obj[Core::jsonctrl::vmap::vitem::JSON_UUID].toString()});
    map.insert({Core::vmapkeys::KEY_COLOR, QColor(obj[Core::jsonctrl::vmap::vitem::JSON_COLOR].toString())});
    map.insert({Core::vmapkeys::KEY_LOCKED, obj[Core::jsonctrl::vmap::vitem::JSON_LOCKED].toBool()});
    map.insert({Core::vmapkeys::KEY_TOOL, obj[Core::jsonctrl::vmap::vitem::JSON_TOOL].toInt()});
    map.insert({Core::vmapkeys::KEY_INITIALIZED, obj[Core::jsonctrl::vmap::vitem::JSON_INITIALIZED].toBool()});

    return res;
}

template <class T>
QString convertToBase64(const T& data)
{
    QByteArray res;
    {
        QDataStream writer(res);
        writer << data;
    }
    return res.toBase64();
}

template <class T>
T convertFromBase64(const QString& data)
{
    T res;
    {
        auto bytes= QByteArray::fromBase64(data.toLocal8Bit());
        QDataStream out(bytes);
        out >> res;
    }
    return res;
}

QJsonObject saveVmapSightController(const vmap::SightController* ctrl)
{
    QJsonObject obj;
    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    obj[Core::jsonctrl::vmap::sight::JSON_CHARACTER_SIHT]= ctrl->characterSight();
    obj[Core::jsonctrl::vmap::sight::JSON_FOW_PATH]= convertToBase64<QPainterPath>(ctrl->fowPath());
    obj[Core::jsonctrl::vmap::sight::JSON_RECT_X]= ctrl->rect().x();
    obj[Core::jsonctrl::vmap::sight::JSON_RECT_Y]= ctrl->rect().y();
    obj[Core::jsonctrl::vmap::sight::JSON_RECT_W]= ctrl->rect().width();
    obj[Core::jsonctrl::vmap::sight::JSON_RECT_H]= ctrl->rect().height();
    obj[Core::jsonctrl::vmap::sight::JSON_CHARACTER_COUNT]= ctrl->characterCount();

    QJsonArray array;
    auto singus= ctrl->singularityList();

    std::for_each(singus.begin(), singus.end(),
                  [&array](const std::pair<QPolygonF, bool>& singu)
                  {
                      QJsonObject obj;
                      obj[Core::jsonctrl::vmap::sight::JSON_SING_POINTS]= convertToBase64<QPolygonF>(singu.first);
                      obj[Core::jsonctrl::vmap::sight::JSON_SING_ADD]= singu.second;
                      array.append(obj);
                  });

    obj[Core::jsonctrl::vmap::sight::JSON_SINGULARITIES] = array;
    return obj;
}

QJsonObject saveVMapRectItemController(const vmap::RectController* ctrl)
{
    QJsonObject obj;
    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    obj[Core::jsonctrl::vmap::rect::JSON_RECT_X]=ctrl->rect().x();
    obj[Core::jsonctrl::vmap::rect::JSON_RECT_Y]=ctrl->rect().y();
    obj[Core::jsonctrl::vmap::rect::JSON_RECT_W]=ctrl->rect().width();
    obj[Core::jsonctrl::vmap::rect::JSON_RECT_H]=ctrl->rect().height();
    obj[Core::jsonctrl::vmap::rect::JSON_FILLED]=ctrl->filled();
    obj[Core::jsonctrl::vmap::rect::JSON_PEN_WIDTH]=ctrl->penWidth();

    return obj;
}

void readRectController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    maps.insert({Core::vmapkeys::KEY_RECT, QRectF{obj[Core::jsonctrl::vmap::rect::JSON_RECT_X].toDouble(),
                                                  obj[Core::jsonctrl::vmap::rect::JSON_RECT_Y].toDouble(),
                                                  obj[Core::jsonctrl::vmap::rect::JSON_RECT_W].toDouble(),
                                                  obj[Core::jsonctrl::vmap::rect::JSON_RECT_H].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_FILLED, obj[Core::jsonctrl::vmap::rect::JSON_FILLED].toBool()});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, obj[Core::jsonctrl::vmap::rect::JSON_PEN_WIDTH].toInt()});
}

QJsonObject saveVMapEllipseItemController(const vmap::EllipseController* ctrl)
{
    QJsonObject obj;
    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    obj[Core::jsonctrl::vmap::ellipse::JSON_R_X]=ctrl->rx();
    obj[Core::jsonctrl::vmap::ellipse::JSON_R_Y]=ctrl->ry();
    obj[Core::jsonctrl::vmap::ellipse::JSON_FILLED]=ctrl->filled();
    obj[Core::jsonctrl::vmap::ellipse::JSON_PEN_WIDTH]=ctrl->penWidth();
     return obj;
}

void readEllipseController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    maps.insert({Core::vmapkeys::KEY_RX,obj[Core::jsonctrl::vmap::ellipse::JSON_R_X].toDouble()});
    maps.insert({Core::vmapkeys::KEY_RY,obj[Core::jsonctrl::vmap::ellipse::JSON_R_Y].toDouble()});
    maps.insert({Core::vmapkeys::KEY_FILLED,obj[Core::jsonctrl::vmap::ellipse::JSON_FILLED].toBool()});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH,obj[Core::jsonctrl::vmap::ellipse::JSON_PEN_WIDTH].toDouble()});
}

QJsonObject saveVMapImageItemController(const vmap::ImageItemController* ctrl)
{
    QJsonObject obj;
    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    obj[Core::jsonctrl::vmap::image::JSON_PIXMAP]=convertToBase64<QPixmap>(ctrl->pixmap());
    obj[Core::jsonctrl::vmap::image::JSON_DATA]=QString(ctrl->data().toBase64());
    obj[Core::jsonctrl::vmap::image::JSON_RECT_X]=ctrl->rect().x();
    obj[Core::jsonctrl::vmap::image::JSON_RECT_Y]=ctrl->rect().y();
    obj[Core::jsonctrl::vmap::image::JSON_RECT_W]=ctrl->rect().width();
    obj[Core::jsonctrl::vmap::image::JSON_RECT_H]=ctrl->rect().height();
    obj[Core::jsonctrl::vmap::image::JSON_PATH]=ctrl->path();
    obj[Core::jsonctrl::vmap::image::JSON_RATIO]=ctrl->ratio();
    return obj;
}

void readImageController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    maps.insert({Core::vmapkeys::KEY_PIXMAP, convertFromBase64<QPixmap>(obj[Core::jsonctrl::vmap::image::JSON_PIXMAP].toString())});
    maps.insert({Core::vmapkeys::KEY_DATA, QByteArray::fromBase64(obj[Core::jsonctrl::vmap::image::JSON_DATA].toString().toLocal8Bit())});
    maps.insert({Core::vmapkeys::KEY_RECT, QRectF{obj[Core::jsonctrl::vmap::image::JSON_RECT_X].toDouble(),
                                                  obj[Core::jsonctrl::vmap::image::JSON_RECT_Y].toDouble(),
                                                  obj[Core::jsonctrl::vmap::image::JSON_RECT_W].toDouble(),
                                                  obj[Core::jsonctrl::vmap::image::JSON_RECT_H].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_PATH, obj[Core::jsonctrl::vmap::image::JSON_PATH].toString()});
    maps.insert({Core::vmapkeys::KEY_RATIO, obj[Core::jsonctrl::vmap::image::JSON_RATIO].toDouble()});
}

QJsonObject saveVMapLineItemController(const vmap::LineController* ctrl)
{
    QJsonObject obj;

    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    obj[Core::jsonctrl::vmap::line::JSON_START_X]=ctrl->startPoint().x();
    obj[Core::jsonctrl::vmap::line::JSON_START_Y]=ctrl->startPoint().y();
    obj[Core::jsonctrl::vmap::line::JSON_END_X]=ctrl->endPoint().x();
    obj[Core::jsonctrl::vmap::line::JSON_END_Y]=ctrl->endPoint().y();
    obj[Core::jsonctrl::vmap::line::JSON_PEN_WIDTH]=ctrl->penWidth();
    return obj;
}

void readLineController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    maps.insert({Core::vmapkeys::KEY_STARTPOINT, QPointF{obj[Core::jsonctrl::vmap::line::JSON_START_X].toDouble(),
                                                         obj[Core::jsonctrl::vmap::line::JSON_START_Y].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_ENDPOINT,  QPointF{obj[Core::jsonctrl::vmap::line::JSON_END_X].toDouble(),
                                                       obj[Core::jsonctrl::vmap::line::JSON_END_Y].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, obj[Core::jsonctrl::vmap::line::JSON_PEN_WIDTH].toInt()});
}

QJsonObject saveVMapPathItemController(const vmap::PathController* ctrl)
{
    QJsonObject obj;

    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    QJsonArray points;
    auto ps = ctrl->points();
    std::for_each(ps.begin(), ps.end(), [&points](const QPointF& p) {
        QJsonObject point;
        point[Core::jsonctrl::vmap::path::JSON_POINT_X]=p.x();
        point[Core::jsonctrl::vmap::path::JSON_POINT_Y]=p.y();
        points.append(point);
    });

    obj[Core::jsonctrl::vmap::path::JSON_POINTS]=points;
    obj[Core::jsonctrl::vmap::path::JSON_PEN_WIDTH]=ctrl->penWidth();
    obj[Core::jsonctrl::vmap::path::JSON_CLOSED]=ctrl->closed();
    obj[Core::jsonctrl::vmap::path::JSON_FILLED]=ctrl->filled();
    obj[Core::jsonctrl::vmap::path::JSON_PEN_LINE]=ctrl->penLine();
    return obj;
}

void readPathController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    auto points = obj[Core::jsonctrl::vmap::path::JSON_POINTS].toArray();
    maps.insert({Core::vmapkeys::KEY_POINTCOUNT, points.size()});

    std::vector<QPointF> vec;
    vec.reserve(static_cast<std::size_t>(points.size()));
    std::transform(std::begin(points), std::end(points),std::back_inserter(vec),[](const QJsonValue& value){
        auto obj = value.toObject();
        return QPointF{obj[Core::jsonctrl::vmap::path::JSON_POINT_X].toDouble(), obj[Core::jsonctrl::vmap::path::JSON_POINT_Y].toDouble()};
    });

    maps.insert({Core::vmapkeys::KEY_POINTS, QVariant::fromValue(vec)});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, obj[Core::jsonctrl::vmap::path::JSON_PEN_WIDTH].toInt()});
    maps.insert({Core::vmapkeys::KEY_CLOSED, obj[Core::jsonctrl::vmap::path::JSON_CLOSED].toBool()});
    maps.insert({Core::vmapkeys::KEY_FILLED, obj[Core::jsonctrl::vmap::path::JSON_FILLED].toBool()});
    maps.insert({Core::vmapkeys::KEY_PENLINE, obj[Core::jsonctrl::vmap::path::JSON_PEN_LINE].toBool()});
}

QJsonObject saveVMapTextItemController(const vmap::TextController* ctrl)
{
    QJsonObject obj;

    if(!ctrl)
        return obj;

    saveVisualItemController(ctrl, obj);

    obj[Core::jsonctrl::vmap::text::JSON_TEXT]=ctrl->text();

    obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_X]=ctrl->textRect().x();
    obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_Y]=ctrl->textRect().y();
    obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_W]=ctrl->textRect().width();
    obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_H]=ctrl->textRect().height();

    obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_X]=ctrl->borderRect().x();
    obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_Y]=ctrl->borderRect().y();
    obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_W]=ctrl->borderRect().width();
    obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_H]=ctrl->borderRect().height();

    obj[Core::jsonctrl::vmap::text::JSON_BORDER]=ctrl->border();
    obj[Core::jsonctrl::vmap::text::JSON_FONT]=convertToBase64<QFont>(ctrl->font());
    obj[Core::jsonctrl::vmap::text::JSON_PEN_WIDTH]=ctrl->penWidth();

    obj[Core::jsonctrl::vmap::text::JSON_POS_X]=ctrl->textPos().x();
    obj[Core::jsonctrl::vmap::text::JSON_POS_Y]=ctrl->textPos().y();
    return obj;
}

void readVmapSightController(vmap::SightController* sight,const QJsonObject& obj)
{
    sight->setCharacterSight(obj[Core::jsonctrl::vmap::sight::JSON_CHARACTER_SIHT].toBool());
    sight->setRect(QRectF{     obj[Core::jsonctrl::vmap::sight::JSON_RECT_X].toDouble(),
                   obj[Core::jsonctrl::vmap::sight::JSON_RECT_Y].toDouble(),
                   obj[Core::jsonctrl::vmap::sight::JSON_RECT_W].toDouble(),
                          obj[Core::jsonctrl::vmap::sight::JSON_RECT_H].toDouble()});
    auto sings = obj[Core::jsonctrl::vmap::sight::JSON_SINGULARITIES].toArray();

    for(auto const& ref : sings)
    {
        auto const& obj = ref.toObject();
        sight->addPolygon(convertFromBase64<QPolygonF>(obj[Core::jsonctrl::vmap::sight::JSON_SING_POINTS].toString().toLocal8Bit()), obj[Core::jsonctrl::vmap::sight::JSON_SING_ADD].toBool());
    }
}

void readTextController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    maps.insert({Core::vmapkeys::KEY_TEXT, obj[Core::jsonctrl::vmap::text::JSON_TEXT].toString()});
    maps.insert({Core::vmapkeys::KEY_TEXTRECT, QRectF{obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_X].toDouble(),
                                                      obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_Y].toDouble(),
                                                      obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_W].toDouble(),
                                                      obj[Core::jsonctrl::vmap::text::JSON_TEXT_RECT_H].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_BORDERRECT, QRectF{obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_X].toDouble(),
                                                        obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_Y].toDouble(),
                                                        obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_W].toDouble(),
                                                        obj[Core::jsonctrl::vmap::text::JSON_BORDER_RECT_H].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_BORDER, obj[Core::jsonctrl::vmap::text::JSON_BORDER].toBool()});
    maps.insert({Core::vmapkeys::KEY_FONT, convertFromBase64<QFont>(obj[Core::jsonctrl::vmap::text::JSON_FONT].toString())});
    maps.insert({Core::vmapkeys::KEY_TEXTPOS, QPointF{obj[Core::jsonctrl::vmap::text::JSON_POS_X].toDouble(),
                                                     obj[Core::jsonctrl::vmap::text::JSON_POS_Y].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_PENWIDTH, obj[Core::jsonctrl::vmap::text::JSON_PEN_WIDTH].toInt()});
}

QJsonObject saveCharacter(Character* c)
{

    QJsonObject obj;

    if(!c)
        return obj;

    obj[Core::jsonctrl::vmap::character::JSON_CHAR_NPC]= c->isNpc();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_ID]= c->uuid();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_NAME]= c->name();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_AVATAR]= QString(c->avatar().toBase64());
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_HEALT_CURRENT]= c->getHealthPointsCurrent();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_HEALT_MAX]= c->getHealthPointsMax();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_HEALT_MIN]= c->getHealthPointsMin();

    obj[Core::jsonctrl::vmap::character::JSON_CHAR_INIT_SCORE]= c->getInitiativeScore();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_DISTANCE_TURN]= c->getDistancePerTurn();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_STATE_ID]= c->stateId();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_LIFE_COLOR]= c->getLifeColor().name();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_INIT_CMD]= c->initCommand();
    obj[Core::jsonctrl::vmap::character::JSON_CHAR_INIT_SCORE_DEFINED]= c->hasInitScore();

    return obj;
}

void readCharacter(std::map<QString, QVariant>& map, const QJsonObject& obj)
{
    map.insert({Core::vmapkeys::KEY_CHARAC_NPC, obj[Core::jsonctrl::vmap::character::JSON_CHAR_NPC].toBool()});
    map.insert({Core::vmapkeys::KEY_CHARAC_ID, obj[Core::jsonctrl::vmap::character::JSON_CHAR_ID].toString()});
    map.insert({Core::vmapkeys::KEY_CHARAC_NAME, obj[Core::jsonctrl::vmap::character::JSON_CHAR_NAME].toString()});
    map.insert({Core::vmapkeys::KEY_CHARAC_AVATAR, QByteArray::fromBase64(obj[Core::jsonctrl::vmap::character::JSON_CHAR_AVATAR].toString().toLocal8Bit())});
    map.insert({Core::vmapkeys::KEY_CHARAC_HPCURRENT, obj[Core::jsonctrl::vmap::character::JSON_CHAR_HEALT_CURRENT].toInt()});
    map.insert({Core::vmapkeys::KEY_CHARAC_HPMAX, obj[Core::jsonctrl::vmap::character::JSON_CHAR_HEALT_MAX].toInt()});
    map.insert({Core::vmapkeys::KEY_CHARAC_HPMIN, obj[Core::jsonctrl::vmap::character::JSON_CHAR_HEALT_MIN].toInt()});
    map.insert({Core::vmapkeys::KEY_CHARAC_INITSCORE, obj[Core::jsonctrl::vmap::character::JSON_CHAR_INIT_SCORE].toInt()});
    map.insert({Core::vmapkeys::KEY_CHARAC_DISTANCEPERTURN, obj[Core::jsonctrl::vmap::character::JSON_CHAR_DISTANCE_TURN].toDouble()});
    map.insert({Core::vmapkeys::KEY_CHARAC_STATEID, obj[Core::jsonctrl::vmap::character::JSON_CHAR_STATE_ID].toString()});
    map.insert({Core::vmapkeys::KEY_CHARAC_LIFECOLOR, QColor(obj[Core::jsonctrl::vmap::character::JSON_CHAR_LIFE_COLOR].toString())});
    map.insert({Core::vmapkeys::KEY_CHARAC_INITCMD, obj[Core::jsonctrl::vmap::character::JSON_CHAR_INIT_CMD].toString()});
    map.insert({Core::vmapkeys::KEY_CHARAC_HASINIT, obj[Core::jsonctrl::vmap::character::JSON_CHAR_INIT_SCORE_DEFINED].toBool()});
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

QJsonObject saveVMapCharacterItemController(const vmap::CharacterItemController* ctrl)
{

    QJsonObject obj;

    if(!ctrl)
        return obj;

    auto c= ctrl->character();
    auto vision= ctrl->vision();

    auto chacObj = saveCharacter(c);

    saveVisualItemController(ctrl, obj);


    obj[Core::jsonctrl::vmap::character::JSON_CHARACTER]=chacObj;
    obj[Core::jsonctrl::vmap::character::JSON_SIDE]=ctrl->side();
    obj[Core::jsonctrl::vmap::character::JSON_STATE_COLOR]=ctrl->stateColor().name();
    obj[Core::jsonctrl::vmap::character::JSON_NUMBER]=ctrl->number();
    obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_X]=ctrl->textRect().x();
    obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_Y]=ctrl->textRect().y();
    obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_W]=ctrl->textRect().width();
    obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_H]=ctrl->textRect().height();

    obj[Core::jsonctrl::vmap::character::JSON_FONT]=convertToBase64<QFont>(ctrl->font());
    obj[Core::jsonctrl::vmap::character::JSON_RADIUS]=ctrl->radius();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_POS_X]=vision->position().x();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_POS_Y]=vision->position().y();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_ANGLE]=vision->angle();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_SHAPE]=vision->shape();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_ROTATION]=vision->rotation();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_VISIBLE]=vision->visible();
    obj[Core::jsonctrl::vmap::character::JSON_VIS_RADIUS]=vision->radius();

    return obj;
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
    hu::setParamIfAny<bool>(cv::KEY_PLAYABLECHARACTER, params, std::bind(&vmap::CharacterItemController::setPlayableCharacter, ctrl, _1));
    // clang-format on
}

void readCharacterItemController(std::map<QString, QVariant>& maps,const QJsonObject& obj)
{
    readCharacter(maps, obj[Core::jsonctrl::vmap::character::JSON_CHARACTER].toObject());

    maps.insert({Core::vmapkeys::KEY_SIDE, obj[Core::jsonctrl::vmap::character::JSON_SIDE].toDouble()});
    maps.insert({Core::vmapkeys::KEY_STATECOLOR, QColor(obj[Core::jsonctrl::vmap::character::JSON_STATE_COLOR].toString())});
    maps.insert({Core::vmapkeys::KEY_NUMBER, obj[Core::jsonctrl::vmap::character::JSON_NUMBER].toInt()});
    maps.insert({Core::vmapkeys::KEY_TEXTRECT, QRectF{  obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_X].toDouble(),
                                                        obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_Y].toDouble(),
                                                        obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_W].toDouble(),
                                                        obj[Core::jsonctrl::vmap::character::JSON_TEXT_RECT_H].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_RADIUS, obj[Core::jsonctrl::vmap::character::JSON_RADIUS]});
    maps.insert({Core::vmapkeys::KEY_FONT, convertFromBase64<QFont>(obj[Core::jsonctrl::vmap::character::JSON_FONT].toString())});

    maps.insert({Core::vmapkeys::KEY_VIS_POS, QPointF{obj[Core::jsonctrl::vmap::character::JSON_VIS_POS_X].toDouble(),
                                                      obj[Core::jsonctrl::vmap::character::JSON_VIS_POS_Y].toDouble()}});
    maps.insert({Core::vmapkeys::KEY_VIS_ANGLE, obj[Core::jsonctrl::vmap::character::JSON_VIS_ANGLE].toDouble()});
    maps.insert({Core::vmapkeys::KEY_VIS_SHAPE, static_cast<CharacterVision::SHAPE>(obj[Core::jsonctrl::vmap::character::JSON_VIS_SHAPE].toInt())});
    maps.insert({Core::vmapkeys::KEY_VIS_VISIBLE, obj[Core::jsonctrl::vmap::character::JSON_VIS_VISIBLE].toBool()});
    maps.insert({Core::vmapkeys::KEY_VIS_RADIUS, obj[Core::jsonctrl::vmap::character::JSON_VIS_RADIUS].toDouble()});
    maps.insert({Core::vmapkeys::KEY_VIS_ROTATION, obj[Core::jsonctrl::vmap::character::JSON_VIS_ROTATION].toDouble()});

    if(maps.end() != maps.find(Core::vmapkeys::KEY_CHARAC_ID))
    {
        CharacterFinder finder;
        auto c= finder.find(maps.at(Core::vmapkeys::KEY_CHARAC_ID).value<QString>());
        if(c)
        {
            maps.insert({Core::vmapkeys::KEY_CHARACTER, QVariant::fromValue(c)});
        }
    }
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

std::map<QString, QVariant> readItemController(const QJsonObject& input)
{
    std::map<QString, QVariant> maps;

    vmap::VisualItemController::ItemType type = readVisualItemController(maps, input);

    switch(type)
    {
    case vmap::VisualItemController::ELLIPSE:
        readEllipseController(maps, input);
        break;
    case vmap::VisualItemController::PATH:
        readPathController(maps, input);
        break;
    case vmap::VisualItemController::RECT:
        readRectController(maps, input);
        break;
    case vmap::VisualItemController::TEXT:
        readTextController(maps, input);
        break;
    case vmap::VisualItemController::CHARACTER:
        readCharacterItemController(maps, input);
        break;
    case vmap::VisualItemController::LINE:
        readLineController(maps, input);
        break;
    case vmap::VisualItemController::IMAGE:
        readImageController(maps, input);
        break;
    default:
        break;
    }

    return maps;
}

void readModel(VectorialMapController* ctrl, const QJsonArray& items)
{
    for(auto ref : items)
    {
        ctrl->addItemController(readItemController(ref.toObject()));
    }
}

QJsonObject saveItemController(vmap::VisualItemController* itemCtrl)
{
    using vv= vmap::VisualItemController;
    if(itemCtrl->removed())
        return {};

    QJsonObject res;

    switch(itemCtrl->itemType())
    {
    case vv::RECT:
        res = saveVMapRectItemController(dynamic_cast<vmap::RectController*>(itemCtrl));
        break;
    case vv::PATH:
        res = saveVMapPathItemController(dynamic_cast<vmap::PathController*>(itemCtrl));
        break;
    case vv::LINE:
        res = saveVMapLineItemController(dynamic_cast<vmap::LineController*>(itemCtrl));
        break;
    case vv::ELLIPSE:
        res = saveVMapEllipseItemController(dynamic_cast<vmap::EllipseController*>(itemCtrl));
        break;
    case vv::CHARACTER:
        res = saveVMapCharacterItemController(dynamic_cast<vmap::CharacterItemController*>(itemCtrl));
        break;
    case vv::TEXT:
        res = saveVMapTextItemController(dynamic_cast<vmap::TextController*>(itemCtrl));
        break;
    case vv::IMAGE:
        res = saveVMapImageItemController(dynamic_cast<vmap::ImageItemController*>(itemCtrl));
        break;
    case vv::RULE:
    case vv::SIGHT:
    case vv::ANCHOR:
    case vv::GRID:
    case vv::HIGHLIGHTER:
        break;
    }

    return res;
}

QByteArray VectorialMapMessageHelper::saveVectorialMap(VectorialMapController* ctrl)
{
    if(!ctrl)
        return {};

    QJsonObject obj;
    //QDataStream output(&data, QIODevice::WriteOnly);
    IOHelper::saveMediaBaseIntoJSon(ctrl, obj);

    // properties
    obj[Core::jsonctrl::vmap::JSON_NPC_NAME_VISIBLE]= ctrl->npcNameVisible();
    obj[Core::jsonctrl::vmap::JSON_PC_NAME_VISIBLE]= ctrl->pcNameVisible();
    obj[Core::jsonctrl::vmap::JSON_NPC_NUMBER_VISIBLE]= ctrl->npcNumberVisible();
    obj[Core::jsonctrl::vmap::JSON_HEALTH_BAR_VISIBLE]= ctrl->healthBarVisible();
    obj[Core::jsonctrl::vmap::JSON_INIT_SCORE_VISIBLE]= ctrl->initScoreVisible();
    obj[Core::jsonctrl::vmap::JSON_STATE_LABEL_VISIBLE]= ctrl->stateLabelVisible();
    obj[Core::jsonctrl::vmap::JSON_COLLISION]= ctrl->collision();
    obj[Core::jsonctrl::vmap::JSON_CHARACTER_VISION]= ctrl->characterVision();
    obj[Core::jsonctrl::vmap::JSON_GRID_COLOR]= ctrl->gridColor().name();
    obj[Core::jsonctrl::vmap::JSON_GRID_SCALE]= ctrl->gridScale();
    obj[Core::jsonctrl::vmap::JSON_GRID_SIZE]= ctrl->gridSize();
    obj[Core::jsonctrl::vmap::JSON_GRID_VISIBLE]= ctrl->gridVisibility();
    obj[Core::jsonctrl::vmap::JSON_GRID_ABOVE]= ctrl->gridAbove();
    obj[Core::jsonctrl::vmap::JSON_SCALE_UNIT]= ctrl->scaleUnit();
    obj[Core::jsonctrl::vmap::JSON_NPC_NAME]= ctrl->npcName();
    obj[Core::jsonctrl::vmap::JSON_PERMISSION]= ctrl->permission();
    obj[Core::jsonctrl::vmap::JSON_GRID_PATTERN]= static_cast<quint8>(ctrl->gridPattern());
    obj[Core::jsonctrl::vmap::JSON_VISIBILITY]= ctrl->visibility();
    obj[Core::jsonctrl::vmap::JSON_BACKGROUND_COLOR]= ctrl->backgroundColor().name();
    obj[Core::jsonctrl::vmap::JSON_TOOL_COLOR]= ctrl->toolColor().name();
    obj[Core::jsonctrl::vmap::JSON_PEN_SIZE]= ctrl->penSize();
    obj[Core::jsonctrl::vmap::JSON_NPC_NUMBER]= ctrl->npcNumber();
    obj[Core::jsonctrl::vmap::JSON_ZOOM_LEVEL]= ctrl->zoomLevel();
    obj[Core::jsonctrl::vmap::JSON_Layer]= static_cast<quint8>(ctrl->layer());
    obj[Core::jsonctrl::vmap::JSON_OPACITY]= ctrl->opacity();
    auto r = ctrl->visualRect();
    obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_X]= r.x();
    obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_Y]= r.y();
    obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_W]= r.width();
    obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_H]= r.height();
    obj[Core::jsonctrl::vmap::JSON_IDLE]= ctrl->idle();
    obj[Core::jsonctrl::vmap::JSON_Z_INDEX]= ctrl->zIndex();

    auto sight= ctrl->sightController();

    obj[Core::jsonctrl::vmap::JSON_SIGHT_CTRL]= saveVmapSightController(sight);


    auto model= ctrl->model();
    auto vec= model->items();
    QJsonArray items;
    for(auto itemCtrl : vec)
    {
        if(itemCtrl->removed())
            continue;
        auto item = saveItemController(itemCtrl);
        if(item.isEmpty())
            continue;

        items.append(item);
    }

    obj[Core::jsonctrl::vmap::JSON_ITEMS]= items;

    QJsonDocument doc;
    doc.setObject(obj);

    return doc.toJson();
}

void VectorialMapMessageHelper::fetchModelFromMap(const QHash<QString, QVariant>& params, VectorialMapController* ctrl)
{
    auto s= params.size();
    for(int i= 0; i < s; ++i)
    {
        auto const& item= params.value(QString("Item_%1").arg(i));
        auto const maps= item.toMap().toStdMap();
        if(!maps.empty())
            ctrl->addItemController(maps);
    }
}

QHash<QString, QVariant> VectorialMapMessageHelper::itemsToHash(const QList<vmap::VisualItemController*>& ctrls)
{
    QHash<QString, QVariant> items;

    int i= 0;
    for(auto const& ctrl : ctrls)
    {
        auto item = saveItemController(ctrl);
        items.insert(QString("Item_%1").arg(i), QVariant::fromValue(readItemController(item)));
        ++i;
    }

    return items;
}

void VectorialMapMessageHelper::fetchSightController(vmap::SightController *ctrl, const QHash<QString, QVariant> &params)
{
    if(!ctrl)
        return;

    namespace ck= Core::vmapkeys;
    namespace hu= helper::utils;
    using std::placeholders::_1;


    auto x = params.value(ck::KEY_SIGHT_X).value<qreal>();
    auto y = params.value(ck::KEY_SIGHT_Y).value<qreal>();
    auto w = params.value(ck::KEY_SIGHT_W).value<qreal>();
    auto h = params.value(ck::KEY_SIGHT_H).value<qreal>();

    ctrl->setRect(QRectF{x,y,w,h});

    auto posx = params.value(ck::KEY_SIGHT_POSX).value<qreal>();
    auto posy = params.value(ck::KEY_SIGHT_POSY).value<qreal>();

    ctrl->setPos(QPointF{posx, posy});

    ctrl->setFowPath(params.value(ck::KEY_SIGHT_PATH).value<QPainterPath>());

}

void VectorialMapMessageHelper::readVectorialMapController(VectorialMapController* ctrl, const QByteArray& array)
{
    if(!ctrl)
        return;

    QJsonDocument doc = QJsonDocument::fromJson(array);

    auto obj = doc.object();

    IOHelper::readBaseFromJson(ctrl, obj);

    auto itemArray = obj[Core::jsonctrl::vmap::JSON_ITEMS].toArray();

    ctrl->setNpcNameVisible(obj[Core::jsonctrl::vmap::JSON_NPC_NAME_VISIBLE].toBool());
    ctrl->setPcNameVisible(obj[Core::jsonctrl::vmap::JSON_PC_NAME_VISIBLE].toBool());
    ctrl->setNpcNumberVisible(obj[Core::jsonctrl::vmap::JSON_NPC_NUMBER_VISIBLE].toBool());
    ctrl->setHealthBarVisible(obj[Core::jsonctrl::vmap::JSON_HEALTH_BAR_VISIBLE].toBool());
    ctrl->setInitScoreVisible(obj[Core::jsonctrl::vmap::JSON_INIT_SCORE_VISIBLE].toBool());
    ctrl->setStateLabelVisible(obj[Core::jsonctrl::vmap::JSON_STATE_LABEL_VISIBLE].toBool());
    ctrl->setCollision(obj[Core::jsonctrl::vmap::JSON_COLLISION].toBool());
    ctrl->setCharacterVision(obj[Core::jsonctrl::vmap::JSON_CHARACTER_VISION].toBool());
    ctrl->setGridColor(QColor(obj[Core::jsonctrl::vmap::JSON_GRID_COLOR].toString()));
    ctrl->setGridScale(obj[Core::jsonctrl::vmap::JSON_GRID_SCALE].toDouble());
    ctrl->setGridSize(obj[Core::jsonctrl::vmap::JSON_GRID_SIZE].toInt());
    ctrl->setGridVisibility(obj[Core::jsonctrl::vmap::JSON_GRID_VISIBLE].toBool());
    ctrl->setGridAbove(obj[Core::jsonctrl::vmap::JSON_GRID_ABOVE].toBool());
    ctrl->setScaleUnit(static_cast<Core::ScaleUnit>(obj[Core::jsonctrl::vmap::JSON_SCALE_UNIT].toInt()));
    ctrl->setNpcName(obj[Core::jsonctrl::vmap::JSON_NPC_NAME].toString());
    ctrl->setPermission(static_cast<Core::PermissionMode>(obj[Core::jsonctrl::vmap::JSON_PERMISSION].toInt()));
    ctrl->setGridPattern(static_cast<Core::GridPattern>(obj[Core::jsonctrl::vmap::JSON_GRID_PATTERN].toInt()));
    ctrl->setVisibility(static_cast<Core::VisibilityMode>(obj[Core::jsonctrl::vmap::JSON_VISIBILITY].toInt()));
    ctrl->setBackgroundColor(QColor(obj[Core::jsonctrl::vmap::JSON_BACKGROUND_COLOR].toString()));
    ctrl->setToolColor(QColor(obj[Core::jsonctrl::vmap::JSON_TOOL_COLOR].toString()));
    ctrl->setPenSize( obj[Core::jsonctrl::vmap::JSON_PEN_SIZE].toInt());
    ctrl->setNpcNumber(obj[Core::jsonctrl::vmap::JSON_NPC_NUMBER].toInt());
    ctrl->setZoomLevel( obj[Core::jsonctrl::vmap::JSON_ZOOM_LEVEL].toDouble());
    ctrl->setLayer(static_cast<Core::Layer>(obj[Core::jsonctrl::vmap::JSON_Layer].toInt()));
    ctrl->setOpacity(obj[Core::jsonctrl::vmap::JSON_OPACITY].toDouble());
    ctrl->setVisualRect(QRectF{obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_X].toDouble(),
                               obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_Y].toDouble(),
                               obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_W].toDouble(),
                               obj[Core::jsonctrl::vmap::JSON_VISUAL_RECT_H].toDouble()});
    ctrl->setIdle(obj[Core::jsonctrl::vmap::JSON_IDLE].toBool());
    ctrl->setZindex(obj[Core::jsonctrl::vmap::JSON_Z_INDEX].toDouble());

    // fog singularies
    auto sight= ctrl->sightController();

    readVmapSightController(sight, obj[Core::jsonctrl::vmap::JSON_SIGHT_CTRL].toObject());

    readModel(ctrl, itemArray);
}
