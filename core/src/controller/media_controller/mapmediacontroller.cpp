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
#include "mapmediacontroller.h"

#include <QHash>
#include <QVariant>

#include "controller/view_controller/mapcontroller.h"
#include "updater/mapupdater.h"
#include "worker/messagehelper.h"

MapMediaController::MapMediaController(QObject* parent) : MediaControllerInterface(parent), m_updater(new MapUpdater) {}

MapMediaController::~MapMediaController() {}

QColor MapMediaController::fogColor() const
{
    return m_fogColor;
}

void MapMediaController::setFogColor(const QColor& color)
{
    if(m_fogColor == color)
        return;
    m_fogColor= color;
    emit fogColorChanged(color);
}

CleverURI::ContentType MapMediaController::type() const
{
    return CleverURI::MAP;
}

bool MapMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    QHash<QString, QVariant> hash(args.begin(), args.end());
    addMapController(uri, hash);
    return true;
}

void MapMediaController::addMapController(CleverURI* uri, const QHash<QString, QVariant>& params)
{
    std::unique_ptr<MapController> mapController(new MapController(uri));

    if(params.contains(QStringLiteral("permission")))
        mapController->setPermission(params.value(QStringLiteral("permission")).value<Core::PermissionMode>());

    if(params.contains(QStringLiteral("bgcolor")))
        mapController->setBgColor(params.value(QStringLiteral("bgcolor")).value<QColor>());

    if(params.contains(QStringLiteral("size")))
        mapController->setSize(params.value(QStringLiteral("size")).toSize());

    if(params.contains(QStringLiteral("hidden")))
        mapController->setHidden(params.value(QStringLiteral("hidden")).toBool());

    mapController->setFogColor(m_fogColor);

    // connect(mapController.get(), &MapController::openImageAs, this, &PdfMediaController::shareImageAs);

    connect(this, &MapMediaController::fogColorChanged, mapController.get(), &MapController::setFogColor);
    connect(this, &MapMediaController::localIsGMChanged, mapController.get(), &MapController::setLocalGM);

    m_updater->addController(mapController.get());

    emit mapControllerCreated(mapController.get());
    m_maps.push_back(std::move(mapController));
}

void MapMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_maps.begin(), m_maps.end(),
                            [id](const std::unique_ptr<MapController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_maps.end())
        return;

    (*it)->aboutToClose();
    m_maps.erase(it, m_maps.end());
}

void MapMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::MapCategory, this);
}

NetWorkReceiver::SendType MapMediaController::processMessage(NetworkMessageReader* msg)
{
    auto type= NetWorkReceiver::NONE;
    if(nullptr == msg)
        return type;

    if(msg->category() == NetMsg::MediaCategory && msg->action() == NetMsg::AddMedia)
    {
        auto data= MessageHelper::readMapData(msg);
        addMapController(new CleverURI(CleverURI::MAP), data);
    }
    else if(msg->action() == NetMsg::CloseMedia && msg->category() == NetMsg::MediaCategory)
    {
    }
    return type;
}

void MapMediaController::setUndoStack(QUndoStack* stack) {}

void MapMediaController::addMapFromImage(const QPixmap& map) {}
