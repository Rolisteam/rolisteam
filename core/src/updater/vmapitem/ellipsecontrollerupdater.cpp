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
#include "ellipsecontrollerupdater.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/ellipsecontroller.h"
#include "worker/convertionhelper.h"

EllipseControllerUpdater::EllipseControllerUpdater(QObject* parent) : VMapItemControllerUpdater(parent) {}

EllipseControllerUpdater::~EllipseControllerUpdater() {}

void EllipseControllerUpdater::addEllipseController(vmap::EllipseController* ctrl)
{
    if(nullptr == ctrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(ctrl, &vmap::EllipseController::filledChanged, this,
            [this, ctrl]() { sendOffVMapChanges<bool>(ctrl, QStringLiteral("filled")); });
    connect(ctrl, &vmap::EllipseController::ryEditionChanged, this,
            [this, ctrl]() { sendOffVMapChanges<qreal>(ctrl, QStringLiteral("ry")); });
    connect(ctrl, &vmap::EllipseController::rxEditionChanged, this,
            [this, ctrl]() { sendOffVMapChanges<qreal>(ctrl, QStringLiteral("rx")); });
    connect(ctrl, &vmap::EllipseController::penWidthChanged, this,
            [this, ctrl]() { sendOffVMapChanges<quint16>(ctrl, QStringLiteral("penWidth")); });
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
