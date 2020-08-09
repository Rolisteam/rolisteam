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
#include "rectcontrollerupdater.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/rectcontroller.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

RectControllerUpdater::RectControllerUpdater(QObject* parent) : VMapItemControllerUpdater(parent) {}

RectControllerUpdater::~RectControllerUpdater() {}

void RectControllerUpdater::addItemController(vmap::VisualItemController* ctrl)
{
    if(nullptr == ctrl)
        return;

    auto rectCtrl= dynamic_cast<vmap::RectController*>(ctrl);

    if(nullptr == rectCtrl)
        return;

    VMapItemControllerUpdater::addItemController(rectCtrl);

    connect(rectCtrl, &vmap::RectController::filledChanged, this,
            [this, rectCtrl]() { sendOffVMapChanges<bool>(rectCtrl, QStringLiteral("filled")); });
    connect(rectCtrl, &vmap::RectController::rectEditFinished, this,
            [this, rectCtrl]() { sendOffVMapChanges<QRectF>(rectCtrl, QStringLiteral("rect")); });
    connect(rectCtrl, &vmap::RectController::penWidthChanged, this,
            [this, rectCtrl]() { sendOffVMapChanges<quint16>(rectCtrl, QStringLiteral("penWidth")); });

    if(!ctrl->remote())
        connect(rectCtrl, &vmap::RectController::initializedChanged, this,
                [rectCtrl]() { MessageHelper::sendOffRect(rectCtrl, rectCtrl->mapUuid()); });
}

bool RectControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
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
    else if(property == QStringLiteral("rect"))
    {
        auto x= msg->real();
        auto y= msg->real();
        auto w= msg->real();
        auto h= msg->real();
        var= QVariant::fromValue(QRectF(x, y, w, h));
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
