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
#include "reparentingnodecommand.h"

#include "model/boxmodel.h"
#include "model/linkmodel.h"

ReparentingNodeCommand::ReparentingNodeCommand(BoxModel* nodeModel, LinkModel* linkModel, MindNode* newParent,
                                               const QString& id)
    : m_nodeModel(nodeModel), m_linkModel(linkModel), m_newParent(newParent)
{
    m_mindNode= m_nodeModel->node(id);
    if(m_mindNode)
        m_oldParent= m_mindNode->parentNode();

    if(m_oldParent)
    {
        auto links= m_oldParent->getSubLinks();
        auto idxLink
            = std::find_if(links.begin(), links.end(), [this](Link* link) { return link->end() == m_mindNode.data(); });
        if(idxLink != links.end())
            m_oldLink= (*idxLink);
    }
}

void ReparentingNodeCommand::undo()
{
    if(m_mindNode.isNull() || m_oldLink.isNull() || m_newLink.isNull())
        return;

    m_linkModel->removeLink(m_newLink);
    m_linkModel->append(m_oldLink);
}

void ReparentingNodeCommand::redo()
{
    if(m_mindNode.isNull() || m_oldLink.isNull())
        return;

    m_linkModel->removeLink(m_oldLink);
    if(m_newLink.isNull())
        m_newLink= m_linkModel->addLink(m_newParent, m_mindNode);
    else
        m_linkModel->append(m_newLink);
}
