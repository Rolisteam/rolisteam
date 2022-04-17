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
#ifndef ADDFOGOFWARCHANGECOMMAND_H
#define ADDFOGOFWARCHANGECOMMAND_H

#include <QPointer>
#include <QPolygonF>
#include <QUndoCommand>
#include <core_global.h>
namespace vmap
{
class SightController;
}
class CORE_EXPORT AddFogOfWarChangeCommand : public QUndoCommand
{
public:
    AddFogOfWarChangeCommand(vmap::SightController* ctrl, const QPolygonF& gone, bool mask);

    void redo() override;
    void undo() override;

private:
    QPointer<vmap::SightController> m_ctrl;
    QPolygonF m_poly;
    bool m_mask= false;
};

#endif // ADDFOGOFWARCHANGECOMMAND_H
