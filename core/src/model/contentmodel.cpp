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
#include "contentmodel.h"

#include <set>

#include "controller/view_controller/mediacontrollerbase.h"
#include "worker/messagehelper.h"

ContentModel::ContentModel(QObject* parent) : QAbstractListModel(parent) {}

ContentModel::~ContentModel()= default;

int ContentModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_medias.size();
}

QVariant ContentModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    std::set<int> acceptedRole({Qt::DisplayRole, Qt::EditRole, Qt::DecorationRole, NameRole, TitleRole, UuidRole,
                                PathRole, ContentTypeRole, ActiveRole, ModifiedRole, OwnerIdRole});

    if(acceptedRole.find(role) == acceptedRole.end())
        return {};

    QVariant value;

    auto medium= m_medias[static_cast<std::size_t>(index.row())].get();

    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case NameRole:
        value= medium->name();
        break;
    case TitleRole:
        value= medium->title();
        break;
    case UuidRole:
        value= medium->uuid();
        break;
    case PathRole:
        value= medium->path();
        break;
    case ContentTypeRole:
        value= QVariant::fromValue(medium->contentType());
        break;
    case ActiveRole:
        value= medium->isActive();
        break;
    case ModifiedRole:
        value= medium->modified();
        break;
    case OwnerIdRole:
        value= medium->ownerId();
        break;
    }

    return value;
}

bool ContentModel::appendMedia(MediaControllerBase* media)
{
    auto size= m_medias.size();
    std::unique_ptr<MediaControllerBase> ctrl(media);
    beginInsertRows(QModelIndex(), size, size);
    m_medias.push_back(std::move(ctrl));
    endInsertRows();

    emit mediaControllerAdded(media);
    return true;
}

bool ContentModel::removeMedia(const QString& uuid)
{
    auto it
        = std::find_if(std::begin(m_medias), std::end(m_medias),
                       [uuid](const std::unique_ptr<MediaControllerBase>& medium) { return uuid == medium->uuid(); });

    if(it == std::end(m_medias))
        return false;

    (*it)->aboutToClose();
    if((*it)->localIsOwner())
        MessageHelper::closeMedia(uuid, (*it)->contentType());

    auto pos= std::distance(std::begin(m_medias), it);

    beginRemoveRows(QModelIndex(), pos, pos);
    m_medias.erase(it);
    endRemoveRows();

    return true;
}

void ContentModel::clearData()
{
    beginResetModel();
    m_medias.clear();
    endResetModel();
}
