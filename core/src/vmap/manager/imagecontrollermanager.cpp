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
#include "imagecontrollermanager.h"

#include <QVariant>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "worker/messagehelper.h"

ImageControllerManager::ImageControllerManager(VectorialMapController* ctrl)
    : m_ctrl(ctrl), m_updater(new ImageControllerUpdater)
{
    auto func= [this]() { m_updater->setSynchronized(m_ctrl->localGM() || m_ctrl->permission() == Core::PC_ALL); };
    connect(ctrl, &VectorialMapController::localGMChanged, this, func);
    connect(ctrl, &VectorialMapController::permissionChanged, this, func);
}

QString ImageControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::ImageController> image(new vmap::ImageController(params, m_ctrl));
    emit imageControllerCreated(image.get());
    auto id= image->uuid();
    emit itemAdded(id);
    prepareController(image.get());
    m_controllers.push_back(std::move(image));
    return id;
}

void ImageControllerManager::addController(vmap::VisualItemController* controller)
{
    auto image= dynamic_cast<vmap::ImageController*>(controller);
    if(nullptr == image)
        return;

    std::unique_ptr<vmap::ImageController> imageCtrl(image);
    emit imageControllerCreated(imageCtrl.get());
    emit itemAdded(imageCtrl->uuid());
    m_controllers.push_back(std::move(imageCtrl));
}

void ImageControllerManager::removeItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::ImageController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return;

    (*it)->aboutToBeRemoved();
    m_controllers.erase(it);
}

void ImageControllerManager::prepareController(vmap::ImageController* ctrl)
{
    auto id= m_ctrl->uuid();
    MessageHelper::sendOffImage(ctrl, id);
    m_updater->addImageController(ctrl);
}

vmap::ImageController* ImageControllerManager::findController(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::ImageController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return nullptr;

    return it->get();
}

void ImageControllerManager::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddItem && msg->category() == NetMsg::VMapCategory)
    {
        auto hash= MessageHelper::readImage(msg);
        auto newRect= new vmap::ImageController(hash, m_ctrl);
        addController(newRect);
    }
    else if(msg->action() == NetMsg::UpdateItem && msg->category() == NetMsg::VMapCategory)
    {
        auto id= msg->string8();
        auto ctrl= findController(id);
        if(nullptr != ctrl)
            m_updater->updateItemProperty(msg, ctrl);
    }
}

bool ImageControllerManager::loadItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::ImageController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return false;

    emit imageControllerCreated(it->get());
    return true;
}

const std::vector<vmap::ImageController*> ImageControllerManager::controllers() const
{
    std::vector<vmap::ImageController*> vect;
    std::transform(m_controllers.begin(), m_controllers.end(), std::back_inserter(vect),
                   [](const std::unique_ptr<vmap::ImageController>& ctrl) { return ctrl.get(); });
    return vect;
}
