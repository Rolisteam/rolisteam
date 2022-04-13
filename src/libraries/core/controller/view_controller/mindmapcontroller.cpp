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
#include "mindmapcontroller.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <QThread>
#include <QUrl>
#include <random>

#include "mindmap/src/command/addimagetonodecommand.h"
#include "mindmap/src/command/addnodecommand.h"
#include "mindmap/src/command/removeimagefromnodecommand.h"
#include "mindmap/src/command/removenodecommand.h"
#include "mindmap/src/command/reparentingnodecommand.h"
#include "mindmap/src/controller/spacingcontroller.h"
#include "mindmap/src/data/link.h"
#include "mindmap/src/data/mindnode.h"
#include "mindmap/src/data/nodestyle.h"
#include "mindmap/src/model/boxmodel.h"
#include "mindmap/src/model/linkmodel.h"
#include "mindmap/src/model/nodestylemodel.h"
#include "mindmap/src/qmlItems/linkitem.h"
#include "mindmap/src/qmlItems/nodeitem.h"
#include "mindmap/src/worker/fileserializer.h"
#include "model/playermodel.h"
#include "updater/media/mindmapupdater.h"
#include "worker/iohelper.h"

void registerQmlType()
{
    static bool b= false;

    if(b)
        return;

    qmlRegisterUncreatableType<MindMapController>("RMindMap", 1, 0, "MindMapController",
                                                  "MindMapController can't be created in qml");
    qmlRegisterType<mindmap::SelectionController>("RMindMap", 1, 0, "SelectionController");
    qmlRegisterUncreatableType<RemotePlayerModel>("RMindMap", 1, 0, "RemotePlayerModel", "property values");
    qmlRegisterType<mindmap::LinkItem>("RMindMap", 1, 0, "MindLink");
    qmlRegisterType<mindmap::NodeStyle>("RMindMap", 1, 0, "NodeStyle");
    b= true;
}

QPointer<RemotePlayerModel> MindMapController::m_remotePlayerModel;
QPointer<MindMapUpdater> MindMapController::m_updater;

MindMapController::MindMapController(const QString& id, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::MINDMAP, parent)
    , m_selectionController(new mindmap::SelectionController())
    , m_linkModel(new mindmap::LinkModel())
    , m_nodeModel(new mindmap::BoxModel())
    , m_styleModel(new mindmap::NodeStyleModel())
    , m_imageModel(new ImageModel())
{
    registerQmlType();
    m_nodeModel->setLinkModel(m_linkModel.get());
    m_selectionController->setUndoStack(&m_stack);

    connect(m_selectionController.get(), &mindmap::SelectionController::hasSelectionChanged, this,
            &MindMapController::hasSelectionChanged);

    connect(&m_stack, &QUndoStack::canRedoChanged, this, &MindMapController::canRedoChanged);
    connect(&m_stack, &QUndoStack::canUndoChanged, this, &MindMapController::canUndoChanged);
    connect(m_nodeModel.get(), &mindmap::BoxModel::defaultStyleIndexChanged, this,
            &MindMapController::defaultStyleIndexChanged);
    connect(m_nodeModel.get(), &mindmap::BoxModel::nodeHeightChanged, this, &MindMapController::contentRectChanged);
    connect(m_nodeModel.get(), &mindmap::BoxModel::nodeWidthChanged, this, &MindMapController::contentRectChanged);

    m_spacing= new QThread();
    m_spacingController.reset(new mindmap::SpacingController(m_nodeModel->nodes(), m_linkModel.get()));
    m_spacingController->moveToThread(m_spacing);
    connect(m_spacing, &QThread::started, m_spacingController.get(), &mindmap::SpacingController::computeInLoop);
    connect(m_spacingController.get(), &mindmap::SpacingController::runningChanged, this,
            &MindMapController::spacingChanged);
    connect(m_spacingController.get(), &mindmap::SpacingController::runningChanged, this, [this]() {
        if(m_spacingController->running())
            m_spacing->start();
    });
    connect(m_spacingController.get(), &mindmap::SpacingController::finished, m_spacing, &QThread::quit);
    m_spacing->start();

    connect(m_nodeModel.get(), &mindmap::BoxModel::nodeAdded, this, &MindMapController::generateTree);
    connect(m_linkModel.get(), &mindmap::LinkModel::linkAdded, this, &MindMapController::generateTree);

    clearData();
}

MindMapController::~MindMapController()
{
    if(m_spacing->isRunning())
    {
        m_spacingController->setRunning(false);
        m_spacing->quit();
        m_spacing->wait();
        delete m_spacing;
    }

    auto model= m_nodeModel.release();
    auto spacingCtrl= m_spacingController.release();

    delete spacingCtrl;
    delete model;
}

QAbstractItemModel* MindMapController::nodeModel() const
{
    return m_nodeModel.get();
}

QAbstractItemModel* MindMapController::linkModel() const
{
    return m_linkModel.get();
}

QAbstractItemModel* MindMapController::styleModel() const
{
    return m_styleModel.get();
}

