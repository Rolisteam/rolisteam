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
#include "controller/item_controllers/visualitemcontroller.h"

#include <QDebug>
#include <QUuid>
#include <QVariant>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "worker/utilshelper.h"

namespace vmap
{

VisualItemController::VisualItemController(ItemType itemType, const std::map<QString, QVariant>& params,
                                           VectorialMapController* ctrl, QObject* parent)
    : QObject(nullptr), m_ctrl(ctrl), m_itemType(itemType), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    Q_UNUSED(parent)
    if(m_ctrl)
    {
        connect(m_ctrl, &VectorialMapController::layerChanged, this, &VisualItemController::selectableChanged);
        connect(m_ctrl, &VectorialMapController::visibilityChanged, this, &VisualItemController::visibilityChanged);
        connect(m_ctrl, &VectorialMapController::localGMChanged, this, &VisualItemController::localIsGMChanged);

        connect(m_ctrl, &VectorialMapController::permissionChanged, this, &VisualItemController::computeEditable);
        connect(m_ctrl, &VectorialMapController::localGMChanged, this, &VisualItemController::computeEditable);
        connect(m_ctrl, &VectorialMapController::layerChanged, this, &VisualItemController::computeEditable);

        setLayer(m_ctrl->layer());
    }
    connect(this, &VisualItemController::lockedChanged, this, &VisualItemController::computeEditable);
    connect(this, &VisualItemController::removedChanged, this, &VisualItemController::visibleChanged);

    initializedVisualItem(params);

    connect(this, &VisualItemController::colorChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::opacityChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::rotationChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::posChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::layerChanged, this,
            [this]
            {
                setOpacity(layer() == Core::Layer::GAMEMASTER_LAYER ? 0.5 : 1.0);
                computeEditable();
                setModified();
            });
    connect(this, &VisualItemController::lockedChanged, this, [this] { setModified(); });
    computeEditable();
}

VisualItemController::~VisualItemController() {}

void VisualItemController::initializedVisualItem(const std::map<QString, QVariant>& params)
{
    if(params.empty())
        return;
    namespace hu= helper::utils;
    using std::placeholders::_1;

    // clang-format off
    hu::setParamIfAny<QString>(Core::vmapkeys::KEY_UUID, params, std::bind(&VisualItemController::setUuid, this, _1));
    hu::setParamIfAny<bool>(Core::vmapkeys::KEY_VISIBLE, params, std::bind(&VisualItemController::setVisible, this, _1));
    hu::setParamIfAny<bool>(Core::vmapkeys::KEY_INITIALIZED, params, std::bind(&VisualItemController::setInitialized, this, _1));
    hu::setParamIfAny<qreal>(Core::vmapkeys::KEY_OPACITY, params, std::bind(&VisualItemController::setOpacity, this, _1));
    hu::setParamIfAny<qreal>(Core::vmapkeys::KEY_ROTATION, params, std::bind(&VisualItemController::setRotation, this, _1));
    hu::setParamIfAny<Core::Layer>(Core::vmapkeys::KEY_LAYER, params, std::bind(&VisualItemController::setLayer, this, _1));
    hu::setParamIfAny<QPointF>(Core::vmapkeys::KEY_POS, params, std::bind(&VisualItemController::setPos, this, _1));
    hu::setParamIfAny<QColor>(Core::vmapkeys::KEY_COLOR, params, std::bind(&VisualItemController::setColor, this, _1));
    hu::setParamIfAny<bool>(Core::vmapkeys::KEY_LOCKED, params, std::bind(&VisualItemController::setLocked, this, _1));
    // clang-format on
}

QPointer<VectorialMapController> VisualItemController::mapController() const
{
    return m_ctrl;
}

bool VisualItemController::selected() const
{
    return m_selected;
}

bool VisualItemController::localIsGM() const
{
    return m_ctrl->localGM();
}

bool VisualItemController::initialized() const
{
    return m_initialized;
}

Core::SelectableTool VisualItemController::tool() const
{
    return m_tool;
}

QColor VisualItemController::color() const
{
    return m_color;
}

VisualItemController::ItemType VisualItemController::itemType() const
{
    return m_itemType;
}

bool VisualItemController::remote() const
{
    return m_remote;
}

QPointF VisualItemController::rotationOriginPoint() const
{
    return rect().center();
}

bool VisualItemController::editable() const
{
    return (selectable() & m_editable);
}

bool VisualItemController::selectable() const
{
    return (m_ctrl->layer() == m_layer);
}

