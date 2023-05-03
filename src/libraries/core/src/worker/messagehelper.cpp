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
#include "worker/messagehelper.h"

#include <QBuffer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRgb>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/pdfcontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/item_controllers/gridcontroller.h"
#include "controller/item_controllers/imageitemcontroller.h"
#include "controller/item_controllers/linecontroller.h"
#include "controller/item_controllers/pathcontroller.h"
#include "controller/item_controllers/rectcontroller.h"
#include "controller/item_controllers/sightcontroller.h"
#include "controller/item_controllers/textcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "charactersheet/worker/ioworker.h"
#include "data/character.h"
#include "data/player.h"
#include "diceparser/dicealias.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/model/minditemmodel.h"

#include "mindmap/data/packagenode.h"
#include "mindmap/model/linkmodel.h"
#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "model/playermodel.h"
#include "model/vmapitemmodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

#include "worker/iohelper.h"
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

    auto const& aliases= model->aliases();
    int i= 0;
    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::DiceAliasModel);
    msg.uint32(static_cast<quint32>(aliases.size()));
    for(auto const& alias : aliases)
    {
        msg.int64(i);
        msg.string32(alias->pattern());
        msg.string32(alias->command());
        msg.int8(alias->isReplace());
        msg.int8(alias->isEnable());
        msg.string32(alias->comment());
        ++i;
    }
    msg.sendToServer();
}

void MessageHelper::sendOffDiceAliasRemoved(int i)
{
    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::removeDiceAlias);
    msg.int64(i);
    msg.sendToServer();
}
void MessageHelper::sendOffDiceAliasMoved(int i, int j)
{
    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::moveDiceAlias);
    msg.int64(i);
    msg.int64(j);
    msg.sendToServer();
}

void MessageHelper::sendOffOneDiceAlias(const DiceAlias* da, int row)
{
    if(nullptr == da)
        return;

    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::addDiceAlias);
    msg.int64(row);
    msg.string32(da->pattern());
    msg.string32(da->command());
    msg.int8(da->isReplace());
    msg.int8(da->isEnable());
    msg.string32(da->comment());
    msg.sendToServer();
}

void MessageHelper::sendOffCharacterStateRemoved(const QString& id)
{
    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::removeCharacterState);
    msg.string8(id);
    msg.sendToServer();
}

void MessageHelper::sendOffCharacterStateMoved(int i, int j)
{
    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::moveCharacterState);
    msg.int64(i);
    msg.int64(j);
    msg.sendToServer();
}

void MessageHelper::sendOffAllCharacterState(CharacterStateModel* model)
{
    if(nullptr == model)
        return;

    auto const& states= model->statesList();
    quint64 i= 0;
    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::CharactereStateModel);
    msg.uint32(static_cast<quint32>(states.size()));
    for(auto const& state : states)
    {
        msg.string32(state->label());
        msg.rgb(state->color().rgb());
        msg.pixmap(state->pixmap());
        ++i;
    }
    msg.sendToServer();
}

void MessageHelper::sendOffOneCharacterState(const CharacterState* state, int row)
{
    if(nullptr == state)
        return;

    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::addCharacterState);
    msg.int64(row);
    msg.string32(state->label());
    msg.rgb(state->color().rgb());
    msg.pixmap(state->pixmap());
    msg.sendToServer();
}

QString MessageHelper::readMediaId(NetworkMessageReader* msg)
{
    return msg->string8();
}

QHash<QString, QVariant> MessageHelper::readMediaData(NetworkMessageReader* msg)
{
    auto uuid= msg->string8();
    auto name= msg->string32();
    auto contentType= msg->uint8();
    auto ownerId= msg->string32();

    return {{"uuid", uuid}, {"name", name}, {"contentType", contentType}, {"ownerId", ownerId}};
}

