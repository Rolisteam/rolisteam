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
#ifndef MINDMAPUPDATER_H
#define MINDMAPUPDATER_H

#include <QPointer>

#include "data/mindnode.h"
#include "mediaupdaterinterface.h"
#include "network/networkmessagewriter.h"

namespace mindmap
{
class MindNode;
}
class FilteredContentModel;
class MindMapController;
class NetworkMessageReader;
class MindMapUpdater : public MediaUpdaterInterface
{
    Q_OBJECT
public:
    explicit MindMapUpdater(FilteredContentModel* model, QObject* parent= nullptr);

    void addMediaController(MediaControllerBase* base) override;

    bool updateVMapProperty(NetworkMessageReader* msg, MindMapController* ctrl);
    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

    template <typename T>
    void sendOffChange(const QString& mapId, const QString& property, QObject* node, bool b);

private:
    QPointer<FilteredContentModel> m_mindmaps;
};

template <typename T>
void MindMapUpdater::sendOffChange(const QString& mapId, const QString& property, QObject* node, bool b)
{
    if(!node)
        return;

    NetworkMessageWriter msg(NetMsg::MindMapCategory, b ? NetMsg::UpdateNode : NetMsg::UpdateLink);
    msg.uint8(static_cast<int>(Core::ContentType::MINDMAP));
    msg.string8(mapId);
    auto id= node->property("id").toString();
    msg.string8(id);
    msg.string16(property);
    auto val= node->property(property.toLocal8Bit().data());
    Helper::variantToType<T>(val.value<T>(), msg);
    msg.sendToServer();
}

#endif // MINDMAPUPDATER_H
