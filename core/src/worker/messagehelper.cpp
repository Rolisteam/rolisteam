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
#include "messagehelper.h"

#include <QBuffer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRgb>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#ifdef WITH_PDF
#include "controller/view_controller/pdfcontroller.h"
#endif
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#include "vmap/controller/characteritemcontroller.h"
#include "vmap/controller/ellipsecontroller.h"
#include "vmap/controller/gridcontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/linecontroller.h"
#include "vmap/controller/pathcontroller.h"
#include "vmap/controller/rectcontroller.h"
#include "vmap/controller/sightcontroller.h"
#include "vmap/controller/textcontroller.h"
#include "vmap/controller/visualitemcontroller.h"

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "data/character.h"
#include "data/cleveruri.h"
#include "data/player.h"
#include "dicealias.h"
#include "model/vmapitemmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "preferences/characterstatemodel.h"
#include "preferences/dicealiasmodel.h"
#include "userlist/playermodel.h"

#include "worker/playermessagehelper.h"

QByteArray imageToByteArray(const QImage& img)
{
    QByteArray array;
    QBuffer buffer(&array);
    img.save(&buffer, "PNG");
    return array;
}

MessageHelper::MessageHelper() {}

void MessageHelper::sendOffGoodBye()
{
    NetworkMessageWriter message(NetMsg::AdministrationCategory, NetMsg::Goodbye);
    message.sendToServer();
}

QString MessageHelper::readPlayerId(NetworkMessageReader& msg)
{
    return msg.string8();
}

void MessageHelper::sendOffAllDiceAlias(DiceAliasModel* model)
{
    if(nullptr == model)
        return;

    auto aliases= model->getAliases();
    int i= 0;
    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::DiceAliasModel);
    msg.uint32(static_cast<quint32>(i));
    for(auto& alias : *aliases)
    {
        msg.int64(i);
        msg.string32(alias->getCommand());
        msg.string32(alias->getValue());
        msg.int8(alias->isReplace());
        msg.int8(alias->isEnable());
        msg.string32(alias->getComment());
        ++i;
    }
    msg.sendToServer();
}

void MessageHelper::sendOffOneDiceAlias(DiceAlias* da, int row)
{
    if(nullptr == da)
        return;

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addDiceAlias);
    msg.int64(row);
    msg.string32(da->getCommand());
    msg.string32(da->getValue());
    msg.int8(da->isReplace());
    msg.int8(da->isEnable());
    msg.string32(da->getComment());
    msg.sendToServer();
}

void MessageHelper::sendOffAllCharacterState(CharacterStateModel* model)
{
    if(nullptr == model)
        return;

    auto states= model->getCharacterStates();
    quint64 i= 0;
    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::CharactereStateModel);
    msg.uint32(static_cast<quint32>(states.size()));
    for(auto state : states)
    {
        msg.uint64(i);
        msg.string32(state->getLabel());
        msg.rgb(state->getColor().rgb());
        if(state->hasImage())
        {
            msg.uint8(static_cast<quint8>(true));

            QByteArray array;
            QBuffer buffer(&array);
            if(!state->getPixmap()->save(&buffer, "PNG"))
            {
                qWarning("error during encoding png");
            }
            msg.byteArray32(array);
        }
        else
        {
            msg.uint8(static_cast<quint8>(false));
        }
        ++i;
    }
    msg.sendToServer();
}

void MessageHelper::sendOffOneCharacterState(CharacterState* state, int row)
{
    if(nullptr == state)
        return;

    NetworkMessageWriter msg(NetMsg::SharePreferencesCategory, NetMsg::addCharacterState);
    msg.int64(row);
    msg.string32(state->getLabel());
    msg.rgb(state->getColor().rgb());
    msg.sendToServer();
}

QString MessageHelper::readMediaId(NetworkMessageReader* msg)
{
    return msg->string8();
}

void MessageHelper::sendOffOpenMedia(AbstractMediaController* ctrl) {}

void MessageHelper::closeMedia(const QString& id, Core::ContentType type)
{
    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::CloseMedia);
    msg.uint8(static_cast<quint8>(type));
    msg.string8(id);
    msg.sendToServer();
}