void MessageHelper::sendOffMediaControllerBase(const MediaControllerBase* ctrl, NetworkMessageWriter& msg)
{
    msg.string8(ctrl->uuid());
    msg.string32(ctrl->name());
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string32(ctrl->ownerId());
}

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
    sendOffMediaControllerBase(ctrl, msg);
    msg.byteArray32(ctrl->data());

    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readImageData(NetworkMessageReader* msg)
{
    auto hash= readMediaData(msg);
    auto data= msg->byteArray32();
    hash.insert(Core::keys::KEY_DATA, data);
    return hash;
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
    if(property.contains(Core::keys::KEY_COLOR))
    {
        var= QVariant::fromValue(QColor(data.rgb()));
    }
    else if(property.contains(Core::keys::KEY_AVATAR))
    {
        auto array= data.byteArray32();
        QDataStream out(&array, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_7);
        QImage img;
        out >> img;
        var= QVariant::fromValue(img);
    }
    else if(property.contains(Core::keys::KEY_STATE))
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

void MessageHelper::stopSharingSheet(const QString& sheetId, const QString& ctrlId, const QString& characterId)
{
    NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::closeCharacterSheet);
    msg.string8(sheetId);
    msg.string8(ctrlId);
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
    msg.byteArray32(doc.toJson());

    msg.string32(ctrl->qmlCode());

    auto imageModel= ctrl->imageModel();
    QJsonArray array= IOWorker::saveImageModel(imageModel);
    // imageModel->save(array);
    QJsonDocument doc2;
    doc2.setArray(array);
    msg.byteArray32(doc2.toJson());

    auto model= ctrl->model();
    QJsonDocument doc3;
    // doc3.setObject(model->rootSectionData());
    msg.byteArray32(doc3.toJson());

    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readCharacterSheet(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto hash= readMediaData(msg);
    auto id= msg->string8();
    auto name= msg->string8();
    auto characterId= msg->string8();
    auto data= msg->byteArray32();
    auto qml= msg->string32();
    auto imageData= msg->byteArray32();
    auto rootSection= msg->byteArray32();

    hash.insert(Core::keys::KEY_ID, id);
    hash.insert(Core::keys::KEY_NAME, name);
    hash.insert(Core::keys::KEY_CHARACTERID, characterId);
    hash.insert(Core::keys::KEY_DATA, data);
    hash.insert(Core::keys::KEY_QML, qml);
    hash.insert(Core::keys::KEY_IMAGEDATA, imageData);
    hash.insert(Core::keys::KEY_ROOTSECTION, rootSection);

    return hash;
}

void MessageHelper::readUpdateField(CharacterSheetController* ctrl, NetworkMessageReader* msg)
{
    if(nullptr == ctrl || nullptr == msg)
        return;

    auto sheetId= msg->string8();
    auto path= msg->string32();
    auto data= msg->byteArray32();

    QJsonDocument doc= QJsonDocument::fromJson(data);
    auto obj= doc.object();
    ctrl->updateFieldFrom(sheetId, obj, data);
}

// mindmap
void fillUpMessageWithMindmap(NetworkMessageWriter& msg, MindMapController* ctrl)
{
    if(ctrl == nullptr)
        return;

    msg.string8(ctrl->uuid());
    msg.uint8(ctrl->sharingToAll() == Core::SharingPermission::ReadWrite);
    msg.uint64(ctrl->defaultStyleIndex());

    auto nodeModel= dynamic_cast<mindmap::MindItemModel*>(ctrl->itemModel());
    auto imageModel= ctrl->imgModel();

    auto nodes= nodeModel->items(mindmap::MindItem::Type::NodeType);

    msg.uint64(static_cast<quint64>(nodes.size()));
    for(auto i : nodes)
    {
        auto node= dynamic_cast<mindmap::MindNode*>(i);

        if(!node)
            continue;

        msg.string8(node->id());
        msg.string8(node->parentId());
        msg.string32(node->text());
        msg.uint64(node->styleIndex());
        msg.real(node->position().x());
        msg.real(node->position().y());
        msg.string16(node->imageUri());
    }

    auto const& links= nodeModel->items(mindmap::MindItem::Type::LinkType);
    msg.uint64(links.size());
    for(auto i : links)
    {
        auto link= dynamic_cast<mindmap::LinkController*>(i);

        if(!link)
            continue;

        msg.string8(link->id());
        msg.uint8(static_cast<quint8>(link->direction()));
        msg.string8(link->start()->id());
        auto end= link->end();
        msg.string8(end ? end->id() : QString());
        msg.string16(link->text());
    }

    auto packs= nodeModel->items(mindmap::MindItem::Type::PackageType);

    msg.uint64(packs.size());
    for(auto const& i : packs)
    {
        auto pack= dynamic_cast<mindmap::PackageNode*>(i);
        if(!pack)
            continue;

        msg.string16(pack->title());
        auto const& children= pack->children();
        msg.uint32(children.size());
        for(auto const& c : children)
        {
            msg.string8(c->id());
        }
    }

    auto imageInfos= imageModel->imageInfos();
    msg.uint64(imageInfos.size());
    for(auto const& image : imageInfos)
    {
        msg.byteArray32(IOHelper::pixmapToData(image.m_pixmap));
        msg.string8(image.m_id);
        msg.string16(image.m_url.toString());
    }
}

void MessageHelper::sendOffMindmapToAll(MindMapController* ctrl)
{
    if(nullptr == ctrl || !ctrl->localIsOwner())
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    sendOffMediaControllerBase(ctrl, msg);
    fillUpMessageWithMindmap(msg, ctrl);
    msg.sendToServer();
}

void MessageHelper::sendOffMindmapPermissionUpdate(MindMapController* ctrl)
{
    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::UpdateMindMapPermission);
    msg.string8(ctrl->uuid());
    msg.uint8(ctrl->sharingToAll() == Core::SharingPermission::ReadWrite);
    msg.sendToServer();
}

