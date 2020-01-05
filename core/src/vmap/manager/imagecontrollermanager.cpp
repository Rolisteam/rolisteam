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

ImageControllerManager::ImageControllerManager(VectorialMapController* ctrl) : m_ctrl(ctrl) {}

QString ImageControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::ImageController> image(new vmap::ImageController(params, m_ctrl));
    emit imageControllerCreated(image.get());
    auto id= image->uuid();
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
