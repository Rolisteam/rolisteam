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
#include "mindmap/command/addimagetonodecommand.h"

#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"

#include "utils/iohelper.h"

namespace mindmap
{
/*AddImageToNodeCommand::AddImageToNodeCommand(MindItemModel* nodeModel, ImageModel* imgModel, const QString& id,
                                             const QPixmap& pix)
    : m_nodeModel(nodeModel), m_id(id), m_imgModel(imgModel), m_pixmap(pix)
{
}*/

AddImageToNodeCommand::AddImageToNodeCommand(MindItemModel* nodeModel, ImageModel* imgModel, const QString& id,
                                             const QString& url)
    : m_nodeModel(nodeModel), m_id(id), m_imgModel(imgModel)
{
    m_pixmap= utils::IOHelper::readPixmapFromFile(url);
    setText(QObject::tr("Add %1 image").arg(url));
}

void mindmap::AddImageToNodeCommand::undo()
{
    m_imgModel->removePixmap(m_id);
    //m_nodeModel->setImageUriToNode(m_id, {});
    m_nodeModel->update(m_id, MindItemModel::HasPicture);
}

void mindmap::AddImageToNodeCommand::redo()
{
    m_imgModel->insertPixmap(m_id, m_pixmap);
    //m_nodeModel->setImageUriToNode(m_id, m_id);
    m_nodeModel->update(m_id, MindItemModel::HasPicture);
}
} // namespace mindmap