void MessageHelper::sendOffMindmapPermissionUpdateTo(Core::SharingPermission perm, MindMapController* ctrl,
                                                     const QString& id)
{
    QStringList list;
    list << id;
    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::UpdateMindMapPermission);
    msg.setRecipientList(list, NetworkMessage::OneOrMany);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    msg.uint8(perm == Core::SharingPermission::ReadWrite);
    msg.sendToServer();
}

void MessageHelper::openMindmapTo(MindMapController* ctrl, const QString& id)
{
    if(nullptr == ctrl)
        return;

    QStringList recipiants;
    recipiants << id;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    sendOffMediaControllerBase(ctrl, msg);
    fillUpMessageWithMindmap(msg, ctrl);
    msg.sendToServer();
}

void MessageHelper::closeMindmapTo(MindMapController* ctrl, const QString& id)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::CloseMedia);
    msg.setRecipientList({id}, NetworkMessage::OneOrMany);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    msg.sendToServer();
}

QHash<QString, QVariant> MessageHelper::readMindMap(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto hash= readMediaData(msg);

    hash["uuid"]= msg->string8();
    hash["readwrite"]= msg->uint8();
    hash["indexStyle"]= msg->uint64();

    QHash<QString, QVariant> nodes;

    auto size= msg->uint64();

    for(quint64 i= 0; i < size; ++i)
    {
        QHash<QString, QVariant> node;
        node["uuid"]= msg->string8();
        node["parentId"]= msg->string8();
        node["text"]= msg->string32();
        node["index"]= msg->uint64();
        node["x"]= msg->real();
        node["y"]= msg->real();
        node["imageUri"]= msg->string16();

        nodes.insert(QString("node_%1").arg(i), node);
    }

    hash["nodes"]= nodes;

    QHash<QString, QVariant> links;
    size= msg->uint64();

    for(quint64 i= 0; i < size; ++i)
    {
        QHash<QString, QVariant> link;
        link["uuid"]= msg->string8();
        link["direction"]= msg->uint8();
        link["startId"]= msg->string8();
        link["endId"]= msg->string8();
        link["text"]= msg->string16();
        links.insert(QString("link_%1").arg(i), link);
    }
    hash["links"]= links;

    QHash<QString, QVariant> imageInfoData;
    size= msg->uint64();
    for(quint64 i= 0; i < size; ++i)
    {
        QHash<QString, QVariant> imgInfo;
        imgInfo["pixmap"]= IOHelper::dataToPixmap(msg->byteArray32());
        imgInfo["id"]= msg->string8();
        imgInfo["url"]= QUrl(msg->string16());
        imageInfoData.insert(QString("img_%1").arg(i), imgInfo);
    }
    hash["imageInfoData"]= imageInfoData;

    return hash;
}
// end - mindmap

// Shared Notes
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

void MessageHelper::shareNotesTo(const SharedNoteController* ctrl, const QStringList& recipiants)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    sendOffMediaControllerBase(ctrl, msg);
    msg.uint8(static_cast<bool>(ctrl->highligthedSyntax() == SharedNoteController::HighlightedSyntax::MarkDown));
    msg.string32(ctrl->text());
    msg.sendToServer();
}
QHash<QString, QVariant> MessageHelper::readSharedNoteData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto hash= readMediaData(msg);
    auto mkH= static_cast<bool>(msg->uint8());
    auto text= msg->string32();

    hash.insert(Core::vmapkeys::KEY_MARKDOWN, mkH);
    hash.insert(Core::vmapkeys::KEY_TEXT, text);

    return hash;
}
// End - Shared Notes

// Webpage
QHash<QString, QVariant> MessageHelper::readWebPageData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    auto hash= readMediaData(msg);

    auto mode= msg->uint8();
    auto data= msg->string32();

    hash.insert(Core::keys::KEY_MODE, mode);
    hash.insert(Core::keys::KEY_DATA, data);
    hash.insert(Core::keys::KEY_STATE, static_cast<int>(WebpageController::RemoteView));

    return hash;
}

