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
#include "removenodecommand.h"
#include "core/updater/media/mindmapupdater.h"
#include "data/link.h"
#include "data/mindnode.h"
#include "model/boxmodel.h"
#include "model/linkmodel.h"
#include <algorithm>

namespace mindmap
{
void readAllNodes(MindNode* node, QList<QPointer<MindNode>>& selection)
{
    auto sublinks= node->subLinks();
    for(const auto& link : sublinks)
    {
        auto node1= link->start();
        auto node2= link->endNode();

        if(node1 != node && !selection.contains(node1))
        {
            selection.append(node1);
            readAllNodes(node1, selection);
        }

        if(node2 != node && !selection.contains(node2))
        {
            selection.append(node2);
            readAllNodes(node2, selection);
        }
    }
}

RemoveNodeCommand::RemoveNodeCommand(const QString& idmap, MindMapUpdater* updater,
                                     const std::vector<MindNode*>& selection, BoxModel* nodeModel, LinkModel* linkModel)
    : m_updater(updater), m_nodeModel(nodeModel), m_linkModel(linkModel), m_idmap(idmap)
{
    std::transform(selection.begin(), selection.end(), std::back_inserter(m_selection),
                   [](MindNode* node) -> QPointer<MindNode> { return QPointer<MindNode>(node); });

    std::for_each(selection.begin(), selection.end(), [this](MindNode* node) { readAllNodes(node, m_selection); });
    std::for_each(std::begin(m_selection), std::end(m_selection), [this](const MindNode* node) {
        auto links= m_linkModel->allLinkWithNodeId(node->id());
        QList<QPointer<Link>> linkData;
        std::transform(links.begin(), links.end(), std::back_inserter(linkData),
                       [](Link* link) -> QPointer<Link> { return QPointer<Link>(link); });

        std::for_each(std::begin(linkData), std::end(linkData), [this](Link* link) {
            if(!m_links.contains(link))
                m_links.append(link);
        });
    });
}

void RemoveNodeCommand::undo()
{
    QList<mindmap::MindNode*> nodes;
    std::transform(m_selection.begin(), m_selection.end(), std::back_inserter(nodes),
                   [](const QPointer<mindmap::MindNode>& node) {
                       mindmap::MindNode* res= nullptr;
                       if(!node.isNull())
                           res= node.data();
                       return res;
                   });
    m_nodeModel->appendNode(nodes);

    QList<mindmap::Link*> links;
    std::transform(m_links.begin(), m_links.end(), std::back_inserter(links), [](const QPointer<Link>& link) {
        mindmap::Link* res= nullptr;
        if(!link.isNull())
            res= link.data();
        return res;
    });
    m_linkModel->append(links);

    if(m_updater)
    {
        m_updater->sendOffAddingMessage(m_idmap, nodes, links);
    }
}

void RemoveNodeCommand::redo()
{
    QStringList allBoxToRemove;
    std::transform(m_selection.begin(), m_selection.end(), std::back_inserter(allBoxToRemove),
                   [](MindNode* node) { return node->id(); });
    m_nodeModel->removeBox(allBoxToRemove);

    QStringList allLinkToRemove;
    std::transform(m_links.begin(), m_links.end(), std::back_inserter(allLinkToRemove),
                   [](Link* link) { return link->id(); });
    m_linkModel->removeLink(allLinkToRemove);

    if(m_updater)
    {
        m_updater->sendOffRemoveMessage(m_idmap, allBoxToRemove, allLinkToRemove);
    }
}
} // namespace mindmap
