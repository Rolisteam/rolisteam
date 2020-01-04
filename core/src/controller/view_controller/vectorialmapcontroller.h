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
#include <vector>

#include "abstractmediacontroller.h"
#include "media/mediatype.h"

namespace vmap
{
class GridController;
class SightController;
} // namespace vmap

class CleverURI;
class VMap;
class NetworkMessageReader;
class VisualItemController;
class VisualItemControllerManager;
class RectControllerManager;
class EllipsControllerManager;
class LineControllerManager;
class ImageControllerManager;
class PathControllerManager;
class TextControllerManager;
class CharacterItemControllerManager;
class VectorialMapController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(bool npcNameVisible READ npcNameVisible WRITE setNpcNameVisible NOTIFY npcNameVisibleChanged)
    Q_PROPERTY(bool pcNameVisible READ pcNameVisible WRITE setPcNameVisible NOTIFY pcNameVisibleChanged)
    Q_PROPERTY(bool npcNumberVisible READ npcNumberVisible WRITE setNpcNumberVisible NOTIFY npcNumberVisibleChanged)
    Q_PROPERTY(bool healthBarVisible READ healthBarVisible WRITE setHealthBarVisible NOTIFY healthBarVisibleChanged)
    Q_PROPERTY(bool initScoreVisible READ initScoreVisible WRITE setInitScoreVisible NOTIFY initScoreVisibleChanged)
    Q_PROPERTY(bool stateLabelVisible READ stateLabelVisible WRITE setStateLabelVisible NOTIFY stateLabelVisibleChanged)
    Q_PROPERTY(bool collision READ collision WRITE setCollision NOTIFY collisionChanged)
    Q_PROPERTY(bool localGM READ localGM WRITE setLocalGM NOTIFY localGMChanged)
    Q_PROPERTY(bool characterVision READ characterVision WRITE setCharacterVision NOTIFY characterVisionChanged)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor NOTIFY gridColorChanged)
    Q_PROPERTY(qreal gridScale READ gridScale WRITE setGridScale NOTIFY gridScaleChanged)
    Q_PROPERTY(int gridSize READ gridSize WRITE setGridSize NOTIFY gridSizeChanged)
    Q_PROPERTY(bool gridVisibility READ gridVisibility WRITE setGridVisibility NOTIFY gridVisibilityChanged)
    Q_PROPERTY(bool gridAbove READ gridAbove WRITE setGridAbove NOTIFY gridAboveChanged)
    Q_PROPERTY(Core::ScaleUnit scaleUnit READ scaleUnit WRITE setScaleUnit NOTIFY scaleUnitChanged)
    Q_PROPERTY(QString npcName READ npcName WRITE setNpcName NOTIFY npcNameChanged)
    Q_PROPERTY(Core::PermissionMode permission READ permission WRITE setPermission NOTIFY permissionChanged)
    Q_PROPERTY(Core::GridPattern gridPattern READ gridPattern WRITE setGridPattern NOTIFY gridPatternChanged)
    Q_PROPERTY(Core::VisibilityMode visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
    Q_PROPERTY(QColor toolColor READ toolColor WRITE setToolColor NOTIFY toolColorChanged)
    Q_PROPERTY(quint16 penSize READ penSize WRITE setPenSize NOTIFY penSizeChanged)
    Q_PROPERTY(int npcNumber READ npcNumber WRITE setNpcNumber NOTIFY npcNumberChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(Core::Layer layer READ layer WRITE setLayer NOTIFY layerChanged)
    Q_PROPERTY(Core::SelectableTool tool READ tool WRITE setTool NOTIFY toolChanged)
    Q_PROPERTY(Core::EditionMode editionMode READ editionMode WRITE setEditionMode NOTIFY editionModeChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)

public:
    explicit VectorialMapController(CleverURI* uri, QObject* parent= nullptr);
    ~VectorialMapController();

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
    QColor gridColor() const;
    quint16 penSize() const;
    QString npcName() const;
    int npcNumber() const;
    qreal zoomLevel() const;
    Core::Layer layer() const;
    Core::SelectableTool tool() const;
    Core::EditionMode editionMode() const;
    qreal opacity() const;
    bool npcNameVisible() const;
    bool pcNameVisible() const;
    bool npcNumberVisible() const;
    bool healthBarVisible() const;
    bool initScoreVisible() const;
    bool characterVision() const;
    bool localGM() const;
    bool stateLabelVisible() const;

    void saveData() const;
    void loadData() const;

    QString getLayerToText(Core::Layer id);

    RectControllerManager* rectManager() const;
    EllipsControllerManager* ellipseManager() const;
    LineControllerManager* lineManager() const;
    ImageControllerManager* imageManager() const;
    PathControllerManager* pathManager() const;
    TextControllerManager* textManager() const;
    CharacterItemControllerManager* characterManager() const;

    vmap::GridController* gridController() const;

    QString addItemController(const std::map<QString, QVariant>& params);
    void removeItemController(QString uuid);

    // Network
    void processAddItemMessage(const NetworkMessageReader& msg);
    void processMoveItemMessage(const NetworkMessageReader& msg);
    void processGeometryChangeItem(const NetworkMessageReader& msg);
    void processOpacityMessage(const NetworkMessageReader& msg);
    void processSetParentItem(const NetworkMessageReader& msg);
    void processDelItemMessage(const NetworkMessageReader& msg);
    void processMovePointMsg(const NetworkMessageReader& msg);
    void processZValueMsg(const NetworkMessageReader& msg);
    void processRotationMsg(const NetworkMessageReader& msg);
    void processRectGeometryMsg(const NetworkMessageReader& msg);
    void processCharacterStateHasChanged(const NetworkMessageReader& msg);
    void processCharacterHasChanged(const NetworkMessageReader& msg);
    void processVisionMsg(const NetworkMessageReader& msg);
    void processColorMsg(const NetworkMessageReader& msg);

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
    void npcNameVisibleChanged();
    void pcNameVisibleChanged();
    void healthBarVisibleChanged();
    void initScoreVisibleChanged();
    void localGMChanged();
    void npcNumberVisibleChanged();
    void gridColorChanged();
    void characterVisionChanged();
    void stateLabelVisibleChanged();
    void visualItemControllerCreated(VisualItemController* ctrl);

public slots:
    void setPermission(Core::PermissionMode mode);
    void setGridVisibility(bool visible);
    void setGridAbove(bool above);
    void setGridSize(int gridSize);
    void setGridPattern(Core::GridPattern pattern);
    void setVisibility(Core::VisibilityMode mode);
    void setBackgroundColor(QColor color);
    void setLocalGM(bool b);
    void setToolColor(QColor color);
    void setPenSize(quint16 size);
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
    void setNpcNameVisible(bool visible);
    void setPcNameVisible(bool visible);
    void setHealthBarVisible(bool visible);
    void setInitScoreVisible(bool visible);
    void setNpcNumberVisible(bool visible);
    void setGridColor(QColor color);
    void setCharacterVision(bool b);
    void setStateLabelVisible(bool b);

    void insertItemAt(const std::map<QString, QVariant>& params);
    void changeFogOfWar(const QPolygonF& poly, bool mask);

private:
    bool m_pcNameVisible= true;
    bool m_npcNameVisible= true;
    bool m_healthBarVisible= true;
    bool m_initScoreVisible= true;
    bool m_localGM= false;
    bool m_npcNumberVisible= true;
    bool m_characterVision= false;
    bool m_stateLabelVisible= false;
    Core::PermissionMode m_permission= Core::GM_ONLY;
    Core::GridPattern m_gridPattern= Core::NONE;
    Core::VisibilityMode m_visibilityMode= Core::HIDDEN;
    qreal m_zoomLevel= 1.0;
    qreal m_opacity= 1.0;
    qreal m_gridScale= 5.0;
    int m_gridSize= 50;
    bool m_gridVisibility= false;
    bool m_gridAbove= false;
    bool m_collision= false;
    QColor m_backgroundColor= Qt::white;
    QColor m_toolColor= Qt::black;
    QColor m_gridColor= Qt::black;
    int m_npcNumber= 1;
    quint16 m_penSize= 15;
    QString m_npcName;
    Core::ScaleUnit m_scaleUnit= Core::M;
    Core::Layer m_layer= Core::Layer::GROUND;
    Core::SelectableTool m_tool= Core::HANDLER;
    Core::EditionMode m_editionMode= Core::EditionMode::Painting;
    std::map<Core::SelectableTool, VisualItemControllerManager*> m_itemControllers;

    std::unique_ptr<RectControllerManager> m_rectControllerManager;
    std::unique_ptr<EllipsControllerManager> m_ellipseControllerManager;
    std::unique_ptr<LineControllerManager> m_lineControllerManager;
    std::unique_ptr<ImageControllerManager> m_imageControllerManager;
    std::unique_ptr<PathControllerManager> m_pathControllerManager;
    std::unique_ptr<TextControllerManager> m_textControllerManager;
    std::unique_ptr<CharacterItemControllerManager> m_characterControllerManager;

    std::unique_ptr<vmap::GridController> m_gridController;
    std::unique_ptr<vmap::SightController> m_sightController;
};

#endif // VECTORIALMAPCONTROLLER_H
