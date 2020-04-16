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
#include "linecontrollermanager.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "updater/linecontrollerupdater.h"
#include "vmap/controller/linecontroller.h"
#include "worker/messagehelper.h"

LineControllerManager::LineControllerManager(VectorialMapController* ctrl)
    : m_ctrl(ctrl), m_updater(new LineControllerUpdater)
{
    auto func= [this]() { m_updater->setSynchronized(m_ctrl->localGM() || m_ctrl->permission() == Core::PC_ALL); };
    connect(ctrl, &VectorialMapController::localGMChanged, this, func);
    connect(ctrl, &VectorialMapController::permissionChanged, this, func);
}

QString LineControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::LineController> line(new vmap::LineController(params, m_ctrl));
    emit LineControllerCreated(line.get(), true);
    auto id= line->uuid();
    prepareController(line.get());
    m_controllers.push_back(std::move(line));
    return id;
}

void LineControllerManager::addController(vmap::VisualItemController* controller)
{
    auto line= dynamic_cast<vmap::LineController*>(controller);
    if(nullptr == line)
        return;

    std::unique_ptr<vmap::LineController> lineCtrl(line);
    emit LineControllerCreated(lineCtrl.get(), false);
    m_controllers.push_back(std::move(lineCtrl));
}

void LineControllerManager::removeItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::LineController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return;

    (*it)->aboutToBeRemoved();
    m_controllers.erase(it);
}

void LineControllerManager::prepareController(vmap::LineController* ctrl)
{
    auto id= m_ctrl->uuid();
    connect(ctrl, &vmap::LineController::initializedChanged, this, [id, ctrl, this]() {
        MessageHelper::sendOffLine(ctrl, id);
        m_updater->addLineController(ctrl);
    });
}

void LineControllerManager::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddItem && msg->category() == NetMsg::VMapCategory)
    {
        auto hash= MessageHelper::readLine(msg);
        auto newRect= new vmap::LineController(hash, m_ctrl);
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

const std::vector<vmap::LineController*> LineControllerManager::controllers() const
{
    std::vector<vmap::LineController*> vect;
    std::transform(m_controllers.begin(), m_controllers.end(), std::back_inserter(vect),
                   [](const std::unique_ptr<vmap::LineController>& ctrl) { return ctrl.get(); });
    return vect;
}

vmap::LineController* LineControllerManager::findController(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::LineController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return nullptr;

    return it->get();
}