void MessageHelper::sendOffImage(ImageController* ctrl)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string16(ctrl->name());
    msg.string8(ctrl->uuid());
    msg.string8(ctrl->ownerId());
    msg.byteArray32(ctrl->data());

    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readImageData(NetworkMessageReader* msg)
{
    // QHash<QString, QVariant> data;
    auto name= msg->string16();
    auto id= msg->string8();
    auto owner= msg->string8();
    auto data= msg->byteArray32();
    return {{"name", name}, {"uuid", id}, {"owner", id}, {"data", data}};
}

void MessageHelper::updatePerson(NetworkMessageReader& data, PlayerModel* playerModel)
{
    QString uuid= data.string8();
    auto person= playerModel->playerById(uuid);
    if(nullptr == person)
        return;

    auto role= data.int32();

    auto property= data.string8();
    QVariant var;
    if(property.contains("color"))
    {
        var= QVariant::fromValue(QColor(data.rgb()));
    }
    else if(property.contains("avatar"))
    {
        auto array= data.byteArray32();
        QDataStream out(&array, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_7);
        QImage img;
        out >> img;
        var= QVariant::fromValue(img);
    }
    else if(property.contains(QStringLiteral("state")))
    {
        auto label= data.string32();
        auto state= Character::getStateFromLabel(label);
        var= QVariant::fromValue(state);
    }
    else
    {
        auto val= data.string32();
        var= QVariant::fromValue(val);
    }

    auto idx= playerModel->personToIndex(person);

    playerModel->setData(idx, var, role);
}

QHash<QString, QVariant> MessageHelper::readWebPageData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto id= msg->string8();
    auto mode= msg->uint8();
    auto data= msg->string32();

    return QHash<QString, QVariant>(
        {{"id", id}, {"mode", mode}, {"data", data}, {"state", static_cast<int>(WebpageController::RemoteView)}});
}

void MessageHelper::stopSharingSheet(const QString& mediaId, const QString& characterId)
{
    NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::closeCharacterSheet);
    msg.string8(mediaId);
    msg.string8(characterId);
    msg.sendToServer();
}

void MessageHelper::shareCharacterSheet(CharacterSheet* sheet, Character* character, CharacterSheetController* ctrl)
{
    if(sheet == nullptr || character == nullptr)
        return;

    Player* parent= character->getParentPlayer();

    if(parent == nullptr)
        return;

    NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::addCharacterSheet);
    QStringList idList;
    idList << parent->uuid();
    msg.setRecipientList(idList, NetworkMessage::OneOrMany);
    // msg.string8(parent->uuid());

    msg.string8(ctrl->uuid());
    msg.string8(ctrl->name());
    msg.string8(character->uuid());

    QJsonObject object;
    sheet->save(object);
    QJsonDocument doc;
    doc.setObject(object);
    msg.byteArray32(doc.toBinaryData());

    msg.string32(ctrl->qmlCode());

    auto imageModel= ctrl->imageModel();
    QJsonArray array;
    imageModel->save(array);
    QJsonDocument doc2;
    doc2.setArray(array);
    msg.byteArray32(doc2.toBinaryData());

    auto model= ctrl->model();
    QJsonDocument doc3;
    doc3.setObject(model->rootSectionData());
    msg.byteArray32(doc3.toBinaryData());

    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readCharacterSheet(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto id= msg->string8();
    auto name= msg->string8();
    auto characterId= msg->string8();
    auto data= msg->byteArray32();
    auto qml= msg->string32();
    auto imageData= msg->byteArray32();
    auto rootSection= msg->byteArray32();

    return {{"id", id},   {"name", name},           {"characterId", characterId}, {"data", data},
            {"qml", qml}, {"imageData", imageData}, {"rootSection", rootSection}};
}

void MessageHelper::readUpdateField(CharacterSheetController* ctrl, NetworkMessageReader* msg)
{
    if(nullptr == ctrl || nullptr == msg)
        return;

    auto sheetId= msg->string8();
    auto path= msg->string32();
    auto data= msg->byteArray32();

    QJsonDocument doc= QJsonDocument::fromBinaryData(data);
    auto obj= doc.object();
    ctrl->updateFieldFrom(sheetId, obj, data);
}

