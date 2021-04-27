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
#include "mindmapupdater.h"

#include "controller/view_controller/mindmapcontroller.h"
#include "data/link.h"
#include "data/mindnode.h"
#include "model/boxmodel.h"
#include "model/contentmodel.h"
#include "model/linkmodel.h"
#include "model/nodestylemodel.h"
#include "worker/messagehelper.h"

#include "network/networkmessagewriter.h"

#include <QDebug>

MindMapUpdater::MindMapUpdater(FilteredContentModel* model, QObject* parent)
    : MediaUpdaterInterface(parent), m_mindmaps(model)
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

    connect(ctrl, &MindMapController::sharingToAllChanged, this,
            [ctrl](Core::SharingPermission perm, Core::SharingPermission old) {
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
            [ctrl](const QString& id, Core::SharingPermission perm) {
                MessageHelper::sendOffMindmapPermissionUpdateTo(perm, ctrl, id);
            });

    setConnection(ctrl);
}

bool MindMapUpdater::updateSubobjectProperty(NetworkMessageReader* msg, MindMapController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    auto id= msg->string8();
    QHash<NetMsg::Action, MindMapController::SubItemType> set{{NetMsg::UpdateLink, MindMapController::Link},
                                                              {NetMsg::UpdateNode, MindMapController::Node},
                                                              {NetMsg::UpdatePackage, MindMapController::Package}};

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
    else if(property == QStringLiteral("direction"))
    {
        var= static_cast<mindmap::Link::Direction>(msg->uint8());
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

    auto nodeModel= dynamic_cast<mindmap::BoxModel*>(ctrl->nodeModel());
    auto linkModel= dynamic_cast<mindmap::LinkModel*>(ctrl->linkModel());
    auto imageModel= ctrl->imageModel();
    info.connections << connect(imageModel, &ImageModel::imageAdded, this, [imageModel, ctrl](const QString& id) {
        auto info= imageModel->imageInfoFromId(id);
        if(info.m_pixmap.isNull())
            return;
        MessageHelper::sendOffImageInfo(info, ctrl);
    });
    auto idCtrl= ctrl->uuid();

    info.connections << connect(imageModel, &ImageModel::imageRemoved, this,
                                [ctrl](const QString& id) { MessageHelper::sendOffRemoveImageInfo(id, ctrl); });

    // connect existing  data
    auto nodes= nodeModel->nodes();
    for(auto const& n : nodes)
    {
        info.connections << connect(n, &mindmap::MindNode::textChanged, this, [this, idCtrl, n]() {
            sendOffChange<QString>(idCtrl, QStringLiteral("text"), n, true);
        });
        info.connections << connect(n, &mindmap::MindNode::imageUriChanged, this, [this, idCtrl, n]() {
            sendOffChange<QString>(idCtrl, QStringLiteral("imageUri"), n, true);
        });
        info.connections << connect(n, &mindmap::MindNode::styleIndexChanged, this, [this, idCtrl, n]() {
            sendOffChange<int>(idCtrl, QStringLiteral("styleIndex"), n, true);
        });
    }

    auto links= linkModel->getDataSet();
    for(auto link : links)
    {
        info.connections << connect(link, &mindmap::Link::textChanged, this, [this, idCtrl, link]() {
            sendOffChange<QString>(idCtrl, QStringLiteral("text"), link, false);
        });
        info.connections << connect(link, &mindmap::Link::directionChanged, this, [this, idCtrl, link]() {
            sendOffChange<mindmap::Link::Direction>(idCtrl, QStringLiteral("direction"), link, false);
        });
    }
    // end of existing data

    /*info.connections << connect(nodeModel, &mindmap::BoxModel::nodeAdded, this,
                                [this, ctrl](QList<mindmap::MindNode*> nodes) {
                                    if(!ctrl || (!ctrl->localIsOwner() && !ctrl->readWrite()))
                                        return;

                                    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddNode);
                                    auto idCtrl= ctrl->uuid();
                                    msg.string8(idCtrl);
                                    MessageHelper::buildAddNodeMessage(msg, nodes);

                                    msg.sendToServer();
                                    auto info= findConnectionInfo(idCtrl);
                                    Q_ASSERT(info);
                                });

    info.connections << connect(linkModel, &mindmap::LinkModel::linkAdded, this,
                                [this, ctrl](QList<mindmap::Link*> links) {
                                    if(!ctrl || (!ctrl->localIsOwner() && !ctrl->readWrite()))
                                        return;

                                    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddLink);
                                    auto idCtrl= ctrl->uuid();
                                    msg.string8(idCtrl);
                                    MessageHelper::buildAddLinkMessage(msg, links);
                                    msg.sendToServer();

                                    Q_ASSERT(info);
                                });*/

    /*info.connections << connect(nodeModel, &mindmap::BoxModel::nodeRemoved, this, [ctrl](const QStringList& ids) {
        if(!ctrl->localIsOwner())
            return;
        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveNode);
        msg.string8(ctrl->uuid());
        msg.uint64(ids.size());
        std::for_each(std::begin(ids), std::end(ids), [&msg](const QString& id) { msg.string8(id); });
        msg.sendToServer();
    });

    info.connections << connect(linkModel, &mindmap::LinkModel::linkRemoved, this, [ctrl](const QStringList& ids) {
        if(!ctrl->localIsOwner())
            return;
        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveLink);
        msg.string8(ctrl->uuid());
        msg.uint64(ids.size());
        std::for_each(std::begin(ids), std::end(ids), [&msg](const QString& id) { msg.string8(id); });
        msg.sendToServer();
    });*/

    m_connections.append(info);
}

void MindMapUpdater::sendOffRemoveMessage(const QString& idCtrl, const QStringList& nodeids, const QStringList& linksId)
{
    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveMessage);
    msg.string8(idCtrl);
    MessageHelper::buildRemoveItemMessage(msg, nodeids, linksId);
    msg.sendToServer();
}

