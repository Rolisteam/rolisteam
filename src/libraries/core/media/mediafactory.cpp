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
#include "mediafactory.h"

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

#include "mindmap/src/data/link.h"
#include "mindmap/src/data/mindnode.h"
#include "mindmap/src/model/boxmodel.h"
#include "mindmap/src/model/linkmodel.h"
#include "model/imagemodel.h"

#include "network/networkmessagereader.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
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

    sheetCtrl->setName(params.value(Core::keys::KEY_NAME).toString());
    sheetCtrl->setQmlCode(params.value(Core::keys::KEY_QML).toString());

    if(params.contains(Core::keys::KEY_IMAGEDATA))
    {
        auto array= params.value(Core::keys::KEY_IMAGEDATA).toByteArray();
        auto imgModel= sheetCtrl->imageModel();
        imgModel->load(IOHelper::byteArrayToJsonArray(array));
    }
    if(params.contains(Core::keys::KEY_ROOTSECTION))
    {
        auto array= params.value(Core::keys::KEY_ROOTSECTION).toByteArray();
        auto sheetModel= sheetCtrl->model();
        sheetModel->setRootSection(IOHelper::byteArrayToJsonObj(array));
    }

    sheetCtrl->setGameMasterId(params.value(Core::keys::KEY_GMID).toString());

    if(params.contains(Core::keys::KEY_DATA) && params.contains(Core::keys::KEY_CHARACTERID))
    {
        auto array= params.value(Core::keys::KEY_DATA).toByteArray();
        auto characterId= params.value(Core::keys::KEY_CHARACTERID).toString();
        sheetCtrl->addCharacterSheet(IOHelper::byteArrayToJsonObj(array), characterId);
    }

    if(params.contains(Core::keys::KEY_SERIALIZED))
    {
        auto serializedData= params.value(Core::keys::KEY_SERIALIZED).toByteArray();
        IOHelper::readCharacterSheetController(sheetCtrl, serializedData);
    }
    return sheetCtrl;
}

VectorialMapController* vectorialMap(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto vmapCtrl= new VectorialMapController(uuid);

    QByteArray serializedData= params.value(Core::keys::KEY_SERIALIZED).toByteArray();

    if(!params.isEmpty())
    {
        namespace ck= Core::keys;
        vmapCtrl->setName(params.value(ck::KEY_NAME).toString());
        vmapCtrl->setLayer(params.value(ck::KEY_LAYER).value<Core::Layer>());
        vmapCtrl->setPermission(params.value(ck::KEY_PERMISSION).value<Core::PermissionMode>());
        vmapCtrl->setBackgroundColor(params.value(ck::KEY_BGCOLOR).value<QColor>());
        vmapCtrl->setVisibility(params.value(ck::KEY_VISIBILITY).value<Core::VisibilityMode>());
        vmapCtrl->setZindex(params.value(ck::KEY_ZINDEX).toInt());
        vmapCtrl->setCharacterVision(params.value(ck::KEY_CHARACTERVISION).toBool());
        vmapCtrl->setGridPattern(params.value(ck::KEY_GRIDPATTERN).value<Core::GridPattern>());
        vmapCtrl->setGridVisibility(params.value(ck::KEY_GRIDVISIBILITY).toBool());
        vmapCtrl->setGridSize(params.value(ck::KEY_GRIDSIZE).toInt());
        vmapCtrl->setGridScale(params.value(ck::KEY_GRIDSCALE).toDouble());
        vmapCtrl->setGridAbove(params.value(ck::KEY_GRIDABOVE).toBool());
        vmapCtrl->setScaleUnit(params.value(ck::KEY_UNIT).value<Core::ScaleUnit>());
        vmapCtrl->setGridColor(params.value(ck::KEY_GRIDCOLOR).value<QColor>());
        vmapCtrl->setIdle(true);

        auto items= params.value(ck::KEY_ITEMS).toHash();
        VectorialMapMessageHelper::fetchModelFromMap(items, vmapCtrl);
    }

    if(!serializedData.isEmpty())
        VectorialMapMessageHelper::readVectorialMapController(vmapCtrl, serializedData);

    return vmapCtrl;
}
PdfController* pdf(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto ownerid= params.value(Core::keys::KEY_OWNERID).toString();
    auto array= params.value(Core::keys::KEY_DATA).toByteArray();
    auto path= params.value(Core::keys::KEY_PATH).toString();
    auto seriaziledData= params.value(Core::keys::KEY_SERIALIZED).toByteArray();
    auto pdfCtrl= new PdfController(uuid, path, array);
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

    /* if(map.contains("indexStyle"))
         mindmapCtrl->setDefaultStyleIndex(map.value("indexStyle").toBool());

      QHash<QString, mindmap::MindNode*> data;
     if(map.contains("nodes"))
     {
         QHash<QString, QVariant> nodes= map.value("nodes").toHash();

         auto model= dynamic_cast<mindmap::BoxModel*>(mindmapCtrl->nodeModel());
         QHash<QString, QString> parentData;
         QList<mindmap::MindNode*> nodesList;
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
         model->appendNode(nodesList);

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

         auto model= dynamic_cast<mindmap::LinkModel*>(mindmapCtrl->linkModel());

         QList<mindmap::Link*> linkList;
         for(const auto& var : links)
         {
             auto link= new mindmap::Link();
             auto linkV= var.toHash();
             link->setId(linkV["uuid"].toString());
             link->setText(linkV["text"].toString());
             link->setDirection(static_cast<Core::ArrowDirection>(linkV["direction"].toInt()));
             auto startId= linkV["startId"].toString();
             auto endId= linkV["endId"].toString();

             Q_ASSERT(data.contains(endId) && data.contains(startId));

             link->setStart(data.value(startId));
             link->setEnd(data.value(endId));

             linkList << link;
         }
         model->append(linkList);
     }

     if(map.contains("imageInfoData"))
     {
         QHash<QString, QVariant> imgInfos= map.value("imageInfoData").toHash();
         auto model= mindmapCtrl->imageModel();
         for(const auto& var : imgInfos)
         {
             auto img= var.toHash();
             auto pix= img["pixmap"].value<QPixmap>();
             auto id= img["id"].toString();
             auto url= QUrl(img["url"].toString());
             model->insertPixmap(id, pix, url);
         }
     }*/

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
                                                    const std::map<QString, QVariant>& map, bool localIsGM)
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
    return base;
}

MediaControllerBase* MediaFactory::createRemoteMedia(Core::ContentType type, NetworkMessageReader* msg, bool localIsGM)
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
        base= vectorialMap(uuid, data);
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
    return base;
}

void MediaFactory::setLocalId(const QString& id)
{
    m_localId= id;
}
} // namespace Media