void MessageHelper::shareNotesTo(const SharedNoteController* ctrl, const QStringList& recipiants)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    msg.uint8(static_cast<bool>(ctrl->highligthedSyntax() == SharedNoteController::HighlightedSyntax::MarkDown));
    msg.string8(ctrl->ownerId());
    msg.string32(ctrl->text());
    msg.sendToServer();
}

void MessageHelper::closeNoteTo(SharedNoteController* sharedCtrl, const QString& id)
{
    if(nullptr == sharedCtrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::CloseMedia);
    msg.setRecipientList({id}, NetworkMessage::OneOrMany);
    msg.uint8(static_cast<quint8>(sharedCtrl->contentType()));
    msg.string8(sharedCtrl->uuid());
    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readSharedNoteData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto id= msg->string8();
    auto mkH= static_cast<bool>(msg->uint8());
    auto ownerId= msg->string8();
    auto text= msg->string32();

    return QHash<QString, QVariant>({{"id", id}, {"markdown", mkH}, {"ownerId", ownerId}, {"text", text}});
}

void MessageHelper::shareWebpage(WebpageController* ctrl)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    auto mode= ctrl->sharingMode();
    msg.uint8(mode);
    if(mode == WebpageController::Html)
        msg.string32(ctrl->html());
    else if(mode == WebpageController::Url)
        msg.string32(ctrl->path());
    msg.sendToServer();
}

void MessageHelper::updateWebpage(WebpageController* ctrl)
{
    if(nullptr == ctrl)
        return;

    auto mode= ctrl->sharingMode();
    if(mode == WebpageController::None)
        return;

    NetworkMessageWriter msg(NetMsg::WebPageCategory, NetMsg::UpdateContent);
    msg.string8(ctrl->uuid());
    msg.uint8(mode);
    if(mode == WebpageController::Html)
        msg.string32(ctrl->html());
    else if(mode == WebpageController::Url)
        msg.string32(ctrl->path());

    msg.sendToServer();
}
#ifdef WITH_PDF
void MessageHelper::sendOffPdfFile(PdfController* ctrl)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    msg.byteArray32(ctrl->data());
    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readPdfData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto id= msg->string8();
    auto data= msg->byteArray32();

    return QHash<QString, QVariant>({{"id", id}, {"data", data}});
}
#endif
void addVisualItemController(const vmap::VisualItemController* ctrl, NetworkMessageWriter& msg)
{
    msg.uint8(ctrl->visible());
    msg.uint8(ctrl->initialized());
    msg.real(ctrl->opacity());
    msg.real(ctrl->rotation());
    msg.uint8(static_cast<quint8>(ctrl->layer()));
    auto pos= ctrl->pos();
    msg.real(pos.x());
    msg.real(pos.y());
    msg.string8(ctrl->uuid());
    msg.rgb(ctrl->color().rgb());
    msg.uint8(ctrl->locked());
}

const std::map<QString, QVariant> readVisualItemController(NetworkMessageReader* msg)
{
    auto visible= msg->uint8();
    auto initialized= msg->uint8();
    auto opacity= msg->real();
    auto rotation= msg->real();
    auto layer= msg->uint8();
    QPointF pos;
    pos.setX(msg->real());
    pos.setY(msg->real());
    auto uuid= msg->string8();
    auto rgb= QColor(msg->rgb());
    auto locked= msg->uint8();

    return std::map<QString, QVariant>({{"visible", visible},
                                        {"initialized", initialized},
                                        {"opacity", opacity},
                                        {"rotation", rotation},
                                        {"layer", layer},
                                        {"position", pos},
                                        {"uuid", uuid},
                                        {"color", rgb},
                                        {"locked", locked}});
}

QHash<QString, QVariant> readSightController(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    QHash<QString, QVariant> hash;

    hash["uuid"]= msg->string8();
    hash["x"]= msg->real();
    hash["y"]= msg->real();
    hash["w"]= msg->real();
    hash["h"]= msg->real();
    hash["posx"]= msg->real();
    hash["posy"]= msg->real();

    hash["count"]= msg->uint64();

    auto count= msg->uint64();
    for(quint64 i= 0; i < count; ++i)
    {
        auto pointsCount= msg->uint64();
        hash[QStringLiteral("points_count_%1").arg(i)]= pointsCount;
        hash[QStringLiteral("adding_%1").arg(i)]= msg->uint8();
        QPolygonF p;
        for(quint64 j= 0; j < pointsCount; ++j)
        {
            auto x= msg->real();
            auto y= msg->real();
            hash[QStringLiteral("points_%1_%2").arg(j).arg(i)]= QPointF(x, y);
        }
    }
    return hash;
}