RemotePlayerModel* MindMapController::remotePlayerModel() const
{
    return m_remotePlayerModel;
}

PlayerModel* MindMapController::playerModel() const
{
    return m_remotePlayerModel->sourceModel();
}

ImageModel* MindMapController::imageModel() const
{
    return m_imageModel.get();
}

const QString& MindMapController::filename() const
{
    return m_filename;
}

QObject* MindMapController::subItem(const QString& id, SubItemType type)
{
    QObject* obj= nullptr;
    switch(type)
    {
    case Node:
        obj= m_nodeModel->nodeFromId(id);
        break;
    case Link:
        obj= m_linkModel->linkFromId(id);
        break;
    case Package:
        /// TODO: must be implemented
        break;
    }

    return obj;
}

const QString& MindMapController::errorMsg() const
{
    return m_errorMsg;
}

QRectF MindMapController::contentRect() const
{
    return {0, 0, m_nodeModel->nodeWidth(), m_nodeModel->nodeHeight()};
}

bool MindMapController::readWrite() const
{
    bool hasDedicatedPermission= false;
    if(m_permissions.contains(m_localId))
        hasDedicatedPermission= (m_permissions[m_localId] == Core::SharingPermission::ReadWrite);

    return (localIsOwner() || m_sharingToAll == Core::SharingPermission::ReadWrite || hasDedicatedPermission);
}

/*void MindMapController::setReadWrite(bool b)
{
    if(b == m_readWrite)
        return;
    m_readWrite= b;
    emit readWriteChanged();
}*/

void MindMapController::clearData()
{
    m_linkModel->clear();
    m_nodeModel->clear();
}

void MindMapController::saveFile()
{
    if(!mindmap::FileSerializer::writeFile(m_nodeModel.get(), m_linkModel.get(), m_filename))
        setErrorMsg(tr("Error: File can't be loaded: %1").arg(m_filename));
}

void MindMapController::setErrorMsg(const QString& msg)
{
    if(m_errorMsg == msg)
        return;
    m_errorMsg= msg;
    emit errorMsgChanged();
}

void MindMapController::loadFile()
{
    clearData();
    if(!mindmap::FileSerializer::readFile(m_nodeModel.get(), m_linkModel.get(), m_filename))
        setErrorMsg(tr("Error: File can't be loaded: %1").arg(m_filename));
}

void MindMapController::importFile(const QString& path)
{
    clearData();
    if(!mindmap::FileSerializer::readTextFile(m_nodeModel.get(), m_linkModel.get(), path))
        setErrorMsg(tr("File can't be loaded: %1").arg(m_filename));
}

void MindMapController::setDefaultStyleIndex(int indx)
{
    m_nodeModel->setDefaultStyleIndex(indx);
}

void MindMapController::setSharingToAll(int b)
{
    auto realPerm= static_cast<Core::SharingPermission>(b);
    if(m_sharingToAll == realPerm)
        return;
    auto old= m_sharingToAll;
    m_sharingToAll= realPerm;
    // qDebug() << "mindmapcontroller" << static_cast<int>(m_sharingToAll) << this << static_cast<int>(old);
    emit sharingToAllChanged(m_sharingToAll, old);
}

Core::SharingPermission MindMapController::sharingToAll() const
{
    return m_sharingToAll;
}

void MindMapController::setPermissionForUser(const QString& userId, int perm)
{
    auto realPerm= static_cast<Core::SharingPermission>(perm);
    switch(realPerm)
    {
    case Core::SharingPermission::None:
        m_permissions.remove(userId);
        emit closeMindMapTo(userId);
        break;
    case Core::SharingPermission::ReadOnly:
    {
        if(m_permissions.contains(userId))
        {
            auto p= m_permissions.find(userId);
            Q_ASSERT(p.value() == Core::SharingPermission::ReadWrite);
            emit permissionChangedForUser(userId, realPerm);
        }
        else
        {
            m_permissions.insert(userId, realPerm);
            emit openMindMapTo(userId);
        }
    }
    break;
    case Core::SharingPermission::ReadWrite:
        m_permissions.insert(userId, realPerm);
        emit permissionChangedForUser(userId, realPerm);
        break;
    }
}

void MindMapController::setFilename(const QString& path)
{
    if(path == m_filename)
        return;
    m_filename= QUrl(path).toLocalFile();
    if(!m_filename.endsWith(".rmap"))
        m_filename+= QStringLiteral(".rmap");
    emit filenameChanged();
}

bool MindMapController::pasteData(const QMimeData& mimeData)
{
    if(!mimeData.hasImage())
        return false;

    auto pix= qvariant_cast<QPixmap>(mimeData.imageData());
    auto id= m_selectionController->lastSelected();
    if(pix.isNull() || id.isEmpty())
        return false;

    auto cmd= new mindmap::AddImageToNodeCommand(m_nodeModel.get(), m_imageModel.get(), id, pix);
    m_stack.push(cmd);
    return true;
}

