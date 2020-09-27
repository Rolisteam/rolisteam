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
#ifndef REPARENTINGNODECOMMAND_H
#define REPARENTINGNODECOMMAND_H

#include <QPointer>
#include <QUndoCommand>
class MindNode;
class BoxModel;
class LinkModel;
class Link;

class ReparentingNodeCommand : public QUndoCommand
{
public:
    ReparentingNodeCommand(BoxModel* nodeModel, LinkModel* linkModel, MindNode* newParent, const QString& id);
    void undo() override;
    void redo() override;

private:
    QPointer<MindNode> m_mindNode;
    QPointer<MindNode> m_oldParent;
    QPointer<Link> m_oldLink;
    QPointer<Link> m_newLink;

    BoxModel* m_nodeModel= nullptr;
    LinkModel* m_linkModel= nullptr;
    QPointer<MindNode> m_newParent;
};

#endif // REPARENTINGNODECOMMAND_H
