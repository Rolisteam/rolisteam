/***************************************************************************
 *	Copyright (C) 2017 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "undoCmd/deletevmapitem.h"

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "utils/logcategories.h"
#include <QDebug>

DeleteVmapItemCommand::DeleteVmapItemCommand(VectorialMapController* ctrl,
                                             const QList<vmap::VisualItemController*>& list, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(ctrl)
{
    std::transform(std::begin(list), std::end(list), std::back_inserter(m_itemCtrls),
                   [](vmap::VisualItemController* itemCtrl) { return QPointer<vmap::VisualItemController>(itemCtrl); });
    setText(QObject::tr("Delete %1 Item(s) From Map").arg(list.size()));
}

void DeleteVmapItemCommand::redo()
{
    qCInfo(logCategory::map) << QStringLiteral("redo command DeleteVmapItemCommand: %1 ").arg(text());

    std::for_each(std::begin(m_itemCtrls), std::end(m_itemCtrls),
                  [](const QPointer<vmap::VisualItemController>& itemCtrl)
                  {
                      if(!itemCtrl)
                          return;
                      return itemCtrl->setRemoved(true);
                  });
}

void DeleteVmapItemCommand::undo()
{
    qCInfo(logCategory::map) << QStringLiteral("undo command DeleteVmapItemCommand: %1 ").arg(text());

    std::for_each(std::begin(m_itemCtrls), std::end(m_itemCtrls),
                  [](const QPointer<vmap::VisualItemController>& itemCtrl)
                  {
                      if(!itemCtrl)
                          return;
                      return itemCtrl->setRemoved(false);
                  });
}
