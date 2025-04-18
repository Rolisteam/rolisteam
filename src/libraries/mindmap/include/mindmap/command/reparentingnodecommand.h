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

#include "mindmap/mindmap_global.h"
#include <QPointer>
#include <QUndoCommand>

namespace mindmap
{
class MindItem;
class MindItemModel;
class LinkController;
class PositionedItem;

class MINDMAP_EXPORT ReparentingNodeCommand : public QUndoCommand
{
public:
    ReparentingNodeCommand(MindItemModel* nodeModel, PositionedItem* newParent, const QString& id);
    void undo() override;
    void redo() override;

private:
    QPointer<PositionedItem> m_mindNode;
    QPointer<PositionedItem> m_oldParent;
    QPointer<LinkController> m_oldLink;
    QPointer<LinkController> m_newLink;

    QPointer<MindItemModel> m_nodeModel;
    QPointer<PositionedItem> m_newParent;
};
} // namespace mindmap
#endif // REPARENTINGNODECOMMAND_H