void MessageHelper::shareWebpage(WebpageController* ctrl)
{
    if(nullptr == ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddMedia);
    msg.uint8(static_cast<quint8>(ctrl->contentType()));
    sendOffMediaControllerBase(ctrl, msg);
    auto mode= ctrl->sharingMode();
    msg.uint8(mode);
    if(mode == WebpageController::Html)
        msg.string32(ctrl->html());
    else if(mode == WebpageController::Url)
        msg.string32(ctrl->url().toString());
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
        msg.string32(ctrl->url().toString());

    msg.sendToServer();
}

void MessageHelper::readUpdateWebpage(WebpageController* ctrl, NetworkMessageReader* msg)
{
    qDebug() << "readUpdateWebpage WebPage NetworkMessageReader";
    if(msg == nullptr || nullptr == ctrl)
        return;

    auto mode= static_cast<WebpageController::SharingMode>(msg->uint8());
    auto data= msg->string32();

    if(mode == WebpageController::Html)
        ctrl->setHtml(data);
    else if(mode == WebpageController::Url)
        ctrl->setUrl(QUrl::fromUserInput(data));
}
// end - Webpage

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

    return QHash<QString, QVariant>({{Core::keys::KEY_ID, id}, {Core::keys::KEY_DATA, data}});
}

void addVisualItemController(const vmap::VisualItemController* ctrl, NetworkMessageWriter& msg)
{
    msg.uint8(ctrl->itemType());
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
    auto itemtype= msg->uint8();
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

    return std::map<QString, QVariant>({{Core::vmapkeys::KEY_VISIBLE, visible},
                                        {Core::vmapkeys::KEY_INITIALIZED, initialized},
                                        {Core::vmapkeys::KEY_OPACITY, opacity},
                                        {Core::vmapkeys::KEY_ROTATION, rotation},
                                        {Core::vmapkeys::KEY_LAYER, layer},
                                        {Core::vmapkeys::KEY_POS, pos},
                                        {Core::vmapkeys::KEY_ITEMTYPE, itemtype},
                                        {Core::vmapkeys::KEY_UUID, uuid},
                                        {Core::vmapkeys::KEY_COLOR, rgb},
                                        {Core::vmapkeys::KEY_LOCKED, locked}});
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

    auto count= msg->uint64();
    hash["count"]= count;

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
    for(auto const& pair : qAsConst(singularity))
    {
        auto points= pair.first;
        msg.uint64(static_cast<quint64>(points.size()));
        msg.uint8(static_cast<quint8>(pair.second));

        std::for_each(points.begin(), points.end(),
                      [&msg](const QPointF& p)
                      {
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
    hash.insert({Core::vmapkeys::KEY_FILLED, filled});
    hash.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});
    hash.insert({Core::vmapkeys::KEY_RECT, QRectF(x, y, w, h)});
    return hash;
}

void MessageHelper::addEllipseController(const vmap::EllipseController* ctrl, NetworkMessageWriter& msg)
{
    addVisualItemController(ctrl, msg);
    msg.uint8(ctrl->filled());
    msg.uint16(ctrl->penWidth());
    msg.real(ctrl->rx());
    msg.real(ctrl->ry());
}

void MessageHelper::addLineController(const vmap::LineController* ctrl, NetworkMessageWriter& msg)
{
    if(!ctrl)
        return;
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
    hash.insert({Core::vmapkeys::KEY_ENDPOINT, QPointF(x1, y1)});
    hash.insert({Core::vmapkeys::KEY_STARTPOINT, QPointF(x2, y2)});
    hash.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});
    return hash;
}

void MessageHelper::addImageController(const vmap::ImageItemController* ctrl, NetworkMessageWriter& msg)
{
    if(!ctrl)
        return;
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
    hash.insert({Core::vmapkeys::KEY_RATIO, ratio});
    hash.insert({Core::vmapkeys::KEY_DATA, data});
    hash.insert({Core::vmapkeys::KEY_RECT, QRectF(x, y, w, h)});
    return hash;
}

void MessageHelper::addTextController(const vmap::TextController* ctrl, NetworkMessageWriter& msg)
{
    if(!ctrl)
        return;
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

    x= msg->real();
    y= msg->real();
    w= msg->real();
    h= msg->real();
    QRectF borderRect(x, y, w, h);

    hash.insert({Core::vmapkeys::KEY_TEXT, text});
    hash.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});
    hash.insert({Core::vmapkeys::KEY_BORDER, border});
    hash.insert({Core::vmapkeys::KEY_BORDERRECT, borderRect});
    hash.insert({Core::vmapkeys::KEY_TEXTPOS, textPos});
    hash.insert({Core::vmapkeys::KEY_TEXTRECT, textRect});

    return hash;
}

