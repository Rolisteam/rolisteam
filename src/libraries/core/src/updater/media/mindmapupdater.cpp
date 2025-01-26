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
#include "updater/media/mindmapupdater.h"

#include "controller/view_controller/mindmapcontroller.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/minditem.h"
#include "mindmap/data/mindmaptypes.h"
#include "mindmap/model/minditemmodel.h"
#include "model/contentmodel.h"
#include "network/networkmessagewriter.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"

#include <QDebug>
#include <QJsonArray>
#include <QTimer>

constexpr int timeout{1000 * 5};

MindMapUpdater::MindMapUpdater(FilteredContentModel* model, campaign::CampaignManager* manager, QObject* parent)
    : MediaUpdaterInterface(manager, parent), m_mindmaps(model)
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::MindMapCategory, this);
}

void MindMapUpdater::addMediaController(MediaControllerBase* base)
{
    if(nullptr == base)
        return;

    auto ctrl= dynamic_cast<MindMapController*>(base);

    if(ctrl == nullptr)
        return;

    auto timer= new QTimer(ctrl);

    connect(timer, &QTimer::timeout, this,
            [this, ctrl, timer]()
            {
                timer->stop();
                if(ctrl->modified())
                {
                    saveMediaController(ctrl);
                }
            });

    connect(ctrl, &MindMapController::sharingToAllChanged, this,
            [this](Core::SharingPermission perm, Core::SharingPermission old)
            {
                auto ctrl= qobject_cast<MindMapController*>(sender());
                if(!ctrl)
                    return;
                if(!ctrl->localIsOwner() || ctrl->remote())
                    return;
                if(old == Core::SharingPermission::None)
                {
                    MessageHelper::sendOffMindmapToAll(ctrl);
                }
                else if(perm == Core::SharingPermission::None)
                {
                    MessageHelper::closeMedia(ctrl->uuid(), Core::ContentType::MINDMAP);
                }
                else
                {
                    MessageHelper::sendOffMindmapPermissionUpdate(ctrl);
                }
            });

    connect(ctrl, &MindMapController::openMindMapTo, this,
            [ctrl](const QString& id) { MessageHelper::openMindmapTo(ctrl, id); });

    connect(ctrl, &MindMapController::closeMindMapTo, this,
            [ctrl](const QString& id) { MessageHelper::closeMindmapTo(ctrl, id); });

    connect(ctrl, &MindMapController::permissionChangedForUser, this,
            [this](const QString& id, Core::SharingPermission perm)
            {
                auto ctrl= qobject_cast<MindMapController*>(sender());
                if(!ctrl)
                    return;
                if(!ctrl->localIsOwner() || ctrl->remote())
                    return;
                MessageHelper::sendOffMindmapPermissionUpdateTo(perm, ctrl, id);
            });

    connect(ctrl, &MindMapController::modifiedChanged, this,
            [this, timer]()
            {
                auto ctrl= qobject_cast<MindMapController*>(sender());
                if(!ctrl)
                    return;
                if(!ctrl->localIsOwner() || ctrl->remote())
                    return;
                if(ctrl->modified())
                {
                    timer->start(timeout);
                }
            });

    if(ctrl->modified())
    {
        saveMediaController(ctrl);
    }
    setConnection(ctrl);
}

bool MindMapUpdater::updateSubobjectProperty(NetworkMessageReader* msg, MindMapController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    auto id= msg->string8();
    QHash<NetMsg::Action, mindmap::MindItem::Type> set{{NetMsg::UpdateLink, mindmap::MindItem::LinkType},
                                                       {NetMsg::UpdateNode, mindmap::MindItem::NodeType},
                                                       {NetMsg::UpdatePackage, mindmap::MindItem::PackageType}};

    if(!set.contains(msg->action()))
        return false;

    auto subobject= ctrl->subItem(id, set.value(msg->action()));
    if(!subobject)
        return false;

    auto property= msg->string16();
    QVariant var;

    if(property == QStringLiteral("text"))
    {
        var= msg->string32();
    }
    else if(property == QStringLiteral("imageUri"))
    {
        var= msg->string32();
    }
    else if(property == QStringLiteral("styleIndex"))
    {
        var= msg->int64();
    }
    else if(property == QStringLiteral("tags"))
    {
        auto dataTags= msg->byteArray32();
        var= IOHelper::jsonArrayToStringList(IOHelper::byteArrayToJsonArray(dataTags));
        // utils::IOHelper::
    }
    else if(property == QStringLiteral("description"))
    {
        var= msg->string32();
    }
    else if(property == QStringLiteral("direction"))
    {
        var= QVariant::fromValue(static_cast<mindmap::ArrowDirection>(msg->uint8()));
    }

    return subobject->setProperty(property.toLocal8Bit().data(), var);
}

