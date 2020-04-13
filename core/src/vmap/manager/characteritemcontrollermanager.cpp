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
#include "characteritemcontrollermanager.h"

#include <algorithm>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "vmap/controller/characteritemcontroller.h"
#include "worker/messagehelper.h"

CharacterItemControllerManager::CharacterItemControllerManager(VectorialMapController* ctrl) : m_ctrl(ctrl)
{
    /* auto func= [this]() { m_updater->setSynchronized(m_ctrl->localGM() || m_ctrl->permission() == Core::PC_MOVE); };
     connect(ctrl, &VectorialMapController::localGMChanged, this, func);
     connect(ctrl, &VectorialMapController::permissionChanged, this, func); */
}

QString CharacterItemControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::CharacterItemController> characterCtrl(new vmap::CharacterItemController(params, m_ctrl));
    emit characterControllerCreated(characterCtrl.get());

    if(characterCtrl->playableCharacter())
        emit playableCharacterControllerCreated();

    auto id= characterCtrl->uuid();
    m_controllers.push_back(std::move(characterCtrl));
    return id;
}

void CharacterItemControllerManager::addController(vmap::VisualItemController* controller)
{
    auto characterCtrl= dynamic_cast<vmap::CharacterItemController*>(controller);
    if(nullptr == characterCtrl)
        return;

    std::unique_ptr<vmap::CharacterItemController> ctrl(characterCtrl);
    emit characterControllerCreated(ctrl.get());
    MessageHelper::sendOffCharacter(ctrl.get(), m_ctrl->uuid());
    if(ctrl->playableCharacter())
        emit playableCharacterControllerCreated();
    m_controllers.push_back(std::move(ctrl));
}

int CharacterItemControllerManager::playableCharacterCount() const
{
    return static_cast<int>(std::count_if(
        m_controllers.begin(), m_controllers.end(),
        [](const std::unique_ptr<vmap::CharacterItemController>& ctrl) { return ctrl->playableCharacter(); }));
}

void CharacterItemControllerManager::removeItem(const QString& id)
{
    auto it
        = std::find_if(m_controllers.begin(), m_controllers.end(),
                       [id](const std::unique_ptr<vmap::CharacterItemController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return;

    (*it)->aboutToBeRemoved();
    m_controllers.erase(it);
}

void CharacterItemControllerManager::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddItem && msg->category() == NetMsg::VMapCategory)
    {
    }
}

const std::vector<vmap::CharacterVisionData> CharacterItemControllerManager::characterVisions() const
{
    std::vector<vmap::CharacterVisionData> dataVec;
    dataVec.reserve(m_controllers.size());
    for(const auto& ctrl : m_controllers)
    {
        if(!ctrl->playableCharacter())
            continue;
        dataVec.push_back({ctrl->pos(), ctrl->rotation(), ctrl->vision(), ctrl->shape(), ctrl->radius()});
    }
    return dataVec;
}
const std::vector<vmap::CharacterItemController*> CharacterItemControllerManager::controllers() const
{
    std::vector<vmap::CharacterItemController*> vect;
    std::transform(m_controllers.begin(), m_controllers.end(), std::back_inserter(vect),
                   [](const std::unique_ptr<vmap::CharacterItemController>& ctrl) { return ctrl.get(); });
    return vect;
}
