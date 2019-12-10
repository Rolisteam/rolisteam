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

#include "undoCmd/addvmapitem.h"
#include "vmap/controller/ellipsecontroller.h"
#include "vmap/controller/imagecontroller.h"
#include "vmap/controller/linecontroller.h"
#include "vmap/controller/rectcontroller.h"

#include "vmap/manager/ellipscontrollermanager.h"
#include "vmap/manager/imagecontrollermanager.h"
#include "vmap/manager/linecontrollermanager.h"
#include "vmap/manager/rectcontrollermanager.h"

#include "worker/iohelper.h"

VectorialMapController::VectorialMapController(CleverURI* uri, QObject* parent)
    : AbstractMediaContainerController(uri, parent) /*, m_vmap(new VMap(this))*/
    , m_rectControllerManager(new RectControllerManager(this))
    , m_ellipseControllerManager(new EllipsControllerManager(this))
    , m_lineControllerManager(new LineControllerManager(this))
    , m_imageControllerManager(new ImageControllerManager(this))
{
    m_itemControllers.insert({Core::SelectableTool::EMPTYRECT, m_rectControllerManager.get()});
    m_itemControllers.insert({Core::SelectableTool::FILLRECT, m_rectControllerManager.get()});
    m_itemControllers.insert({Core::SelectableTool::FILLEDELLIPSE, m_ellipseControllerManager.get()});
    m_itemControllers.insert({Core::SelectableTool::EMPTYELLIPSE, m_ellipseControllerManager.get()});
    m_itemControllers.insert({Core::SelectableTool::LINE, m_lineControllerManager.get()});
    m_itemControllers.insert({Core::SelectableTool::IMAGE, m_imageControllerManager.get()});

    /* if(uri->hasData() || !uri->getUri().isEmpty())
         IOHelper::loadVMap(m_vmap.get(), uri, this);*/
}

VectorialMapController::~VectorialMapController()= default;

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

bool VectorialMapController::localGM() const
{
    return m_localGM;
}

bool VectorialMapController::stateLabelVisible() const
{
    return m_stateLabelVisible;
}

void VectorialMapController::saveData() const {}

void VectorialMapController::loadData() const {}

QString VectorialMapController::getLayerToText(Core::Layer id)
{
    static QStringList layerNames({QObject::tr("Ground"), QObject::tr("Object"), QObject::tr("Character")});

    auto idx= qBound(0, static_cast<int>(id), layerNames.size());
    return layerNames.at(idx);
}

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

void VectorialMapController::setLocalGM(bool b)
{
    if(m_localGM == b)
        return;
    m_localGM= b;
    emit localGMChanged();
}

void VectorialMapController::setToolColor(QColor color)
{
    if(color == m_toolColor)
        return;
    m_toolColor= color;
    emit toolColorChanged();
}

void VectorialMapController::setPenSize(quint16 size)
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

void VectorialMapController::setNpcNameVisible(bool visible)
{
    if(visible == m_npcNameVisible)
        return;
    m_npcNameVisible= visible;
    emit npcNameVisibleChanged();
}

void VectorialMapController::setPcNameVisible(bool visible)
{
    if(visible == m_pcNameVisible)
        return;
    m_pcNameVisible= visible;
    emit pcNameVisibleChanged();
}

void VectorialMapController::setHealthBarVisible(bool visible)
{
    if(visible == m_healthBarVisible)
        return;
    m_healthBarVisible= visible;
    emit healthBarVisibleChanged();
}

void VectorialMapController::setInitScoreVisible(bool visible)
{
    if(m_initScoreVisible == visible)
        return;
    m_initScoreVisible= visible;
    emit initScoreVisibleChanged();
}

void VectorialMapController::setNpcNumberVisible(bool visible)
{
    if(visible == m_npcNumberVisible)
        return;
    m_npcNumberVisible= visible;
    emit npcNumberVisibleChanged();
}

