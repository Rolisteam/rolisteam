/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                                 *
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
#include "controller/view_controller/mindmapcontrollerbase.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QUrl>
// #include <random>

#include "mindmap/command/addimagetonodecommand.h"
#include "mindmap/command/additemcommand.h"
#include "mindmap/command/removeimagefromnodecommand.h"
#include "mindmap/command/removenodecommand.h"
#include "mindmap/command/reparentingnodecommand.h"
#include "mindmap/controller/selectioncontroller.h"
#include "mindmap/controller/spacingcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/data/packagenode.h"
#include "mindmap/data/positioneditem.h"
#include "mindmap/model/imagemodel.h"
#include "mindmap/model/nodestylemodel.h"
#include "mindmap/worker/fileserializer.h"
#include "utils/iohelper.h"
#include "worker/campaignfinder.h"

namespace mindmap
{
MindMapControllerBase::MindMapControllerBase(bool hasNetwork, const QString& id, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::MINDMAP, parent)
    , m_selectionController(new SelectionController())
    , m_imgModel(new mindmap::ImageModel)
    , m_itemModel(new MindItemModel(m_imgModel.get()))
    , m_styleModel(new NodeStyleModel())
    , m_hasNetwork(hasNetwork)
{
    m_selectionController->setUndoStack(&m_stack);

    connect(&m_stack, &QUndoStack::canRedoChanged, this, &MindMapControllerBase::canRedoChanged);
    connect(&m_stack, &QUndoStack::canUndoChanged, this, &MindMapControllerBase::canUndoChanged);
    connect(m_itemModel.get(), &MindItemModel::defaultStyleIndexChanged, this,
            &MindMapControllerBase::defaultStyleIndexChanged);

    connect(m_itemModel.get(), &MindItemModel::latestInsertedPackage, this, &MindMapControllerBase::setCurrentPackage);
    connect(m_itemModel.get(), &MindItemModel::rowsInserted, this,
            [this]() { setContentRect(m_itemModel->contentRect()); });
    connect(m_itemModel.get(), &MindItemModel::rowsRemoved, this,
            [this]() { setContentRect(m_itemModel->contentRect()); });
    connect(m_itemModel.get(), &MindItemModel::geometryChanged, this,
            [this]() { setContentRect(m_itemModel->contentRect()); });

    connect(m_selectionController.get(), &SelectionController::hasSelectionChanged, this,
            &MindMapControllerBase::hasSelectionChanged);

    // Spacing
    m_spacing.reset(new QThread());
    m_spacingController.reset(new SpacingController(m_itemModel.get()));
    m_spacingController->moveToThread(m_spacing.get());
    connect(m_spacing.get(), &QThread::started, m_spacingController.get(), &SpacingController::computeInLoop);
    connect(m_spacingController.get(), &SpacingController::runningChanged, this,
            &MindMapControllerBase::spacingChanged);
    connect(m_spacingController.get(), &SpacingController::runningChanged, this,
            [this]()
            {
                if(m_spacingController->running())
                    m_spacing->start();
            });
    connect(m_spacingController.get(), &SpacingController::finished, m_spacing.get(), &QThread::quit);
    m_spacing->start();
    // end Spacing

    m_contentRect= m_itemModel->contentRect();

    resetData();
}

MindMapControllerBase::~MindMapControllerBase()
{
    if(m_spacing && m_spacing->isRunning())
    {
        m_spacingController->setRunning(false);
        m_spacing->quit();
        m_spacing->wait();
        m_spacing.release()->deleteLater();
    }

    auto styles= m_styleModel.release();
    auto model= m_itemModel.release();
    auto spacingCtrl= m_spacingController.release();

    delete spacingCtrl;
    delete model;
    delete styles;
}

MindItemModel* MindMapControllerBase::itemModel() const
{
    return m_itemModel.get();
}

QAbstractItemModel* MindMapControllerBase::styleModel() const
{
    return m_styleModel.get();
}

ImageModel* MindMapControllerBase::imgModel() const
{
    return m_imgModel.get();
}

mindmap::SpacingController* MindMapControllerBase::spacingCtrl() const
{
    return m_spacingController.get();
}

const QString& MindMapControllerBase::errorMsg() const
{
    return m_errorMsg;
}

QRectF MindMapControllerBase::contentRect() const
{
    return m_contentRect;
}

void MindMapControllerBase::clearData()
{
    // m_linkModel->clear();
    m_itemModel->clear();
}

void MindMapControllerBase::resetData()
{
    clearData();

    /*auto root= new MindNode();
    root->setText(tr("Root"));
    m_itemModel->appendItem({root});*/
}

void MindMapControllerBase::setErrorMsg(const QString& msg)
{
    if(m_errorMsg == msg)
        return;
    m_errorMsg= msg;
    emit errorMsgChanged();
}

void MindMapControllerBase::importFile(const QString& path)
{
    clearData();
    /*  if(!FileSerializer::readTextFile(m_itemModel.get(), path))
          setErrorMsg(tr("File can't be loaded: %1").arg(m_filename));*/
}

void MindMapControllerBase::setDefaultStyleIndex(int indx)
{
    m_itemModel->setDefaultStyleIndex(indx);
}

NodeStyle* MindMapControllerBase::style(int index) const
{
    return m_styleModel->getStyle(index);
}

void MindMapControllerBase::setSpacing(bool status)
{
    m_spacingController->setRunning(status);
}

void MindMapControllerBase::redo()
{
    m_stack.redo();
}

void MindMapControllerBase::undo()
{
    m_stack.undo();
}

bool MindMapControllerBase::spacing() const
{
    return m_spacingController->running();
}

SelectionController* MindMapControllerBase::selectionController() const
{
    return m_selectionController.get();
}

bool MindMapControllerBase::canRedo() const
{
    return readWrite() && m_stack.canRedo();
}

void MindMapControllerBase::addNode(const QString& idparent)
{
    if(!readWrite())
        return;
    auto cmd= new mindmap::AddItemCommand(m_itemModel.get(), MindItem::NodeType, idparent);
    m_stack.push(cmd);
}

void MindMapControllerBase::centerItems(qreal w, qreal h)
{
    auto isSpacing= spacing();
    if(isSpacing)
        setSpacing(false);

    auto rect= contentRect();
    auto viewport= QRectF{0, 0, w, h};

    auto vec= viewport.center() - rect.center();

    auto items= m_itemModel->positionnedItems();

    for(auto& item : std::as_const(items))
    {
        item->setPosition(item->position() + vec);
    }

    if(isSpacing)
        setSpacing(true);
}

void MindMapControllerBase::addImageFor(const QString& idNode, const QString& path, const QByteArray& data)
{
    if(!readWrite())
        return;
    m_stack.push(new mindmap::AddImageToNodeCommand(m_itemModel.get(), m_imgModel.get(), idNode, path, data,
                                                    CampaignFinder::campaignRoot(),
                                                    CampaignFinder::staticContentRoot()));
}

void MindMapControllerBase::removeImageFor(const QString& nodeId)
{
    if(!readWrite())
        return;
    m_stack.push(new mindmap::RemoveImageFromNodeCommand(m_itemModel.get(), m_imgModel.get(), nodeId));
}

void MindMapControllerBase::refresh()
{
    emit contentRectChanged();
}

/*void MindMapControllerBase::openImage(const QString& id, const QUrl& path)
{
    auto map= utils::IOHelper::readPixmapFromURL(path);
    // QPixmap map(path.toLocalFile());

    if(map.isNull())
        return;

    // if gm copy image to campaign

    auto realUrl= path;
    if(localGM() && path.isLocalFile())
    {
        realUrl= QUrl::fromLocalFile(QString("%1/%2_%3").arg(staticDir(), uuid(), path.fileName()));
        utils::IOHelper::savePixmapInto(map, realUrl);
    }
    // else change nothing

    m_imgModel->insertPixmap(id, map, realUrl);
    m_itemModel->update(id, MindItemModel::HasPicture);
}*/

void MindMapControllerBase::removeImage(const QString& id, bool network)
{
    if(!readWrite() && !network)
        return;
    m_imgModel->removePixmap(id);
}

void MindMapControllerBase::removeLink(const QStringList& id, bool network)
{
    if(!readWrite() && !network)
        return;
    std::vector<MindItem*> res;
    std::transform(std::begin(id), std::end(id), std::back_inserter(res),
                   [this](const QString& id) { return m_itemModel->item(id); });

    res.erase(std::remove(std::begin(res), std::end(res), nullptr), std::end(res));

    if(res.empty())
        return;

    auto cmd= new mindmap::RemoveNodeCommand(uuid(), res, m_itemModel.get());
    m_stack.push(cmd);
}

void MindMapControllerBase::removeNode(const QStringList& id, bool network)
{
    if(!readWrite() && !network)
        return;
    std::vector<MindItem*> res;
    std::transform(std::begin(id), std::end(id), std::back_inserter(res),
                   [this](const QString& id) { return m_itemModel->item(id); });

    res.erase(std::remove(std::begin(res), std::end(res), nullptr), std::end(res));

    if(res.empty())
        return;

    auto cmd= new mindmap::RemoveNodeCommand(uuid(), res, m_itemModel.get());
    m_stack.push(cmd);
}

void MindMapControllerBase::addPackage(const QPointF& pos)
{
    if(!readWrite())
        return;
    auto cmd= new mindmap::AddItemCommand(m_itemModel.get(), MindItem::PackageType, {}, pos);
    m_stack.push(cmd);
}

void MindMapControllerBase::updatePackage(const QPointF& pos)
{
    if(!m_package)
        return;

    auto offset= pos - m_package->position();
    m_package->setWidth(offset.x());
    m_package->setHeight(offset.y());
}

void MindMapControllerBase::addLink(const QString& start, const QString& id)
{
    if(start == id || start.isEmpty() || id.isEmpty() || !readWrite())
        return;

    auto cmd= new mindmap::AddLinkCommand(m_itemModel.get(), start, id);
    m_stack.push(cmd);
}

void MindMapControllerBase::addNode(const QList<MindNode*>& nodes, bool network)
{
    if(!readWrite() && !network)
        return;
    QList<mindmap::MindItem*> items;

    std::transform(std::begin(nodes), std::end(nodes), std::back_inserter(items),
                   [](MindNode* node) -> mindmap::MindItem* { return node; });

    m_itemModel->appendItem(items);
}

void MindMapControllerBase::addItem(mindmap::MindItem* node, bool network)
{
    if(network || readWrite())
        m_itemModel->appendItem({node}, network);
}

bool MindMapControllerBase::pasteData(const QMimeData& mimeData)
{
    return false;
}

void MindMapControllerBase::reparenting(MindItem* parent, const QString& id)
{
    auto cmd= new ReparentingNodeCommand(m_itemModel.get(), dynamic_cast<PositionedItem*>(parent), id);
    m_stack.push(cmd);
}
void MindMapControllerBase::removeSelection()
{
    auto nodes= m_selectionController->selectedNodes();
    auto cmd= new mindmap::RemoveNodeCommand(uuid(), nodes, m_itemModel.get());
    m_stack.push(cmd);
    m_selectionController->clearSelection();
}

void MindMapControllerBase::setCurrentPackage(PositionedItem* item)
{
    m_package= item;
}

bool MindMapControllerBase::canUndo() const
{
    return readWrite() && m_stack.canUndo();
}

int MindMapControllerBase::defaultStyleIndex() const
{
    return m_itemModel->defaultStyleIndex();
}

bool MindMapControllerBase::linkLabelVisibility() const
{
    return m_linkLabelVisibility;
}

void MindMapControllerBase::setLinkLabelVisibility(bool newLinkLabelVisibility)
{
    if(m_linkLabelVisibility == newLinkLabelVisibility)
        return;
    m_linkLabelVisibility= newLinkLabelVisibility;
    emit linkLabelVisibilityChanged();
}

bool MindMapControllerBase::hasSelection() const
{
    return m_selectionController->hasSelection();
}

mindmap::MindNode* MindMapControllerBase::nodeFromId(const QString& id) const
{
    return dynamic_cast<mindmap::MindNode*>(m_itemModel->item(id));
}

void MindMapControllerBase::addLinks(const QList<LinkController*>& link, bool network)
{
    qDebug() << "Add link" << link.size() << network;
}

void MindMapControllerBase::addItemIntoPackage(const QString& idNode, const QString& idPack, bool network)
{

    auto node= dynamic_cast<PositionedItem*>(m_itemModel->item(idNode));
    auto pack= dynamic_cast<PackageNode*>(m_itemModel->item(idPack));

    if(!node || !pack || !readWrite())
        return;

    pack->addChild(node, network);
    node->setLocked(true);
}

void MindMapControllerBase::removeItemFromPackage(const QString& idNode, const QString& idPack, bool network)
{
    auto node= dynamic_cast<PositionedItem*>(m_itemModel->item(idNode));
    auto pack= dynamic_cast<PackageNode*>(m_itemModel->item(idPack));

    if(!node || !pack || !readWrite())
        return;

    pack->removeChild(idNode, network);
    node->setLocked(false);
}

QObject* MindMapControllerBase::subItem(const QString& id, mindmap::MindItem::Type type) const
{
    auto item= m_itemModel->item(id);
    if(!item)
        return nullptr;
    return item->type() == type ? item : nullptr;
}

void MindMapControllerBase::setContentRect(const QRectF& rect)
{
    if(m_contentRect == rect || !qFuzzyCompare(m_zoomLevel, 1.))
        return;
    m_contentRect= rect;
    emit contentRectChanged();
}

qreal MindMapControllerBase::zoomLevel() const
{
    return m_zoomLevel;
}

void MindMapControllerBase::setZoomLevel(qreal newZoomLevel)
{
    if(qFuzzyCompare(m_zoomLevel, newZoomLevel))
        return;
    m_zoomLevel= newZoomLevel;
    emit zoomLevelChanged();
}

bool MindMapControllerBase::hasNetwork() const
{
    return m_hasNetwork;
}

MindMapControllerBase::MindMapTool MindMapControllerBase::tool() const
{
    return m_tool;
}

void MindMapControllerBase::setTool(MindMapControllerBase::MindMapTool newTool)
{
    if(m_tool == newTool)
        return;
    m_tool= newTool;
    emit toolChanged();
}

bool MindMapControllerBase::isPackage() const
{
    return m_tool == MindMapControllerBase::Package;
}

bool MindMapControllerBase::isArrow() const
{
    return m_tool == MindMapControllerBase::Arrow;
}

bool MindMapControllerBase::readWrite() const
{
    return true;
}

bool MindMapControllerBase::hideEmptyLabel() const
{
    return m_hideEmptyLabel;
}

void MindMapControllerBase::setHideEmptyLabel(bool newHideEmptyLabel)
{
    if(m_hideEmptyLabel == newHideEmptyLabel)
        return;
    m_hideEmptyLabel= newHideEmptyLabel;
    emit hideEmptyLabelChanged();
}

} // namespace mindmap
