/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#ifndef ROLLINITCOMMAND_H
#define ROLLINITCOMMAND_H

#include <QPointer>
#include <QUndoCommand>

namespace vmap
{
class CharacterItemController;
} // namespace vmap
class DiceRoller;
struct InitInfo
{
    bool m_hasInit;
    int m_score;
};

class RollInitCommand : public QUndoCommand
{
public:
    RollInitCommand(const QList<QPointer<vmap::CharacterItemController>>& data, DiceRoller* dice);

    void undo() override;
    void redo() override;

private:
    QList<QPointer<vmap::CharacterItemController>> m_data;
    QList<InitInfo> m_formerValues;
    QPointer<DiceRoller> m_diceparser;
};

class CleanUpRollCommand : public QUndoCommand
{
public:
    CleanUpRollCommand(const QList<QPointer<vmap::CharacterItemController>>& data);

    void undo() override;
    void redo() override;

private:
    QList<QPointer<vmap::CharacterItemController>> m_data;
    QList<InitInfo> m_formerValues;
};

#endif // ROLLINITCOMMAND_H
