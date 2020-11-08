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
                    MessageHelper::sendOffMindmapPermissionUpdate(perm, ctrl);
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

    auto nodeModel= dynamic_cast<mindmap::BoxModel*>(ctrl->nodeModel());
    auto linkModel= dynamic_cast<mindmap::LinkModel*>(ctrl->linkModel());
    // auto styleModel= dynamic_cast<mindmap::NodeStyleModel*>(ctrl->styleModel());

    connect(nodeModel, &mindmap::BoxModel::nodeAdded, this, [this, ctrl](mindmap::MindNode* node) {
        if(!ctrl || !ctrl->localIsOwner())
            return;

        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddNode);
        auto idCtrl= ctrl->uuid();
        msg.string8(idCtrl);
        msg.string8(node->id());
        msg.string32(node->text());
        msg.string8(node->imageUri());
        msg.string8(node->parentId());
        msg.uint64(node->styleIndex());

        auto subs= node->subLinks();
        msg.uint64(static_cast<quint64>(subs.size()));
        for(auto link : subs)
        {
            msg.string8(link->id());
        }

        msg.sendToServer();
        connect(node, &mindmap::MindNode::textChanged, this,
                [this, idCtrl, node]() { sendOffChange<QString>(idCtrl, QStringLiteral("text"), node, true); });
        connect(node, &mindmap::MindNode::imageUriChanged, this,
                [this, idCtrl, node]() { sendOffChange<QString>(idCtrl, QStringLiteral("imageUri"), node, true); });
        connect(node, &mindmap::MindNode::styleIndexChanged, this,
                [this, idCtrl, node]() { sendOffChange<int>(idCtrl, QStringLiteral("styleIndex"), node, true); });
    });

    connect(linkModel, &mindmap::LinkModel::linkAdded, this, [this, ctrl](mindmap::Link* link) {
        if(!ctrl || !ctrl->localIsOwner())
            return;

        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::AddLink);
        auto idCtrl= ctrl->uuid();
        msg.string8(idCtrl);
        msg.string8(link->id());
        msg.string32(link->text());
        msg.string8(link->start()->id());
        msg.string8(link->end()->id());
        msg.uint8(static_cast<int>(link->direction()));
        msg.sendToServer();
        connect(link, &mindmap::Link::textChanged, this,
                [this, idCtrl, link]() { sendOffChange<QString>(idCtrl, QStringLiteral("text"), link, false); });
        connect(link, &mindmap::Link::directionChanged, this, [this, idCtrl, link]() {
            sendOffChange<mindmap::Link::Direction>(idCtrl, QStringLiteral("direction"), link, false);
        });
    });

    connect(nodeModel, &mindmap::BoxModel::nodeRemoved, this, [ctrl](const QString& id) {
        if(!ctrl->localIsOwner())
            return;
        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveNode);
        msg.string8(ctrl->uuid());
        msg.string8(id);
        msg.sendToServer();
    });

    connect(linkModel, &mindmap::LinkModel::linkRemoved, this, [ctrl](const QString& id) {
        if(!ctrl->localIsOwner())
            return;
        NetworkMessageWriter msg(NetMsg::MindMapCategory, NetMsg::RemoveLink);
        msg.string8(ctrl->uuid());
        msg.string8(id);
        msg.sendToServer();
    });
}

NetWorkReceiver::SendType MindMapUpdater::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddNode)
    {
        auto id= msg->string8();
        auto ctrls= m_mindmaps->contentController<MindMapController*>();
        auto it= std::find_if(ctrls.begin(), ctrls.end(), [id](MindMapController* ctrl) { return id == ctrl->uuid(); });
        if(it != ctrls.end())
            MessageHelper::readAddMindMapNode((*it), msg);
    }
    return NetWorkReceiver::NONE;
}