MindMapController* findController(const QString& id, FilteredContentModel* mindmaps)
{
    if(id.isEmpty() || !mindmaps)
        return nullptr;

    auto ctrls= mindmaps->contentController<MindMapController*>();

    auto it= std::find_if(ctrls.begin(), ctrls.end(), [id](MindMapController* ctrl) { return id == ctrl->uuid(); });

    if(it == ctrls.end())
        return nullptr;
    return *it;
}

void MindMapUpdater::setConnection(MindMapController* ctrl)
{
    if(ctrl == nullptr)
        return;
    disconnectController(ctrl);
    ConnectionInfo info;
    info.id= ctrl->uuid();

    auto nodeModel= dynamic_cast<mindmap::MindItemModel*>(ctrl->itemModel());
    auto imageModel= ctrl->imgModel();
    info.connections << connect(imageModel, &mindmap::ImageModel::imageAdded, this,
                                [imageModel, ctrl](const QString& id)
                                {
                                    auto info= imageModel->imageInfoFromId(id);
                                    if(info.m_pixmap.isNull())
                                        return;
                                    MessageHelper::sendOffImageInfo(info, ctrl);
                                });
    auto idCtrl= ctrl->uuid();

    info.connections << connect(imageModel, &mindmap::ImageModel::imageRemoved, this,
                                [ctrl](const QString& id) { MessageHelper::sendOffRemoveImageInfo(id, ctrl); });

    // connect existing  data
    auto nodes= nodeModel->items(mindmap::MindItem::NodeType);
    for(auto const& i : nodes)
    {
        auto n= dynamic_cast<mindmap::MindNode*>(i);
        if(!n)
            continue;
        info.connections << connect(n, &mindmap::MindNode::textChanged, this,
                                    [this, idCtrl, n]()
                                    { sendOffChange<QString>(idCtrl, QStringLiteral("text"), n, true); });
        info.connections << connect(n, &mindmap::MindNode::imageUriChanged, this,
                                    [this, idCtrl, n]()
                                    { sendOffChange<QString>(idCtrl, QStringLiteral("imageUri"), n, true); });
        info.connections << connect(n, &mindmap::MindNode::styleIndexChanged, this,
                                    [this, idCtrl, n]()
                                    { sendOffChange<int>(idCtrl, QStringLiteral("styleIndex"), n, true); });
        info.connections << connect(n, &mindmap::MindNode::tagsChanged, this,
                                    [this, idCtrl, n]()
                                    { sendOffChange<QStringList>(idCtrl, QStringLiteral("tags"), n, true); });
        info.connections << connect(n, &mindmap::MindNode::descriptionChanged, this,
                                    [this, idCtrl, n]()
                                    { sendOffChange<QString>(idCtrl, QStringLiteral("description"), n, true); });
    }

    auto links= nodeModel->items(mindmap::MindItem::LinkType);
    for(auto i : links)
    {
        auto link= dynamic_cast<mindmap::LinkController*>(i);
        if(!link)
            continue;

        info.connections << connect(link, &mindmap::LinkController::textChanged, this,
                                    [this, idCtrl, link]()
                                    { sendOffChange<QString>(idCtrl, QStringLiteral("text"), link, false); });
        info.connections << connect(
            link, &mindmap::LinkController::directionChanged, this,
            [this, idCtrl, link]()
            { sendOffChange<mindmap::ArrowDirection>(idCtrl, QStringLiteral("direction"), link, false); });
    }

    auto packages= nodeModel->items(mindmap::MindItem::PackageType);
    for(auto i : packages)
    {
        auto pack= dynamic_cast<mindmap::PackageNode*>(i);
        if(!pack)
            continue;

        info.connections << connect(pack, &mindmap::PackageNode::titleChanged, this,
                                    [this, idCtrl, pack]()
                                    { sendOffChange<QString>(idCtrl, QStringLiteral("title"), pack, false); });
        info.connections << connect(pack, &mindmap::PackageNode::minimumMarginChanged, this,
                                    [this, idCtrl, pack]()
                                    { sendOffChange<int>(idCtrl, QStringLiteral("minimumMargin"), pack, false); });

        info.connections << connect(pack, &mindmap::PackageNode::childAdded, this,
                                    [idCtrl, pack](const QString& childId)
                                    {
                                        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddChildToPackage);
                                        msg.string8(idCtrl);
                                        msg.string8(pack->id());
                                        msg.string8(childId);
                                        msg.sendToServer();
                                    });
        info.connections << connect(pack, &mindmap::PackageNode::childRemoved, this,
                                    [idCtrl, pack](const QString& childId)
                                    {
                                        NetworkMessageWriter msg(NetMsg::MindMapCategory,
                                                                 NetMsg::RemoveChildFromPackage);
                                        msg.string8(idCtrl);
                                        msg.string8(pack->id());
                                        msg.string8(childId);
                                        msg.sendToServer();
                                    });
    }
    // end of existing data

    connect(nodeModel, &mindmap::MindItemModel::itemAdded, this,
            [this, ctrl](const QList<mindmap::MindItem*>& items)
            {
                auto idCtrl= ctrl->uuid();
                auto info= findConnectionInfo(idCtrl);
                for(auto item : items)
                {
                    if(item->type() == mindmap::MindItem::NodeType)
                    {
                        auto node= dynamic_cast<mindmap::MindNode*>(item);

                        if(!node)
                            continue;

                        info->connections
                            << connect(node, &mindmap::MindNode::textChanged, this,
                                       [this, idCtrl, node]()
                                       { sendOffChange<QString>(idCtrl, QStringLiteral("text"), node, true); });
                        info->connections
                            << connect(node, &mindmap::MindNode::tagsChanged, this,
                                       [this, idCtrl, node]()
                                       { sendOffChange<QStringList>(idCtrl, QStringLiteral("tags"), node, true); });
                        info->connections
                            << connect(node, &mindmap::MindNode::descriptionChanged, this,
                                       [this, idCtrl, node]()
                                       { sendOffChange<QString>(idCtrl, QStringLiteral("description"), node, true); });
                        info->connections
                            << connect(node, &mindmap::MindNode::imageUriChanged, this,
                                       [this, idCtrl, node]()
                                       { sendOffChange<QString>(idCtrl, QStringLiteral("imageUri"), node, true); });
                        info->connections
                            << connect(node, &mindmap::MindNode::styleIndexChanged, this,
                                       [this, idCtrl, node]()
                                       { sendOffChange<int>(idCtrl, QStringLiteral("styleIndex"), node, true); });
                    }
                    else if(item->type() == mindmap::MindItem::LinkType)
                    {
                        auto link= dynamic_cast<mindmap::LinkController*>(item);

                        if(!link)
                            continue;

                        info->connections
                            << connect(link, &mindmap::LinkController::textChanged, this,
                                       [this, idCtrl, link]()
                                       {
                                           qDebug() << "update text of link:" << link->text();
                                           sendOffChange<QString>(idCtrl, QStringLiteral("text"), link, false);
                                       });
                        info->connections << connect(link, &mindmap::LinkController::directionChanged, this,
                                                     [this, idCtrl, link]() {
                                                         sendOffChange<mindmap::ArrowDirection>(
                                                             idCtrl, QStringLiteral("direction"), link, false);
                                                     });
                    }
                    else if(item->type() == mindmap::MindItem::PackageType)
                    {
                        auto pack= dynamic_cast<mindmap::PackageNode*>(item);
                        if(!pack)
                            continue;

                        info->connections
                            << connect(pack, &mindmap::PackageNode::titleChanged, this,
                                       [this, idCtrl, pack]()
                                       { sendOffChange<QString>(idCtrl, QStringLiteral("title"), pack, false); });
                        info->connections
                            << connect(pack, &mindmap::PackageNode::minimumMarginChanged, this,
                                       [this, idCtrl, pack]()
                                       { sendOffChange<int>(idCtrl, QStringLiteral("minimumMargin"), pack, false); });
                        info->connections
                            << connect(pack, &mindmap::PackageNode::parentNodeChanged, this,
                                       [this, idCtrl, pack]()
                                       { sendOffChange<QString>(idCtrl, QStringLiteral("parentId"), pack, false); });
                    }
                }

                NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddNodes);
                msg.string8(ctrl->uuid());
                MessageHelper::buildAddItemMessage(msg, items);
                msg.sendToServer();
            });

    info.connections << connect(nodeModel, &mindmap::MindItemModel::itemRemoved, this,
                                [ctrl](QStringList ids)
                                {
                                    if(!ctrl->localIsOwner())
                                        return;

                                    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveNode);
                                    msg.string8(ctrl->uuid());
                                    msg.uint64(ids.size());
                                    std::for_each(std::begin(ids), std::end(ids),
                                                  [&msg](const QString& id) { msg.string8(id); });
                                    msg.sendToServer();
                                });

    m_connections.append(info);
}

