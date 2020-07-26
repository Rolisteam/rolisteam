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
#include "vmapupdater.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "network/networkmessagewriter.h"
#include "worker/convertionhelper.h"

VMapUpdater::VMapUpdater(QObject* parent) : MediaUpdaterInterface(parent) {}

void VMapUpdater::addController(VectorialMapController* ctrl)
{
    if(nullptr == ctrl)
        return;

    connect(ctrl, &VectorialMapController::collisionChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("collision")); });
    connect(ctrl, &VectorialMapController::characterVisionChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("characterVision")); });
    connect(ctrl, &VectorialMapController::gridColorChanged, this,
            [this, ctrl]() { sendOffChanges<QColor>(ctrl, QStringLiteral("gridColor")); });
    connect(ctrl, &VectorialMapController::gridScaleChanged, this,
            [this, ctrl]() { sendOffChanges<qreal>(ctrl, QStringLiteral("gridScale")); });
    connect(ctrl, &VectorialMapController::gridSizeChanged, this,
            [this, ctrl]() { sendOffChanges<int>(ctrl, QStringLiteral("gridSize")); });
    connect(ctrl, &VectorialMapController::gridVisibilityChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("gridVisibility")); });
    connect(ctrl, &VectorialMapController::gridAboveChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("gridAbove")); });
    connect(ctrl, &VectorialMapController::scaleUnitChanged, this,
            [this, ctrl]() { sendOffChanges<Core::ScaleUnit>(ctrl, QStringLiteral("scaleUnit")); });
    connect(ctrl, &VectorialMapController::permissionChanged, this,
            [this, ctrl]() { sendOffChanges<Core::PermissionMode>(ctrl, QStringLiteral("permission")); });
    connect(ctrl, &VectorialMapController::gridPatternChanged, this,
            [this, ctrl]() { sendOffChanges<Core::GridPattern>(ctrl, QStringLiteral("gridPattern")); });
    connect(ctrl, &VectorialMapController::visibilityChanged, this,
            [this, ctrl]() { sendOffChanges<Core::VisibilityMode>(ctrl, QStringLiteral("visibility")); });
    connect(ctrl, &VectorialMapController::backgroundColorChanged, this,
            [this, ctrl]() { sendOffChanges<QColor>(ctrl, QStringLiteral("backgroundColor")); });
    connect(ctrl, &VectorialMapController::layerChanged, this,
            [this, ctrl]() { sendOffChanges<Core::Layer>(ctrl, QStringLiteral("layer")); });
    connect(ctrl, &VectorialMapController::zIndexChanged, this,
            [this, ctrl]() { sendOffChanges<int>(ctrl, QStringLiteral("zIndex")); });
}

bool VMapUpdater::updateVMapProperty(NetworkMessageReader* msg, VectorialMapController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;
    QSet<QString> boolProperties(
        {QString("collision"), QString("characterVision"), QString("gridVisibility"), QString("gridAbove")});
    QSet<QString> colorProperties({QString("backgroundColor"), QString("gridColor")});
    QSet<QString> uint8Properties(
        {QString("scaleUnit"), QString("permission"), QString("gridPattern"), QString("visibility"), QString("layer")});
    QSet<QString> intProperties({QString("zIndex"), QString("gridSize")});
    QSet<QString> realProperties({QString("gridScale")});

    if(boolProperties.contains(property))
    {
        var= QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(colorProperties.contains(property))
    {
        var= QVariant::fromValue(QColor(msg->rgb()));
    }
    else if(intProperties.contains(property))
    {
        var= QVariant::fromValue(msg->int64());
    }
    else if(realProperties.contains(property))
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(uint8Properties.contains(property))
    {
        auto value= msg->uint8();
        if(property == QStringLiteral("scaleUnit"))
        {
            var= QVariant::fromValue(static_cast<Core::ScaleUnit>(value));
        }
        else if(property == QStringLiteral("scaleUnit"))
        {
            var= QVariant::fromValue(static_cast<Core::ScaleUnit>(value));
        }
        else if(property == QStringLiteral("permission"))
        {
            var= QVariant::fromValue(static_cast<Core::PermissionMode>(value));
        }
        else if(property == QStringLiteral("gridPattern"))
        {
            var= QVariant::fromValue(static_cast<Core::GridPattern>(value));
        }
        else if(property == QStringLiteral("visibility"))
        {
            var= QVariant::fromValue(static_cast<Core::VisibilityMode>(value));
        }
        else if(property == QStringLiteral("layer"))
        {
            var= QVariant::fromValue(static_cast<Core::Layer>(value));
        }
    }
    else if(uint8Properties.contains(property))
    {
        var= QVariant::fromValue(msg->int64());
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