void MessageHelper::addPathController(const vmap::PathController* ctrl, NetworkMessageWriter& msg)
{
    if(!ctrl)
        return;

    addVisualItemController(ctrl, msg);
    msg.uint8(ctrl->filled());
    msg.uint8(ctrl->closed());
    msg.uint8(ctrl->penLine());
    msg.uint16(ctrl->penWidth());
    auto const& points= ctrl->points();
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
    hash.insert({Core::vmapkeys::KEY_FILLED, filled});
    hash.insert({Core::vmapkeys::KEY_CLOSED, closed});
    hash.insert({Core::vmapkeys::KEY_PENLINE, penLine});
    hash.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});
    hash.insert({Core::vmapkeys::KEY_POINTS, QVariant::fromValue(points)});

    return hash;
}

const std::map<QString, QVariant> MessageHelper::readCharacter(NetworkMessageReader* msg)
{
    auto hash= readVisualItemController(msg);
    auto side= msg->real();
    auto stateColor= QColor(msg->rgb());
    auto number= msg->uint32();
    auto playableCharacter= static_cast<bool>(msg->uint8());

    auto x= msg->real();
    auto y= msg->real();
    auto w= msg->real();
    auto h= msg->real();
    QRectF rect(x, y, w, h);

    auto text= msg->string32();
    auto hasAvatar= static_cast<bool>(msg->uint8());

    auto img= msg->byteArray32();
    auto font= QFont(msg->string32());
    auto radius= msg->real();

    auto count= msg->uint64();
    std::vector<QPointF> points;
    points.reserve(static_cast<std::size_t>(count));
    for(unsigned int i= 0; i < count; ++i)
    {
        auto x= msg->real();
        auto y= msg->real();
        points.push_back(QPointF(x, y));
    }
    hash.insert({Core::vmapkeys::KEY_SIDE, side});
    hash.insert({Core::vmapkeys::KEY_STATECOLOR, stateColor});
    hash.insert({Core::vmapkeys::KEY_NUMBER, number});
    hash.insert({Core::vmapkeys::KEY_PLAYABLECHARACTER, playableCharacter});
    hash.insert({Core::vmapkeys::KEY_RECT, QVariant::fromValue(rect)});
    hash.insert({Core::vmapkeys::KEY_TEXT, text});
    hash.insert({Core::vmapkeys::KEY_HASAVATAR, hasAvatar});
    hash.insert({Core::vmapkeys::KEY_IMAGE, img});
    hash.insert({Core::vmapkeys::KEY_FONT, font});
    hash.insert({Core::vmapkeys::KEY_RADIUS, radius});
    hash.insert({Core::vmapkeys::KEY_COUNT, count});
    hash.insert({Core::vmapkeys::KEY_POINTS, QVariant::fromValue(points)});

    return hash;
}

