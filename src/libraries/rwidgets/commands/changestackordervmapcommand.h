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
#ifndef CHANGESTACKORDERVMAPCOMMAND_H
#define CHANGESTACKORDERVMAPCOMMAND_H

#include <QPointer>
#include <QUndoCommand>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "graphicsItems/visualitem.h"

#include "rwidgets_global.h"

struct RWIDGET_EXPORT ItemToControllerInfo
{
    vmap::VisualItemController* ctrl;
    VisualItem* item;
};

class RWIDGET_EXPORT ChangeStackOrderVMapCommand : public QUndoCommand
{
    Q_DECLARE_TR_FUNCTIONS(ChangeStackOrderVMapCommand)

public:
    ChangeStackOrderVMapCommand(VectorialMapController* vmapCtrl, const QList<ItemToControllerInfo>& first,
                                const QList<ItemToControllerInfo>& second);

    void undo() override;
    void redo() override;

private:
    QPointer<VectorialMapController> m_vmapCtrl;
    QList<ItemToControllerInfo> m_first;
    QList<ItemToControllerInfo> m_second;
    QStringList m_firstIds;
    QStringList m_secondIds;
};

#endif // CHANGESTACKORDERVMAPCOMMAND_H
