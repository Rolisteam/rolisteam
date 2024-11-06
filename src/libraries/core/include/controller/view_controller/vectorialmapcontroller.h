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
#include <QPointer>
#include <QRectF>
#include <memory>
#include <vector>

#include <core_global.h>

#include "controller/item_controllers/gridcontroller.h"
#include "controller/item_controllers/sightcontroller.h"
#include "controller/item_controllers/visualitemcontroller.h"
#include "media/mediatype.h"
#include "mediacontrollerbase.h"
#include "model/playermodel.h"
#include "model/vmapitemmodel.h"

namespace vmap
{
class VisualItemController;
} // namespace vmap

class VMap;
class Character;
class DiceRoller;
class CORE_EXPORT VectorialMapController : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(vmap::VmapItemModel* model READ model CONSTANT)
    Q_PROPERTY(vmap::GridController* gridController READ gridController CONSTANT)
    Q_PROPERTY(vmap::SightController* sightController READ sightController CONSTANT)
    Q_PROPERTY(bool npcNameVisible READ npcNameVisible WRITE setNpcNameVisible NOTIFY npcNameVisibleChanged)
    Q_PROPERTY(bool pcNameVisible READ pcNameVisible WRITE setPcNameVisible NOTIFY pcNameVisibleChanged)
    Q_PROPERTY(bool npcNumberVisible READ npcNumberVisible WRITE setNpcNumberVisible NOTIFY npcNumberVisibleChanged)
    Q_PROPERTY(bool healthBarVisible READ healthBarVisible WRITE setHealthBarVisible NOTIFY healthBarVisibleChanged)
    Q_PROPERTY(bool initScoreVisible READ initScoreVisible WRITE setInitScoreVisible NOTIFY initScoreVisibleChanged)
    Q_PROPERTY(bool stateLabelVisible READ stateLabelVisible WRITE setStateLabelVisible NOTIFY stateLabelVisibleChanged)
    Q_PROPERTY(bool collision READ collision WRITE setCollision NOTIFY collisionChanged)
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
    Q_PROPERTY(QRectF visualRect READ visualRect WRITE setVisualRect NOTIFY visualRectChanged)
    Q_PROPERTY(bool idle READ idle WRITE setIdle NOTIFY idleChanged)
    Q_PROPERTY(qreal zIndex READ zIndex WRITE setZindex NOTIFY zIndexChanged)
public:
    enum Method
    {
        Bigger,
        Smaller,
        UnderMouse,
        Average
    };
    Q_ENUM(Method)
    enum StackOrder
    {
        FRONT,
        RAISE,
        LOWER,
        BACK
    };
    Q_ENUM(StackOrder)
    explicit VectorialMapController(const QString& id= QString(), QObject* parent= nullptr);
    ~VectorialMapController();

    vmap::VmapItemModel* model() const;
    vmap::GridController* gridController() const;
    vmap::SightController* sightController() const;
    vmap::VisualItemController* itemController(const QString& id) const;

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
    bool stateLabelVisible() const;
    QRectF visualRect() const;
    bool idle() const;
    qreal zIndex() const;
    void addCommand(QUndoCommand* cmd);
    void addVision(CharacterVision* vision);

    QString layerToText(Core::Layer id);
    QString addItemController(const std::map<QString, QVariant>& params, bool isRemote= false);
    void addRemoteItem(vmap::VisualItemController* ctrl);
    void removeItemController(const QSet<QString>& ids, bool fromNetwork= false);
    void normalizeSize(const QList<vmap::VisualItemController*>& list, Method method, const QPointF& mousePos);
    bool pasteData(const QMimeData& data) override;
    void setDiceParser(DiceRoller* parser);
public slots:
    void showTransparentItems();
    void hideOtherLayers(bool b);
    void rollInit(Core::CharacterScope scope);
    void rollInit(QList<QPointer<vmap::CharacterItemController>> list);
    void cleanUpInit(Core::CharacterScope scope);
    void cleanUpInit(QList<QPointer<vmap::CharacterItemController>> list);
    void runDiceCommand(QList<QPointer<vmap::CharacterItemController>> list, QString cmd);
    void changeZValue(const QList<vmap::VisualItemController*>& list, VectorialMapController::StackOrder order);
    void setParent(vmap::VisualItemController* child, vmap::VisualItemController* newParent);

