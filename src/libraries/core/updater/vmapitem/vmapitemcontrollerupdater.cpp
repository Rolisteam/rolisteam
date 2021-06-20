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
#include "vmapitemcontrollerupdater.h"

#include <QFont>
#include <QSet>

#include "data/charactervision.h"
#include "network/networkmessagereader.h"

VMapItemControllerUpdater::VMapItemControllerUpdater(QObject* parent)
{
    // Force template generation
    /*sendOffVMapChanges<QRectF>(nullptr, "");
    sendOffVMapChanges<quint16>(nullptr, "");
    sendOffVMapChanges<QByteArray>(nullptr, "");
    sendOffVMapChanges<QFont>(nullptr, "");
    sendOffVMapChanges<QString>(nullptr, "");
    sendOffVMapChanges<std::vector<QPointF>>(nullptr, "");
    sendOffVMapChanges<CharacterVision::SHAPE>(nullptr, "");*/
}

void VMapItemControllerUpdater::addItemController(vmap::VisualItemController* ctrl)
{
    if(nullptr == ctrl)
        return;

    connect(ctrl, &vmap::VisualItemController::visibleChanged, this,
            [this, ctrl]() { sendOffVMapChanges<bool>(ctrl, QStringLiteral("visible")); });
    connect(ctrl, &vmap::VisualItemController::opacityChanged, this,
            [this, ctrl]() { sendOffVMapChanges<qreal>(ctrl, QStringLiteral("opacity")); });
    connect(ctrl, &vmap::VisualItemController::rotationEditFinished, this,
            [this, ctrl]() { sendOffVMapChanges<qreal>(ctrl, QStringLiteral("rotation")); });
    connect(ctrl, &vmap::VisualItemController::layerChanged, this,
            [this, ctrl]() { sendOffVMapChanges<Core::Layer>(ctrl, QStringLiteral("layer")); });
    connect(ctrl, &vmap::VisualItemController::posEditFinished, this,
            [this, ctrl]() { sendOffVMapChanges<QPointF>(ctrl, QStringLiteral("pos")); });
    connect(ctrl, &vmap::VisualItemController::colorChanged, this,
            [this, ctrl]() { sendOffVMapChanges<QColor>(ctrl, QStringLiteral("color")); });
    connect(ctrl, &vmap::VisualItemController::lockedChanged, this,
            [this, ctrl]() { sendOffVMapChanges<bool>(ctrl, QStringLiteral("locked")); });
}

bool VMapItemControllerUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;
    QSet<QString> boolProperties({QString("visible"), QString("locked")});
    QSet<QString> colorProperties({QString("color")});
    QSet<QString> uint8Properties({QString("layer")});
    QSet<QString> pointProperties({QString("pos")});
    QSet<QString> realProperties({QString("opacity"), QString("rotation")});

    if(boolProperties.contains(property))
    {
        var= QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(colorProperties.contains(property))
    {
        var= QVariant::fromValue(QColor(msg->rgb()));
    }
    else if(pointProperties.contains(property))
    {
        auto x= msg->real();
        auto y= msg->real();
        var= QVariant::fromValue(QPointF(x, y));
    }
    else if(realProperties.contains(property))
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(uint8Properties.contains(property))
    {
        auto value= msg->uint8();
        if(property == QStringLiteral("layer"))
        {
            var= QVariant::fromValue(static_cast<Core::Layer>(value));
        }
    }
    else
    {
        auto val= msg->string32();
        var= QVariant::fromValue(val);
    }

    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}

bool VMapItemControllerUpdater::synchronized() const
{
    return m_synchronized;
}

void VMapItemControllerUpdater::setSynchronized(bool b)
{
    if(b == m_synchronized)
        return;
    m_synchronized= b;
    emit synchronizedChanged(m_synchronized);
}
