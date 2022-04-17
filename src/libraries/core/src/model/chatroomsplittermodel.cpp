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
#include "model/chatroomsplittermodel.h"

#include "model/filterinstantmessagingmodel.h"
#include "model/instantmessagingmodel.h"
#include <set>

namespace InstantMessaging
{
ChatroomSplitterModel::ChatroomSplitterModel(QObject* parent) : QAbstractListModel(parent) {}

int ChatroomSplitterModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_filterModels.size());
}

QVariant ChatroomSplitterModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    std::set<int> acceptedRole({FilterModelRole, UuidRole});
    auto it= acceptedRole.find(role);
    if(it == acceptedRole.end())
        return QVariant();

    QVariant var;
    auto model= m_filterModels.at(static_cast<std::size_t>(index.row())).get();
    switch(role)
    {
    case FilterModelRole:
        var= QVariant::fromValue(model);
        break;
    case UuidRole:
        var= model->uuid();
        break;
    }

    return var;
}

QHash<int, QByteArray> ChatroomSplitterModel::roleNames() const
{
    static QHash<int, QByteArray> roles({{FilterModelRole, "filterModel"}, {UuidRole, "uuid"}});
    return roles;
}

void ChatroomSplitterModel::addFilterModel(InstantMessaging::InstantMessagingModel* sourceModel, QStringList list,
                                           bool all)
{
    std::for_each(m_filterModels.begin(), m_filterModels.end(),
                  [list](const std::unique_ptr<FilterInstantMessagingModel>& model)
                  {
                      if(model->all())
                          model->setFilterParameter(true, list);
                  });

    std::unique_ptr<InstantMessaging::FilterInstantMessagingModel> model(new FilterInstantMessagingModel());
    model->setFilterParameter(all, list);

    model->setSourceModel(sourceModel);

    auto size= static_cast<int>(m_filterModels.size());

    beginInsertRows(QModelIndex(), size, size);
    m_filterModels.push_back(std::move(model));
    endInsertRows();
}

} // namespace InstantMessaging
