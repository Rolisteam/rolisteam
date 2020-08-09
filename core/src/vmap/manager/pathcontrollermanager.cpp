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
#include "pathcontrollermanager.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "updater/pathcontrollerupdater.h"
#include "vmap/controller/pathcontroller.h"
#include "worker/messagehelper.h"

#include <QDebug>

PathControllerManager::PathControllerManager(VectorialMapController* ctrl, QObject* parent)
    : VisualItemControllerManager(parent), m_ctrl(ctrl), m_updater(new PathControllerUpdater)
{
    auto func= [this]() { m_updater->setSynchronized(m_ctrl->localGM() || m_ctrl->permission() == Core::PC_ALL); };
    connect(ctrl, &VectorialMapController::localGMChanged, this, func);
    connect(ctrl, &VectorialMapController::permissionChanged, this, func);
}

PathControllerManager::~PathControllerManager()= default;

QString PathControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::PathController> path(new vmap::PathController(params, m_ctrl));
    emit pathControllerCreated(path.get(), true);
    auto id= path->uuid();
    emit itemAdded(id);
    prepareController(path.get());
    m_controllers.push_back(std::move(path));
    return id;
}

void PathControllerManager::addController(vmap::VisualItemController* controller)
{
    auto path= dynamic_cast<vmap::PathController*>(controller);
    if(nullptr == path)
        return;

    std::unique_ptr<vmap::PathController> pathCtrl(path);
    emit pathControllerCreated(pathCtrl.get(), false);
    emit itemAdded(pathCtrl->uuid());
    m_controllers.push_back(std::move(pathCtrl));
}

void PathControllerManager::removeItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::PathController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return;

    (*it)->aboutToBeRemoved();
    m_controllers.erase(it);
}

void PathControllerManager::prepareController(vmap::PathController* ctrl)
{
    qDebug() << "PathControllerManager::prepareController" << ctrl->penLine();
    auto id= m_ctrl->uuid();
    if(ctrl->penLine())
    {
        connect(ctrl, &vmap::PathController::initializedChanged, this, [id, ctrl, this]() {
            MessageHelper::sendOffPath(ctrl, id);
            // m_updater->addPathController(ctrl);
        });
    }
    else
    {
        MessageHelper::sendOffPath(ctrl, id);
        m_updater->addPathController(ctrl);
    }
}

void PathControllerManager::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddItem && msg->category() == NetMsg::VMapCategory)
    {
        auto hash= MessageHelper::readPath(msg);
        auto newRect= new vmap::PathController(hash, m_ctrl);
        addController(newRect);
    }
    else if(msg->action() == NetMsg::UpdateItem && msg->category() == NetMsg::VMapCategory)
    {
        auto id= msg->string8();
        auto ctrl= findController(id);
        if(nullptr != ctrl)
            m_updater->updateItemProperty(msg, ctrl);
    }
    else if(msg->action() == NetMsg::MovePoint && msg->category() == NetMsg::VMapCategory)
    {
        auto id= msg->string8();
        auto ctrl= findController(id);
        if(nullptr != ctrl)
            m_updater->movePoint(msg, ctrl);
    }
}

bool PathControllerManager::loadItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::PathController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return false;

    emit pathControllerCreated(it->get(), false);

    return true;
}

const std::vector<vmap::PathController*> PathControllerManager::controllers() const
{
    std::vector<vmap::PathController*> vect;
    std::transform(m_controllers.begin(), m_controllers.end(), std::back_inserter(vect),
                   [](const std::unique_ptr<vmap::PathController>& ctrl) { return ctrl.get(); });
    return vect;
}

vmap::PathController* PathControllerManager::findController(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::PathController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return nullptr;

    return it->get();
}
