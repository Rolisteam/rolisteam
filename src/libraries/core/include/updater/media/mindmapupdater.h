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

#include <QDebug>
#include <QMetaObject>
#include <QPointer>
#include <QVector>

#include "mediaupdaterinterface.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/data/packagenode.h"
#include "network/networkmessagewriter.h"
#include <core_global.h>

namespace mindmap
{
class MindNode;
}
class FilteredContentModel;
class MindMapController;
class NetworkMessageReader;
struct CORE_EXPORT ConnectionInfo
{
    QString id;
    QVector<QMetaObject::Connection> connections;
};
class CORE_EXPORT MindMapUpdater : public MediaUpdaterInterface
{
    Q_OBJECT
public:
    explicit MindMapUpdater(FilteredContentModel* model, campaign::CampaignManager* manager, QObject* parent= nullptr);

    void addMediaController(MediaControllerBase* base) override;

    bool updateSubobjectProperty(NetworkMessageReader* msg, MindMapController* ctrl);
    /* sendOffAddingMessage(const QString& idCtrl, const QList<mindmap::MindNode*>& nodes,
                              const QList<mindmap::LinkController*>& links);*/
    void sendOffRemoveMessage(const QString& idCtrl, const QStringList& nodeids, const QStringList& linksId);

    NetWorkReceiver::SendType processMessage(NetworkMessageReader* msg) override;

    template <typename T>
    void sendOffChange(const QString& mapId, const QString& property, QObject* node, bool b);

private:
    void setConnection(MindMapController* ctrl);
    void disconnectController(MindMapController* media);

    ConnectionInfo* findConnectionInfo(const QString& id);

private:
    QPointer<FilteredContentModel> m_mindmaps;
    QVector<ConnectionInfo> m_connections;
};

template <typename T>
void MindMapUpdater::sendOffChange(const QString& mapId, const QString& property, QObject* node, bool b)
{
    qDebug() << "networkmessageWriter Id " << mapId << property << node << b;
    if(!node)
        return;

    qDebug() << "networkmessageWriter Id " << mapId;
    NetworkMessageWriter msg(NetMsg::MindMapCategory, b ? NetMsg::UpdateNode : NetMsg::UpdateLink);
    msg.string8(mapId);
    auto id= node->property("id").toString();
    qDebug() << "id link" << id;
    msg.string8(id);
    msg.string16(property);
    auto val= node->property(property.toLocal8Bit().data());
    Helper::variantToType<T>(val.value<T>(), msg);
    msg.sendToServer();
}

#endif // MINDMAPUPDATER_H
