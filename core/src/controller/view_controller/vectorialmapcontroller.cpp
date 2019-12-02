/***************************************************************************
 *  Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "vectorialmapcontroller.h"

#include "data/cleveruri.h"
#include "vmap/vmap.h"

#include "worker/iohelper.h"

VectorialMapController::VectorialMapController(CleverURI* uri, QObject* parent)
    : AbstractMediaContainerController(uri, parent), m_vmap(new VMap(this))
{
    if(uri->hasData() || !uri->getUri().isEmpty())
        IOHelper::loadVMap(m_vmap.get(), uri, this);

    m_properties.insert({Core::ShowNpcName, false});
    m_properties.insert({Core::ShowPcName, false});
    m_properties.insert({Core::ShowNpcNumber, false});
    m_properties.insert({Core::ShowHealthStatus, false});
    m_properties.insert({Core::ShowInitScore, true});
    m_properties.insert({Core::ShowHealthBar, false});
    m_properties.insert({Core::ShowGrid, false});
    m_properties.insert({Core::LocalIsGM, false});
    m_properties.insert({Core::EnableCharacterVision, false});
    m_properties.insert({Core::CollisionStatus, false});
    m_properties.insert({Core::PermissionModeProperty, Core::GM_ONLY});
    m_properties.insert({Core::FogOfWarStatus, false});
    m_properties.insert({Core::VisibilityModeProperty, Core::HIDDEN});
    m_properties.insert({Core::MapLayer, static_cast<int>(m_editionMode)});
    m_properties.insert({Core::GridPatternProperty, Core::NONE});
    m_properties.insert({Core::GridColor, QColor(Qt::black)});
    m_properties.insert({Core::GridSize, 50});
    m_properties.insert({Core::Scale, 1.0});
    m_properties.insert({Core::Unit, Core::M});
    m_properties.insert({Core::GridAbove, false});
    m_properties.insert({Core::HideOtherLayers, false});
}

Core::PermissionMode VectorialMapController::permission() const
{
    return m_permission;
}

int VectorialMapController::gridSize() const
{
    return m_gridSize;
}

bool VectorialMapController::gridVisibility() const
{
    return m_gridVisibility;
}

qreal VectorialMapController::gridScale() const
{
    return m_gridScale;
}

bool VectorialMapController::gridAbove() const
{
    return m_gridAbove;
}

bool VectorialMapController::collision() const
{
    return m_collision;
}

Core::ScaleUnit VectorialMapController::scaleUnit() const
{
    return m_scaleUnit;
}

Core::GridPattern VectorialMapController::gridPattern() const
{
    return m_gridPattern;
}

Core::VisibilityMode VectorialMapController::visibility() const
{
    return m_visibilityMode;
}

QColor VectorialMapController::backgroundColor() const
{
    return m_backgroundColor;
}

QColor VectorialMapController::toolColor() const
{
    return m_toolColor;
}

int VectorialMapController::penSize() const
{
    return m_penSize;
}

QString VectorialMapController::npcName() const
{
    return m_npcName;
}

int VectorialMapController::npcNumber() const
{
    return m_npcNumber;
}

qreal VectorialMapController::zoomLevel() const
{
    return m_zoomLevel;
}

Core::Layer VectorialMapController::layer() const
{
    return m_layer;
}

Core::SelectableTool VectorialMapController::tool() const
{
    return m_tool;
}

Core::EditionMode VectorialMapController::editionMode() const
{
    return m_editionMode;
}

qreal VectorialMapController::opacity() const
{
    return m_opacity;
}

void VectorialMapController::saveData() const {}

void VectorialMapController::loadData() const {}

void VectorialMapController::setPermission(Core::PermissionMode mode)
{
    if(m_permission == mode)
        return;
    m_permission= mode;
    emit permissionChanged();
}

void VectorialMapController::setGridVisibility(bool visible)
{
    if(visible == m_gridVisibility)
        return;
    m_gridVisibility= visible;
    emit gridVisibilityChanged();
}

void VectorialMapController::setGridAbove(bool above)
{
    if(m_gridAbove == above)
        return;
    m_gridAbove= above;
    emit gridAboveChanged();
}

void VectorialMapController::setGridSize(int gridSize)
{
    if(m_gridSize == gridSize)
        return;
    m_gridSize= gridSize;
    emit gridSizeChanged();
}

void VectorialMapController::setGridPattern(Core::GridPattern pattern)
{
    if(pattern == m_gridPattern)
        return;
    m_gridPattern= pattern;
    emit gridPatternChanged();
}

void VectorialMapController::setVisibility(Core::VisibilityMode mode)
{
    if(mode == m_visibilityMode)
        return;
    m_visibilityMode= mode;
    emit visibilityChanged();
}

void VectorialMapController::setBackgroundColor(QColor color)
{
    if(color == m_backgroundColor)
        return;
    m_backgroundColor= color;
    emit backgroundColorChanged();
}

void VectorialMapController::setToolColor(QColor color)
{
    if(color == m_toolColor)
        return;
    m_toolColor= color;
    emit toolColorChanged();
}

void VectorialMapController::setPenSize(int size)
{
    if(size == m_penSize)
        return;
    m_penSize= size;
    emit penSizeChanged();
}

void VectorialMapController::setNpcName(const QString& name)
{
    if(name == m_npcName)
        return;
    m_npcName= name;
    emit npcNameChanged();
}

void VectorialMapController::setNpcNumber(int number)
{
    if(number == m_npcNumber)
        return;
    m_npcNumber= number;
    emit npcNumberChanged();
}

void VectorialMapController::setGridScale(qreal scale)
{
    if(qFuzzyCompare(m_gridScale, scale))
        return;
    m_gridScale= scale;
    emit gridScaleChanged();
}

void VectorialMapController::setScaleUnit(Core::ScaleUnit unit)
{
    if(m_scaleUnit == unit)
        return;
    m_scaleUnit= unit;
    emit scaleUnitChanged();
}

void VectorialMapController::setZoomLevel(qreal lvl)
{
    lvl= qBound(0.2, lvl, 4.0);
    if(qFuzzyCompare(lvl, m_zoomLevel))
        return;

    m_zoomLevel= lvl;
    emit zoomLevelChanged();
}

void VectorialMapController::zoomIn(qreal step)
{
    setZoomLevel(m_zoomLevel + step);
}

void VectorialMapController::zoomOut(qreal step)
{
    setZoomLevel(m_zoomLevel - step);
}

void VectorialMapController::setCollision(bool col)
{
    if(m_collision == col)
        return;
    m_collision= col;
    emit collisionChanged();
}

void VectorialMapController::setLayer(Core::Layer layer)
{
    if(layer == m_layer)
        return;
    m_layer= layer;
    emit layerChanged();
}

void VectorialMapController::setTool(Core::SelectableTool tool)
{
    if(tool == m_tool)
        return;
    m_tool= tool;
    emit toolChanged();
}

void VectorialMapController::setEditionMode(Core::EditionMode mode)
{
    if(mode == m_editionMode)
        return;
    m_editionMode= mode;
    emit editionMode();
}

void VectorialMapController::setOpacity(qreal opacity)
{
    if(qFuzzyCompare(m_opacity, opacity))
        return;
    m_opacity= opacity;
    emit opacityChanged();
}
