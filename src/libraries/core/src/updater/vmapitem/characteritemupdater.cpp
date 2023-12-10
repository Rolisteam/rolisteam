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
#include "updater/vmapitem/characteritemupdater.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "network/networkmessagereader.h"
#include "worker/convertionhelper.h"
#include "worker/messagehelper.h"

namespace properties
{
constexpr auto side{"side"};
constexpr auto stateColor{"stateColor"};
constexpr auto number{"number"};
constexpr auto playableCharacter{"playableCharacter"};
constexpr auto thumnailRect{"thumnailRect"};
constexpr auto visionShape{"visionShape"};
constexpr auto font{"font"};

constexpr auto vision_radius{"radius"};
constexpr auto vision_angle{"angle"};
constexpr auto vision_rotation{"rotation"};
constexpr auto vision_position{"position"};
//constexpr auto vision_shape{"shape"};
constexpr auto vision_path{"path"};
constexpr auto vision_removed{"removed"};

}

CharacterItemUpdater::CharacterItemUpdater(QObject* parent) : VMapItemControllerUpdater(parent) {}

void CharacterItemUpdater::addItemController(vmap::VisualItemController* ctrl)
{
    if(nullptr == ctrl)
        return;

    auto itemCtrl= dynamic_cast<vmap::CharacterItemController*>(ctrl);

    if(nullptr == itemCtrl)
        return;

    VMapItemControllerUpdater::addItemController(ctrl);

    connect(itemCtrl, &vmap::CharacterItemController::sideEdited, this,
            [this, itemCtrl]() { sendOffVMapChanges<qreal>(itemCtrl, properties::side); });
    connect(itemCtrl, &vmap::CharacterItemController::stateColorChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QColor>(itemCtrl, properties::stateColor); });
    connect(itemCtrl, &vmap::CharacterItemController::numberChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QColor>(itemCtrl, properties::number); });
    connect(itemCtrl, &vmap::CharacterItemController::playableCharacterChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<bool>(itemCtrl, properties::playableCharacter); });
    connect(itemCtrl, &vmap::CharacterItemController::visionShapeChanged, this,
            [this, itemCtrl]()
            { sendOffVMapChanges<CharacterVision::SHAPE>(itemCtrl, properties::visionShape); });
    connect(itemCtrl, &vmap::CharacterItemController::fontChanged, this,
            [this, itemCtrl]() { sendOffVMapChanges<QFont>(itemCtrl, properties::font); });
    connect(itemCtrl, &vmap::CharacterItemController::rectEditFinished, this,
            [this, itemCtrl]() { sendOffVMapChanges<QRectF>(itemCtrl, properties::thumnailRect); });

    {//vision
        auto vision = itemCtrl->vision();

        connect(vision, &CharacterVision::radiusEdited, this, [this, itemCtrl](){
            sendOffVisionChanges<qreal>(itemCtrl,properties::vision_radius);
        });
        connect(vision, &CharacterVision::angleEdited, this, [this, itemCtrl](){
            sendOffVisionChanges<qreal>(itemCtrl,properties::vision_angle);
        });
        connect(vision, &CharacterVision::rotationEdited, this, [this, itemCtrl](){
            sendOffVisionChanges<qreal>(itemCtrl,properties::vision_rotation);
        });
        connect(vision, &CharacterVision::positionEdited, this, [this, itemCtrl](){
            sendOffVisionChanges<QPointF>(itemCtrl,properties::vision_position);
        });
        connect(vision, &CharacterVision::pathEdited, this, [this, itemCtrl](){
            sendOffVisionChanges<QPainterPath>(itemCtrl,properties::vision_path);
        });
        connect(vision, &CharacterVision::removedChanged, this, [this, itemCtrl](){
            sendOffVisionChanges<bool>(itemCtrl,properties::vision_removed);
        });
    }

    connect(itemCtrl, &vmap::CharacterItemController::rectEditFinished, this,
            [this, itemCtrl]() { sendOffVMapChanges<QRectF>(itemCtrl, properties::thumnailRect); });

    if(!ctrl->remote())
        MessageHelper::sendOffCharacter(itemCtrl, ctrl->mapUuid());
}

bool CharacterItemUpdater::updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    qDebug() << "update Character Item property";
    if(nullptr == msg || nullptr == ctrl)
        return false;

    if(msg->action() == NetMsg::CharacterVisionChanged)
    {
        return updateVisionProperty(msg, ctrl);
    }

    // TODO implement save/restore
    auto datapos= msg->pos();

    if(VMapItemControllerUpdater::updateItemProperty(msg, ctrl))
        return true;

    msg->resetToPos(datapos);

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;

    if(property == properties::side)
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == properties::stateColor)
    {
        var= QVariant::fromValue(QColor(msg->rgb()));
    }
    else if(property == properties::number)
    {
        var= QVariant::fromValue(msg->uint64());
    }
    else if(property == properties::playableCharacter)
    {
        var= QVariant::fromValue(msg->uint8());
    }
    else if(property == properties::thumnailRect)
    {
        auto x= msg->real();
        auto y= msg->real();
        auto w= msg->real();
        auto h= msg->real();
        var= QVariant::fromValue(QRectF(x, y, w, h));
    }
    else if(property == properties::visionShape)
    {
        var= QVariant::fromValue(static_cast<CharacterVision::SHAPE>(msg->uint8()));
    }
    else if(property == properties::font)
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


bool CharacterItemUpdater::updateVisionProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    auto itemCtrl= dynamic_cast<vmap::CharacterItemController*>(ctrl);

    if(nullptr == itemCtrl)
        return false;

    auto vision = itemCtrl->vision();

    if(nullptr == vision)
        return false;

    auto property= msg->string16();

    QVariant var;

    if(property == properties::vision_radius)
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == properties::vision_angle)
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == properties::vision_rotation)
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(property == properties::vision_position)
    {
        auto x = msg->real();
        auto y = msg->real();
        var= QVariant::fromValue(QPointF{x,y});
    }
    else if(property == properties::vision_removed)
    {
        var = QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(property == properties::vision_path)
    {
        auto data= msg->byteArray32();
        {
            QDataStream read(&data, QIODevice::ReadOnly);
            QPainterPath path;
            read >> path;
            var = QVariant::fromValue(path);
        }
    }
    else
    {
        return false;
    }

    m_updatingFromNetwork= true;
    auto feedback= vision->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}
