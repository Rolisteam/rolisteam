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
#include "mindmap/command/removenodecommand.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "mindmap/model/minditemmodel.h"
#include <algorithm>

namespace mindmap
{

RemoveNodeCommand::RemoveNodeCommand(const QString& idmap, const std::vector<MindItem*>& selection,
                                     MindItemModel* nodeModel)
    : m_nodeModel(nodeModel), m_idmap(idmap)
{
    std::transform(selection.begin(), selection.end(), std::back_inserter(m_selection),
                   [](MindItem* node) -> QPointer<MindItem> { return QPointer<MindItem>(node); });

    std::for_each(selection.begin(), selection.end(), [this](MindItem* node) {
        auto sublinks= m_nodeModel->sublink(node->id());
        std::copy(sublinks.begin(), sublinks.end(), std::back_inserter(m_links));
    });
}

void RemoveNodeCommand::undo()
{
    std::for_each(m_selection.begin(), m_selection.end(), [this](MindItem* node) { m_nodeModel->appendItem({node}); });
    std::for_each(m_links.begin(), m_links.end(), [this](LinkController* link) { m_nodeModel->appendItem({link}); });
    /*if(m_updater)
    {
        m_updater->sendOffAddingMessage(m_idmap, nodes, links);
    }*/
}

void RemoveNodeCommand::redo()
{
    std::for_each(m_selection.begin(), m_selection.end(), [this](MindItem* node) { m_nodeModel->removeItem(node); });
    std::for_each(m_links.begin(), m_links.end(), [this](LinkController* link) { m_nodeModel->removeItem(link); });
    /*if(m_updater)
    {
        m_updater->sendOffRemoveMessage(m_idmap, allBoxToRemove, allLinkToRemove);
    }*/
}
} // namespace mindmap
