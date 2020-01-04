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

#include <QUuid>

#include "controller/view_controller/vectorialmapcontroller.h"
namespace vmap
{
VisualItemController::VisualItemController(VectorialMapController* ctrl, QObject* parent)
    : QObject(parent), m_ctrl(ctrl), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    connect(m_ctrl, &VectorialMapController::layerChanged, this, &VisualItemController::selectableChanged);
    connect(m_ctrl, &VectorialMapController::localGMChanged, this, &VisualItemController::localIsGMChanged);
    m_layer= m_ctrl->layer();
}

VisualItemController::~VisualItemController() {}

bool VisualItemController::selected() const
{
    return m_selected;
}

bool VisualItemController::localIsGM() const
{
    return m_ctrl->localGM();
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
    return m_visible;
}

qreal VisualItemController::opacity() const
{
    return m_visible;
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

QString VisualItemController::getLayerText(Core::Layer layer) const
{
    return m_ctrl->getLayerToText(layer);
}

void VisualItemController::setSelected(bool b)
{
    if(b == m_selected)
        return;
    m_selected= b;
    emit selectedChanged();
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
    emit posChanged();
}
} // namespace vmap
