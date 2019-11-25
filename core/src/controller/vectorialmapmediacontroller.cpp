/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "vectorialmapmediacontroller.h"
#include "network/networkmessagereader.h"
#include "network/receiveevent.h"

#include "vmap/vmapframe.h"

VectorialMapMediaController::VectorialMapMediaController() {}

CleverURI::ContentType VectorialMapMediaController::type() const
{
    return CleverURI::VMAP;
}

void VectorialMapMediaController::registerNetworkReceiver()
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

/*CleverURI* findMedia(QString uuid, const std::vector<CleverURI*>& media)
{
    return nullptr;
}*/

NetWorkReceiver::SendType VectorialMapMediaController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    switch(msg->action())
    {
    case NetMsg::loadVmap:
        break;
    case NetMsg::closeVmap:
    {
        QString vmapId= msg->string8();
        clodeMedia(vmapId);
    }
    break;
    case NetMsg::addVmap:
    case NetMsg::DelPoint:
        break;
    case NetMsg::AddItem:
    case NetMsg::DelItem:
    case NetMsg::MoveItem:
    case NetMsg::GeometryItemChanged:
    case NetMsg::OpacityItemChanged:
    case NetMsg::LayerItemChanged:
    case NetMsg::MovePoint:
    case NetMsg::vmapChanges:
    case NetMsg::GeometryViewChanged:
    case NetMsg::SetParentItem:
    case NetMsg::RectGeometryItem:
    case NetMsg::RotationItem:
    case NetMsg::CharacterStateChanged:
    case NetMsg::CharacterChanged:
    case NetMsg::VisionChanged:
    case NetMsg::ColorChanged:
    case NetMsg::ZValueItem:
    {
        QString vmapId= msg->string8();
        // CleverURI* tmp= findMedia(vmapId, m_openMedia);
        //  if(nullptr != tmp)
        {
            /*VMapFrame* mapF= dynamic_cast<VMapFrame*>(tmp);
            if(nullptr != mapF)
            {
                type= mapF->processMessage(msg);
            }*/
        }
    }
    break;
    default:
        qWarning("Unexpected Action - MainWindow::processVMapMessage");
        break;
    }

    return type;
}

void VectorialMapMediaController::clodeMedia(const QString& id)
{
    m_openMedia.erase(std::remove_if(m_openMedia.begin(), m_openMedia.end(),
                                     [id](const CleverURI* uri) { return uri->name() == id; }));
}

bool VectorialMapMediaController::openMedia(CleverURI* uri)
{
    // m_openMedia.push_back(uri);
}
