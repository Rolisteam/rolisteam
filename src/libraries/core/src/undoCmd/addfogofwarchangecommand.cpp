/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
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
#include "undoCmd/addfogofwarchangecommand.h"

#include "controller/item_controllers/sightcontroller.h"
#include <QObject>

AddFogOfWarChangeCommand::AddFogOfWarChangeCommand(vmap::SightController* ctrl, const QPolygonF& gone, bool mask)
    : QUndoCommand(), m_ctrl(ctrl), m_poly(gone), m_mask(mask)
{
    setText(mask ? QObject::tr("Conceal vectorial map") : QObject::tr("Unveil vectorial map"));
    QObject::connect(m_ctrl, &vmap::SightController::destroyed, m_ctrl, [this](){
        setObsolete(true);
    });
}

void AddFogOfWarChangeCommand::redo()
{
    if(m_ctrl)
        m_ctrl->addPolygon(m_poly, m_mask);
}

void AddFogOfWarChangeCommand::undo()
{
    if(m_ctrl)
        m_ctrl->addPolygon(m_poly, !m_mask);
}
