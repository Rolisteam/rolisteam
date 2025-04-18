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
#include "controller/view_controller/vectorialmapcontroller.h"

#include <set>

#include "diceparser_qobject/diceroller.h"

#include "undoCmd/addfogofwarchangecommand.h"
#include "undoCmd/addvmapitem.h"
#include "undoCmd/anchorvmapitemcommand.h"
#include "undoCmd/changecoloritem.h"
#include "undoCmd/changesizevmapitem.h"
#include "undoCmd/deletevmapitem.h"
#include "undoCmd/dupplicatevmapitem.h"
#include "undoCmd/hideotherlayercommand.h"
#include "undoCmd/rollinitcommand.h"
#include "undoCmd/showtransparentitemcommand.h"

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/item_controllers/vmapitemfactory.h"

#include "worker/iohelper.h"

VectorialMapController::VectorialMapController(const QString& id, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::VECTORIALMAP, parent)
    , m_vmapModel(new vmap::VmapItemModel)
    , m_gridController(new vmap::GridController(this))
{
    m_sightController.reset(new vmap::SightController(this));

    connect(m_vmapModel.get(), &vmap::VmapItemModel::itemControllerAdded, this,
            &VectorialMapController::visualItemControllerCreated);

    connect(m_vmapModel.get(), &vmap::VmapItemModel::itemControllersRemoved, this,
            &VectorialMapController::visualItemControllersRemoved);

    connect(m_vmapModel.get(), &vmap::VmapItemModel::npcAdded, this, [this]() { setNpcNumber(npcNumber() + 1); });
    connect(m_vmapModel.get(), &vmap::VmapItemModel::rowsInserted, this, [this] { setModified(); });
    connect(m_vmapModel.get(), &vmap::VmapItemModel::rowsRemoved, this, [this] { setModified(); });
    connect(m_vmapModel.get(), &vmap::VmapItemModel::dataChanged, this, [this] { setModified(); });
    connect(this, &VectorialMapController::modifiedChanged, this,
            [this](bool b)
            {
                if(!b)
                    m_vmapModel->setModifiedToAllItem();
            });
}

VectorialMapController::~VectorialMapController()= default;

vmap::GridController* VectorialMapController::gridController() const
{
    return m_gridController.get();
}

vmap::SightController* VectorialMapController::sightController() const
{
    return m_sightController.get();
}

vmap::VisualItemController* VectorialMapController::itemController(const QString& id) const
{
    auto res= m_vmapModel->item(id);
    if(!res)
    {
        if(id == m_sightController->uuid())
            res= sightController();
    }
    return res;
}

QString VectorialMapController::addItemController(const std::map<QString, QVariant>& params, bool initialized,
                                                  bool isRemote)
{
    auto tool= m_tool;
    auto it= params.find(Core::vmapkeys::KEY_TOOL);
    if(it != params.end())
        tool= static_cast<Core::SelectableTool>(it->second.toInt());

    auto item= vmap::VmapItemFactory::createVMapItem(this, tool, params);

    if(!item)
        return {};

    item->setInitialized(initialized);
    item->setRemote(isRemote);
    m_vmapModel->appendItemController(item);
    return item->uuid();
}

void VectorialMapController::addRemoteItem(vmap::VisualItemController* ctrl)
{
    m_vmapModel->appendItemController(ctrl);
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
    auto color= m_toolColor;
    switch(m_editionMode)
    {
    case Core::EditionMode::Mask:
        color= Qt::black;
        break;
    case Core::EditionMode::Painting:
        color.setAlpha(255);
        break;
    case Core::EditionMode::Unmask:
        color.setAlpha(128);
        break;
    }

    return color;
}

QColor VectorialMapController::gridColor() const
{
    return m_gridColor;
}

quint16 VectorialMapController::penSize() const
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

bool VectorialMapController::npcNameVisible() const
{
    return m_npcNameVisible;
}

bool VectorialMapController::pcNameVisible() const
{
    return m_pcNameVisible;
}

bool VectorialMapController::npcNumberVisible() const
{
    return m_npcNumberVisible;
}

bool VectorialMapController::healthBarVisible() const
{
    return m_healthBarVisible;
}

bool VectorialMapController::initScoreVisible() const
{
    return m_initScoreVisible;
}

bool VectorialMapController::characterVision() const
{
    return m_characterVision;
}

