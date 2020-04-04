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
#include "textcontrollermanager.h"

#include <QVariant>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "vmap/controller/textcontroller.h"
#include "worker/messagehelper.h"

TextControllerManager::TextControllerManager(VectorialMapController* ctrl)
    : m_ctrl(ctrl), m_updater(new TextControllerUpdater)
{
}

QString TextControllerManager::addItem(const std::map<QString, QVariant>& params)
{
    std::unique_ptr<vmap::TextController> text(new vmap::TextController(params, m_ctrl));
    emit textControllerCreated(text.get());
    auto id= text->uuid();
    MessageHelper::sendOffText(text.get(), m_ctrl->uuid());
    m_updater->addTextController(text.get());
    m_controllers.push_back(std::move(text));
    return id;
}

void TextControllerManager::addController(vmap::VisualItemController* controller)
{
    auto textCtrl= dynamic_cast<vmap::TextController*>(controller);
    if(nullptr == textCtrl)
        return;

    std::unique_ptr<vmap::TextController> text(textCtrl);
    emit textControllerCreated(text.get());
    m_controllers.push_back(std::move(text));
}

void TextControllerManager::removeItem(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::TextController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return;

    (*it)->aboutToBeRemoved();
    m_controllers.erase(it);
}

void TextControllerManager::processMessage(NetworkMessageReader* msg)
{
    if(msg->action() == NetMsg::AddItem && msg->category() == NetMsg::VMapCategory)
    {
        auto hash= MessageHelper::readText(msg);
        auto newRect= new vmap::TextController(hash, m_ctrl);
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
const std::vector<vmap::TextController*> TextControllerManager::controllers() const
{
    std::vector<vmap::TextController*> vect;
    std::transform(m_controllers.begin(), m_controllers.end(), std::back_inserter(vect),
                   [](const std::unique_ptr<vmap::TextController>& ctrl) { return ctrl.get(); });
    return vect;
}

vmap::TextController* TextControllerManager::findController(const QString& id)
{
    auto it= std::find_if(m_controllers.begin(), m_controllers.end(),
                          [id](const std::unique_ptr<vmap::TextController>& ctrl) { return id == ctrl->uuid(); });

    if(it == m_controllers.end())
        return nullptr;

    return it->get();
}
