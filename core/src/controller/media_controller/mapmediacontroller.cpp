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

#include "controller/view_controller/mapcontroller.h"

MapMediaController::MapMediaController(QObject* parent) : MediaControllerInterface(parent) {}

MapMediaController::~MapMediaController() {}

CleverURI::ContentType MapMediaController::type() const
{
    return CleverURI::MAP;
}

bool MapMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    std::unique_ptr<MapController> mapCtrl(new MapController(uri, args));

    emit mapControllerCreated(mapCtrl.get());
    m_maps.push_back(std::move(mapCtrl));
    return true;
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
    return NetWorkReceiver::NONE;
}

void MapMediaController::setUndoStack(QUndoStack* stack) {}

void MapMediaController::addMapFromImage(const QPixmap& map) {}