bool VectorialMapController::stateLabelVisible() const
{
    return m_stateLabelVisible;
}

QRectF VectorialMapController::visualRect() const
{
    return m_visualRect;
}

void VectorialMapController::setVisualRect(QRectF visualRect)
{
    if(m_visualRect == visualRect)
        return;

    m_visualRect= visualRect;
    emit visualRectChanged(m_visualRect);
}

QString VectorialMapController::layerToText(Core::Layer id)
{
    static QStringList layerNames(
        {QObject::tr("Ground"), QObject::tr("Object"), QObject::tr("Character"), QObject::tr("GameMaster")});

    auto idx= qBound(0, static_cast<int>(id), layerNames.size() - 1);
    return layerNames.at(idx);
}

vmap::VmapItemModel* VectorialMapController::model() const
{
    return m_vmapModel.get();
}

void VectorialMapController::addVision(CharacterVision* vision)
{
    m_sightController->addCharacterVision(vision);
}

void VectorialMapController::setPermission(Core::PermissionMode mode)
{
    if(m_permission == mode)
        return;
    m_permission= mode;
    emit permissionChanged(m_permission);
}

void VectorialMapController::setGridVisibility(bool visible)
{
    if(visible == m_gridVisibility)
        return;
    m_gridVisibility= visible;
    emit gridVisibilityChanged(m_gridVisibility);
}

void VectorialMapController::setGridAbove(bool above)
{
    if(m_gridAbove == above)
        return;
    m_gridAbove= above;
    emit gridAboveChanged(m_gridAbove);
}

void VectorialMapController::setGridSize(int gridSize)
{
    if(m_gridSize == gridSize)
        return;
    m_gridSize= gridSize;
    emit gridSizeChanged(m_gridSize);
}

void VectorialMapController::setGridPattern(Core::GridPattern pattern)
{
    if(pattern == m_gridPattern)
        return;
    m_gridPattern= pattern;
    emit gridPatternChanged(m_gridPattern);
}

void VectorialMapController::setVisibility(Core::VisibilityMode mode)
{
    if(mode == m_visibilityMode)
        return;
    m_visibilityMode= mode;
    emit visibilityChanged(m_visibilityMode);
}

void VectorialMapController::setBackgroundColor(QColor color)
{
    if(color == m_backgroundColor)
        return;
    m_backgroundColor= color;
    emit backgroundColorChanged(m_backgroundColor);
}

void VectorialMapController::setToolColor(QColor color)
{
    if(color == m_toolColor)
        return;
    m_toolColor= color;
    emit toolColorChanged(color);
}

void VectorialMapController::setPenSize(quint16 size)
{
    if(size == m_penSize)
        return;
    m_penSize= size;
    emit penSizeChanged(m_penSize);
}

void VectorialMapController::setNpcName(const QString& name)
{
    if(name == m_npcName)
        return;
    m_npcName= name;
    emit npcNameChanged(m_npcName);
}

void VectorialMapController::setNpcNumber(int number)
{
    if(number == m_npcNumber)
        return;
    m_npcNumber= number;
    emit npcNumberChanged(m_npcNumber);
}

void VectorialMapController::setGridScale(qreal scale)
{
    if(qFuzzyCompare(m_gridScale, scale))
        return;
    m_gridScale= scale;
    emit gridScaleChanged(m_gridScale);
}

void VectorialMapController::setScaleUnit(Core::ScaleUnit unit)
{
    if(m_scaleUnit == unit)
        return;
    m_scaleUnit= unit;
    emit scaleUnitChanged(m_scaleUnit);
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
    emit collisionChanged(m_collision);
}

void VectorialMapController::setLayer(Core::Layer layer)
{
    if(layer == m_layer)
        return;
    m_layer= layer;
    emit layerChanged(m_layer);
}

void VectorialMapController::setTool(Core::SelectableTool tool)
{
    if(tool == m_tool)
        return;
    m_tool= tool;
    emit toolChanged(m_tool);
}

void VectorialMapController::setEditionMode(Core::EditionMode mode)
{
    if(mode == m_editionMode)
        return;
    m_editionMode= mode;
    emit editionModeChanged(m_editionMode);
    emit toolColorChanged(toolColor());
}

void VectorialMapController::setOpacity(qreal opacity)
{
    if(qFuzzyCompare(m_opacity, opacity))
        return;
    m_opacity= opacity;
    emit opacityChanged(m_opacity);
}

