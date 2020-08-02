/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "addvmapitem.h"

#include "vmap/items/anchoritem.h"
#include "vmap/items/characteritem.h"
#include "vmap/items/ellipsitem.h"
#include "vmap/items/highlighteritem.h"
#include "vmap/items/imageitem.h"
#include "vmap/items/lineitem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/rectitem.h"
#include "vmap/items/ruleitem.h"
#include "vmap/items/textitem.h"

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/character.h"
#include "model/vmapitemmodel.h"
#include "network/networkmessagewriter.h"
#include "vmap/controller/vmapitemfactory.h"
#include "vmap/manager/visualitemcontrollermanager.h"

AddVmapItemCommand::AddVmapItemCommand(vmap::VmapItemModel* model, Core::SelectableTool tool,
                                       VectorialMapController* mapCtrl, const std::map<QString, QVariant>& args,
                                       QUndoCommand* parent)
    : QUndoCommand(parent), m_model(model), m_mapCtrl(mapCtrl), m_params(args), m_tool(tool)
{
    QHash<Core::SelectableTool, QString> data({{Core::SelectableTool::LINE, QObject::tr("Line")},
                                               {Core::SelectableTool::FILLEDELLIPSE, QObject::tr("Filled Ellipse")},
                                               {Core::SelectableTool::EMPTYELLIPSE, QObject::tr("Empty Ellipse")},
                                               {Core::SelectableTool::EMPTYRECT, QObject::tr("Empty Rectangle")},
                                               {Core::SelectableTool::FILLRECT, QObject::tr("Filled Rectangle")},
                                               {Core::SelectableTool::TEXT, QObject::tr("Text")},
                                               {Core::SelectableTool::TEXTBORDER, QObject::tr("Text with border")},
                                               {Core::SelectableTool::NonPlayableCharacter, QObject::tr("NPC Token")},
                                               {Core::SelectableTool::PlayableCharacter, QObject::tr("PC Token")},
                                               {Core::SelectableTool::IMAGE, QObject::tr("Image")},
                                               {Core::SelectableTool::PEN, QObject::tr("Pen")},
                                               {Core::SelectableTool::PATH, QObject::tr("Path")}});
    auto string= data.value(tool);

    setText(QObject::tr("add %1 item").arg(string));
}

void AddVmapItemCommand::undo()
{
    if(m_model.isNull())
        return;
    m_model->removeItemController(m_uuid);
}
void AddVmapItemCommand::redo()
{
#ifndef UNIT_TEST
    qInfo() << QStringLiteral("Redo command AddVmapItemCommand: %1 ").arg(text());
#endif
    if(m_model.isNull())
        return;

    auto item= vmap::VmapItemFactory::createVMapItem(m_mapCtrl, m_tool, m_params);

    m_uuid= m_model->appendItemController(item);
}
