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
#include "model/filteredplayermodel.h"
#include "model/playermodel.h"

namespace InstantMessaging
{
FilteredPlayerModel::FilteredPlayerModel(const QStringList& list, QObject* parent)
    : QSortFilterProxyModel(parent), m_participants(list)
{
    connect(this, &FilteredPlayerModel::sourceModelChanged, this, &FilteredPlayerModel::invalidateFilter);
}

QStringList FilteredPlayerModel::recipiantIds() const
{
    return m_participants;
}
/*
int FilteredPlayerModel::(const QModelIndex& idx) const
{
    if(idx.isValid())
        return 0;
    auto z= m_participants.size();

    if(!m_allow)
    {
        auto s= sourceModel();

        if(s)
            z= s->rowCount() - z;
    }

    return z;
}

QVariant FilteredPlayerModel::data(const QModelIndex& idx, int role) const
{
    auto source= sourceModel();

    if(!source)
        return {};

    auto i= source->data(mapToSource(idx), role);
    qDebug() << "data:" << i << role;
    return i;
}

QHash<int, QByteArray> FilteredPlayerModel::roleNames() const
{
    static QHash<int, QByteArray> role({{PlayerModel::IdentifierRole, "uuid"},
                                        {PlayerModel::PersonPtrRole, "person"},
                                        {PlayerModel::NameRole, "name"},
                                        {PlayerModel::LocalRole, "local"},
                                        {PlayerModel::GmRole, "gm"},
                                        {PlayerModel::CharacterRole, "character"},
                                        {PlayerModel::CharacterStateIdRole, "characterState"},
                                        {PlayerModel::NpcRole, "npc"},
                                        {PlayerModel::AvatarRole, "avatar"}});
    return role;
}
*/
bool FilteredPlayerModel::hasRecipiant(const QString& uuid)
{
    return m_participants.contains(uuid);
}

QString FilteredPlayerModel::recipiantName(const QString& uuid)
{
    auto source= sourceModel();
    for(int i= 0; i < source->rowCount(); ++i)
    {
        auto indexModel= source->index(i, 0);
        auto id= indexModel.data(PlayerModel::IdentifierRole).toString();
        if(uuid == id)
            return indexModel.data(PlayerModel::NameRole).toString();
    }
    return {};
}

bool FilteredPlayerModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    auto idx= sourceModel()->index(sourceRow, 0, sourceParent);
    auto uuid= sourceModel()->data(idx, PlayerModel::IdentifierRole).toString();
    auto b= m_participants.contains(uuid);
    return m_allow ? b : !b;
}

bool FilteredPlayerModel::allow() const
{
    return m_allow;
}

void FilteredPlayerModel::setAllow(bool newAllow)
{
    if(m_allow == newAllow)
        return;
    m_allow= newAllow;
    emit allowChanged();
    invalidateFilter();
}

void FilteredPlayerModel::setRecipiants(const QStringList& list)
{
    if(m_participants == list)
        return;
    m_participants= list;
    emit recipiantsChanged();
    invalidateFilter();
}

void FilteredPlayerModel::addRecipiants(const QString& id)
{
    if(id.isEmpty())
        return;
    m_participants << id;
    emit recipiantsChanged();
    invalidateFilter();
    emit recipiantAdded(id);
}

void FilteredPlayerModel::removeRecipiants(const QString& id)
{
    auto c= m_participants.removeAll(id);
    if(c > 0)
        emit recipiantsChanged();
    invalidateFilter();
    emit recipiantRemoved(id);
}

} // namespace InstantMessaging