bool VisualItemController::visible() const
{
    return (m_visible && !m_removed);
}

qreal VisualItemController::opacity() const
{
    return m_opacity;
}
Core::VisibilityMode VisualItemController::visibility() const
{
    return m_ctrl->visibility();
}
Core::Layer VisualItemController::layer() const
{
    return m_layer;
}

QPointF VisualItemController::pos() const
{
    return m_pos;
}

QString VisualItemController::uuid() const
{
    return m_uuid;
}

qreal VisualItemController::rotation() const
{
    return m_rotation;
}

int VisualItemController::gridSize() const
{
    return m_ctrl->gridSize();
}

bool VisualItemController::locked() const
{
    return m_locked;
}

QString VisualItemController::getLayerText(Core::Layer layer) const
{
    return m_ctrl->layerToText(layer);
}

void VisualItemController::endGeometryChange()
{
    if(m_posEditing)
    {
        emit posEditFinished();
        m_posEditing= false;
    }

    if(m_rotationEditing)
    {
        emit rotationEditFinished();
        m_rotationEditing= false;
    }
    setInitialized(true);
}

const QString VisualItemController::mapUuid() const
{
    return m_ctrl->uuid();
}

void VisualItemController::setColor(const QColor& color)
{
    if(m_color == color)
        return;

    m_color= color;
    emit colorChanged(m_color);
}

void VisualItemController::setSelected(bool b)
{
    if(b == m_selected)
        return;
    m_selected= b;
    emit selectedChanged(m_selected);
}

void VisualItemController::setUuid(QString uuid)
{
    if(uuid == m_uuid)
        return;
    m_uuid= uuid;
    emit uuidChanged();
}

void VisualItemController::setRotation(qreal rota)
{
    if(qFuzzyCompare(rota, m_rotation))
        return;
    m_rotation= rota;
    emit rotationChanged(m_rotation);
    m_rotationEditing= true;
}

void VisualItemController::setEditable(bool b)
{
    if(b == m_editable)
        return;
    m_editable= b;
    emit editableChanged();
}

void VisualItemController::setVisible(bool b)
{
    if(b == m_visible)
        return;
    m_visible= b;
    emit visibleChanged();
}

void VisualItemController::setOpacity(qreal b)
{
    if(qFuzzyCompare(b, m_opacity))
        return;
    m_opacity= b;
    emit opacityChanged();
}

void VisualItemController::setLayer(Core::Layer layer)
{
    if(layer == m_layer)
        return;
    m_layer= layer;
    emit layerChanged();
    emit selectableChanged();
}

void VisualItemController::setPos(const QPointF& pos)
{
    if(m_pos == pos)
        return;
    m_pos= pos;
    m_posEditing= true;
    emit posChanged(m_pos);
}
void VisualItemController::setLocked(bool locked)
{
    if(m_locked == locked)
        return;

    m_locked= locked;
    emit lockedChanged(m_locked);
}
void VisualItemController::setInitialized(bool b)
{
    if(b == m_initialized)
        return;
    m_initialized= b;
    emit initializedChanged(m_initialized);
}

void VisualItemController::setRemote(bool b)
{
    if(b == m_remote)
        return;
    m_remote= b;
    emit remoteChanged(m_remote);
}

void VisualItemController::setModified()
{
    emit modifiedChanged();
}

void VisualItemController::computeEditable()
{
    if(!m_ctrl)
        return;
    bool isAll= m_ctrl->permission() == Core::PermissionMode::PC_ALL;
    auto editableByPermission= (localIsGM() || isAll);
    auto sameLayer= m_ctrl->layer() == layer();
    setEditable(!m_locked && sameLayer && editableByPermission);
}

qreal VisualItemController::zOrder() const
{
    return m_zOrder;
}

void VisualItemController::setZOrder(qreal newZOrder)
{
    if(qFuzzyCompare(m_zOrder, newZOrder))
        return;
    m_zOrder= newZOrder;
    emit zOrderChanged(m_zOrder);
}

bool VisualItemController::removed() const
{
    return m_removed;
}

void VisualItemController::setRemoved(bool newRemoved)
{
    if(m_removed == newRemoved)
        return;
    m_removed= newRemoved;
    emit removedChanged();
}

QString VisualItemController::parentUuid() const
{
    return m_parentUuid;
}

void VisualItemController::setParentUuid(const QString& newParentUuid)
{
    if(m_parentUuid == newParentUuid)
        return;
    m_parentUuid= newParentUuid;
    emit parentUuidChanged();
}

} // namespace vmap
