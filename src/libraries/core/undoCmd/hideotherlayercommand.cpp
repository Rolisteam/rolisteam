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
#include "hideotherlayercommand.h"

#include "model/vmapitemmodel.h"
#include "vmap/controller/visualitemcontroller.h"

HideOtherLayerCommand::HideOtherLayerCommand(Core::Layer layer, vmap::VmapItemModel* model, bool b)
    : m_model(model), m_layer(layer), m_hide(b)
{
}

void HideOtherLayerCommand::undo()
{
    if(!m_model)
        return;
    auto items= m_model->items();
    std::for_each(std::begin(items), std::end(items), [this](vmap::VisualItemController* item) {
        if(item->layer() != m_layer)
            item->setVisible(m_hide);
    });
}

void HideOtherLayerCommand::redo()
{
    if(!m_model)
        return;
    auto items= m_model->items();
    std::for_each(std::begin(items), std::end(items), [this](vmap::VisualItemController* item) {
        if(item->layer() != m_layer)
            item->setVisible(!m_hide);
    });
}
