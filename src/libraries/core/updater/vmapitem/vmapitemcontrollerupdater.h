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
#ifndef MEDIAUPDATER_H
#define MEDIAUPDATER_H

#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QRectF>

#include "controller/item_controllers/visualitemcontroller.h"
#include "network/networkmessagewriter.h"
#include "worker/convertionhelper.h"

namespace vmap
{
class VisualItemController;
}
class NetworkMessageReader;
class VMapItemControllerUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool synchronized READ synchronized WRITE setSynchronized NOTIFY synchronizedChanged)
public:
    explicit VMapItemControllerUpdater(QObject* parent= nullptr);

    virtual void addItemController(vmap::VisualItemController* ctrl);
    virtual bool updateItemProperty(NetworkMessageReader* msg, vmap::VisualItemController* ctrl);

    template <typename T>
    void sendOffVMapChanges(vmap::VisualItemController* ctrl, const QString& property);
    bool synchronized() const;

public slots:
    void setSynchronized(bool);

signals:
    void synchronizedChanged(bool);

protected:
    bool m_updatingFromNetwork= false;
    vmap::VisualItemController* updatingCtrl= nullptr;
    bool m_synchronized= true;
};

template <typename T>
void VMapItemControllerUpdater::sendOffVMapChanges(vmap::VisualItemController* ctrl, const QString& property)
{
    if(nullptr == ctrl || property.isEmpty() || !m_synchronized || (m_updatingFromNetwork && updatingCtrl == ctrl)
       || !ctrl->initialized())
        return;

    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::UpdateItem);
    msg.string8(ctrl->mapUuid());
    msg.uint8(ctrl->itemType());
    msg.string8(ctrl->uuid());
    msg.string16(property);
    auto val= ctrl->property(property.toLocal8Bit().data());
    Helper::variantToType<T>(val.value<T>(), msg);
    msg.sendToServer();
}
#endif // MEDIAUPDATER_H
