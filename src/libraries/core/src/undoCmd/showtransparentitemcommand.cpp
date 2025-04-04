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
#include "undoCmd/showtransparentitemcommand.h"

#include "controller/item_controllers/visualitemcontroller.h"

ShowTransparentItemCommand::ShowTransparentItemCommand(const QList<vmap::VisualItemController*>& list)
{
    std::transform(std::begin(list), std::end(list), std::back_inserter(m_data),
                   [](vmap::VisualItemController* a) { return a; });
}

void ShowTransparentItemCommand::undo()
{
    std::for_each(std::begin(m_data), std::end(m_data), [](const QPointer<vmap::VisualItemController>& a) {
        if(!a)
            return;
        a->setOpacity(0.0);
    });
}

void ShowTransparentItemCommand::redo()
{
    std::for_each(std::begin(m_data), std::end(m_data), [](const QPointer<vmap::VisualItemController>& a) {
        if(!a)
            return;
        a->setOpacity(1.0);
    });
}