void addSightController(vmap::SightController* ctrl, NetworkMessageWriter& msg)
{
    if(nullptr == ctrl)
        return;

    msg.string8(ctrl->uuid());
    auto rect= ctrl->rect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());

    auto pos= ctrl->pos();
    msg.real(pos.x());
    msg.real(pos.y());
    // msg.real(ctrl->zValue());
    auto singularity= ctrl->singularityList();
    msg.uint64(static_cast<quint64>(singularity.size()));
    for(auto pair : singularity)
    {
        auto points= pair.first;
        msg.uint64(static_cast<quint64>(points.size()));
        msg.uint8(static_cast<quint8>(pair.second));

        std::for_each(points.begin(), points.end(), [&msg](const QPointF& p) {
            msg.real(p.x());
            msg.real(p.y());
        });
    }
}

void addRectController(const vmap::RectController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.uint8(ctrl->filled());
    msg.uint16(ctrl->penWidth());
    auto rect= ctrl->rect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());
}

const std::map<QString, QVariant> MessageHelper::readRect(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto filled= msg->uint8();
    auto penWidth= msg->uint16();
    auto x= msg->real();
    auto y= msg->real();
    auto w= msg->real();
    auto h= msg->real();
    hash.insert({"filled", filled});
    hash.insert({"penWidth", penWidth});
    hash.insert({"rect", QRectF(x, y, w, h)});
    return hash;
}

QHash<QString, QVariant> readRectManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readRect(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

void addRectManager(RectControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::RectController* ctrl) { addRectController(ctrl, msg); });
}

QHash<QString, QVariant> readLineManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readLine(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

QHash<QString, QVariant> readPathManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readPath(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

QHash<QString, QVariant> readImageManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readImage(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

QHash<QString, QVariant> readTextManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readImage(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

QHash<QString, QVariant> readCharacterManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readImage(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

void addEllipseController(const vmap::EllipseController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.uint8(ctrl->filled());
    msg.uint16(ctrl->penWidth());
    msg.real(ctrl->rx());
    msg.real(ctrl->ry());
}

QHash<QString, QVariant> readEllipseManager(NetworkMessageReader* msg)
{
    QHash<QString, QVariant> result;
    auto count= msg->uint64();
    result["count"]= count;
    for(quint64 i= 0; i < count; ++i)
    {
        auto param= MessageHelper::readEllipse(msg);
        QHash<QString, QVariant> paramHash(param.begin(), param.end());
        result[QString("param_%1").arg(i)]= paramHash;
    }
    return result;
}

void addEllipseManager(EllipsControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::EllipseController* ctrl) { addEllipseController(ctrl, msg); });
}

void addLineController(const vmap::LineController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    auto start= ctrl->startPoint();
    msg.real(start.x());
    msg.real(start.y());

    msg.uint16(ctrl->penWidth());

    auto end= ctrl->endPoint();
    msg.real(end.x());
    msg.real(end.y());
}

const std::map<QString, QVariant> MessageHelper::readLine(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto x1= msg->real();
    auto y1= msg->real();

    auto penWidth= msg->uint16();

    auto x2= msg->real();
    auto y2= msg->real();
    hash.insert({"endPoint", QPointF(x1, y1)});
    hash.insert({"startPoint", QPointF(x2, y2)});
    hash.insert({"penWidth", penWidth});
    return hash;
}

void addLineManager(LineControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::LineController* ctrl) { addLineController(ctrl, msg); });
}

void addImageController(const vmap::ImageController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.real(ctrl->ratio());
    msg.byteArray32(ctrl->data());
    auto rect= ctrl->rect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());
}

const std::map<QString, QVariant> MessageHelper::readImage(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto ratio= msg->real();
    auto data= msg->byteArray32();
    auto x= msg->real();
    auto y= msg->real();
    auto w= msg->real();
    auto h= msg->real();
    hash.insert({"ratio", ratio});
    hash.insert({"data", data});
    hash.insert({"rect", QRectF(x, y, w, h)});
    return hash;
}

