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
#include "rectcontrollermanager.h"

#include <QDebug>
#include <QVariant>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "updater/vmaprectcontrollerupdater.h"
#include "vmap/controller/rectcontroller.h"
#include "worker/messagehelper.h"

RectControllerManager::RectControllerManager(VectorialMapController* ctrl)
    : m_ctrl(ctrl), m_updater(new RectControllerUpdater)
{
    auto func= [this]() { m_updater->setSynchronized(m_ctrl->localGM() || m_ctrl->permission() == Core::PC_ALL); };
    connect(m_ctrl, &VectorialMapController::localGMChanged, this, func);
    connect(m_ctrl, &VectorialMapController::permissionChanged, this, func);
}

QString RectControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::RectController> rect(new vmap::RectController(params, m_ctrl));
    emit rectControllerCreated(rect.get(), true);
    auto id= rect->uuid();
    prepareController(rect.get());
    m_controllers.push_back(std::move(rect));
    return id;
}

void RectControllerManager::addController(vmap::VisualItemController* controller)
{ // never called
    auto rectCtrl= dynamic_cast<vmap::RectController*>(controller);
    if(nullptr == rectCtrl)
        return;

    std::unique_ptr<vmap::RectController> ctrl(rectCtrl);
    emit rectControllerCreated(ctrl.get(), false);
    m_controllers.push_back(std::move(ctrl));
}

void RectControllerManager::prepareController(vmap::RectController* ctrl)
{
    auto id= m_ctrl->uuid();
    connect(ctrl, &vmap::RectController::initializedChanged, this,
            [id, ctrl]() { MessageHelper::sendOffRect(ctrl, id); });
    m_updater->addRectController(ctrl);
}

void RectControllerManager::removeItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::RectController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return;

    (*it)->aboutToBeRemoved();
    it->release();
    m_controllers.erase(it);
}

void RectControllerManager::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddItem && msg->category() == NetMsg::VMapCategory)
    {
        auto hash= MessageHelper::readRect(msg);
        auto newRect= new vmap::RectController(hash, m_ctrl);
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
const std::vector<vmap::RectController*> RectControllerManager::controllers() const
{
    std::vector<vmap::RectController*> vect;
    std::transform(m_controllers.begin(), m_controllers.end(), std::back_inserter(vect),
                   [](const std::unique_ptr<vmap::RectController>& ctrl) { return ctrl.get(); });
    return vect;
}
vmap::RectController* RectControllerManager::findController(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::RectController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return nullptr;

    return it->get();
}
