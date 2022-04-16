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
#include "historymodel.h"

#include <QSet>
#include <QUuid>

#include "worker/iohelper.h"

namespace history
{
auto findLink(const QString& id, const QList<LinkInfo>& links)
{
    return std::find_if(std::begin(links), std::end(links), [id](const LinkInfo& info) { return info.id == id; });
}

HistoryModel::HistoryModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_columns << tr("Path") << tr("id") << tr("type") << tr("last access") << tr("bookmarked");
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return m_columns[section];
    }
    else
        return {};
}

int HistoryModel::rowCount(const QModelIndex& parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if(parent.isValid())
        return 0;

    // FIXME: Implement me!
    return m_links.size();
}

int HistoryModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return m_columns.size();
}

QVariant HistoryModel::data(const QModelIndex& index, int role) const
{
    QSet<int> allowedRole({Qt::DisplayRole, Qt::EditRole, PathRole, IdRole, TypeRole, LastAccessRole, BookmarkRole});

    if(!index.isValid() || !allowedRole.contains(role))
        return QVariant();

    int wantedData= 0;
    if(role < PathRole)
        wantedData= PathRole + index.column();
    else
        wantedData= role;

    auto const& info= m_links.at(index.row());

    QVariant res;
    switch(wantedData)
    {
    case PathRole:
        res= info.url;
        break;
    case NameRole:
        res= info.displayName;
        break;
    case IdRole:
        res= info.id;
        break;
    case TypeRole:
        res= QVariant::fromValue(info.type);
        break;
    case LastAccessRole:
        res= info.lastAccess;
        break;
    case BookmarkRole:
        res= info.bookmarked;
        break;
    default:
        break;
    }

    return res;
}

bool HistoryModel::setData(const QModelIndex& idx, const QVariant& data, int role)
{
    int wantedData= 0;
    if(role < PathRole)
        wantedData= PathRole + idx.column();
    else
        wantedData= role;

    auto& info= m_links[idx.row()];

    QVector<int> roleVec;
    switch(wantedData)
    {
    case PathRole:
        info.url= QUrl::fromUserInput(data.toString());
        break;
    case NameRole:
        info.displayName= data.toString();
        break;
    case BookmarkRole:
        info.bookmarked= data.toBool();
        break;
    default:
        break;
    }
    emit dataChanged(index(idx.row(), 0), index(idx.row(), columnCount()), roleVec);

    return true;
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex& index) const
{
    int role= index.column() + PathRole;
    QSet<int> editableRole({PathRole, NameRole, BookmarkRole});

    if(editableRole.contains(role))
    {
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}

LinkInfo HistoryModel::idToPath(const QString& id) const
{
    auto it= findLink(id, m_links);
    if(it != std::end(m_links))
        return *it;
    else
        return {};
}

void HistoryModel::refreshAccess(const QString& id)
{
    auto const& itc= findLink(id, m_links);
    if(itc != std::end(m_links))
    {
        auto it= m_links.begin() + (itc - m_links.begin());
        it->lastAccess= QDateTime::currentDateTime();
        sortModel();
    }
}

void HistoryModel::addLink(const QUrl& path, const QString& displayName, Core::ContentType type)
{
    if(path.isEmpty())
        return;

    auto it= std::find_if(std::begin(m_links), std::end(m_links), [path, type](const LinkInfo& info) {
        qDebug() << "info and path:" << info.url << path;
        return info.url == path && info.type == type;
    });

    if(it != std::end(m_links))
    {
        refreshAccess(it->id);
    }
    else
    {
        beginInsertRows(QModelIndex(), 0, 0);
        LinkInfo info{QUuid::createUuid().toString(QUuid::WithoutBraces), displayName, path, type};
        m_links.prepend(info);
        endInsertRows();
    }
}

int HistoryModel::maxCapacity() const
{
    return m_maxCapacity;
}

void HistoryModel::setLinks(const QList<LinkInfo>& links)
{
    beginResetModel();
    m_links.clear();
    m_links << links;
    endResetModel();
}

void HistoryModel::clear()
{
    beginResetModel();
    m_links.clear();
    endResetModel();
}

void HistoryModel::setMaxCapacity(int max)
{
    if(m_maxCapacity == max)
        return;
    m_maxCapacity= max;
    emit maxCapacityChanged();
}

void HistoryModel::sortModel()
{
    beginResetModel();
    std::sort(std::begin(m_links), std::end(m_links), [](const LinkInfo& a, const LinkInfo& b) {
        if(a.bookmarked != b.bookmarked)
            return a.bookmarked;
        else
        {
            return a.lastAccess < b.lastAccess;
        }
    });
    endResetModel();
}
const QList<LinkInfo>& HistoryModel::data() const
{
    return m_links;
}
} // namespace history