void VectorialMapController::setGridColor(QColor color)
{
    if(color == m_gridColor)
        return;
    m_gridColor= color;
    emit gridColorChanged();
}

void VectorialMapController::setCharacterVision(bool b)
{
    if(b == m_characterVision)
        return;
    m_characterVision= b;
    emit characterVisionChanged();
}

void VectorialMapController::setStateLabelVisible(bool b)
{
    if(b == m_stateLabelVisible)
        return;
    m_stateLabelVisible= b;
    emit stateLabelVisibleChanged();
}

void VectorialMapController::insertItemAt(const std::map<QString, QVariant>& params)
{
    auto tool= m_tool;
    if(params.end() != params.find(QStringLiteral("tool")))
        tool= params.at(QStringLiteral("tool")).value<Core::SelectableTool>();

    if(m_itemControllers.end() == m_itemControllers.find(tool))
        return;

    auto ctrl= m_itemControllers.at(tool);
    emit performCommand(new AddVmapItemCommand(this, ctrl, params));
}

/*QString VectorialMapController::addItemController(const std::map<QString, QVariant>& params)
{
    if(m_itemControllers.empty())
        return {};

    auto tool= params.at(QStringLiteral("tool")).value<Core::SelectableTool>();

    auto manager= m_itemControllers.at(tool);

    if(nullptr == manager)
        return {};

    return manager->addItem(params);*/
/* std::unique_ptr<VisualItemController> ctrl;
 switch(tool)
 {
 case Core::SelectableTool::FILLRECT:
 case Core::SelectableTool::EMPTYRECT:
     ctrl.reset(new RectController(this));
     break;
 default:
     break;
 }

 if(!ctrl)
     return nullptr;

 auto pCtrl= ctrl.get();
 m_items.push_back(std::move(ctrl));
 return pCtrl;*/
//}

RectControllerManager* VectorialMapController::rectManager() const
{
    return m_rectControllerManager.get();
}

EllipsControllerManager* VectorialMapController::ellipseManager() const
{
    return m_ellipseControllerManager.get();
}

LineControllerManager* VectorialMapController::lineManager() const
{
    return m_lineControllerManager.get();
}

ImageControllerManager* VectorialMapController::imageManager() const
{
    return m_imageControllerManager.get();
}

void VectorialMapController::removeItemController(QString uuid)
{
    std::for_each(m_itemControllers.begin(), m_itemControllers.end(),
                  [uuid](std::pair<Core::SelectableTool, VisualItemControllerManager*> itemManager) {
                      itemManager.second->removeItem(uuid);
                  });
    /* auto it
         = std::find_if(m_items.begin(), m_items.end(), [uuid](const std::unique_ptr<VisualItemController>& itemCtrl) {
               return itemCtrl->uuid() == uuid;
           });

     if(it == m_items.end())
         return;

     m_items.erase(it);*/
}

