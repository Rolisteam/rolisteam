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
#include "characteritemupdater.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "network/networkmessagereader.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

CharacterItemUpdater::CharacterItemUpdater(QObject* parent) : VMapItemControllerUpdater(parent) {}

void CharacterItemUpdater::addItemController(vmap::VisualItemController* ctrl)
{
    if(nullptr == ctrl)
        return;

    auto itemCtrl= dynamic_cast<vmap::CharacterItemController*>(ctrl);

    if(nullptr == itemCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(itemCtrl, &vmap::CharacterItemController::sideChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<qreal>(itemCtrl, QStringLiteral("side")); });
    connect(itemCtrl, &vmap::CharacterItemController::stateColorChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QColor>(itemCtrl, QStringLiteral("stateColor")); });
    connect(itemCtrl, &vmap::CharacterItemController::numberChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QColor>(itemCtrl, QStringLiteral("number")); });
    connect(itemCtrl, &vmap::CharacterItemController::playableCharacterChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<bool>(itemCtrl, QStringLiteral("playableCharacter")); });
    connect(itemCtrl, &vmap::CharacterItemController::thumnailRectChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QRectF>(itemCtrl, QStringLiteral("thumnailRect")); });
    connect(itemCtrl, &vmap::CharacterItemController::visionShapeChanged, this, [this, itemCtrl]() {
        sendOffVMapChanges<CharacterVision::SHAPE>(itemCtrl, QStringLiteral("visionShape"));
    });
    connect(itemCtrl, &vmap::CharacterItemController::fontChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QFont>(itemCtrl, QStringLiteral("font")); });

    if(!ctrl->remote())
        MessageHelper::sendOffCharacter(itemCtrl, ctrl->mapUuid());
}

bool CharacterItemUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
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

    if(property == QStringLiteral("side"))
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == QStringLiteral("stateColor"))
    {
        var= QVariant::fromValue(QColor(msg->rgb()));
    }
    else if(property == QStringLiteral("number"))
    {
        var= QVariant::fromValue(msg->uint64());
    }
    else if(property == QStringLiteral("playableCharacter"))
    {
        var= QVariant::fromValue(msg->uint8());
    }
    else if(property == QStringLiteral("thumnailRect"))
    {
        auto x= msg->real();
        auto y= msg->real();
        auto w= msg->real();
        auto h= msg->real();
        var= QVariant::fromValue(QRectF(x, y, w, h));
    }
    else if(property == QStringLiteral("visionShape"))
    {
        var= QVariant::fromValue(static_cast<CharacterVision::SHAPE>(msg->uint8()));
    }
    else if(property == QStringLiteral("font"))
    {
        auto str= msg->string8();
        var= QVariant::fromValue(QFont(str));
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
