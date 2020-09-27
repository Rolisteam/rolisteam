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
#ifndef DRAGNODECOMMAND_H
#define DRAGNODECOMMAND_H

#include <QPointF>
#include <QPointer>
#include <QUndoCommand>

class MindNode;

class DragNodeCommand : public QUndoCommand
{
public:
    DragNodeCommand(const QPointF& motion, const std::vector<QPointer<MindNode>>& selection);
    void undo() override;
    void redo() override;
    int id() const override;

    bool mergeWith(const QUndoCommand* other) override;

    const QPointF& getMotion() const;
    const std::vector<QPointer<MindNode>> getSelection() const;

private:
    QPointF m_motion;
    std::vector<QPointer<MindNode>> m_mindNodes;
};

#endif // DRAGNODECOMMAND_H
