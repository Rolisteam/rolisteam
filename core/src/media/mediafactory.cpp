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

#include "data/link.h"
#include "data/mindnode.h"
#include "model/boxmodel.h"
#include "model/linkmodel.h"

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
    QByteArray serializedData= map.value(QStringLiteral("serializedData")).toByteArray();
    QByteArray pix= map.value(QStringLiteral("data")).toByteArray();
    QString path= map.value(QStringLiteral("path")).toString();
    QString name= map.value(QStringLiteral("name")).toString();

    return new ImageController(uuid, name, path, pix);
}

CharacterSheetController* sheetCtrl(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto path= params.value(QStringLiteral("path")).toString();

    CharacterSheetController* sheetCtrl= new CharacterSheetController(uuid, path);

    sheetCtrl->setName(params.value(QStringLiteral("name")).toString());
    sheetCtrl->setQmlCode(params.value(QStringLiteral("qml")).toString());

    if(params.contains(QStringLiteral("imageData")))
    {
        auto array= params.value(QStringLiteral("imageData")).toByteArray();
        auto imgModel= sheetCtrl->imageModel();
        imgModel->load(IOHelper::byteArrayToJsonArray(array));
    }
    if(params.contains(QStringLiteral("rootSection")))
    {
        auto array= params.value(QStringLiteral("rootSection")).toByteArray();
        auto sheetModel= sheetCtrl->model();
        sheetModel->setRootSection(IOHelper::byteArrayToJsonObj(array));
    }

    sheetCtrl->setGameMasterId(params.value("gameMasterId").toString());

    if(params.contains(QStringLiteral("data")) && params.contains(QStringLiteral("characterId")))
    {
        auto array= params.value(QStringLiteral("data")).toByteArray();
        auto characterId= params.value(QStringLiteral("characterId")).toString();
        sheetCtrl->addCharacterSheet(IOHelper::byteArrayToJsonObj(array), characterId);
    }

    if(params.contains(QStringLiteral("serializedData")))
    {
        auto serializedData= params.value(QStringLiteral("serializedData")).toByteArray();
        IOHelper::readCharacterSheetController(sheetCtrl, serializedData);
    }
    return sheetCtrl;
}