void VectorialMapController::setNpcNameVisible(bool visible)
{
    if(visible == m_npcNameVisible)
        return;
    m_npcNameVisible= visible;
    emit npcNameVisibleChanged(m_npcNameVisible);
}

void VectorialMapController::setPcNameVisible(bool visible)
{
    if(visible == m_pcNameVisible)
        return;
    m_pcNameVisible= visible;
    emit pcNameVisibleChanged(m_pcNameVisible);
}

void VectorialMapController::setHealthBarVisible(bool visible)
{
    if(visible == m_healthBarVisible)
        return;
    m_healthBarVisible= visible;
    emit healthBarVisibleChanged(m_healthBarVisible);
}

void VectorialMapController::setInitScoreVisible(bool visible)
{
    if(m_initScoreVisible == visible)
        return;
    m_initScoreVisible= visible;
    emit initScoreVisibleChanged(m_initScoreVisible);
}

void VectorialMapController::setNpcNumberVisible(bool visible)
{
    if(visible == m_npcNumberVisible)
        return;
    m_npcNumberVisible= visible;
    emit npcNumberVisibleChanged(m_npcNumberVisible);
}

void VectorialMapController::setGridColor(QColor color)
{
    if(color == m_gridColor)
        return;
    m_gridColor= color;
    emit gridColorChanged(m_gridColor);
}

void VectorialMapController::setCharacterVision(bool b)
{
    if(b == m_characterVision)
        return;
    m_characterVision= b;
    emit characterVisionChanged(m_characterVision);
}

void VectorialMapController::setStateLabelVisible(bool b)
{
    if(b == m_stateLabelVisible)
        return;
    m_stateLabelVisible= b;
    emit stateLabelVisibleChanged(m_stateLabelVisible);
}

void VectorialMapController::insertItemAt(const std::map<QString, QVariant>& params)
{
    auto tool= m_tool;

    if(params.end() != params.find(Core::vmapkeys::KEY_TOOL))
        tool= params.at(Core::vmapkeys::KEY_TOOL).value<Core::SelectableTool>();

    setIdle(false);
    emit performCommand(new AddVmapItemCommand(m_vmapModel.get(), tool, this, params));
}

void VectorialMapController::aboutToRemove(const QList<vmap::VisualItemController*>& list)
{
    emit performCommand(new DeleteVmapItemCommand(this, list));
}

void VectorialMapController::askForColorChange(vmap::VisualItemController* itemCtrl)
{
    emit performCommand(new ChangeColorItemCmd(itemCtrl, toolColor()));
}

void VectorialMapController::changeFogOfWar(const QPolygonF& poly, vmap::VisualItemController* itemCtrl, bool mask,
                                            bool temp)
{
    Q_UNUSED(itemCtrl)
    if(poly.isEmpty())
        return;

    if(temp)
    {
        m_sightController->addPolygon(poly, mask, temp);
        return;
    }
    emit popCommand(); // remove controller of shape
    emit performCommand(new AddFogOfWarChangeCommand(m_sightController.get(), poly, mask));
}

void VectorialMapController::addHighLighter(const QPointF& point)
{
    auto color= localGM() ? m_toolColor : m_localColor;
    emit sendOffHighLightAt(point, m_penSize, color);
    emit highLightAt(point, m_penSize, color);
}

void VectorialMapController::showHightLighter(const QPointF& p, const qreal& penSize, const QColor& color)
{
    emit highLightAt(p, penSize, color);
}

bool VectorialMapController::idle() const
{
    return m_idle;
}

qreal VectorialMapController::zIndex() const
{
    return m_zIndex;
}

void VectorialMapController::setIdle(bool b)
{
    if(b == m_idle)
        return;
    m_idle= b;
    emit idleChanged(m_idle);
}

void VectorialMapController::setZindex(qreal index)
{
    if(qFuzzyCompare(index, m_zIndex))
        return;
    m_zIndex= index;
    emit zIndexChanged(m_zIndex);
}

void VectorialMapController::setRemote(bool remote)
{
    MediaControllerBase::setRemote(remote);
    m_sightController->setRemote(remote);
    // m_gridController->setRemote(remote);
}

