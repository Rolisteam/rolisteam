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

#include "controller/imagecontroller.h"
#include "data/cleveruri.h"

/*ImageController* findImage(QString uuid, const std::vector<std::unique_ptr<ImageController>>& images)
{
    auto it= std::find_if(images.begin(), images.end(), [uuid](const std::unique_ptr<ImageController>& image) {
        if(nullptr == image)
            return false;
        return image->uuid() == uuid;
    });
    return (*it).get();
}*/

ImageMediaController::ImageMediaController() {}

ImageMediaController::~ImageMediaController() {}

CleverURI::ContentType ImageMediaController::type() const
{
    return CleverURI::PICTURE;
}

void ImageMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

NetWorkReceiver::SendType ImageMediaController::processMessage(NetworkMessageReader*)
{
    return NetWorkReceiver::NONE;
}

void ImageMediaController::clodeMedia(const QString& id)
{
    auto it= std::remove_if(m_images.begin(), m_images.end(),
                            [id](const std::unique_ptr<ImageController>& uri) { return uri->uuid() == id; });
    if(it == m_images.end())
        return;
    m_images.erase(it, m_images.end());
}

bool ImageMediaController::openMedia(CleverURI* uri)
{
    std::unique_ptr<ImageController> imgCtrl(new ImageController(uri));
    emit imageControllerCreated(imgCtrl.get());
    m_images.push_back(std::move(imgCtrl));
    return true;
}
