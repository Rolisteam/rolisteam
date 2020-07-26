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
#include "model/singlecontenttypemodel.h"
#include "undoCmd/removemediacontrollercommand.h"
#include "worker/iohelper.h"

ImageMediaController::ImageMediaController(ContentModel* contentModel)
    : MediaManagerBase(Core::ContentType::PICTURE, contentModel)
{
}

ImageMediaController::~ImageMediaController()= default;

void ImageMediaController::registerNetworkReceiver() {}

ImageController* ImageMediaController::addImageController(const QString& id, const QString& path, const QString& name,
                                                          const QByteArray& pix, const QByteArray& serializedData)
{
    auto imgCtrl= new ImageController(id, name, path, pix);
    if(!serializedData.isEmpty())
    {
        IOHelper::readImageController(imgCtrl, serializedData);
    }
    emit imageControllerCreated(imgCtrl);
    emit mediaAdded(imgCtrl->uuid(), imgCtrl->path(), type(), imgCtrl->name());

    connect(this, &ImageMediaController::localIsGMChanged, imgCtrl, &ImageController::setLocalGM);
   /* connect(imgCtrl, &ImageController::closeMe, this, [this, imgCtrl]() {
        if(!m_undoStack)
            return;
        m_undoStack->push(new RemoveMediaControllerCommand(imgCtrl, this));
    });*/

    // m_images.push_back(std::move(imgCtrl));
    return imgCtrl;
}

void ImageMediaController::addImage(const QPixmap& image)
{
    // addImageController(, image);
}

int ImageMediaController::managerCount() const
{
    return m_model->rowCount();
}

std::vector<ImageController*> ImageMediaController::controllers() const
{
    std::vector<ImageController*> vec;
    /*std::transform(m_images.begin(), m_images.end(), std::back_inserter(vec),
                   [](const std::unique_ptr<ImageController>& ctrl) { return ctrl.get(); });*/
    return vec;
}