VectorialMapController* vectorialMap(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto vmapCtrl= new VectorialMapController(uuid);

    QByteArray serializedData= params.value(QStringLiteral("serializedData")).toByteArray();

    if(!params.isEmpty())
    {
        vmapCtrl->setPermission(params.value(QStringLiteral("permission")).value<Core::PermissionMode>());
        vmapCtrl->setName(params.value(QStringLiteral("name")).toString());
        vmapCtrl->setBackgroundColor(params.value(QStringLiteral("bgcolor")).value<QColor>());
        vmapCtrl->setGridSize(params.value(QStringLiteral("gridSize")).toInt());
        vmapCtrl->setGridPattern(params.value(QStringLiteral("gridPattern")).value<Core::GridPattern>());
        vmapCtrl->setGridColor(params.value(QStringLiteral("gridColor")).value<QColor>());
        vmapCtrl->setVisibility(params.value(QStringLiteral("visibility")).value<Core::VisibilityMode>());
        vmapCtrl->setGridScale(params.value(QStringLiteral("scale")).toDouble());
        vmapCtrl->setScaleUnit(params.value(QStringLiteral("unit")).value<Core::ScaleUnit>());
    }

    if(!serializedData.isEmpty())
        VectorialMapMessageHelper::readVectorialMapController(vmapCtrl, serializedData);

    return vmapCtrl;
}
PdfController* pdf(const QString& uuid, const QHash<QString, QVariant>& params)
{
    auto ownerid= params.value(QStringLiteral("ownerId")).toString();
    auto array= params.value(QStringLiteral("data")).toByteArray();
    auto path= params.value(QStringLiteral("path")).toString();
    auto seriaziledData= params.value(QStringLiteral("seriaziledData")).toByteArray();
    auto pdfCtrl= new PdfController(uuid, path, array);
    pdfCtrl->setOwnerId(ownerid);
    if(!seriaziledData.isEmpty())
        IOHelper::readPdfController(pdfCtrl, seriaziledData);
    return pdfCtrl;
}
NoteController* note(const QString& uuid, const QHash<QString, QVariant>& map)
{
    auto name= map.value(QStringLiteral("name")).toString();
    auto path= map.value(QStringLiteral("path")).toString();

    auto ownerid= map.value(QStringLiteral("ownerId")).toString();
    auto serializedData= map.value(QStringLiteral("serializedData")).toByteArray();

    auto noteCtrl= new NoteController(uuid);

    if(!name.isEmpty())
        noteCtrl->setName(name);
    noteCtrl->setOwnerId(ownerid);
    noteCtrl->setPath(path);

    if(!serializedData.isEmpty())
        IOHelper::readNoteController(noteCtrl, serializedData);

    return noteCtrl;
}
MindMapController* mindmap(const QString& uuid, const QHash<QString, QVariant>& map)
{
    auto name= map.value(QStringLiteral("name")).toString();
    auto path= map.value(QStringLiteral("path")).toString();

    auto ownerid= map.value(QStringLiteral("ownerId")).toString();
    auto serializedData= map.value(QStringLiteral("serializedData")).toByteArray();

    auto mindmapCtrl= new MindMapController(uuid);

    if(map.contains("indexStyle"))
        mindmapCtrl->setReadWrite(map.value("indexStyle").toBool());

    QHash<QString, mindmap::MindNode*> data;
    if(map.contains("nodes"))
    {
        QHash<QString, QVariant> nodes= map.value("nodes").toHash();

        auto model= dynamic_cast<mindmap::BoxModel*>(mindmapCtrl->nodeModel());
        QHash<QString, QString> parentData;
        for(auto var : nodes)
        {
            auto node= new mindmap::MindNode();
            auto nodeV= var.toHash();
            node->setId(nodeV["uuid"].toString());
            node->setText(nodeV["text"].toString());
            node->setImageUri(nodeV["imageUri"].toString());
            QPointF pos(nodeV["x"].toReal(), nodeV["y"].toReal());
            node->setPosition(pos);
            node->setStyleIndex(nodeV["index"].toInt());
            model->appendNode(node);

            data.insert(node->id(), node);
            parentData.insert(node->id(), nodeV["parentId"].toString());
        }

        for(auto key : data.keys())
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

        for(auto var : links)
        {
            auto link= new mindmap::Link();
            auto linkV= var.toHash();
            link->setId(linkV["uuid"].toString());
            link->setText(linkV["text"].toString());
            link->setDirection(static_cast<mindmap::Link::Direction>(linkV["direction"].toInt()));
            auto startId= linkV["startId"].toString();
            auto endId= linkV["endId"].toString();

            Q_ASSERT(data.contains(endId) && data.contains(startId));

            link->setStart(data.value(startId));
            link->setEnd(data.value(endId));

            model->append(link);
        }
    }

    if(!name.isEmpty())
        mindmapCtrl->setName(name);

    mindmapCtrl->setOwnerId(ownerid);
    mindmapCtrl->setPath(path);

    if(!serializedData.isEmpty())
        IOHelper::readMindmapController(mindmapCtrl, serializedData);

    return mindmapCtrl;
}
SharedNoteController* sharedNote(const QString& uuid, const QHash<QString, QVariant>& params, const QString& localId)
{
    auto ownerId= params.value(QStringLiteral("ownerId")).toString();
    auto b= params.value(QStringLiteral("markdown"), false).toBool();
    auto path= params.value(QStringLiteral("path")).toString();
    auto text= params.value(QStringLiteral("text")).toString();
    auto noteCtrl= new SharedNoteController(ownerId, localId, uuid);

    if(!path.isEmpty())
    {
        noteCtrl->setPath(path);
        QFileInfo info(path);
        noteCtrl->setName(info.fileName());
        b= path.endsWith(".md");
    }

    noteCtrl->setHighligthedSyntax(b ? SharedNoteController::HighlightedSyntax::MarkDown :
                                       SharedNoteController::HighlightedSyntax::None);
    if(!text.isEmpty())
        noteCtrl->setText(text);

    return noteCtrl;
}
WebpageController* webPage(const QString& uuid, const QHash<QString, QVariant>& params)
{
    QByteArray serializedData= params.value(QStringLiteral("serializedData")).toByteArray();
    auto webCtrl= new WebpageController(uuid);
    webCtrl->setOwnerId(params.value(QStringLiteral("ownerId")).toString());

    if(params.contains(QStringLiteral("mode")))
    {
        auto mode= static_cast<WebpageController::SharingMode>(params.value(QStringLiteral("mode")).toInt());
        auto data= params.value(QStringLiteral("data")).toString();

        if(mode == WebpageController::Url)
            webCtrl->setPath(data);
        else if(mode == WebpageController::Html)
            webCtrl->setHtml(data);
    }
    if(params.contains(QStringLiteral("state")))
    {
        webCtrl->setState(static_cast<WebpageController::State>(params.value(QStringLiteral("state")).toInt()));
    }
    if(!serializedData.isEmpty())
        IOHelper::readWebpageController(webCtrl, serializedData);

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
    case C::ONLINEPICTURE:
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
        uuid= data["uuid"].toString();
        base= vectorialMap(uuid, data);
    }
    break;
    case C::PICTURE:
    case C::ONLINEPICTURE:
    {
        auto data= MessageHelper::readImageData(msg);
        uuid= data["uuid"].toString();
        base= image(uuid, data);
    }
    break;
    case C::MINDMAP:
    {
        auto data= MessageHelper::readMindMap(msg);
        uuid= data["uuid"].toString();
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
        uuid= data["uuid"].toString();
        base= sheetCtrl(uuid, data);
    }
    break;
    case C::SHAREDNOTE:
    {
        auto data= MessageHelper::readSharedNoteData(msg);
        uuid= data["uuid"].toString();
        base= sharedNote(uuid, data, m_localId);
    }
    break;
    case C::PDF:
    {
        auto data= MessageHelper::readPdfData(msg);
        uuid= data["uuid"].toString();
        base= pdf(uuid, data);
    }
    break;
    case C::WEBVIEW:
    {
        auto data= MessageHelper::readWebPageData(msg);
        uuid= data["uuid"].toString();
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
