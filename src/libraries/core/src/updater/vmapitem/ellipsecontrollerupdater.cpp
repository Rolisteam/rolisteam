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
#include "updater/vmapitem/ellipsecontrollerupdater.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "controller/item_controllers/ellipsecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

EllipseControllerUpdater::EllipseControllerUpdater(QObject* parent) : VMapItemControllerUpdater(parent) {}

EllipseControllerUpdater::~EllipseControllerUpdater() {}

void EllipseControllerUpdater::addItemController(vmap::VisualItemController* ctrl, bool sendOff)
{
    if(nullptr == ctrl)
        return;

    auto elliCtrl= dynamic_cast<vmap::EllipseController*>(ctrl);

    if(nullptr == elliCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(elliCtrl, &vmap::EllipseController::filledChanged, this,
            [this, elliCtrl]() { sendOffVMapChanges<bool>(elliCtrl, QStringLiteral("filled")); });
    connect(elliCtrl, &vmap::EllipseController::ryEditionChanged, this,
            [this, elliCtrl]() { sendOffVMapChanges<qreal>(elliCtrl, QStringLiteral("ry")); });
    connect(elliCtrl, &vmap::EllipseController::rxEditionChanged, this,
            [this, elliCtrl]() { sendOffVMapChanges<qreal>(elliCtrl, QStringLiteral("rx")); });
    connect(elliCtrl, &vmap::EllipseController::penWidthChanged, this,
            [this, elliCtrl]() { sendOffVMapChanges<quint16>(elliCtrl, QStringLiteral("penWidth")); });

    if(!ctrl->remote() && sendOff)
        connect(elliCtrl, &vmap::EllipseController::initializedChanged, this,
                [elliCtrl]() { MessageHelper::sendOffEllispe(elliCtrl, elliCtrl->mapUuid()); });
}

bool EllipseControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
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

    if(property == QStringLiteral("filled"))
    {
        var= QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(property == QStringLiteral("rx"))
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == QStringLiteral("ry"))
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == QStringLiteral("penWidth"))
    {
        var= QVariant::fromValue(msg->int64());
    }
    else
    {
        return false;
    }

    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}
