/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "removeimagefromnodecommand.h"

#include "core/model/imagemodel.h"
#include "mindmap/src/model/boxmodel.h"

namespace mindmap
{
RemoveImageFromNodeCommand::RemoveImageFromNodeCommand(BoxModel* nodeModel, ImageModel* imgModel, const QString& id)
    : m_nodeModel(nodeModel), m_id(id), m_imgModel(imgModel)
{
    m_pixmap= imgModel->pixmapFromId(m_id);
    Q_ASSERT(!m_pixmap.isNull());
    setText(QObject::tr("Removing image from node"));
}

void RemoveImageFromNodeCommand::undo()
{
    m_imgModel->insertPixmap(m_id, m_pixmap);
    m_nodeModel->setImageUriToNode(m_id, m_id);
}

void RemoveImageFromNodeCommand::redo()
{
    m_imgModel->removePixmap(m_id);
    m_nodeModel->setImageUriToNode(m_id, {});
}
} // namespace mindmap
