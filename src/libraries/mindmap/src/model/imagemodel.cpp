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
#include "mindmap/model/imagemodel.h"

#include <QSet>
namespace mindmap
{
ImageModel::ImageModel() {}
QVariant ImageModel::data(const QModelIndex& index, int role) const
{
    QSet<int> allowedRole{Qt::DisplayRole, PixmapRole, IdRole, UrlRole, LocalFileRole};

    auto pos= index.row();

    if(!allowedRole.contains(role) || pos < 0 || pos >= static_cast<int>(m_data.size()))
        return {};

    auto info= m_data[pos];

    QVariant res;
    switch(role)
    {
    case Qt::DisplayRole:
        res= info.m_url.toString();
        break;
    case UrlRole:
        res= info.m_url;
        break;
    case IdRole:
        res= info.m_id;
        break;
    case LocalFileRole:
        res= info.m_url.isLocalFile();
        break;
    default:
        break;
    }

    return res;
}

int ImageModel::rowCount(const QModelIndex& index) const
{
    if(index.isValid())
        return 0;
    return static_cast<int>(m_data.size());
}

void ImageModel::insertPixmap(const QString& id, const QPixmap& map, const QUrl& url, bool network)
{
    auto pos= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), pos, pos);
    m_data.push_back(ImageInfo{map, id, url});
    endInsertRows();
    if(!network)
        emit imageAdded(id);
}

int ImageModel::removePixmap(const QString& id)
{
    auto size= m_data.size();

    auto it
        = std::find_if(std::begin(m_data), std::end(m_data), [id](const ImageInfo& info) { return id == info.m_id; });

    if(it == std::end(m_data))
        return 0;

    auto pos= std::distance(std::begin(m_data), it);
    beginRemoveRows(QModelIndex(), pos, pos);
    m_data.erase(it);
    endRemoveRows();
    emit imageRemoved(id);
    return size - m_data.size();
}

bool ImageModel::hasPixmap(const QString& id) const
{
    auto it
        = std::find_if(std::begin(m_data), std::end(m_data), [id](const ImageInfo& info) { return id == info.m_id; });

    return (it != std::end(m_data));
}

QPixmap ImageModel::pixmapFromId(const QString& id) const
{

    auto it
        = std::find_if(std::begin(m_data), std::end(m_data), [id](const ImageInfo& info) { return id == info.m_id; });

    if(it == std::end(m_data))
        return {};

    return it->m_pixmap;
}

const ImageInfo ImageModel::imageInfoFromId(const QString& id) const
{
    auto it
        = std::find_if(std::begin(m_data), std::end(m_data), [id](const ImageInfo& info) { return id == info.m_id; });

    if(it == std::end(m_data))
        return {};

    return *it;
}

const std::vector<ImageInfo>& ImageModel::imageInfos() const
{
    return m_data;
}
} // namespace mindmap