void MindMapUpdater::sendOffAddingMessage(const QString& idCtrl, const QList<mindmap::MindNode*>& nodes,
                                          const QList<mindmap::Link*>& links)
{
    auto info= findConnectionInfo(idCtrl);

    for(auto node : nodes)
    {
        info->connections << connect(node, &mindmap::MindNode::textChanged, this, [this, idCtrl, node]() {
            sendOffChange<QString>(idCtrl, QStringLiteral("text"), node, true);
        });
        info->connections << connect(node, &mindmap::MindNode::imageUriChanged, this, [this, idCtrl, node]() {
            sendOffChange<QString>(idCtrl, QStringLiteral("imageUri"), node, true);
        });
        info->connections << connect(node, &mindmap::MindNode::styleIndexChanged, this, [this, idCtrl, node]() {
            sendOffChange<int>(idCtrl, QStringLiteral("styleIndex"), node, true);
        });
    }

    for(auto link : qAsConst(links))
    {
        info->connections << connect(link, &mindmap::Link::textChanged, this, [this, idCtrl, link]() {
            sendOffChange<QString>(idCtrl, QStringLiteral("text"), link, false);
        });
        info->connections << connect(link, &mindmap::Link::directionChanged, this, [this, idCtrl, link]() {
            sendOffChange<mindmap::Link::Direction>(idCtrl, QStringLiteral("direction"), link, false);
        });
    }
    NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddMessage);
    msg.string8(idCtrl);
    MessageHelper::buildAddItemMessage(msg, nodes, links);
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
                  [](const QMetaObject::Connection& connection) { disconnect(connection); });

    m_connections.erase(it);
}

NetWorkReceiver::SendType MindMapUpdater::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddMessage && msg->category() == NetMsg::MindMapCategory)
    {
        auto id= msg->string8();
        MessageHelper::readMindMapAddItem(findController(id, m_mindmaps), msg);
    }
    else if((msg->action() == NetMsg::UpdateNode || msg->action() == NetMsg::UpdateLink)
            && msg->category() == NetMsg::MindMapCategory)
    {
        auto id= msg->string8();
        // auto idnode= msg->string8();
        updateSubobjectProperty(msg, findController(id, m_mindmaps));
    }
    else if(msg->category() == NetMsg::MindMapCategory && msg->action() == NetMsg::RemoveMessage)
    {
        auto id= msg->string8();
        MessageHelper::readMindMapRemoveMessage(findController(id, m_mindmaps), msg);

        // ctrl->removeNode(MessageHelper::readIdList(*msg));
    }
    else if(msg->action() == NetMsg::UpdateMindMapPermission && msg->category() == NetMsg::MindMapCategory)
    {
        auto id= msg->string8();
        auto ctrl= findController(id, m_mindmaps);
        if(ctrl)
        {
            bool readWrite= static_cast<bool>(msg->uint8());

            ctrl->setSharingToAll(readWrite ? static_cast<int>(Core::SharingPermission::ReadWrite) :
                                              static_cast<int>(Core::SharingPermission::ReadOnly));
            readWrite ? setConnection(ctrl) : disconnectController(ctrl);
        }
    }
    else if(msg->action() == NetMsg::AddSubImage && msg->category() == NetMsg::MediaCategory)
    {
        auto id= msg->string8();
        auto ctrl= findController(id, m_mindmaps);
        MessageHelper::readAddSubImage(ctrl->imageModel(), msg);
    }
    else if(msg->action() == NetMsg::RemoveSubImage && msg->category() == NetMsg::MediaCategory)
    {
        auto id= msg->string8();
        auto ctrl= findController(id, m_mindmaps);
        MessageHelper::readRemoveSubImage(ctrl->imageModel(), msg);
    }
    return NetWorkReceiver::NONE;
}