void VectorialMapController::normalizeSize(const QList<vmap::VisualItemController*>& list, Method method,
                                           const QPointF& mousePos)
{
    if(list.isEmpty())
        return;

    emit performCommand(new ChangeSizeVmapItemCommand(list, method, mousePos));
}

bool VectorialMapController::pasteData(const QMimeData& mimeData)
{
    if(!mimeData.hasImage())
        return false;

    auto pix= qvariant_cast<QPixmap>(mimeData.imageData());
    if(pix.isNull())
        return false;

    auto databytes= IOHelper::pixmapToData(pix);

    insertItemAt({{"tool", Core::SelectableTool::IMAGE}, {"data", databytes}});
    return true;
}

void VectorialMapController::showTransparentItems()
{
    auto const& items= m_vmapModel->items();
    QList<vmap::VisualItemController*> list;
    std::transform(std::begin(items), std::end(items), std::back_inserter(list),
                   [](vmap::VisualItemController* item)
                   { return qFuzzyCompare(item->opacity(), 0.0) ? item : nullptr; });
    list.removeAll(nullptr);
    emit performCommand(new ShowTransparentItemCommand(list));
}

void VectorialMapController::hideOtherLayers(bool b)
{
    emit performCommand(new HideOtherLayerCommand(m_layer, m_vmapModel.get(), b));
}

QList<QPointer<vmap::CharacterItemController>> sublist(Core::CharacterScope scope,
                                                       const std::vector<vmap::VisualItemController*>& data)
{
    QList<QPointer<vmap::CharacterItemController>> list;
    for(auto const& ctrl : data)
    {
        if(ctrl->itemType() != vmap::VisualItemController::CHARACTER)
            continue;
        auto characterCtrl= dynamic_cast<vmap::CharacterItemController*>(ctrl);
        if(!characterCtrl)
            continue;
        bool res= false;
        switch(scope)
        {
        case Core::CharacterScope::AllNPC:
            res= !characterCtrl->playableCharacter();
            break;
        case Core::CharacterScope::AllCharacter:
            res= true;
            break;
        case Core::CharacterScope::SelectionOnly:
            res= characterCtrl->selected();
            break;
        }

        if(res)
        {
            list << characterCtrl;
        }
    }
    return list;
}
void VectorialMapController::rollInit(Core::CharacterScope scope)
{
    auto items= m_vmapModel->items();
    auto list= sublist(scope, items);
    emit performCommand(new RollInitCommand(list, m_diceParser));
}

void VectorialMapController::rollInit(QList<QPointer<vmap::CharacterItemController>> list)
{
    emit performCommand(new RollInitCommand(list, m_diceParser));
}

void VectorialMapController::setDiceParser(DiceRoller* parser)
{
    m_diceParser= parser;
}

void VectorialMapController::cleanUpInit(Core::CharacterScope scope)
{
    auto items= m_vmapModel->items();
    auto list= sublist(scope, items);
    addCommand(new CleanUpRollCommand(list));
}
void VectorialMapController::cleanUpInit(QList<QPointer<vmap::CharacterItemController>> list)
{
    addCommand(new CleanUpRollCommand(list));
}

void VectorialMapController::runDiceCommand(QList<QPointer<vmap::CharacterItemController>> list, QString cmd)
{
    addCommand(new CleanUpRollCommand(list));
}

void VectorialMapController::changeZValue(const QList<vmap::VisualItemController*>& list, StackOrder order)
{
    qDebug() << list.size() << "list size" << order;
    // emit performCommand(new ChangeStackOrderVMapCommand(this, list, order));
}

void VectorialMapController::setParent(vmap::VisualItemController* child, vmap::VisualItemController* newParent)
{
    if(!child)
        return;
    addCommand(new AnchorVMapItemCommand(child, newParent));
}

void VectorialMapController::stackBefore(const QStringList& first, const QStringList& second, bool fromNetwork)
{
    emit itemHasBeenStacked(first, second, fromNetwork);
}

void VectorialMapController::removeItemController(const QSet<QString>& ids, bool fromNetwork)
{
    m_vmapModel->removeItemController(ids, fromNetwork);
}
void VectorialMapController::addCommand(QUndoCommand* cmd)
{
    emit performCommand(cmd);
}
void VectorialMapController::dupplicateItem(const QList<vmap::VisualItemController*>& vitem)
{
    addCommand(new DupplicateVMapItem(vitem, this));
}
