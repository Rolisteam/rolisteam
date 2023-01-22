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
#include "controller/view_controller/mindmapcontroller.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <QThread>
#include <QUrl>
#include <random>

#include "mindmap/controller/spacingcontroller.h"
#include "mindmap/model/minditemmodel.h"
#include "updater/media/mindmapupdater.h"

QPointer<RemotePlayerModel> MindMapController::m_remotePlayerModel;
QPointer<MindMapUpdater> MindMapController::m_updater;

MindMapController::MindMapController(const QString& id, QObject* parent) : MindMapControllerBase(true, id, parent)
{
    m_selectionController->setUndoStack(&m_stack);

    MediaControllerBase::connect(m_itemModel.get(), &mindmap::MindItemModel::rowsInserted,
                                 static_cast<MediaControllerBase*>(this),
                                 [this]
                                 {
                                     // qDebug() << "Row inserted";
                                     setModified();
                                 });
    MediaControllerBase::connect(m_itemModel.get(), &mindmap::MindItemModel::dataChanged,
                                 static_cast<MediaControllerBase*>(this),
                                 [this](const QModelIndex&, const QModelIndex&, QVector<int> roles)
                                 {
                                     if(!roles.contains(mindmap::MindItemModel::LinkPositionFromSpacing))
                                     {
                                         // qDebug() << "dataChanged" << roles;
                                         setModified();
                                     }
                                 });
    MediaControllerBase::connect(m_itemModel.get(), &mindmap::MindItemModel::rowsRemoved,
                                 static_cast<MediaControllerBase*>(this),
                                 [this]
                                 {
                                     // qDebug() << "Row rowsRemoved";
                                     setModified();
                                 });

    MediaControllerBase::connect(this, &MindMapController::defaultStyleIndexChanged,
                                 static_cast<MediaControllerBase*>(this),
                                 [this]
                                 {
                                     // qDebug() << "Default style changed";
                                     setModified();
                                 });

    MediaControllerBase::connect(this, &MindMapController::nameChanged, static_cast<MediaControllerBase*>(this),
                                 [this]
                                 {
                                     // qDebug() << "name Changed";
                                     setModified();
                                 });

    /*MediaControllerBase::connect(this, &MindMapController::titleChanged, static_cast<MediaControllerBase*>(this),
                                 [this] {
                                     //qDebug() << "title changed";
                                     setModified();
                                 });*/
    MediaControllerBase::connect(this, &MindMapController::urlChanged, static_cast<MediaControllerBase*>(this),
                                 [this]
                                 {
                                     qDebug() << "url changed";
                                     setModified();
                                 });

    clearData();
}

MindMapController::~MindMapController()
{
    if(m_spacing->isRunning())
    {
        m_spacingController->setRunning(false);
        m_spacing->quit();
        m_spacing->wait();
        delete m_spacing.release();
    }

    auto model= m_itemModel.release();
    auto spacingCtrl= m_spacingController.release();

    delete spacingCtrl;
    delete model;
}

RemotePlayerModel* MindMapController::remotePlayerModel() const
{
    return m_remotePlayerModel;
}

PlayerModel* MindMapController::playerModel() const
{
    return m_remotePlayerModel->sourceModel();
}

bool MindMapController::readWrite() const
{
    bool hasDedicatedPermission= false;
    if(m_permissions.contains(m_localId))
        hasDedicatedPermission= (m_permissions[m_localId] == Core::SharingPermission::ReadWrite);

    return (localIsOwner() || m_sharingToAll == Core::SharingPermission::ReadWrite || hasDedicatedPermission);
}

void MindMapController::setSharingToAll(int b)
{
    auto realPerm= static_cast<Core::SharingPermission>(b);
    if(m_sharingToAll == realPerm)
        return;
    auto old= m_sharingToAll;
    m_sharingToAll= realPerm;
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

void MindMapController::setRemotePlayerModel(RemotePlayerModel* model)
{
    m_remotePlayerModel= model;
}

void MindMapController::setMindMapUpdater(MindMapUpdater* updater)
{
    m_updater= updater;
}

void MindMapController::generateTree()
{
    return;
    /* QString res;
     {
         QTextStream output(&res);

         output << QString("digraph G {") << "\n";

         auto const& nodes= m_itemModel->items(mindmap::MindItem::NodeType);
         output << QString("    subgraph cluster_0 {") << "\n";
         for(auto const& i : nodes)
         {
             auto node= dynamic_cast<mindmap::MindNode*>(i);
             if(!node)
                 continue;
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
         auto const& links= m_itemModel->items(mindmap::MindItem::LinkType);
         output << QString("    subgraph cluster_1 {") << "\n";
         for(auto const& i : links)
         {
             auto link= dynamic_cast<mindmap::LinkController*>(i);
             auto end= link->end();
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

     // qDebug() << res;*/
}
