/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#include "commands/changestackordervmapcommand.h"

ChangeStackOrderVMapCommand::ChangeStackOrderVMapCommand(VectorialMapController* vmapCtrl,
                                                         const QList<ItemToControllerInfo>& first,
                                                         const QList<ItemToControllerInfo>& second)
    : m_vmapCtrl(vmapCtrl), m_first(first), m_second(second)
{
    setText(tr("Change stack order of %1 item(s)").arg(first.size()));
    auto getId= [](const ItemToControllerInfo& info) -> QString
    {
        if(!info.ctrl)
            return {};
        return info.ctrl->uuid();
    };

    std::transform(std::begin(m_first), std::end(m_first), std::back_inserter(m_firstIds), getId);
    std::transform(std::begin(m_second), std::end(m_second), std::back_inserter(m_secondIds), getId);
}

void ChangeStackOrderVMapCommand::undo()
{

    for(auto info : m_second)
    {
        for(auto data : m_first)
        {
            info.item->stackBefore(data.item);
        }
    }
    m_vmapCtrl->stackBefore(m_firstIds, m_secondIds);
}

void ChangeStackOrderVMapCommand::redo()
{
    for(auto info : m_first)
    {
        for(auto data : m_second)
        {
            info.item->stackBefore(data.item);
        }
    }
    m_vmapCtrl->stackBefore(m_secondIds, m_firstIds);
}