void MessageHelper::addCharacterController(const vmap::CharacterItemController* ctrl, NetworkMessageWriter& msg)
{
    if(!ctrl)
        return;
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

QHash<QString, QVariant> MessageHelper::readVectorialMapData(NetworkMessageReader* msg)
{
    if(nullptr == msg)
        return {};

    QHash<QString, QVariant> hash;
    hash[Core::keys::KEY_UUID]= msg->string8();
    hash[Core::keys::KEY_NAME]= msg->string16();
    hash[Core::keys::KEY_LAYER]= msg->uint8();
    hash[Core::keys::KEY_PERMISSION]= msg->uint8();
    hash[Core::keys::KEY_BGCOLOR]= QColor(msg->rgb());
    hash[Core::keys::KEY_VISIBILITY]= msg->uint8();
    hash[Core::keys::KEY_ZINDEX]= msg->uint64();
    hash[Core::keys::KEY_CHARACTERVISION]= msg->uint8();
    hash[Core::keys::KEY_GRIDPATTERN]= msg->uint8();
    hash[Core::keys::KEY_GRIDVISIBILITY]= msg->uint8();
    hash[Core::keys::KEY_GRIDSIZE]= msg->uint32();
    hash[Core::keys::KEY_GRIDSCALE]= msg->real();
    hash[Core::keys::KEY_GRIDABOVE]= msg->uint8();
    hash[Core::keys::KEY_COLLISION]= msg->uint8();
    hash[Core::keys::KEY_UNIT]= msg->uint8();
    hash[Core::keys::KEY_GRIDCOLOR]= QColor(msg->rgb());

    hash[Core::keys::KEY_SIGHT]= readSightController(msg);

    auto itemCount= msg->uint64();
    QHash<QString, QVariant> items;
    for(quint64 i= 0; i < itemCount; ++i)
    {
        auto restore= msg->pos();
        auto type= static_cast<vmap::VisualItemController::ItemType>(msg->uint8());
        msg->resetToPos(restore);
        std::map<QString, QVariant> map;
        switch(type)
        {
        case vmap::VisualItemController::LINE:
            map= readLine(msg);
            break;
        case vmap::VisualItemController::PATH:
            map= readPath(msg);
            break;
        case vmap::VisualItemController::RECT:
            map= readRect(msg);
            break;
        case vmap::VisualItemController::TEXT:
            map= readText(msg);
            break;
        case vmap::VisualItemController::ELLIPSE:
            map= readEllipse(msg);
            break;
        case vmap::VisualItemController::CHARACTER:
            map= readCharacter(msg);
            break;
        case vmap::VisualItemController::IMAGE:
            map= readImage(msg);
            break;
        default:
            break;
        }
        QHash<QString, QVariant> qvals(map.begin(), map.end());
        items.insert(QString("Item_%1").arg(i), qvals);
    }
    hash[Core::keys::KEY_ITEMS]= items;

    return hash;
}

void MessageHelper::convertVisualItemCtrlAndAdd(vmap::VisualItemController* ctrl, NetworkMessageWriter& msg)
{
    switch(ctrl->itemType())
    {
    case vmap::VisualItemController::LINE:
        addLineController(dynamic_cast<vmap::LineController*>(ctrl), msg);
        break;
    case vmap::VisualItemController::PATH:
        addPathController(dynamic_cast<vmap::PathController*>(ctrl), msg);
        break;
    case vmap::VisualItemController::RECT:
        addRectController(dynamic_cast<vmap::RectController*>(ctrl), msg);
        break;
    case vmap::VisualItemController::TEXT:
        addTextController(dynamic_cast<vmap::TextController*>(ctrl), msg);
        break;
    case vmap::VisualItemController::ELLIPSE:
        addEllipseController(dynamic_cast<vmap::EllipseController*>(ctrl), msg);
        break;
    case vmap::VisualItemController::CHARACTER:
        addCharacterController(dynamic_cast<vmap::CharacterItemController*>(ctrl), msg);
        break;
    case vmap::VisualItemController::IMAGE:
        addImageController(dynamic_cast<vmap::ImageItemController*>(ctrl), msg);
        break;
    default:
        break;
    }
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
    msg.real(ctrl->gridScale());
    msg.uint8(ctrl->gridAbove());
    msg.uint8(ctrl->collision());
    msg.uint8(static_cast<quint8>(ctrl->scaleUnit()));
    msg.rgb(ctrl->gridColor().rgb());

    // sight item
    auto sightCtrl= ctrl->sightController();
    addSightController(sightCtrl, msg);

    auto model= ctrl->model();
    auto data= model->items();

    msg.uint64(data.size());

    std::for_each(data.begin(), data.end(),
                  [&](vmap::VisualItemController* ctrl) { convertVisualItemCtrlAndAdd(ctrl, msg); });

    msg.sendToServer();
}

void MessageHelper::sendOffRect(const vmap::RectController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
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
    hash.insert({Core::vmapkeys::KEY_FILLED, filled});
    hash.insert({Core::vmapkeys::KEY_PENWIDTH, penWidth});
    hash.insert({Core::vmapkeys::KEY_RX, rx});
    hash.insert({Core::vmapkeys::KEY_RY, ry});
    return hash;
}

void MessageHelper::sendOffText(const vmap::TextController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
    addTextController(ctrl, msg);
    msg.sendToServer();
}

void MessageHelper::sendOffLine(const vmap::LineController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
    addLineController(ctrl, msg);
    msg.sendToServer();
}

void MessageHelper::sendOffEllispe(const vmap::EllipseController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
    addEllipseController(ctrl, msg);
    msg.sendToServer();
}
void MessageHelper::sendOffPath(const vmap::PathController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
    addPathController(ctrl, msg);
    msg.sendToServer();
}
void MessageHelper::sendOffImage(const vmap::ImageItemController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
    addImageController(ctrl, msg);
    msg.sendToServer();
}
void MessageHelper::sendOffCharacter(const vmap::CharacterItemController* ctrl, const QString& mapId)
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::AddItem);
    msg.string8(mapId);
    // msg.uint8(ctrl->itemType());
    addCharacterController(ctrl, msg);
    msg.sendToServer();
}

