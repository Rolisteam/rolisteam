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
#include "vectorialmapmediacontroller.h"

#include <QUndoStack>
#include <map>

#include "controller/networkcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "network/networkmessagereader.h"
#include "network/receiveevent.h"
#include "vmap/vmapframe.h"

VectorialMapController* findActive(const std::vector<std::unique_ptr<VectorialMapController>>& vmaps)
{
    auto it= std::find_if(vmaps.begin(), vmaps.end(),
                          [](const std::unique_ptr<VectorialMapController>& ctrl) { return ctrl->isActive(); });
    if(vmaps.end() == it)
        return nullptr;
    return (*it).get();
}

VectorialMapMediaController::VectorialMapMediaController(NetworkController* networkCtrl) : m_networkCtrl(networkCtrl)
{
    auto func= [this]() {
        std::for_each(m_vmaps.begin(), m_vmaps.end(), [this](const std::unique_ptr<VectorialMapController>& ctrl) {
            ctrl->setLocalGM(m_networkCtrl->isGM());
        });
    };

    connect(m_networkCtrl, &NetworkController::isGMChanged, this, func);
    func();
}

CleverURI::ContentType VectorialMapMediaController::type() const
{
    return CleverURI::VMAP;
}

VectorialMapMediaController::~VectorialMapMediaController()= default;

void VectorialMapMediaController::registerNetworkReceiver()
{
    ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

/*CleverURI* findMedia(QString uuid, const std::vector<CleverURI*>& media)
{
    return nullptr;
}*/

NetWorkReceiver::SendType VectorialMapMediaController::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    switch(msg->action())
    {
    case NetMsg::loadVmap:
        break;
    case NetMsg::closeVmap:
    {
        QString vmapId= msg->string8();
        closeMedia(vmapId);
    }
    break;
    case NetMsg::addVmap:
    case NetMsg::DelPoint:
        break;
    case NetMsg::AddItem:
    case NetMsg::DelItem:
    case NetMsg::MoveItem:
    case NetMsg::GeometryItemChanged:
    case NetMsg::OpacityItemChanged:
    case NetMsg::LayerItemChanged:
    case NetMsg::MovePoint:
    case NetMsg::vmapChanges:
    case NetMsg::GeometryViewChanged:
    case NetMsg::SetParentItem:
    case NetMsg::RectGeometryItem:
    case NetMsg::RotationItem:
    case NetMsg::CharacterStateChanged:
    case NetMsg::CharacterChanged:
    case NetMsg::VisionChanged:
    case NetMsg::ColorChanged:
    case NetMsg::ZValueItem:
    {
        QString vmapId= msg->string8();
        // CleverURI* tmp= findMedia(vmapId, m_openMedia);
        //  if(nullptr != tmp)
        {
            /*VMapFrame* mapF= dynamic_cast<VMapFrame*>(tmp);
            if(nullptr != mapF)
            {
                type= mapF->processMessage(msg);
            }*/
        }
    }
    break;
    default:
        qWarning("Unexpected Action - MainWindow::processVMapMessage");
        break;
    }

    return type;
}

void VectorialMapMediaController::setUndoStack(QUndoStack* stack)
{
    m_stack= stack;
}

Core::SelectableTool VectorialMapMediaController::tool() const
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return Core::HANDLER;

    return ctrl->tool();
}

void VectorialMapMediaController::closeMedia(const QString& id)
{
    auto it= std::remove_if(m_vmaps.begin(), m_vmaps.end(),
                            [id](const std::unique_ptr<VectorialMapController>& ctrl) { return ctrl->uuid() == id; });
    if(it == m_vmaps.end())
        return;
    (*it)->aboutToClose();
    m_vmaps.erase(it);
}

bool VectorialMapMediaController::openMedia(CleverURI* uri, const std::map<QString, QVariant>& args)
{
    if(args.empty())
        return false;

    std::unique_ptr<VectorialMapController> vmapCtrl(new VectorialMapController(uri));

    vmapCtrl->setLocalGM(m_networkCtrl->isGM());

    vmapCtrl->setPermission(args.at(QStringLiteral("permission")).value<Core::PermissionMode>());
    vmapCtrl->setName(args.at(QStringLiteral("title")).toString());
    vmapCtrl->setBackgroundColor(args.at(QStringLiteral("bgcolor")).value<QColor>());
    vmapCtrl->setGridSize(args.at(QStringLiteral("gridSize")).toInt());
    vmapCtrl->setGridPattern(args.at(QStringLiteral("gridPattern")).value<Core::GridPattern>());
    vmapCtrl->setGridColor(args.at(QStringLiteral("gridColor")).value<QColor>());
    vmapCtrl->setVisibility(args.at(QStringLiteral("visibility")).value<Core::VisibilityMode>());
    vmapCtrl->setGridScale(args.at(QStringLiteral("scale")).toDouble());
    vmapCtrl->setScaleUnit(args.at(QStringLiteral("unit")).value<Core::ScaleUnit>());

    connect(vmapCtrl.get(), &VectorialMapController::activeChanged, this,
            &VectorialMapMediaController::updateProperties);
    connect(vmapCtrl.get(), &VectorialMapController::performCommand, m_stack, &QUndoStack::push);
    emit vmapControllerCreated(vmapCtrl.get());
    m_vmaps.push_back(std::move(vmapCtrl));
    return true;
}
void VectorialMapMediaController::updateProperties()
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;

    emit npcNumberChanged(ctrl->npcNumber());
    emit colorChanged(ctrl->toolColor());
    // emit opacityChanged(ctrl->);
    // emit editionModeChanged(ctrl->editionMode());
}
void VectorialMapMediaController::setNpcNumber(int number)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setNpcNumber(number);
}

void VectorialMapMediaController::setNpcName(const QString& name)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setNpcName(name);
}

void VectorialMapMediaController::setOpacity(qreal opacity)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setOpacity(opacity);
}

void VectorialMapMediaController::setPenSize(int penSize)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setPenSize(penSize);
}

void VectorialMapMediaController::setEditionMode(Core::EditionMode mode)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setEditionMode(mode);
}
void VectorialMapMediaController::setColor(const QColor& color)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setToolColor(color);
}
void VectorialMapMediaController::setTool(Core::SelectableTool tool)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setTool(tool);
}
void VectorialMapMediaController::setCharacterVision(bool b)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setCharacterVision(b);
}

void VectorialMapMediaController::setGridUnit(Core::ScaleUnit unit)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setScaleUnit(unit);
}

void VectorialMapMediaController::setGridSize(int size)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridSize(size);
}

void VectorialMapMediaController::setGridScale(double scale)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridScale(scale);
}

void VectorialMapMediaController::setVisibilityMode(Core::VisibilityMode mode)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setVisibility(mode);
}

void VectorialMapMediaController::setPermissionMode(Core::PermissionMode mode)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setPermission(mode);
}

void VectorialMapMediaController::setLayer(Core::Layer layer)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setLayer(layer);
}

void VectorialMapMediaController::setGridPattern(Core::GridPattern pattern)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridPattern(pattern);
}

void VectorialMapMediaController::setGridVisibility(bool visible)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridVisibility(visible);
}

void VectorialMapMediaController::setGridAbove(bool above)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setGridAbove(above);
}

void VectorialMapMediaController::setCollision(bool b)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setCollision(b);
}

void VectorialMapMediaController::showTransparentItem()
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    // ctrl->setGridScale(scale);
}
void VectorialMapMediaController::setBackgroundColor(const QColor& color)
{
    auto ctrl= findActive(m_vmaps);
    if(nullptr == ctrl)
        return;
    ctrl->setBackgroundColor(color);
}
