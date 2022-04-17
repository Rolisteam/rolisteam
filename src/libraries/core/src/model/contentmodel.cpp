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
#include "model/contentmodel.h"

#include <QDebug>
#include <set>

#include "controller/view_controller/mediacontrollerbase.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include "worker/messagehelper.h"

ContentModel::ContentModel(QObject* parent) : QAbstractListModel(parent) {}

ContentModel::~ContentModel()= default;

int ContentModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_medias.size());
}

QVariant ContentModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    std::set<int> acceptedRole({Qt::DisplayRole, Qt::EditRole, Qt::DecorationRole, NameRole, TitleRole, UuidRole,
                                PathRole, ContentTypeRole, ActiveRole, ModifiedRole, OwnerIdRole, ControllerRole});

    if(acceptedRole.find(role) == acceptedRole.end())
        return {};

    QVariant value;

    auto medium= m_medias[static_cast<std::size_t>(index.row())].get();

    if(nullptr == medium)
        return {};

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
        value= medium->url();
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
    case ControllerRole:
        value= QVariant::fromValue(medium);
        break;
    }

    return value;
}

bool ContentModel::appendMedia(MediaControllerBase* media)
{
    auto size= static_cast<int>(m_medias.size());
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

    auto pos= static_cast<int>(std::distance(std::begin(m_medias), it));

    beginRemoveRows(QModelIndex(), pos, pos);
    m_medias.erase(it);
    endRemoveRows();

    return true;
}

QString ContentModel::activeMediaId() const
{
    auto it= std::find_if(std::begin(m_medias), std::end(m_medias),
                          [](const std::unique_ptr<MediaControllerBase>& medium) { return medium->isActive(); });

    if(it == m_medias.end())
        return {};

    return (*it)->uuid();
}

MediaControllerBase* ContentModel::media(const QString& mediaId)
{
    auto it= std::find_if(std::begin(m_medias), std::end(m_medias),
                          [mediaId](const std::unique_ptr<MediaControllerBase>& medium)
                          { return mediaId == medium->uuid(); });

    if(it == m_medias.end())
        return nullptr;

    return it->get();
}

std::vector<MediaControllerBase*> ContentModel::controllers() const
{
    std::vector<MediaControllerBase*> vec;
    std::transform(std::begin(m_medias), std::end(m_medias), std::back_inserter(vec),
                   [](const std::unique_ptr<MediaControllerBase>& media) { return media.get(); });
    return vec;
}

QHash<int, QByteArray> ContentModel::roleNames() const
{
    return QHash<int, QByteArray>({{NameRole, "name"},
                                   {TitleRole, "title"},
                                   {UuidRole, "uuid"},
                                   {PathRole, "path"},
                                   {ContentTypeRole, "type"},
                                   {ActiveRole, "active"},
                                   {ModifiedRole, "modified"},
                                   {OwnerIdRole, "owner"},
                                   {ControllerRole, "controller"}});
}

void ContentModel::clearData()
{
    beginResetModel();
    m_medias.clear();
    endResetModel();
}

int ContentModel::mediaCount(Core::ContentType type)
{
    return std::count_if(std::begin(m_medias), std::end(m_medias),
                         [type](const std::unique_ptr<MediaControllerBase>& media)
                         { return media->contentType() == type; });
}

///////////////////////////////////////////
//
//////////////////////////////////////////

FilteredContentModel::FilteredContentModel(Core::ContentType type) : m_type(type)
{
    // contentController<VectorialMapController*>();
    // contentController<SharedNoteController*>();
    setFilterRole(ContentModel::ContentTypeRole);

    setDynamicSortFilter(true);
}

bool FilteredContentModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    auto idx= sourceModel()->index(source_row, 0, source_parent);
    return idx.data(ContentModel::ContentTypeRole).value<Core::ContentType>() == m_type;
}
