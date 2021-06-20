/***************************************************************************
 *	 Copyright (C) 2017 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#ifndef ADDVMAPITEMCOMMAND_H
#define ADDVMAPITEMCOMMAND_H

#include "vmap/vmap.h"
#include "vmap/vtoolbar.h"
#include <QUndoCommand>

class VectorialMapController;
class AddVmapItemCommand : public QUndoCommand
{
public:
    AddVmapItemCommand(vmap::VmapItemModel* model, Core::SelectableTool tool, VectorialMapController* mapCtrl,
                       const std::map<QString, QVariant>& args, QUndoCommand* parent= nullptr);

    void undo() override;
    void redo() override;

private:
    QPointer<vmap::VmapItemModel> m_model;
    QPointer<VectorialMapController> m_mapCtrl;
    std::map<QString, QVariant> m_params;
    Core::SelectableTool m_tool;
    QString m_uuid;
};

#endif // ADDFIELDCOMMAND_H