/*
 * void VMap::processCharacterStateHasChanged(NetworkMessageReader& msg)
{
    QString idItem= msg.string8();
    VisualItem* item= m_itemMap->value(idItem);
    if(item->getType() == VisualItem::CHARACTER)
    {
        QString idCharacter= msg.string8();
        QList<CharacterItem*> list= getCharacterOnMap(idCharacter);
        for(auto characterItem : list)
        {
            if(characterItem->getId() == idItem)
            {
                characterItem->readCharacterStateChanged(msg);
            }
        }
    }
}

void VMap::processCharacterHasChanged(NetworkMessageReader& msg)
{
    QString idItem= msg.string8();
    VisualItem* item= m_itemMap->value(idItem);
    if(item->getType() == VisualItem::CHARACTER)
    {
        auto character= dynamic_cast<CharacterItem*>(item);
        if(character == nullptr)
            return;

        QString idCharacter= msg.string8();
        if(idCharacter == character->getCharacterId())
            character->readCharacterChanged(msg);
    }
}

void VMap::processAddItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        VisualItem* item= nullptr;
        auto type= static_cast<VisualItem::ItemType>(msg->uint8());
        CharacterItem* charItem= nullptr;
        switch(type)
        {
        case VisualItem::TEXT:
            item= new TextItem(m_ctrl);
            break;
        case VisualItem::CHARACTER:
            charItem= new CharacterItem(m_ctrl);
            item= charItem;
            break;
        case VisualItem::LINE:
            item= new LineItem(m_ctrl);
            break;
        case VisualItem::RECT:
            item= new RectItem(m_ctrl);
            break;
        case VisualItem::ELLISPE:
            item= new EllipsItem(m_ctrl);
            break;
        case VisualItem::IMAGE:
            item= new ImageItem(m_ctrl);
            break;
        case VisualItem::PATH:
            item= new PathItem(m_ctrl);
            break;
        case VisualItem::SIGHT:
            item= m_sightItem;
            break;
        case VisualItem::GRID:
            item= m_gridItem;
            break;
        case VisualItem::HIGHLIGHTER:
            item= new HighlighterItem(m_ctrl);
            break;
        default:
            break;
        }
        if(nullptr != item)
        {
            item->readItem(msg);
            QPointF pos= item->pos();
            qreal z= item->zValue();
            // addNewItem(new AddVmapItemCommand(item, false, this), false, true);
            item->initChildPointItem();
            if(nullptr != charItem)
            {
                insertCharacterInMap(charItem);
            }
            item->setPos(pos);
            item->setZValue(z);
        }
    }
}
void VMap::processGeometryChangeItem(NetworkMessageReader* msg)
{
    QString idItem= msg->string16();
    if(m_itemMap->contains(idItem))
    {
        VisualItem* item= m_itemMap->value(idItem);
        item->readItem(msg);
    }
    else if(idItem == m_sightItem->getId())
    {
        m_sightItem->readItem(msg);
    }
}
void VMap::processMoveItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readPositionMsg(msg);
        }
    }
}
void VMap::processOpacityMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readOpacityMsg(msg);
        }
    }
}
void VMap::processLayerMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readLayerMsg(msg);
        }
    }
}
void VMap::processDelItemMessage(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        removeItemFromScene(id, false);
    }
}
void VMap::processSetParentItem(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString childId= msg->string8();
        QString parentId= msg->string8();
        VisualItem* childItem= m_itemMap->value(childId);

        VisualItem* parentItem= nullptr;
        if(parentId != QStringLiteral("nullptr"))
        {
            parentItem= m_itemMap->value(parentId);
        }
        if(nullptr != childItem)
        {
            setAnchor(childItem, parentItem, false);
        }
    }
}
void VMap::processZValueMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readZValueMsg(msg);
            ensureFogAboveAll();
        }
    }
}


void VMap::processRotationMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readRotationMsg(msg);
        }
    }
}
void VMap::processRectGeometryMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readRectGeometryMsg(msg);
        }
    }
}

void VMap::processVisionMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString CharacterId= msg->string16();
        QString itemId= msg->string16();
        QList<CharacterItem*> itemList= m_characterItemMap->values(CharacterId);
        for(auto item : itemList)
        {
            if((nullptr != item) && (item->getId() == itemId))
            {
                item->readVisionMsg(msg);
            }
        }
    }
}
void VMap::processColorMsg(NetworkMessageReader* msg)
{
    if(msg == nullptr)
        return;

    QString id= msg->string16();
    VisualItem* item= m_itemMap->value(id);
    QColor color= QColor::fromRgb(msg->rgb());
    if(nullptr != item)
    {
        item->setPenColor(color);
    }
}

void VMap::processMovePointMsg(NetworkMessageReader* msg)
{
    if(nullptr != msg)
    {
        QString id= msg->string16();
        VisualItem* item= m_itemMap->value(id);
        if(nullptr != item)
        {
            item->readMovePointMsg(msg);
        }
    }
}




























*/
