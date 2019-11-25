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
#include "imagemediacontroller.h"

#include "data/cleveruri.h"

CleverURI* findMedia(QString uuid, const std::vector<CleverURI*>& media)
{
    auto it= std::find_if(media.begin(), media.end(), [uuid](const CleverURI* uri) {
        if(nullptr == uri)
            return false;
        return uri->uuid() == uuid;
    });
    return (*it);
}

ImageMediaController::ImageMediaController() {}

CleverURI::ContentType ImageMediaController::type() const
{
    return CleverURI::PICTURE;
}

void ImageMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

NetWorkReceiver::SendType ImageMediaController::processMessage(NetworkMessageReader*) {}

void ImageMediaController::clodeMedia(const QString& id)
{
    m_media.erase(
        std::remove_if(m_media.begin(), m_media.end(), [id](const CleverURI* uri) { return uri->name() == id; }));
}

bool ImageMediaController::openMedia(CleverURI* uri)
{
    m_media.push_back(uri);

    return true;
}
