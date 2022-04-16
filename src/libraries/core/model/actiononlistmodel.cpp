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
#include "actiononlistmodel.h"

#include <QDebug>

ActionOnListModel::ActionOnListModel(const QStringList& data, const QList<ActionInfo>& actions, const QString& root,
                                     QObject* parent)
    : QAbstractListModel(parent), m_actions(actions), m_root(root)

{
    std::transform(std::begin(data), std::end(data), std::back_inserter(m_data), [](const QString& path) {
        DataInfo info;
        info.data= path;
        return info;
    });
}

int ActionOnListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.size();
}

QVariant ActionOnListModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto curr= m_data.at(index.row());
    QVariant res;
    if(role == Qt::DisplayRole || role == Name)
    {
        res= curr.data.replace(m_root, "");
    }
    else if(role == Action)
        res= curr.indexAction;
    else if(role == PossibleAction)
    {
        QVariantList list;
        std::transform(std::begin(m_actions), std::end(m_actions), std::back_inserter(list),
                       [](const ActionInfo& info) { return info.name; });
        res= list;
    }
    else if(role == PossibleIcon)
    {
        QVariantList list;
        std::transform(std::begin(m_actions), std::end(m_actions), std::back_inserter(list),
                       [](const ActionInfo& info) { return info.icon; });
        res= list;
    }
    return res;
}

QHash<int, QByteArray> ActionOnListModel::roleNames() const
{
    return {{Name, "name"}, {Action, "action"}, {PossibleAction, "actions"}, {PossibleIcon, "icons"}};
}

bool ActionOnListModel::canValidate() const
{
    bool res= false;

    if(m_data.isEmpty())
        res= true;
    else
    {
        res= std::all_of(std::begin(m_data), std::end(m_data),
                         [](const DataInfo& info) { return info.indexAction != -1; });
    }

    return res;
}

void ActionOnListModel::setAction(int idx, int action)
{
    if(idx < 0 || idx >= m_data.size() || action < 0 || action >= m_actions.size())
        return;

    auto& info= m_data[idx];
    info.indexAction= action;
    auto idxItem= index(idx, 0);
    emit dataChanged(idxItem, idxItem);
}

const QList<DataInfo>& ActionOnListModel::dataset() const
{
    return m_data;
}