void addTextController(const vmap::TextController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.string32(ctrl->text());
    msg.uint16(ctrl->penWidth());
    msg.uint8(ctrl->border());

    auto p= ctrl->textPos();
    msg.real(p.x());
    msg.real(p.y());

    auto rect= ctrl->textRect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());

    rect= ctrl->borderRect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());
}

const std::map<QString, QVariant> MessageHelper::readText(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto text= msg->string32();
    auto penWidth= msg->uint16();
    auto border= static_cast<bool>(msg->uint8());

    auto x= msg->real();
    auto y= msg->real();
    QPointF textPos(x, y);

    x= msg->real();
    y= msg->real();
    auto w= msg->real();
    auto h= msg->real();
    QRectF textRect(x, y, w, h);

    hash.insert({"text", text});
    hash.insert({"penWidth", penWidth});
    hash.insert({"border", border});
    hash.insert({"textPos", textPos});
    hash.insert({"textRect", textRect});

    return hash;
}

void addImageManager(ImageControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::ImageController* ctrl) { addImageController(ctrl, msg); });
}

void addPathController(const vmap::PathController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.uint8(ctrl->filled());
    msg.uint8(ctrl->closed());
    msg.uint8(ctrl->penLine());
    msg.uint16(ctrl->penWidth());
    auto points= ctrl->points();
    msg.uint64(static_cast<quint64>(points.size()));
    for(auto p : points)
    {
        msg.real(p.x());
        msg.real(p.y());
    }
}

const std::map<QString, QVariant> MessageHelper::readPath(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto filled= static_cast<bool>(msg->uint8());
    auto closed= static_cast<bool>(msg->uint8());
    auto penLine= static_cast<bool>(msg->uint8());
    auto penWidth= msg->uint16();

    auto count= msg->uint64();
    std::vector<QPointF> points;
    points.reserve(static_cast<std::size_t>(count));
    for(unsigned int i= 0; i < count; ++i)
    {
        auto x= msg->real();
        auto y= msg->real();
        points.push_back(QPointF(x, y));
    }
    hash.insert({QStringLiteral("filled"), filled});
    hash.insert({QStringLiteral("closed"), closed});
    hash.insert({QStringLiteral("penLine"), penLine});
    hash.insert({QStringLiteral("penWidth"), penWidth});
    hash.insert({QStringLiteral("points"), QVariant::fromValue(points)});

    return hash;
}

void addPathManager(PathControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::PathController* ctrl) { addPathController(ctrl, msg); });
}

void addTextManager(TextControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::TextController* ctrl) { addTextController(ctrl, msg); });
}

void addCharacterController(const vmap::CharacterItemController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.real(ctrl->side());
    msg.rgb(ctrl->stateColor().rgb());
    msg.uint32(static_cast<quint32>(ctrl->number()));
    msg.uint8(ctrl->playableCharacter());

    auto rect= ctrl->thumnailRect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());

    msg.uint8(ctrl->visionShape());

    rect= ctrl->textRect();
    msg.real(rect.x());
    msg.real(rect.y());
    msg.real(rect.width());
    msg.real(rect.height());

    msg.string32(ctrl->text());
    msg.uint8(ctrl->hasAvatar());

    auto image= ctrl->avatar();
    msg.byteArray32(imageToByteArray(*image));

    msg.string32(ctrl->font().toString());
    msg.real(ctrl->radius());

    auto character= ctrl->character();
    msg.uint8(nullptr == character);
    if(nullptr != character)
    {
        PlayerMessageHelper::writeCharacterIntoMessage(msg, character);
    }
}

void addCharacterManager(CharacterItemControllerManager* ctrl, NetworkMessageWriter& msg)
{
    auto ctrls= ctrl->controllers();
    msg.uint64(static_cast<quint64>(ctrls.size()));
    std::for_each(ctrls.begin(), ctrls.end(),
                  [&msg](const vmap::CharacterItemController* ctrl) { addCharacterController(ctrl, msg); });
}

