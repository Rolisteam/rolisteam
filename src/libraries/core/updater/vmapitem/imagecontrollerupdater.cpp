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
#include "imagecontrollerupdater.h"

#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "controller/item_controllers/imagecontroller.h"
#include "network/networkmessagereader.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

ImageControllerUpdater::ImageControllerUpdater() {}

void ImageControllerUpdater::addItemController(vmap::VisualItemController* ctrl)
{
    if(nullptr == ctrl)
        return;

    auto imgCtrl= dynamic_cast<vmap::ImageController*>(ctrl);

    if(nullptr == imgCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(imgCtrl, &vmap::ImageController::dataChanged, this,
            [this, imgCtrl]() { sendOffVMapChanges<QByteArray>(imgCtrl, QStringLiteral("data")); });
    connect(imgCtrl, &vmap::ImageController::rectEditFinished, this,
            [this, imgCtrl]() { sendOffVMapChanges<QRectF>(imgCtrl, QStringLiteral("rect")); });

    if(!ctrl->remote())
        MessageHelper::sendOffImage(imgCtrl, imgCtrl->mapUuid());
}

bool ImageControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
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

    if(property == QStringLiteral("data"))
    {
        var= QVariant::fromValue(msg->byteArray32());
    }
    else if(property == QStringLiteral("rect"))
    {
        auto x= msg->real();
        auto y= msg->real();
        auto w= msg->real();
        auto h= msg->real();
        var= QVariant::fromValue(QRectF(x, y, w, h));
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