void MessageHelper::readAddSubImage(mindmap::ImageModel* model, NetworkMessageReader* msg)
{
    if(!msg || !model)
        return;
    auto pix= IOHelper::dataToPixmap(msg->byteArray32());
    auto id= msg->string8();
    auto url= QUrl(msg->string16());
    if(pix.isNull())
        return;
    model->insertPixmap(id, pix, url, true);
}

void MessageHelper::readRemoveSubImage(mindmap::ImageModel* model, NetworkMessageReader* msg)
{
    if(!msg || !model)
        return;
    model->removePixmap(msg->string8());
}

void MessageHelper::sendOffImageInfo(const mindmap::ImageInfo& info, MediaControllerBase* ctrl)
{
    if(!ctrl || info.m_pixmap.isNull())
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddSubImage);
    msg.int8(static_cast<int>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    msg.byteArray32(IOHelper::pixmapToData(info.m_pixmap));
    msg.string8(info.m_id);
    msg.string16(info.m_url.toString());
    msg.sendToServer();
}

void MessageHelper::sendOffRemoveImageInfo(const QString& id, MediaControllerBase* ctrl)
{
    if(!ctrl)
        return;

    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::AddSubImage);
    msg.int8(static_cast<int>(ctrl->contentType()));
    msg.string8(ctrl->uuid());
    msg.string8(id);
    msg.sendToServer();
}

void buildAddLinkMessage(NetworkMessageWriter& msg, QList<mindmap::LinkController*> links)
{
    msg.uint64(links.size());
    for(auto link : links)
    {
        Q_ASSERT(link);
        msg.string8(link->id());
        msg.string32(link->text());
        msg.string8(link->start()->id());
        auto end= link->end();
        msg.string8(end ? end->id() : QString());
        msg.uint8(static_cast<quint8>(link->direction()));
    }
}

void buildAddNodeMessage(NetworkMessageWriter& msg, QList<mindmap::MindNode*> nodes)
{
    msg.uint64(nodes.size());
    for(auto node : nodes)
    {

        msg.string8(node->id());
        auto f= node->position();
        msg.real(f.x());
        msg.real(f.y());
        msg.string32(node->text());
        msg.string8(node->imageUri());
        msg.string8(node->parentId());
        msg.uint64(node->styleIndex());

        auto subs= node->subLinks();
        msg.uint64(static_cast<quint64>(subs.size()));
        for(const auto& link : subs)
        {
            msg.string8(link->id());
        }
    }
}
void readMindMapLink(MindMapController* ctrl, NetworkMessageReader* msg)
{
    if(!ctrl || !msg)
        return;

    QList<mindmap::LinkController*> links;

    auto count= msg->uint64();

    for(quint64 i= 0; i < count; ++i)
    {
        auto id= msg->string8();
        auto text= msg->string32();
        auto idStart= msg->string8();
        auto idEnd= msg->string8();

        auto dir= static_cast<mindmap::LinkController::Direction>(msg->uint8());

        auto start= ctrl->nodeFromId(idStart);
        auto end= ctrl->nodeFromId(idEnd);

        if(!start || !end)
            continue;

        auto link= new mindmap::LinkController();
        link->setId(id);
        link->setText(text);
        link->setDirection(dir);
        link->setStart(start);
        link->setEnd(end);
        links.append(link);
    }

    ctrl->addLink(links, true);
}

void readAddMindMapNode(MindMapController* ctrl, NetworkMessageReader* msg)
{
    if(!ctrl || !msg)
        return;

    QList<mindmap::MindNode*> nodes;
    QList<QPair<QString, QString>> parentIdList;

    auto count= msg->uint64();

    for(quint64 i= 0; i < count; ++i)
    {

        auto id= msg->string8();
        auto x= msg->real();
        auto y= msg->real();
        auto text= msg->string32();
        auto uri= msg->string8();
        auto parentId= msg->string8();
        auto indx= msg->uint64();

        auto count= msg->uint64();
        QStringList list;
        for(quint64 i= 0; i < count; ++i)
        {
            list << msg->string8();
        }

        mindmap::MindNode* node= new mindmap::MindNode(ctrl);
        node->setPosition(QPointF(x, y));
        node->setText(text);
        node->setId(id);
        node->setImageUri(uri);
        if(!parentId.isEmpty())
        {
            auto parent= ctrl->nodeFromId(parentId);
            node->setParentNode(parent);
        }
        node->setStyleIndex(indx);
        ctrl->addNode({node}, true);

        if(!parentId.isNull())
            parentIdList.append({parentId, id});
        // ctrl->createLink(parentId, id);

        for(const auto& tmp : qAsConst(list))
            parentIdList.append({id, tmp});
        //  ctrl->createLink(id, tmp);
    }
    ctrl->addNode(nodes, true);
}
void MessageHelper::readMindMapAddItem(MindMapController* ctrl, NetworkMessageReader* msg)
{
    readAddMindMapNode(ctrl, msg);
    readMindMapLink(ctrl, msg);
}

