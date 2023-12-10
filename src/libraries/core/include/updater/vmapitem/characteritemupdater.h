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
#ifndef CHARACTERITEMUPDATER_H
#define CHARACTERITEMUPDATER_H

#include "vmapitemcontrollerupdater.h"
#include <QObject>
#include <core_global.h>
#include "controller/item_controllers/characteritemcontroller.h"

class CORE_EXPORT CharacterItemUpdater : public VMapItemControllerUpdater
{
    Q_OBJECT
public:
    explicit CharacterItemUpdater(QObject* parent= nullptr);

    void addItemController(vmap::VisualItemController* ctrl) override;
    bool updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl) override;
    bool updateVisionProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl);

    template <typename T>
    void sendOffVisionChanges(vmap::CharacterItemController* ctrl, const QString& property);
};


template <typename T>
void CharacterItemUpdater::sendOffVisionChanges(vmap::CharacterItemController* ctrl,  const QString& property)
{
    if(nullptr == ctrl || property.isEmpty() || !m_synchronized || (m_updatingFromNetwork && updatingCtrl == ctrl)
       || !ctrl->initialized())
        return;

    CharacterVision* vision = ctrl->vision();

    if(!vision)
        return;

    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::CharacterVisionChanged);
    msg.string8(ctrl->mapUuid());
    msg.uint8(ctrl->itemType());
    msg.string8(ctrl->uuid());
    msg.string16(property);
    auto val= vision->property(property.toLocal8Bit().data());
    Helper::variantToType<T>(val.value<T>(), msg);
    msg.sendToServer();
}

#endif // CHARACTERITEMUPDATER_H
