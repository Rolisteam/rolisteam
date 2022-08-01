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
#ifndef ADDITEMCOMMAND_H
#define ADDITEMCOMMAND_H

#include <QCoreApplication>>
#include <QPointer>
#include <QUndoCommand>

#include "mindmap/data/minditem.h"

namespace mindmap
{
class LinkController;
class MindItemModel;
class MindMapControllerBase;
class AddLinkCommand : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddLinkCommand)

public:
    AddLinkCommand(MindItemModel* nodeModel, const QString& idStart, const QString& idEnd);

    void undo() override;
    void redo() override;

private:
    QPointer<LinkController> m_link;
    QPointer<MindItemModel> m_nodeModel;
    QString m_idStart;
    QString m_idEnd;
};

class AddItemCommand : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(AddItemCommand)

public:
    AddItemCommand(MindItemModel* nodeModel, mindmap::MindItem::Type type, const QString& idParent= {},
                   QPointF pos= {});
    void undo() override;
    void redo() override;

private:
    QPointer<MindItem> m_mindItem;
    QPointer<LinkController> m_link;
    QPointer<MindItemModel> m_nodeModel;
    QString m_idParent;
    MindItem::Type m_type;
    QPointF m_pos;
};
} // namespace mindmap
#endif // ADDITEMCOMMAND_H
