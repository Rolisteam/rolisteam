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
#include "undoCmd/removemediacontrollercommand.h"
#include "worker/messagehelper.h"

ImageMediaController::ImageMediaController() : MediaManagerBase(Core::ContentType::PICTURE) {}

ImageMediaController::~ImageMediaController()= default;

void ImageMediaController::registerNetworkReceiver()
{
    // ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

NetWorkReceiver::SendType ImageMediaController::processMessage(NetworkMessageReader* msg)
{
    if(msg->category() == NetMsg::MediaCategory && msg->action() == NetMsg::AddMedia)
    {
        auto uri= MessageHelper::readImageData(msg);
        auto name= uri.value(QStringLiteral("name")).toString();
        auto uuid= uri.value(QStringLiteral("uuid")).toString();
        auto data= uri.value(QStringLiteral("data")).toByteArray();
        auto ctrl= addImageController(uuid, "", data);
        ctrl->setName(name);
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

bool ImageMediaController::openMedia(const QString& id, const std::map<QString, QVariant>& args)
{
    if(id == nullptr || args.empty())
        return false;

    auto it= args.find(QStringLiteral("data"));
    QByteArray pix;
    if(it != args.end())
        pix= it->second.value<QByteArray>();

    it= args.find(QStringLiteral("path"));

    QString path;
    if(it != args.end())
        path= it->second.toString();

    auto imgCtrl= addImageController(id, path, pix);

    QString name;
    it= args.find(QStringLiteral("name"));
    if(it != args.end())
        name= it->second.toString();

    imgCtrl->setName(name);

    MessageHelper::sendOffImage(imgCtrl);
    return true;
}

ImageController* ImageMediaController::addImageController(const QString& id, const QString& path, const QByteArray& pix)
{
    std::unique_ptr<ImageController> imgCtrl(new ImageController(id, path, pix));
    emit imageControllerCreated(imgCtrl.get());
    auto img= imgCtrl.get();

    connect(this, &ImageMediaController::localIsGMChanged, imgCtrl.get(), &ImageController::setLocalGM);
    connect(imgCtrl.get(), &ImageController::closeMe, this, [this, img]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(img, this));
    });

    m_images.push_back(std::move(imgCtrl));
    return img;
}

void ImageMediaController::addImage(const QPixmap& image)
{
    // addImageController(, image);
}