QHash<QString, QVariant> MessageHelper::readVectorialMapData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    QHash<QString, QVariant> hash;
    hash["uuid"]= msg->string8();
    hash["name"]= msg->string16();
    hash["layer"]= msg->uint8();
    hash["permission"]= msg->uint8();
    hash["bgcolor"]= msg->rgb();
    hash["visibility"]= msg->uint8();
    hash["zindex"]= msg->uint64();
    hash["charactervision"]= msg->uint8();
    hash["gridpattern"]= msg->uint8();
    hash["GridVisibility"]= msg->uint8();
    hash["GridSize"]= msg->uint32();
    hash["GridAbove"]= msg->uint8();
    hash["Collision"]= msg->uint8();
    hash["GridColor"]= msg->rgb();

    hash["sight"]= readSightController(msg);
    hash["rect"]= readRectManager(msg);
    hash["ellipse"]= readEllipseManager(msg);
    hash["line"]= readLineManager(msg);
    hash["image"]= readImageManager(msg);
    hash["path"]= readPathManager(msg);
    hash["text"]= readTextManager(msg);
    hash["character"]= readCharacterManager(msg);

    return hash;
}

void MessageHelper::sendOffVMap(VectorialMapController* ctrl)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string8(ctrl->uuid());

    msg.string16(ctrl->name());
    msg.uint8(static_cast<quint8>(ctrl->layer()));
    msg.uint8(static_cast<quint8>(ctrl->permission()));
    msg.rgb(ctrl->backgroundColor().rgb());
    msg.uint8(static_cast<quint8>(ctrl->visibility()));
    msg.uint64(ctrl->zIndex());
    msg.uint8(ctrl->characterVision());
    msg.uint8(static_cast<quint8>(ctrl->gridPattern()));
    msg.uint8(ctrl->gridVisibility());
    msg.uint32(static_cast<quint32>(ctrl->gridSize()));
    msg.uint8(ctrl->gridAbove());
    msg.uint8(ctrl->collision());
    msg.rgb(ctrl->gridColor().rgb());

    // sight item
    auto sightCtrl= ctrl->sightController();
    addSightController(sightCtrl, msg);

    /*auto rectCtrl= ctrl->rectManager();
    addRectManager(rectCtrl, msg);

    auto ellipseCtrl= ctrl->ellipseManager();
    addEllipseManager(ellipseCtrl, msg);

    auto lineCtrl= ctrl->lineManager();
    addLineManager(lineCtrl, msg);

    auto imageCtrl= ctrl->imageManager();
    addImageManager(imageCtrl, msg);

    auto pathCtrl= ctrl->pathManager();
    addPathManager(pathCtrl, msg);

    auto textCtrl= ctrl->textManager();
    addTextManager(textCtrl, msg);

    auto characterCtrl= ctrl->characterManager();
    addCharacterManager(characterCtrl, msg);*/

    msg.sendToServer();
}

void MessageHelper::sendOffRect(const vmap::RectController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addRectController(ctrl, msg);
    msg.sendToServer();
}

const std::map<QString, QVariant> MessageHelper::readEllipse(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto filled= msg->uint8();
    auto penWidth= msg->uint16();
    auto rx= msg->real();
    auto ry= msg->real();
    hash.insert({"filled", filled});
    hash.insert({"penWidth", penWidth});
    hash.insert({"rx", rx});
    hash.insert({"ry", ry});
    return hash;
}

void MessageHelper::sendOffText(const vmap::TextController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addTextController(ctrl, msg);
    msg.sendToServer();
}

void MessageHelper::sendOffLine(const vmap::LineController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addLineController(ctrl, msg);
    msg.sendToServer();
}

void MessageHelper::sendOffEllispe(const vmap::EllipseController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addEllipseController(ctrl, msg);
    msg.sendToServer();
}
void MessageHelper::sendOffPath(const vmap::PathController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addPathController(ctrl, msg);
    msg.sendToServer();
}
void MessageHelper::sendOffImage(const vmap::ImageController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addImageController(ctrl, msg);
    msg.sendToServer();
}
void MessageHelper::sendOffCharacter(const vmap::CharacterItemController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    msg.uint8(ctrl->itemType());
    addCharacterController(ctrl, msg);
    msg.sendToServer();
}
