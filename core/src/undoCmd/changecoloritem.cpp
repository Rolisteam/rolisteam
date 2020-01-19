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
#include "changecoloritem.h"
#include "vmap/controller/visualitemcontroller.h"
#include <QDebug>

ChangeColorItemCmd::ChangeColorItemCmd(vmap::VisualItemController* item, QColor newColor, QUndoCommand* parent)
    : QUndoCommand(parent), m_ctrl(item), m_newColor(newColor)
{
    m_oldColor= m_ctrl->color();

    setText(QObject::tr("Change Item Color to %1").arg(newColor.name()));
}
void ChangeColorItemCmd::redo()
{
    qInfo() << QStringLiteral("Redo command ChangeColorItemCmd: %1 ").arg(text());
    m_ctrl->setColor(m_newColor);
}
void ChangeColorItemCmd::undo()
{
    qInfo() << QStringLiteral("undo command ChangeColorItemCmd: %1 ").arg(text());
    m_ctrl->setColor(m_oldColor);
}

/*void ChangeColorItemCmd::sendOffColor()
{
    NetworkMessageWriter msg(NetMsg::VMapCategory, NetMsg::ColorChanged);
    msg.string8(m_mapId);          // id map
    msg.string16(m_item->getId()); // id item
                                   // msg.rgb(m_item->getColor().rgb());
    msg.sendToServer();
}*/
