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

#include <QUrl>

#include "mindmap/model/imagemodel.h"
#include "mindmap/model/minditemmodel.h"
#include "utils/iohelper.h"

namespace mindmap
{
AddImageToNodeCommand::AddImageToNodeCommand(MindItemModel* nodeModel, ImageModel* imgModel, const QString& id,
                                             const QString& url, const QByteArray& imageData,
                                             const QString& campaignRoot, const QString& destinationRoot)
    : m_nodeModel(nodeModel), m_id(id), m_imgModel(imgModel)
{
    m_url= QUrl::fromUserInput(url);
    qDebug() << "AddImageNode: mindmap:" << imageData.isEmpty() << m_url << url;

    if(imageData.isEmpty())
        m_pixmap= utils::IOHelper::readPixmapFromURL(m_url);
    else
        m_pixmap= QPixmap::fromImage(utils::IOHelper::dataToImage(imageData));

    if(!m_url.isLocalFile())
    {
        QUrl newUrl= QUrl(QString("file://%1/%3_%2").arg(destinationRoot, m_url.fileName(), id));
        if(utils::IOHelper::savePixmapInto(m_pixmap, newUrl))
            m_url= newUrl;
    }
    else if(!m_url.toString().contains(campaignRoot))
    {
        QUrl newUrl= QUrl(QString("file://%1/%3_%2").arg(destinationRoot, m_url.fileName(), id));
        auto pathText= utils::IOHelper::copyFile(m_url.toLocalFile(), newUrl.toLocalFile(), true);
        if(!pathText.isEmpty())
            m_url= newUrl;
    }

    qDebug() << "AddImageNode: mindmap: 2" << imageData.isEmpty() << m_url << url;

    setText(QObject::tr("Add %1 image").arg(m_url.toString()));
}

void mindmap::AddImageToNodeCommand::undo()
{
    m_imgModel->removePixmap(m_id);
    m_nodeModel->update(m_id, MindItemModel::HasPicture);
}

void mindmap::AddImageToNodeCommand::redo()
{
    m_imgModel->insertPixmap(m_id, m_pixmap, m_url);
    m_nodeModel->update(m_id, MindItemModel::HasPicture);
}
} // namespace mindmap
