/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "addlinkcommand.h"

#include "data/linkcontroller.h"
#include "model/minditemmodel.h"

AddLinkCommand::AddLinkCommand(MindItemModel* model, const QString& start, const QString& end)
    : m_model(model), m_startId(start), m_endId(end)
{
}

void AddLinkCommand::undo()
{
    m_model->removeItem(m_model->item(m_linkId));
}

void AddLinkCommand::redo()
{
    auto start= dynamic_cast<PositionedItem*>(m_model->item(m_startId));
    auto end= dynamic_cast<PositionedItem*>(m_model->item(m_endId));

    if(!start || !end)
        return;

    auto link= new LinkController();
    link->setConstraint(false);
    link->setColor(false);
    link->setLineStyle(Qt::DashLine);

    link->setStart(start);
    link->setEnd(end);

    m_model->appendItem(link);
    m_linkId= link->id();
}
