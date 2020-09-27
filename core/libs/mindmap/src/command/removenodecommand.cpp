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
#include "data/mindnode.h"
#include "model/boxmodel.h"
#include "model/linkmodel.h"
#include <algorithm>

RemoveNodeCommand::RemoveNodeCommand(const std::vector<MindNode*>& selection, BoxModel* nodeModel, LinkModel* linkModel)
    : m_nodeModel(nodeModel), m_linkModel(linkModel)
{
    std::transform(selection.begin(), selection.end(), std::back_inserter(m_selection),
                   [](MindNode* node) -> QPointer<MindNode> { return QPointer<MindNode>(node); });

    std::for_each(selection.begin(), selection.end(), [this](MindNode* node) {
        auto sublinks= node->getSubLinks();
        std::copy(sublinks.begin(), sublinks.end(), std::back_inserter(m_links));
    });
}

void RemoveNodeCommand::undo()
{
    std::for_each(m_selection.begin(), m_selection.end(), [this](MindNode* node) { m_nodeModel->appendNode(node); });
    std::for_each(m_links.begin(), m_links.end(), [this](Link* link) { m_linkModel->append(link); });
}

void RemoveNodeCommand::redo()
{
    std::for_each(m_selection.begin(), m_selection.end(), [this](MindNode* node) { m_nodeModel->removeBox(node); });
    std::for_each(m_links.begin(), m_links.end(), [this](Link* link) { m_linkModel->removeLink(link); });
}
