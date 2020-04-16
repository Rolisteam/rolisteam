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

#include "controller/view_controller/imagecontroller.h"
#include "data/cleveruri.h"
#include "worker/messagehelper.h"

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

NetWorkReceiver::SendType ImageMediaController::processMessage(NetworkMessageReader* msg)
{
    if(msg->category() == NetMsg::MediaCategory && msg->action() == NetMsg::AddMedia)
    {
        QPixmap pix;
        auto uri= MessageHelper::readImageData(msg, pix);
        addImageController(uri, pix);
    }
    return NetWorkReceiver::NONE;
}

void ImageMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_images.begin(), m_images.end(),
                            [id](const std::unique_ptr<ImageController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_images.end())
        return;

    (*it)->aboutToClose();
    m_images.erase(it, m_images.end());
}

bool ImageMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(uri == nullptr || (args.empty() && uri->getUri().isEmpty()))
        return false;

    auto it= args.find(QStringLiteral("pixmap"));
    QPixmap pix;
    if(it != args.end())
        pix= it->second.value<QPixmap>();

    auto imgCtrl= addImageController(uri, pix);
    MessageHelper::sendOffImage(imgCtrl);
    return true;
}

ImageController* ImageMediaController::addImageController(CleverURI* uri, const QPixmap& pix)
{
    std::unique_ptr<ImageController> imgCtrl(new ImageController(uri, pix));
    emit imageControllerCreated(imgCtrl.get());
    auto img= imgCtrl.get();

    connect(this, &ImageMediaController::localIsGMChanged, imgCtrl.get(), &ImageController::setLocalGM);

    m_images.push_back(std::move(imgCtrl));
    return img;
}

void ImageMediaController::addImage(const QPixmap& image)
{
    addImageController(new CleverURI(CleverURI::PICTURE), image);
}
