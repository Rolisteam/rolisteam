/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "updater/media/vmapupdater.h"

#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QSet>
#include <QTimer>

#include "controller/item_controllers/vmapitemfactory.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/campaignmanager.h"
#include "model/contentmodel.h"
#include "worker/vectorialmapmessagehelper.h"

#include "updater/vmapitem/characteritemupdater.h"
#include "updater/vmapitem/ellipsecontrollerupdater.h"
#include "updater/vmapitem/imagecontrollerupdater.h"
#include "updater/vmapitem/linecontrollerupdater.h"
#include "updater/vmapitem/pathcontrollerupdater.h"
#include "updater/vmapitem/rectcontrollerupdater.h"
#include "updater/vmapitem/textcontrollerupdater.h"
#include "updater/vmapitem/vmapitemcontrollerupdater.h"
#include "updater/vmapitem/sightupdater.h"

VectorialMapController* findMap(const std::vector<VectorialMapController*>& vmaps, const QString& id)
{
    auto it
        = std::find_if(vmaps.begin(), vmaps.end(), [id](VectorialMapController* ctrl) { return ctrl->uuid() == id; });
    if(vmaps.end() == it)
        return nullptr;
    return (*it);
}

VMapUpdater::VMapUpdater(campaign::CampaignManager* manager, FilteredContentModel* model, QObject* parent)
    : MediaUpdaterInterface(manager, parent)
    , m_vmapModel(model)
    , m_diceParser(manager ? manager->diceparser() : nullptr)
{
    m_updaters.insert(
        {vmap::VisualItemController::LINE, std::unique_ptr<LineControllerUpdater>(new LineControllerUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::PATH, std::unique_ptr<PathControllerUpdater>(new PathControllerUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::RECT, std::unique_ptr<RectControllerUpdater>(new RectControllerUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::IMAGE, std::unique_ptr<ImageControllerUpdater>(new ImageControllerUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::CHARACTER, std::unique_ptr<CharacterItemUpdater>(new CharacterItemUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::ELLIPSE, std::unique_ptr<EllipseControllerUpdater>(new EllipseControllerUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::TEXT, std::unique_ptr<TextControllerUpdater>(new TextControllerUpdater)});
    m_updaters.insert(
        {vmap::VisualItemController::SIGHT, std::unique_ptr<SightUpdater>(new SightUpdater)});

    ReceiveEvent::registerNetworkReceiver(NetMsg::VMapCategory, this);
}

void VMapUpdater::addMediaController(MediaControllerBase* base)
{
    if(nullptr == base)
        return;

    auto ctrl= dynamic_cast<VectorialMapController*>(base);

    if(ctrl == nullptr)
        return;

    ctrl->setDiceParser(m_diceParser);

    connect(ctrl, &VectorialMapController::collisionChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("collision")); });
    connect(ctrl, &VectorialMapController::characterVisionChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("characterVision")); });
    connect(ctrl, &VectorialMapController::gridColorChanged, this,
            [this, ctrl]() { sendOffChanges<QColor>(ctrl, QStringLiteral("gridColor")); });
    connect(ctrl, &VectorialMapController::gridScaleChanged, this,
            [this, ctrl]() { sendOffChanges<qreal>(ctrl, QStringLiteral("gridScale")); });
    connect(ctrl, &VectorialMapController::gridSizeChanged, this,
            [this, ctrl]() { sendOffChanges<int>(ctrl, QStringLiteral("gridSize")); });
    connect(ctrl, &VectorialMapController::gridVisibilityChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("gridVisibility")); });
    connect(ctrl, &VectorialMapController::gridAboveChanged, this,
            [this, ctrl]() { sendOffChanges<bool>(ctrl, QStringLiteral("gridAbove")); });
    connect(ctrl, &VectorialMapController::scaleUnitChanged, this,
            [this, ctrl]() { sendOffChanges<Core::ScaleUnit>(ctrl, QStringLiteral("scaleUnit")); });
    connect(ctrl, &VectorialMapController::permissionChanged, this,
            [this, ctrl]() { sendOffChanges<Core::PermissionMode>(ctrl, QStringLiteral("permission")); });
    connect(ctrl, &VectorialMapController::gridPatternChanged, this,
            [this, ctrl]() { sendOffChanges<Core::GridPattern>(ctrl, QStringLiteral("gridPattern")); });
    connect(ctrl, &VectorialMapController::visibilityChanged, this,
            [this, ctrl]() { sendOffChanges<Core::VisibilityMode>(ctrl, QStringLiteral("visibility")); });
    connect(ctrl, &VectorialMapController::backgroundColorChanged, this,
            [this, ctrl]() { sendOffChanges<QColor>(ctrl, QStringLiteral("backgroundColor")); });
    connect(ctrl, &VectorialMapController::layerChanged, this,
            [this, ctrl]() { sendOffChanges<Core::Layer>(ctrl, QStringLiteral("layer")); });
    connect(ctrl, &VectorialMapController::zIndexChanged, this,
            [this, ctrl]() { sendOffChanges<int>(ctrl, QStringLiteral("zIndex")); });

    connect(ctrl, &VectorialMapController::sendOffHighLightAt, this,
            [ctrl](const QPointF& p, const qreal& penSize, const QColor& color) {
                VectorialMapMessageHelper::sendOffHighLight(p, penSize, color, ctrl->uuid());
            });

    connect(ctrl, &VectorialMapController::visualItemControllerCreated, this,
            [this](vmap::VisualItemController* itemCtrl) {
                auto type= itemCtrl->itemType();
                auto mapCtrl=itemCtrl->mapController();

                if(!mapCtrl)
                    return;

                auto it= m_updaters.find(type);

                if(it == m_updaters.end())
                    return;

                auto updater= it->second.get();
                if(updater && mapCtrl->editionMode() == Core::EditionMode::Painting)
                    updater->addItemController(itemCtrl);
            });
    connect(ctrl, &VectorialMapController::visualItemControllersRemoved, this,
            [ctrl](const QStringList& list) { VectorialMapMessageHelper::sendOffRemoveItems(list, ctrl->uuid()); });

    auto it = m_updaters.find(vmap::VisualItemController::SIGHT);
    if(it != std::end(m_updaters))
    {
        auto updater = it->second.get();
        if(updater)
            updater->addItemController(ctrl->sightController());
    }

    if(!ctrl->remote())
    {
        connect(ctrl, &VectorialMapController::modifiedChanged, this, [ctrl, this]() {
            if(ctrl->modified())
            {
                saveMediaController(ctrl);
            }
        });
        QTimer::singleShot(1000,ctrl,[ctrl, this](){
            saveMediaController(ctrl);
        });
    }
}

