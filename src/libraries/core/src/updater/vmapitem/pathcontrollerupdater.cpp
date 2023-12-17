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
#include "updater/vmapitem/pathcontrollerupdater.h"

#include <QVariant>

#include "controller/item_controllers/pathcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/messagehelper.h"

PathControllerUpdater::PathControllerUpdater() {}

void PathControllerUpdater::addItemController(vmap::VisualItemController* ctrl, bool sendOff)
{
    if(nullptr == ctrl)
        return;

    auto pathCtrl= dynamic_cast<vmap::PathController*>(ctrl);

    if(nullptr == pathCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(pathCtrl, &vmap::PathController::pointCountChanged, this,
            [this, pathCtrl]() { sendOffVMapChanges<std::vector<QPointF>>(pathCtrl, QStringLiteral("points")); });
    connect(pathCtrl, &vmap::PathController::penWidthChanged, this,
            [this, pathCtrl]() { sendOffVMapChanges<quint16>(pathCtrl, QStringLiteral("penWidth")); });
    connect(pathCtrl, &vmap::PathController::closedChanged, this,
            [this, pathCtrl]() { sendOffVMapChanges<bool>(pathCtrl, QStringLiteral("closed")); });
    connect(pathCtrl, &vmap::PathController::filledChanged, this,
            [this, pathCtrl]() { sendOffVMapChanges<bool>(pathCtrl, QStringLiteral("filled")); });

    connect(pathCtrl, &vmap::PathController::pointPositionEditFinished, this,
            [pathCtrl](int idx, const QPointF& pos)
            {
                if(idx < 0)
                    return;
                NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::MovePoint);
                msg.string8(pathCtrl->mapUuid());
                msg.uint8(pathCtrl->itemType());
                msg.string8(pathCtrl->uuid());
                msg.int64(idx);
                msg.real(pos.x());
                msg.real(pos.y());
                msg.sendToServer();
            });

    if(!ctrl->remote() && sendOff)
    {
        if(pathCtrl->penLine())
            connect(pathCtrl, &vmap::PathController::initializedChanged, this,
                    [pathCtrl]() { MessageHelper::sendOffPath(pathCtrl, pathCtrl->mapUuid()); });
        else
            MessageHelper::sendOffPath(pathCtrl, pathCtrl->mapUuid());
    }
}

bool PathControllerUpdater::movePoint(NetworkMessageReader* msg, vmap::PathController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    auto idx= msg->int64();
    auto x= msg->real();
    auto y= msg->real();

    ctrl->setPoint(QPointF(x, y), static_cast<int>(idx));
    return true;
}

bool PathControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    // TODO implement save/restore
    auto datapos= msg->pos();

    if(VMapItemControllerUpdater::updateItemProperty(msg, ctrl))
        return true;

    msg->resetToPos(datapos);

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;
    if(property == QStringLiteral("penWidth"))
    {
        var= QVariant::fromValue(msg->uint16());
    }
    else if(property == QStringLiteral("closed"))
    {
        var= QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(property == QStringLiteral("filled"))
    {
        var= QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(property == QStringLiteral("points"))
    {
        auto count= msg->int64();
        std::vector<QPointF> points;
        points.reserve(static_cast<std::size_t>(count));
        for(int i= 0; i < count; ++i)
        {
            auto x= msg->real();
            auto y= msg->real();
            points.push_back(QPointF(x, y));
        }
        var= QVariant::fromValue(points);
    }
    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}