signals:
    void permissionChanged(Core::PermissionMode mode);
    void gridSizeChanged(int);
    void gridPatternChanged(Core::GridPattern p);
    void gridVisibilityChanged(bool);
    void visibilityChanged(Core::VisibilityMode a);
    void backgroundColorChanged(QColor);
    void toolColorChanged(const QColor& color);
    void penSizeChanged(quint16);
    void npcNameChanged(QString);
    void npcNumberChanged(int);
    void zoomLevelChanged();
    void gridAboveChanged(bool);
    void scaleUnitChanged(Core::ScaleUnit);
    void gridScaleChanged(qreal);
    void collisionChanged(bool);
    void layerChanged(Core::Layer);
    void toolChanged(Core::SelectableTool);
    void editionModeChanged(Core::EditionMode);
    void opacityChanged(qreal);
    void npcNameVisibleChanged(bool);
    void pcNameVisibleChanged(bool);
    void healthBarVisibleChanged(bool);
    void initScoreVisibleChanged(bool);
    void npcNumberVisibleChanged(bool);
    void gridColorChanged(QColor);
    void characterVisionChanged(bool);
    void stateLabelVisibleChanged(bool);
    void idleChanged(bool);
    void zIndexChanged(int);
    void visualRectChanged(QRectF visualRect);
    void visualItemControllerCreated(vmap::VisualItemController* ctrl);
    void visualItemControllersRemoved(const QStringList& ids);

    void sendOffHighLightAt(const QPointF& p, const qreal& penSize, const QColor& color);
    void highLightAt(const QPointF& p, const qreal& penSize, const QColor& color);

public slots:
    void setPermission(Core::PermissionMode mode);
    void setGridVisibility(bool visible);
    void setGridAbove(bool above);
    void setGridSize(int gridSize);
    void setGridPattern(Core::GridPattern pattern);
    void setVisibility(Core::VisibilityMode mode);
    void setBackgroundColor(QColor color);
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
    void setVisualRect(QRectF visualRect);
    void setIdle(bool b);
    void setZindex(qreal index);
    void setRemote(bool remote) override;

    void insertItemAt(const std::map<QString, QVariant>& params);
    void changeFogOfWar(const QPolygonF& poly, vmap::VisualItemController* itemCtrl, bool mask, bool temp= false);

    void aboutToRemove(const QList<vmap::VisualItemController*>& list);
    void askForColorChange(vmap::VisualItemController* itemCtrl);
    void addHighLighter(const QPointF& point);
    void showHightLighter(const QPointF& p, const qreal& penSize, const QColor& color);
    void dupplicateItem(const QList<vmap::VisualItemController*>& vitem);

private:
    std::unique_ptr<vmap::VmapItemModel> m_vmapModel;
    std::unique_ptr<vmap::GridController> m_gridController;
    std::unique_ptr<vmap::SightController> m_sightController;

    bool m_pcNameVisible= true;
    bool m_npcNameVisible= true;
    bool m_healthBarVisible= true;
    bool m_initScoreVisible= true;
    bool m_npcNumberVisible= true;
    bool m_characterVision= false;
    bool m_stateLabelVisible= false;
    Core::PermissionMode m_permission= Core::GM_ONLY;
    Core::GridPattern m_gridPattern= Core::GridPattern::NONE;
    Core::VisibilityMode m_visibilityMode= Core::HIDDEN;
    qreal m_zoomLevel= 1.0;
    qreal m_opacity= 1.0;
    qreal m_gridScale= 5.0;
    int m_gridSize= 50;
    bool m_gridVisibility= false;
    bool m_gridAbove= false;
    bool m_collision= false;
    bool m_idle= true;
    QColor m_backgroundColor= Qt::white;
    QColor m_toolColor= Qt::black;
    QColor m_gridColor= Qt::black;
    int m_npcNumber= 1;
    quint16 m_penSize= 15;
    QString m_npcName;
    QRectF m_visualRect;
    qreal m_zIndex= 0.;
    Core::ScaleUnit m_scaleUnit= Core::M;
    Core::Layer m_layer= Core::Layer::GROUND;
    Core::SelectableTool m_tool= Core::HANDLER;
    Core::EditionMode m_editionMode= Core::EditionMode::Painting;
    QPointer<DiceRoller> m_diceParser;
};

#endif // VECTORIALMAPCONTROLLER_H
