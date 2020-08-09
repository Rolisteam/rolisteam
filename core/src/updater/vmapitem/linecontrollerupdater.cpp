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
#include "linecontrollerupdater.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "network/networkmessagereader.h"
#include "vmap/controller/linecontroller.h"
#include "worker/convertionhelper.h"

LineControllerUpdater::LineControllerUpdater() {}

void LineControllerUpdater::addLineController(vmap::LineController* ctrl)
{
    if(nullptr == ctrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(ctrl, &vmap::LineController::endPointEditFinished, this,
            [this, ctrl]() { sendOffVMapChanges<QPointF>(ctrl, QStringLiteral("endPoint")); });
    connect(ctrl, &vmap::LineController::startPointEditFinished, this,
            [this, ctrl]() { sendOffVMapChanges<QPointF>(ctrl, QStringLiteral("startPoint")); });
    connect(ctrl, &vmap::LineController::penWidthChanged, this,
            [this, ctrl]() { sendOffVMapChanges<quint16>(ctrl, QStringLiteral("penWidth")); });
}

bool LineControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
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

    if(property == QStringLiteral("endPoint"))
    {
        auto x= msg->real();
        auto y= msg->real();
        var= QVariant::fromValue(QPointF(x, y));
    }
    else if(property == QStringLiteral("startPoint"))
    {
        auto x= msg->real();
        auto y= msg->real();
        var= QVariant::fromValue(QPointF(x, y));
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
