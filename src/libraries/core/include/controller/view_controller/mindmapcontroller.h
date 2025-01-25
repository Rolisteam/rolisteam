/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#ifndef MINDMAPCONTROLLER_H
#define MINDMAPCONTROLLER_H

#include <QAbstractItemModel>
#include <QPointer>
#include <QRectF>
#include <QUndoStack>
#include <core_global.h>
#include <memory>

#include "controller/view_controller/mindmapcontrollerbase.h"
#include "model/playermodel.h"
#include "model/remoteplayermodel.h"

class QQuickImageProvider;
class MindMapUpdater;

namespace mindmap
{
class MindItemModel;
class LinkModel;
class LinkController;
class MindNode;
class SpacingController;
class SelectionController;
class NodeStyleModel;
class NodeStyle;
class NodeImageProvider;
} // namespace mindmap

class CORE_EXPORT MindMapController : public mindmap::MindMapControllerBase
{
    Q_OBJECT
    Q_PROPERTY(RemotePlayerModel* remotePlayerModel READ remotePlayerModel CONSTANT)
    Q_PROPERTY(PlayerModel* playerModel READ playerModel CONSTANT)
    Q_PROPERTY(Core::SharingPermission sharingToAll READ sharingToAll NOTIFY sharingToAllChanged)
public:
    explicit MindMapController(const QString& id, QObject* parent= nullptr);
    ~MindMapController();

    RemotePlayerModel* remotePlayerModel() const;
    PlayerModel* playerModel() const;
    bool readWrite() const override;
    Core::SharingPermission sharingToAll() const;

    static void setRemotePlayerModel(RemotePlayerModel* model);
    static void setMindMapUpdater(MindMapUpdater* updater);

signals:
    void sharingToAllChanged(Core::SharingPermission newPerm, Core::SharingPermission formerPerm);
    void recipientsChanged();
    void openMindMapTo(QString id);
    void closeMindMapTo(QString id);
    void permissionChangedForUser(QString id, Core::SharingPermission perm);

public slots:
    void setSharingToAll(int perm);
    void setPermissionForUser(const QString& userId, int perm);
    void generateTree();

private:
    static QPointer<RemotePlayerModel> m_remotePlayerModel;
    static QPointer<MindMapUpdater> m_updater;
    Core::SharingPermission m_sharingToAll= Core::SharingPermission::None;
    QHash<QString, Core::SharingPermission> m_permissions;
};

#endif // MINDMAPCONTROLLER_H