bool VMapUpdater::updateVMapProperty(NetworkMessageReader* msg, VectorialMapController* ctrl)
{
    if(nullptr == msg || nullptr == ctrl)
        return false;

    updatingCtrl= ctrl;

    auto property= msg->string16();

    QVariant var;
    QSet<QString> boolProperties(
        {QString("collision"), QString("characterVision"), QString("gridVisibility"), QString("gridAbove")});
    QSet<QString> colorProperties({QString("backgroundColor"), QString("gridColor")});
    QSet<QString> uint8Properties(
        {QString("scaleUnit"), QString("permission"), QString("gridPattern"), QString("visibility"), QString("layer")});
    QSet<QString> intProperties({QString("zIndex"), QString("gridSize")});
    QSet<QString> realProperties({QString("gridScale")});

    if(boolProperties.contains(property))
    {
        var= QVariant::fromValue(static_cast<bool>(msg->uint8()));
    }
    else if(colorProperties.contains(property))
    {
        var= QVariant::fromValue(QColor(msg->rgb()));
    }
    else if(intProperties.contains(property))
    {
        var= QVariant::fromValue(msg->int64());
    }
    else if(realProperties.contains(property))
    {
        var= QVariant::fromValue(msg->real());
    }
    else if(uint8Properties.contains(property))
    {
        auto value= msg->uint8();
        if(property == QStringLiteral("scaleUnit"))
        {
            var= QVariant::fromValue(static_cast<Core::ScaleUnit>(value));
        }
        else if(property == QStringLiteral("scaleUnit"))
        {
            var= QVariant::fromValue(static_cast<Core::ScaleUnit>(value));
        }
        else if(property == QStringLiteral("permission"))
        {
            var= QVariant::fromValue(static_cast<Core::PermissionMode>(value));
        }
        else if(property == QStringLiteral("gridPattern"))
        {
            var= QVariant::fromValue(static_cast<Core::GridPattern>(value));
        }
        else if(property == QStringLiteral("visibility"))
        {
            var= QVariant::fromValue(static_cast<Core::VisibilityMode>(value));
        }
        else if(property == QStringLiteral("layer"))
        {
            var= QVariant::fromValue(static_cast<Core::Layer>(value));
        }
    }
    else if(uint8Properties.contains(property))
    {
        var= QVariant::fromValue(msg->int64());
    }
    else
    {
        auto val= msg->string32();
        var= QVariant::fromValue(val);
    }

    m_updatingFromNetwork= true;
    auto feedback= ctrl->setProperty(property.toLocal8Bit().data(), var);
    m_updatingFromNetwork= false;
    updatingCtrl= nullptr;

    return feedback;
}


NetWorkReceiver::SendType VMapUpdater::processMessage(NetworkMessageReader* msg)
{
    NetWorkReceiver::SendType type= NetWorkReceiver::NONE;
    QString vmapId= msg->string8();
    auto map= findMap(m_vmapModel->contentController<VectorialMapController*>(), vmapId);
    if(nullptr == map){
        qWarning() << QString("Map with id %1 has not been found").arg(vmapId);
        return type;
    }

    if(is(msg, NetMsg::MediaCategory, NetMsg::UpdateMediaProperty))
    {
        updateVMapProperty(msg, map);
    }
    else if(is(msg, NetMsg::VMapCategory, NetMsg::AddItem))
    {
        auto item= vmap::VmapItemFactory::createRemoteVMapItem(map, msg);
        map->addRemoteItem(item);
    }
    else if(is(msg, NetMsg::VMapCategory, NetMsg::UpdateItem) || is(msg, NetMsg::VMapCategory, NetMsg::CharacterVisionChanged))
    {     
        auto itemType= static_cast<vmap::VisualItemController::ItemType>(msg->uint8());
        QString itemId= msg->string8();
        auto itemCtrl= map->itemController(itemId);
        auto it= m_updaters.find(itemType);
        if(it != m_updaters.end())
        {
            it->second->updateItemProperty(msg, itemCtrl);
        }
    }
    else if(is(msg, NetMsg::VMapCategory, NetMsg::HighLightPosition))
    {
        VectorialMapMessageHelper::readHighLight(map, msg);
    }
    else if(is(msg, NetMsg::VMapCategory, NetMsg::DeleteItem))
    {
        auto list= VectorialMapMessageHelper::readRemoveItems(msg);
        QSet<QString> ids{list.begin(), list.end()};
        map->removeItemController(ids, true);
    }
    return type;
}
