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
#include "visualitemcontroller.h"

#include <QDebug>
#include <QUuid>
#include <QVariant>

#include "controller/view_controller/vectorialmapcontroller.h"
namespace vmap
{

VisualItemController::VisualItemController(ItemType itemType, const std::map<QString, QVariant>& params,
                                           VectorialMapController* ctrl, QObject* parent)
    : QObject(parent), m_ctrl(ctrl), m_itemType(itemType), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    connect(m_ctrl, &VectorialMapController::layerChanged, this, &VisualItemController::selectableChanged);
    connect(m_ctrl, &VectorialMapController::visibilityChanged, this, &VisualItemController::visibilityChanged);
    connect(m_ctrl, &VectorialMapController::localGMChanged, this, &VisualItemController::localIsGMChanged);

    connect(m_ctrl, &VectorialMapController::permissionChanged, this, &VisualItemController::computeEditable);
    connect(m_ctrl, &VectorialMapController::localGMChanged, this, &VisualItemController::computeEditable);
    connect(m_ctrl, &VectorialMapController::layerChanged, this, &VisualItemController::computeEditable);
    connect(this, &VisualItemController::lockedChanged, this, &VisualItemController::computeEditable);

    m_layer= m_ctrl->layer();

    initializedVisualItem(params);

    computeEditable();

    connect(this, &VisualItemController::colorChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::opacityChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::rotationChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::posChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::layerChanged, this, [this] { setModified(); });
    connect(this, &VisualItemController::lockedChanged, this, [this] { setModified(); });
}

VisualItemController::~VisualItemController() {}

void VisualItemController::initializedVisualItem(const std::map<QString, QVariant>& params)
{
    if(params.empty())
        return;

    if(params.end() != params.find(Core::vmapkeys::KEY_UUID))
        m_uuid= params.at(Core::vmapkeys::KEY_UUID).toString();

    if(params.end() != params.find(Core::vmapkeys::KEY_VISIBLE))
    {
        m_visible= params.at(Core::vmapkeys::KEY_VISIBLE).toBool();
        qDebug() << "setVisible to " << m_visible << "in initilazied";
    }

    if(params.end() != params.find(Core::vmapkeys::KEY_INITIALIZED))
        m_initialized= params.at(Core::vmapkeys::KEY_INITIALIZED).toBool();

    if(params.end() != params.find(Core::vmapkeys::KEY_OPACITY))
        m_opacity= params.at(Core::vmapkeys::KEY_OPACITY).toReal();

    if(params.end() != params.find(Core::vmapkeys::KEY_ROTATION))
        m_rotation= params.at(Core::vmapkeys::KEY_ROTATION).toReal();

    if(params.end() != params.find(Core::vmapkeys::KEY_LAYER))
        m_layer= params.at(Core::vmapkeys::KEY_LAYER).value<Core::Layer>();

    if(params.end() != params.find(Core::vmapkeys::KEY_POS))
        m_pos= params.at(Core::vmapkeys::KEY_POS).toPointF();

    if(params.end() != params.find(Core::vmapkeys::KEY_COLOR))
        m_color= params.at(Core::vmapkeys::KEY_COLOR).value<QColor>();

    if(params.end() != params.find(Core::vmapkeys::KEY_LOCKED))
        m_locked= params.at(Core::vmapkeys::KEY_LOCKED).toBool();
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

bool VisualItemController::modified() const
{
    return m_modified;
}

bool VisualItemController::visible() const
{
    return m_visible; // && ((m_ctrl->visibility() != Core::HIDDEN) || m_ctrl->localGM()));
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
    setInitialized(true);
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
    emit rotationChanged();
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
    emit posChanged();
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

void VisualItemController::setModified(bool b)
{
    if(b == m_modified)
        return;
    m_modified= b;
    emit modifiedChanged(m_modified);
}

void VisualItemController::computeEditable()
{
    auto editableByPermission= (localIsGM() || m_ctrl->permission() == Core::PermissionMode::PC_ALL);
    setEditable(!m_locked && m_ctrl->layer() == layer() && editableByPermission);
}
} // namespace vmap
