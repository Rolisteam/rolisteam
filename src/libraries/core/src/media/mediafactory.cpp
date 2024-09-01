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
#include "media/mediafactory.h"

#include <QFileInfo>
#include <QVariant>

#include "controller/view_controller/charactersheetcontroller.h"
#include "controller/view_controller/imagecontroller.h"
#include "controller/view_controller/mindmapcontroller.h"
#include "controller/view_controller/notecontroller.h"
#include "controller/view_controller/pdfcontroller.h"

#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "controller/view_controller/webpagecontroller.h"

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "charactersheet/worker/ioworker.h"

#include "mindmap/data/mindnode.h"
#include "mindmap/data/minditem.h"

#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/minditem.h"
#include "mindmap/data/mindnode.h"
#include "utils/iohelper.h"
#include "mindmap/model/imagemodel.h"

#include "network/networkmessagereader.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/utilshelper.h"
#include "worker/vectorialmapmessagehelper.h"

namespace Media
{

namespace
{
ImageController* image(const QString& uuid, const QHash<QString, QVariant>& map)
{
    QByteArray serializedData= map.value(Core::keys::KEY_SERIALIZED).toByteArray();
    QByteArray pix= map.value(Core::keys::KEY_DATA).toByteArray();
    QUrl url= map.value(Core::keys::KEY_URL).toUrl();
    QString name= map.value(Core::keys::KEY_NAME).toString();

    return new ImageController(uuid, name, url, pix);
}

CharacterSheetController* sheetCtrl(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto path= params.value(Core::keys::KEY_PATH).toString();

    CharacterSheetController* sheetCtrl= new CharacterSheetController(uuid, path);

    namespace ck= Core::keys;
    namespace hu= helper::utils;
    using std::placeholders::_1;

    std::map<QString, QVariant> sheetData;

    for(auto it= std::begin(params); it != std::end(params); ++it)
    {
        sheetData.insert({it.key(), it.value()});
    }
    hu::setParamIfAny<QString>(ck::KEY_NAME, sheetData, std::bind(&CharacterSheetController::setName, sheetCtrl, _1));
    hu::setParamIfAny<QString>(ck::KEY_QML, sheetData, std::bind(&CharacterSheetController::setQmlCode, sheetCtrl, _1));
    hu::setParamIfAny<QString>(ck::KEY_GMID, sheetData,
                               std::bind(&CharacterSheetController::setGameMasterId, sheetCtrl, _1));
    hu::setParamIfAny<QString>(ck::KEY_OWNERID, sheetData,
                               std::bind(&CharacterSheetController::setGameMasterId, sheetCtrl, _1));
    hu::setParamIfAny<QByteArray>(ck::KEY_IMAGEDATA, sheetData,
                                  [sheetCtrl](const QByteArray& array)
                                  {
                                      auto imgModel= sheetCtrl->imageModel();
                                      IOWorker::fetchImageModel(imgModel, IOHelper::byteArrayToJsonArray(array));
                                  });
    hu::setParamIfAny<QByteArray>(ck::KEY_ROOTSECTION, sheetData,
                                  [sheetCtrl](const QByteArray& array)
                                  {
                                      auto sheetModel= sheetCtrl->model();
                                      // sheetModel->setRootSection(IOHelper::byteArrayToJsonObj(array));
                                  });
    hu::setParamIfAny<QByteArray>(
        ck::KEY_CHARACTERDATA, sheetData,
        [sheetCtrl, sheetData](const QByteArray& array)
        {
            hu::setParamIfAny<QString>(
                ck::KEY_CHARACTERID, sheetData,
                [sheetCtrl, array](const QString& characterId)
                {
                    sheetCtrl->addCharacterSheet(IOHelper::textByteArrayToJsonObj(array), characterId);
                });
        });
    hu::setParamIfAny<QByteArray>(ck::KEY_SERIALIZED, sheetData,
                                  [sheetCtrl](const QByteArray& array)
                                  { IOHelper::readCharacterSheetController(sheetCtrl, array); });
    if(!params.contains(ck::KEY_SERIALIZED))
    {
        hu::setParamIfAny<QString>(ck::KEY_PATH, sheetData, [sheetCtrl](const QString& path){
            IOHelper::readCharacterSheetController(sheetCtrl, utils::IOHelper::loadFile(path));
        });
    }
    return sheetCtrl;
}

VectorialMapController* vectorialMap(const QString& uuid, const QHash<QString, QVariant>& params, bool isRemote= false)
{
    auto vmapCtrl= new VectorialMapController(uuid);

    QByteArray serializedData= params.value(Core::keys::KEY_SERIALIZED).toByteArray();

    if(!params.isEmpty() /*&& serializedData.isEmpty()*/)
    {
        namespace ck= Core::keys;
        namespace hu= helper::utils;
        using std::placeholders::_1;

        std::map<QString, QVariant> mapData;

        for(auto it= std::begin(params); it != std::end(params); ++it)
        {
            mapData.insert({it.key(), it.value()});
        }

        // clang-format off
        hu::setParamIfAny<QString>(ck::KEY_NAME, mapData, std::bind(&VectorialMapController::setName, vmapCtrl, _1));
        hu::setParamIfAny<Core::Layer>(ck::KEY_LAYER, mapData, std::bind(&VectorialMapController::setLayer, vmapCtrl, _1));
        hu::setParamIfAny<Core::PermissionMode>(ck::KEY_PERMISSION, mapData, std::bind(&VectorialMapController::setPermission, vmapCtrl, _1));
        hu::setParamIfAny<QColor>(ck::KEY_BGCOLOR, mapData, std::bind(&VectorialMapController::setBackgroundColor, vmapCtrl, _1));
        hu::setParamIfAny<Core::VisibilityMode>(ck::KEY_VISIBILITY, mapData, std::bind(&VectorialMapController::setVisibility, vmapCtrl, _1));
        hu::setParamIfAny<int>(ck::KEY_ZINDEX, mapData, std::bind(&VectorialMapController::setZindex, vmapCtrl, _1));
        hu::setParamIfAny<bool>(ck::KEY_CHARACTERVISION, mapData, std::bind(&VectorialMapController::setCharacterVision, vmapCtrl, _1));
        hu::setParamIfAny<Core::GridPattern>(ck::KEY_GRIDPATTERN, mapData, std::bind(&VectorialMapController::setGridPattern, vmapCtrl, _1));
        hu::setParamIfAny<bool>(ck::KEY_GRIDVISIBILITY, mapData, std::bind(&VectorialMapController::setGridVisibility, vmapCtrl, _1));
        hu::setParamIfAny<int>(ck::KEY_GRIDSIZE, mapData, std::bind(&VectorialMapController::setGridSize, vmapCtrl, _1));
        hu::setParamIfAny<qreal>(ck::KEY_GRIDSCALE, mapData, std::bind(&VectorialMapController::setGridScale, vmapCtrl, _1));
        hu::setParamIfAny<bool>(ck::KEY_GRIDABOVE, mapData, std::bind(&VectorialMapController::setGridAbove, vmapCtrl, _1));
        hu::setParamIfAny<Core::ScaleUnit>(ck::KEY_UNIT, mapData, std::bind(&VectorialMapController::setScaleUnit, vmapCtrl, _1));
        hu::setParamIfAny<QColor>(ck::KEY_GRIDCOLOR, mapData, std::bind(&VectorialMapController::setGridColor, vmapCtrl, _1));
        hu::setParamIfAny<bool>(Core::vmapkeys::KEY_LOCKED, mapData, std::bind(&VectorialMapController::setGridVisibility, vmapCtrl, _1));
        auto sightCtrl = vmapCtrl->sightController();
        auto sightParam = params.value(Core::keys::KEY_SIGHT).toHash();
        VectorialMapMessageHelper::fetchSightController(sightCtrl, sightParam);
        // clang-format on

        vmapCtrl->setIdle(true);

        auto items= params.value(ck::KEY_ITEMS).toHash();
        VectorialMapMessageHelper::fetchModelFromMap(items, vmapCtrl, isRemote);
    }

    if(!serializedData.isEmpty())
        VectorialMapMessageHelper::readVectorialMapController(vmapCtrl, serializedData);

    return vmapCtrl;
}

PdfController* pdf(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto ownerid= params.value(Core::keys::KEY_OWNERID).toString();
    auto array= params.value(Core::keys::KEY_DATA).toByteArray();
    auto path= QUrl::fromUserInput(params.value(Core::keys::KEY_PATH).toString());
    auto seriaziledData= params.value(Core::keys::KEY_SERIALIZED).toByteArray();
    auto pdfCtrl= new PdfController(uuid, path, array.isEmpty() ? seriaziledData : array);
    pdfCtrl->setOwnerId(ownerid);
    if(!seriaziledData.isEmpty())
        IOHelper::readPdfController(pdfCtrl, seriaziledData);
    return pdfCtrl;
}

NoteController* note(const QString& uuid, const QHash<QString, QVariant>& map)
{
    auto name= map.value(Core::keys::KEY_NAME).toString();
    auto url= map.value(Core::keys::KEY_URL).toUrl();

    auto ownerid= map.value(Core::keys::KEY_OWNERID).toString();
    auto serializedData= map.value(Core::keys::KEY_SERIALIZED).toByteArray();

    auto noteCtrl= new NoteController(uuid);

    if(!name.isEmpty())
        noteCtrl->setName(name);
    noteCtrl->setOwnerId(ownerid);
    noteCtrl->setUrl(url);

    if(!serializedData.isEmpty())
        IOHelper::readNoteController(noteCtrl, serializedData);

    return noteCtrl;
}
MindMapController* mindmap(const QString& uuid, const QHash<QString, QVariant>& map)
{
    auto name= map.value(Core::keys::KEY_NAME).toString();
    auto url= map.value(Core::keys::KEY_URL).toUrl();

    auto ownerid= map.value(Core::keys::KEY_OWNERID).toString();
    auto serializedData= map.value(Core::keys::KEY_SERIALIZED).toByteArray();

    auto mindmapCtrl= new MindMapController(uuid);

    if(map.contains("indexStyle"))
        mindmapCtrl->setDefaultStyleIndex(map.value("indexStyle").toBool());

    QHash<QString, mindmap::MindNode*> data;
    if(map.contains("nodes"))
    {
        QHash<QString, QVariant> nodes= map.value("nodes").toHash();

        auto model= dynamic_cast<mindmap::MindItemModel*>(mindmapCtrl->itemModel());
        QHash<QString, QString> parentData;
        QList<mindmap::MindItem*> nodesList;
        for(const auto& var : nodes)
        {
            auto node= new mindmap::MindNode();
            auto nodeV= var.toHash();
            node->setId(nodeV["uuid"].toString());
            node->setText(nodeV["text"].toString());
            node->setImageUri(nodeV["imageUri"].toString());
            QPointF pos(nodeV["x"].toReal(), nodeV["y"].toReal());
            node->setPosition(pos);
            node->setStyleIndex(nodeV["index"].toInt());
            nodesList.append(node);
            data.insert(node->id(), node);
            parentData.insert(node->id(), nodeV["parentId"].toString());
        }
        model->appendItem(nodesList);

        for(const auto& key : data.keys())
        {
            auto node= data.value(key);
            auto parentId= parentData.value(key);

            if(!parentId.isEmpty())
            {
                auto parent= data.value(parentId);
                if(parent)
                    node->setParentNode(parent);
            }
        }
    }

    if(map.contains("links"))
    {
        QHash<QString, QVariant> links= map.value("links").toHash();

        auto model= dynamic_cast<mindmap::MindItemModel*>(mindmapCtrl->itemModel());

        QList<mindmap::MindItem*> linkList;
        for(const auto& var : links)
        {
            auto link= new mindmap::LinkController();
            auto linkV= var.toHash();
            link->setId(linkV["uuid"].toString());
            link->setText(linkV["text"].toString());
            link->setDirection(static_cast<mindmap::LinkController::Direction>(linkV["direction"].toInt()));
            auto startId= linkV["startId"].toString();
            auto endId= linkV["endId"].toString();

            Q_ASSERT(data.contains(endId) && data.contains(startId));

            link->setStart(data.value(startId));
            link->setEnd(data.value(endId));

            linkList << link;
        }
        model->appendItem(linkList);
    }

    if(map.contains("packages"))
    {
        QHash<QString, QVariant> links= map.value("links").toHash();

        auto model= dynamic_cast<mindmap::MindItemModel*>(mindmapCtrl->itemModel());

        QList<mindmap::MindItem*> linkList;
        for(const auto& var : links)
        {
            auto pack= new mindmap::PackageNode();
            auto packV= var.toHash();
            pack->setId(packV["uuid"].toString());
            pack->setTitle(packV["title"].toString());
            auto childrenId= packV["children"].toStringList();
            for(auto const& id : childrenId)
            {
                pack->addChild(model->positionItem(id));
            }
        }
        model->appendItem(linkList);
    }

    if(map.contains("imageInfoData"))
    {
        QHash<QString, QVariant> imgInfos= map.value("imageInfoData").toHash();
        auto model= mindmapCtrl->imgModel();
        for(const auto& var : imgInfos)
        {
            auto img= var.toHash();
            auto pix= img["pixmap"].value<QPixmap>();
            auto id= img["id"].toString();
            auto url= QUrl(img["url"].toString());
            model->insertPixmap(id, pix, url);
        }
    }

    if(!name.isEmpty())
        mindmapCtrl->setName(name);

    mindmapCtrl->setOwnerId(ownerid);
    mindmapCtrl->setUrl(url);

    if(!serializedData.isEmpty())
        IOHelper::readMindmapController(mindmapCtrl, serializedData);

    return mindmapCtrl;
}
SharedNoteController* sharedNote(const QString& uuid, const QHash<QString, QVariant>& params, const QString& localId)
{
    auto ownerId= params.value(Core::keys::KEY_OWNERID).toString();
    auto b= params.value(Core::keys::KEY_MARKDOWN, false).toBool();
    auto url= params.value(Core::keys::KEY_URL).toUrl();
    auto text= params.value(Core::keys::KEY_TEXT).toString();
    auto noteCtrl= new SharedNoteController(ownerId, localId, uuid);

    if(!url.isEmpty())
    {
        noteCtrl->setUrl(url);
        QFileInfo info(url.toLocalFile());
        noteCtrl->setName(info.fileName());
        b= url.toLocalFile().endsWith(".md");
    }

    noteCtrl->setHighligthedSyntax(b ? SharedNoteController::HighlightedSyntax::MarkDown :
                                       SharedNoteController::HighlightedSyntax::None);
    if(!text.isEmpty())
        noteCtrl->setText(text);

    return noteCtrl;
}
WebpageController* webPage(const QString& uuid, const QHash<QString, QVariant>& params)
{
    QByteArray serializedData= params.value(Core::keys::KEY_SERIALIZED).toByteArray();
    auto webCtrl= new WebpageController(uuid);

    if(!serializedData.isEmpty())
        IOHelper::readWebpageController(webCtrl, serializedData);
    else
    {
        webCtrl->setOwnerId(params.value(Core::keys::KEY_OWNERID).toString());

        if(params.contains(Core::keys::KEY_MODE))
        {
            auto mode= static_cast<WebpageController::SharingMode>(params.value(Core::keys::KEY_MODE).toInt());
            auto data= params.value(Core::keys::KEY_DATA).toString();

            if(mode == WebpageController::Url)
                webCtrl->setUrl(data);
            else if(mode == WebpageController::Html)
                webCtrl->setHtml(data);
        }
        if(params.contains(Core::keys::KEY_NAME))
        {
            webCtrl->setName(params.value(Core::keys::KEY_NAME).toString());
        }
        if(params.contains(Core::keys::KEY_STATE))
        {
            webCtrl->setState(static_cast<WebpageController::State>(params.value(Core::keys::KEY_STATE).toInt()));
        }
        if(params.contains(Core::keys::KEY_PATH))
        {
            auto url= params.value(Core::keys::KEY_PATH).toString();
            webCtrl->setPageUrl(QUrl::fromUserInput(url));
        }
        if(params.contains(Core::keys::KEY_URL))
        {
            auto url= params.value(Core::keys::KEY_URL).toString();
            webCtrl->setUrl(QUrl::fromUserInput(url));
        }
    }

    return webCtrl;
}
} // namespace

QString MediaFactory::m_localId= "";

MediaControllerBase* MediaFactory::createLocalMedia(const QString& uuid, Core::ContentType type,
                                                    const std::map<QString, QVariant>& map, const QColor& localColor,
                                                    bool localIsGM)
{
    QHash<QString, QVariant> params(map.begin(), map.end());
    using C= Core::ContentType;
    MediaControllerBase* base= nullptr;

    switch(type)
    {
    case C::VECTORIALMAP:
        base= vectorialMap(uuid, params);
        break;
    case C::PICTURE:
        // case C::ONLINEPICTURE:
        base= image(uuid, params);
        break;
    case C::NOTES:
        base= note(uuid, params);
        break;
    case C::CHARACTERSHEET:
        base= sheetCtrl(uuid, params);
        break;
    case C::SHAREDNOTE:
        base= sharedNote(uuid, params, m_localId);
        break;
    case C::PDF:
        base= pdf(uuid, params);
        break;
    case C::WEBVIEW:
        base= webPage(uuid, params);
        break;
    case C::MINDMAP:
        base= mindmap(uuid, params);
        break;
    default:
        break;
    }
    Q_ASSERT(base != nullptr);
    base->setLocalGM(localIsGM);
    base->setLocalId(m_localId);
    base->setOwnerId(m_localId);
    base->setLocalColor(localColor);
    return base;
}

MediaControllerBase* MediaFactory::createRemoteMedia(Core::ContentType type, NetworkMessageReader* msg,
                                                     const QColor& localColor, bool localIsGM)
{
    using C= Core::ContentType;
    MediaControllerBase* base= nullptr;

    QString uuid;

    switch(type)
    {
    case C::VECTORIALMAP:
    {
        auto data= MessageHelper::readVectorialMapData(msg);
        uuid= data[Core::keys::KEY_UUID].toString();
        base= vectorialMap(uuid, data, true);
    }
    break;
    case C::PICTURE:
        // case C::ONLINEPICTURE:
        {
            auto data= MessageHelper::readImageData(msg);
            uuid= data[Core::keys::KEY_UUID].toString();
            base= image(uuid, data);
        }
        break;
    case C::MINDMAP:
    {
        auto data= MessageHelper::readMindMap(msg);
        uuid= data[Core::keys::KEY_UUID].toString();
        base= mindmap(uuid, data);
    }
    break;
    case C::NOTES:
    {
        // base= note(uuid, map);
    }
    break;
    case C::CHARACTERSHEET:
    {
        auto data= MessageHelper::readCharacterSheet(msg);
        uuid= data[Core::keys::KEY_UUID].toString();
        base= sheetCtrl(uuid, data);
    }
    break;
    case C::SHAREDNOTE:
    {
        auto data= MessageHelper::readSharedNoteData(msg);
        uuid= data[Core::keys::KEY_UUID].toString();
        base= sharedNote(uuid, data, m_localId);
    }
    break;
    case C::PDF:
    {
        auto data= MessageHelper::readPdfData(msg);
        uuid= data[Core::keys::KEY_UUID].toString();
        base= pdf(uuid, data);
    }
    break;
    case C::WEBVIEW:
    {
        auto data= MessageHelper::readWebPageData(msg);
        uuid= data[Core::keys::KEY_UUID].toString();
        base= webPage(uuid, data);
    }
    break;
    default:
        break;
    }
    Q_ASSERT(base != nullptr);
    base->setRemote(true);
    base->setLocalGM(localIsGM);
    base->setLocalId(m_localId);
    base->setLocalColor(localColor);
    return base;
}

void MediaFactory::setLocalId(const QString& id)
{
    m_localId= id;
}
} // namespace Media
