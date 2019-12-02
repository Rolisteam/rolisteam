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

#ifndef VECTORIALMAPCONTROLLER_H
#define VECTORIALMAPCONTROLLER_H

#include <QColor>
#include <memory>

#include "abstractmediacontroller.h"
#include "media/mediatype.h"

class CleverURI;
class VMap;
class VectorialMapController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(Core::PermissionMode permission READ permission WRITE setPermission NOTIFY permissionChanged)
    Q_PROPERTY(bool gridVisibility READ gridVisibility WRITE setGridVisibility NOTIFY gridVisibilityChanged)
    Q_PROPERTY(bool gridAbove READ gridAbove WRITE setGridAbove NOTIFY gridAboveChanged)
    Q_PROPERTY(qreal gridScale READ gridScale WRITE setGridScale NOTIFY gridScaleChanged)
    Q_PROPERTY(Core::ScaleUnit scaleUnit READ scaleUnit WRITE setScaleUnit NOTIFY scaleUnitChanged)
    Q_PROPERTY(int gridSize READ gridSize WRITE setGridSize NOTIFY gridSizeChanged)
    Q_PROPERTY(Core::GridPattern gridPattern READ gridPattern WRITE setGridPattern NOTIFY gridPatternChanged)
    Q_PROPERTY(Core::VisibilityMode visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor toolColor READ toolColor WRITE setToolColor NOTIFY toolColorChanged)
    Q_PROPERTY(int penSize READ penSize WRITE setPenSize NOTIFY penSizeChanged)
    Q_PROPERTY(QString npcName READ npcName WRITE setNpcName NOTIFY npcNameChanged)
    Q_PROPERTY(int npcNumber READ npcNumber WRITE setNpcNumber NOTIFY npcNumberChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(bool collision READ collision WRITE setCollision NOTIFY collisionChanged)
    Q_PROPERTY(Core::Layer layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Core::SelectableTool tool READ tool WRITE setTool NOTIFY toolChanged)
    Q_PROPERTY(Core::EditionMode editionMode READ editionMode WRITE setEditionMode NOTIFY editionModeChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
    explicit VectorialMapController(CleverURI* uri, QObject* parent= nullptr);

    Core::PermissionMode permission() const;
    int gridSize() const;
    bool gridVisibility() const;
    qreal gridScale() const;
    bool gridAbove() const;
    bool collision() const;
    Core::ScaleUnit scaleUnit() const;
    Core::GridPattern gridPattern() const;
    Core::VisibilityMode visibility() const;
    QColor backgroundColor() const;
    QColor toolColor() const;
    int penSize() const;
    QString npcName() const;
    int npcNumber() const;
    qreal zoomLevel() const;
    Core::Layer layer() const;
    Core::SelectableTool tool() const;
    Core::EditionMode editionMode() const;
    qreal opacity() const;

    bool setOption(Core::Properties pop, QVariant value);
    QVariant getOption(Core::Properties pop) const;

    void saveData() const;
    void loadData() const;

signals:
    void permissionChanged();
    void gridSizeChanged();
    void gridPatternChanged();
    void gridVisibilityChanged();
    void visibilityChanged();
    void backgroundColorChanged();
    void toolColorChanged();
    void penSizeChanged();
    void npcNameChanged();
    void npcNumberChanged();
    void zoomLevelChanged();
    void gridAboveChanged();
    void scaleUnitChanged();
    void gridScaleChanged();
    void collisionChanged();
    void layerChanged();
    void toolChanged();
    void editionModeChanged();
    void opacityChanged();

public slots:
    void setPermission(Core::PermissionMode mode);
    void setGridVisibility(bool visible);
    void setGridAbove(bool above);
    void setGridSize(int gridSize);
    void setGridPattern(Core::GridPattern pattern);
    void setVisibility(Core::VisibilityMode mode);
    void setBackgroundColor(QColor color);
    void setToolColor(QColor color);
    void setPenSize(int size);
    void setNpcName(const QString& name);
    void setNpcNumber(int number);
    void setGridScale(qreal scale);
    void setScaleUnit(Core::ScaleUnit unit);
    void setZoomLevel(qreal lvl);
    void zoomIn(qreal step= 0.2);
    void zoomOut(qreal step= 0.2);
    void setCollision(bool col);
    void setLayer(Core::Layer layer);
    void setTool(Core::SelectableTool tool);
    void setEditionMode(Core::EditionMode mode);
    void setOpacity(qreal opacity);

private:
    Core::PermissionMode m_permission;
    Core::GridPattern m_gridPattern;
    Core::VisibilityMode m_visibilityMode;
    qreal m_zoomLevel= 1.0;
    qreal m_opacity= 1.0;
    qreal m_gridScale= 5.0;
    int m_gridSize= 50;
    bool m_gridVisibility= false;
    bool m_gridAbove= false;
    bool m_collision= false;
    QColor m_backgroundColor= Qt::white;
    QColor m_toolColor= Qt::black;
    int m_npcNumber= 1;
    int m_penSize= 15;
    QString m_npcName;
    Core::ScaleUnit m_scaleUnit= Core::M;
    Core::Layer m_layer= Core::Layer::GROUND;
    Core::SelectableTool m_tool= Core::HANDLER;
    Core::EditionMode m_editionMode= Core::EditionMode::Painting;
    std::unique_ptr<VMap> m_vmap;
    std::map<Core::Properties, QVariant> m_properties;
};

#endif // VECTORIALMAPCONTROLLER_H
