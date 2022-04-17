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
#include "mindmap/command/addnodecommand.h"

#include "mindmap/model/boxmodel.h"
#include "mindmap/model/linkmodel.h"

#include <QDebug>

namespace mindmap
{

AddNodeCommand::AddNodeCommand(const QString& idmap, BoxModel* nodeModel, LinkModel* linkModel, const QString& idParent)
    : m_idmap(idmap), m_nodeModel(nodeModel), m_linkModel(linkModel), m_idParent(idParent)
{
}

void AddNodeCommand::setData(const QString& text, const QString& imgUrl)
{
    m_text= text;
    m_imgUrl= imgUrl;
}

void AddNodeCommand::undo()
{
    auto nodeId= m_mindNode->id();
    QString linkId;
    m_nodeModel->removeBox({nodeId});
    if(m_link)
    {
        linkId= m_link->id();
        m_linkModel->removeLink({linkId});
    }

    emit removeNodes(m_idmap, {nodeId}, linkId.isEmpty() ? QStringList{} : QStringList{linkId});
    /*if(m_updater)
    {
        m_updater->sendOffRemoveMessage(m_idmap, {nodeId}, linkId.isEmpty() ? QStringList{} : QStringList{linkId});
    }*/
}

void AddNodeCommand::redo()
{
    if(m_mindNode.isNull())
    {
        auto pair= m_nodeModel->addBox(m_idParent);

        m_mindNode= pair.first;
        if(!m_imgUrl.isNull())
            m_mindNode->setImageUri(m_imgUrl);
        if(!m_text.isNull())
            m_mindNode->setText(m_text);
        m_link= pair.second;
    }
    else
    {
        m_nodeModel->appendNode({m_mindNode.data()});
        if(m_link)
            m_linkModel->append({m_link});
    }
    emit addNodes(m_idmap, {m_mindNode.data()}, m_link ? QList<mindmap::Link*>{m_link} : QList<mindmap::Link*>{});
    /*if(m_updater)
         m_updater->sendOffAddingMessage(m_idmap, {m_mindNode.data()},
                                         m_link ? QList<mindmap::Link*>{m_link} : QList<mindmap::Link*>{});*/
}
} // namespace mindmap