void MessageHelper::buildAddItemMessage(NetworkMessageWriter& msg, const QList<mindmap::MindNode*>& nodes,
                                        const QList<mindmap::LinkController*>& links)
{
    buildAddNodeMessage(msg, nodes);
    buildAddLinkMessage(msg, links);
}
void MessageHelper::buildRemoveItemMessage(NetworkMessageWriter& msg, const QStringList& nodes,
                                           const QStringList& links)
{

    auto func= [&msg](const QString& id) { msg.string8(id); };
    msg.uint64(nodes.size());
    std::for_each(std::begin(nodes), std::end(nodes), func);

    msg.uint64(links.size());
    std::for_each(std::begin(links), std::end(links), func);
}

void MessageHelper::readMindMapRemoveMessage(MindMapController* ctrl, NetworkMessageReader* msg)
{
    auto nodes= readIdList(*msg);
    auto links= readIdList(*msg);

    ctrl->removeNode(nodes);
    ctrl->removeLink(links);
}

QStringList MessageHelper::readIdList(NetworkMessageReader& data)
{
    QStringList res;
    auto count= data.uint64();

    for(quint64 i= 0; i < count; ++i)
    {
        if(data.isValid())
            res << data.string8();
    }

    return res;
}

void MessageHelper::fetchCharacterStatesFromNetwork(NetworkMessageReader* msg, CharacterStateModel* model)
{
    model->clear();
    auto size= msg->uint32();
    for(quint32 i= 0; i < size; ++i)
    {
        msg->uint64();

        auto label= msg->string32();
        auto color= msg->rgb();
        auto pixmap= msg->pixmap();
        CharacterState state;
        state.setLabel(label);
        state.setColor(color);
        state.setPixmap(pixmap);
        model->appendState(std::move(state));
    }
}

void MessageHelper::fetchDiceAliasFromNetwork(NetworkMessageReader* msg, QList<DiceAlias*>* list)
{
    qDeleteAll(*list);
    list->clear();
    auto size= msg->uint32();
    for(quint32 i= 0; i < size; ++i)
    {
        msg->uint64();

        auto pattern= msg->string32();
        auto command= msg->string32();
        auto replace= msg->int8();
        auto enable= msg->int8();
        auto comment= msg->string32();

        list->append(new DiceAlias(pattern, command, comment, replace, enable));
    }
}

/*void MessageHelper::AddCharacterStateFromNetwork(NetworkMessageReader* msg, QList<DiceAlias*>* list)
{
    CharacterState state;

    quint64 id= msg->uint64();
    state.setLabel(msg->string32());
    state.setColor(msg->rgb());
    state.setPixmap(msg->pixmap());

    model->appendState(std::move(state));
}
void MessageHelper::moveStateFromNetwork(NetworkMessageReader* msg, CharacterStateModel* model)
{
    int from= msg->int64();
    int to= msg->int64();

    model->moveState(from, to);
}
void MessageHelper::removeState(NetworkMessageReader* msg, CharacterStateModel* model)
{
    int pos= static_cast<int>(msg->int64());
    model->removeStateAt(pos);
}
*/

void MessageHelper::sendOffPlaySong(const QString& songName, qint64 time, int player)
{
    qDebug() << "sendOffPlaySong: " << songName << player;
    NetworkMessageWriter msg(NetMsg::MusicCategory, NetMsg::NewSong);
    msg.uint8(static_cast<quint8>(player));
    msg.string32(songName);
    msg.int64(time);
    msg.sendToServer();
}
void MessageHelper::sendOffMusicPlayerOrder(NetMsg::Action netAction, int player)
{
    qDebug() << "sendOffMusicPlayerOrder: " << netAction << player;
    NetworkMessageWriter message(NetMsg::MusicCategory, netAction);
    message.uint8(static_cast<quint8>(player));
    message.sendToServer();
}
void MessageHelper::sendOffTime(qint64 time, int player)
{
    qDebug() << "sendOffTime: " << time << player;
    NetworkMessageWriter message(NetMsg::MusicCategory, NetMsg::ChangePositionSong);
    message.uint8(static_cast<quint8>(player));
    message.int64(time);
    message.sendToServer();
}