void MindMapController::openImage(const QString& id, const QUrl& path)
{
    auto cmd= new mindmap::AddImageToNodeCommand(m_nodeModel.get(), m_imageModel.get(), id, path);
    m_stack.push(cmd);
}

void MindMapController::removeImage(const QString& id)
{
    auto cmd= new mindmap::RemoveImageFromNodeCommand(m_nodeModel.get(), m_imageModel.get(), id);
    m_stack.push(cmd);
}

void MindMapController::removeLink(const QStringList& id)
{
    m_linkModel->removeLink(id, true);
}

void MindMapController::removeNode(const QStringList& id)
{
    m_nodeModel->removeBox(id, true);
}

mindmap::NodeStyle* MindMapController::getStyle(int index) const
{
    return m_styleModel->getStyle(index);
}

void MindMapController::setSpacing(bool status)
{
    m_spacingController->setRunning(status);
}

void MindMapController::redo()
{
    m_stack.redo();
    generateTree();
}

void MindMapController::undo()
{
    m_stack.undo();
    generateTree();
}

bool MindMapController::spacing() const
{
    return m_spacingController->running();
}

mindmap::SelectionController* MindMapController::selectionController() const
{
    return m_selectionController.get();
}

bool MindMapController::canRedo() const
{
    return m_stack.canRedo();
}

void MindMapController::addBox(const QString& idparent)
{
    auto cmd= new mindmap::AddNodeCommand(uuid(), m_updater, m_nodeModel.get(), m_linkModel.get(), idparent);
    m_stack.push(cmd);
}

void MindMapController::addCharacterBox(const QString& idparent, const QString& name, const QString& url, const QColor&)
{
    auto cmd= new mindmap::AddNodeCommand(uuid(), m_updater, m_nodeModel.get(), m_linkModel.get(), idparent);
    cmd->setData(name, url);
    m_stack.push(cmd);
}

void MindMapController::reparenting(mindmap::MindNode* parent, const QString& id)
{
    auto cmd= new mindmap::ReparentingNodeCommand(m_nodeModel.get(), m_linkModel.get(), parent, id);
    m_stack.push(cmd);
}

void MindMapController::removeSelection()
{
    auto nodes= m_selectionController->selectedNodes();
    auto cmd= new mindmap::RemoveNodeCommand(uuid(), m_updater, nodes, m_nodeModel.get(), m_linkModel.get());
    m_stack.push(cmd);
}

void MindMapController::addNode(QList<mindmap::MindNode*> nodes, bool network)
{
    m_nodeModel->appendNode(nodes, network);
    generateTree();
}

mindmap::Link* MindMapController::linkFromId(const QString& id) const
{
    return m_linkModel->linkFromId(id);
}
mindmap::MindNode* MindMapController::nodeFromId(const QString& id) const
{
    return m_nodeModel->nodeFromId(id);
}

void MindMapController::createLink(const QString& id, const QString& id2)
{
    m_linkModel->addLink(nodeFromId(id), nodeFromId(id2));
    generateTree();
}

void MindMapController::addLink(const QList<mindmap::Link*>& link, bool network)
{
    m_linkModel->append(link, network);
    generateTree();
}

bool MindMapController::canUndo() const
{
    return m_stack.canUndo();
}

bool MindMapController::hasSelection() const
{
    return m_selectionController->hasSelection();
}

int MindMapController::defaultStyleIndex() const
{
    return m_nodeModel->defaultStyleIndex();
}

void MindMapController::setRemotePlayerModel(RemotePlayerModel* model)
{
    m_remotePlayerModel= model;
}

void MindMapController::setMindMapUpdater(MindMapUpdater* updater)
{
    m_updater= updater;
}

mindmap::SpacingController* MindMapController::spacingController() const
{
    return m_spacingController.get();
}

void MindMapController::generateTree()
{
    return;
    QString res;
    {
        QTextStream output(&res);

        output << QString("digraph G {") << "\n";

        auto nodes= m_nodeModel->nodes();
        output << QString("    subgraph cluster_0 {") << "\n";
        for(auto node : nodes)
        {
            output << "0_" << node->toString(true) << ";\n";
            auto links= node->subLinks();
            output << "# sublinks of :" << node->id();
            for(auto link : links)
            {
                output << "0_" << link->toString(true) << ";\n";
                output << "0_" << link->toString(false) << "->"
                       << "0_" << node->toString(false) << ";\n";
            }
        }
        output << "    }\n";
        output << "# from link model point of view";
        auto links= m_linkModel->getDataSet();
        output << QString("    subgraph cluster_1 {") << "\n";
        for(auto link : links)
        {
            // output << link->toString(true) << ";\n";
            auto end= link->endNode();
            auto start= link->start();

            if(!end)
                continue;

            output << end->toString(true) << ";\n";

            if(!start)
                continue;

            output << start->toString(true) << ";\n";
            output << start->toString(false) << "->" << end->toString(false)
                   << QString("[Label=\"Link-id:%1 text:%2\"]").arg(link->id(), link->text());
        }
        output << "    }\n";
        output.flush();
    }

    // qDebug() << res;
}
