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
#include "addimagetonodecommand.h"

#include "core/model/imagemodel.h"
#include "core/worker/iohelper.h"
#include "model/boxmodel.h"

namespace mindmap
{
AddImageToNodeCommand::AddImageToNodeCommand(BoxModel* nodeModel, ImageModel* imgModel, const QString& id,
                                             const QPixmap& pix)
    : m_nodeModel(nodeModel), m_id(id), m_imgModel(imgModel), m_pixmap(pix)
{
}
AddImageToNodeCommand::AddImageToNodeCommand(BoxModel* nodeModel, ImageModel* imgModel, const QString& id,
                                             const QUrl& url)
    : m_nodeModel(nodeModel), m_id(id), m_imgModel(imgModel)
{
    m_pixmap= IOHelper::readPixmapFromURL(url);
    setText(QObject::tr("Add %1 image").arg(url.fileName()));
}

void mindmap::AddImageToNodeCommand::undo()
{
    m_imgModel->removePixmap(m_id);
    m_nodeModel->setImageUriToNode(m_id, {});
}

void mindmap::AddImageToNodeCommand::redo()
{
    m_imgModel->insertPixmap(m_id, m_pixmap);
    m_nodeModel->setImageUriToNode(m_id, m_id);
}
} // namespace mindmap