void MindMapUpdater::sendOffRemoveMessage(const QString& idCtrl, const QStringList& nodeids, const QStringList& linksId)
{
    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveNode);
    msg.string8(idCtrl);
    MessageHelper::buildRemoveItemMessage(msg, nodeids, linksId);
    msg.sendToServer();
}

ConnectionInfo* MindMapUpdater::findConnectionInfo(const QString& id)
{
    auto it= std::find_if(std::begin(m_connections), std::end(m_connections),
                          [id](const ConnectionInfo& info) { return id == info.id; });

    if(it == std::end(m_connections))
        return nullptr;
    else
        return &(*it);
}

void MindMapUpdater::disconnectController(MindMapController* media)
{
    if(!media)
        return;

    auto it= std::find_if(std::begin(m_connections), std::end(m_connections),
                          [media](const ConnectionInfo& info) { return media->uuid() == info.id; });

    if(it == std::end(m_connections))
        return;

    std::for_each(std::begin(it->connections), std::end(it->connections),
                  [this](const QMetaObject::Connection& connection) { this->disconnect(connection); });

    m_connections.erase(it);
}

NetWorkReceiver::SendType MindMapUpdater::processMessage(NetworkMessageReader* msg)
{
    auto res= NetWorkReceiver::NONE;
    if(!msg)
        return res;

    auto id= msg->string8();
    auto ctrl= findController(id, m_mindmaps);
    if(!ctrl)
        return res;

    if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::AddNodes))
    {
        MessageHelper::readMindMapAddItem(ctrl, msg);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::UpdateNode)
            || checkAction(msg, NetMsg::MindMapCategory, NetMsg::UpdateLink))
    {
        updateSubobjectProperty(msg, ctrl);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::RemoveNode))
    {
        MessageHelper::readMindMapRemoveMessage(ctrl, msg);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::UpdateMindMapPermission))
    {
        bool readWrite= static_cast<bool>(msg->uint8());

        ctrl->setSharingToAll(readWrite ? static_cast<int>(Core::SharingPermission::ReadWrite) :
                                          static_cast<int>(Core::SharingPermission::ReadOnly));
        readWrite ? setConnection(ctrl) : disconnectController(ctrl);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::AddSubImage))
    {
        MessageHelper::readAddSubImage(ctrl->imgModel(), ctrl->itemModel(), msg);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::RemoveSubImage))
    {
        MessageHelper::readRemoveSubImage(ctrl->imgModel(), msg);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::AddChildToPackage))
    {
        MessageHelper::readChildPackageAction(true, msg, ctrl);
    }
    else if(checkAction(msg, NetMsg::MindMapCategory, NetMsg::RemoveChildFromPackage))
    {
        MessageHelper::readChildPackageAction(false, msg, ctrl);
    }
    return res;
}
