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
#ifndef HISTORY_HISTORYMODEL_H
#define HISTORY_HISTORYMODEL_H

#include "media/mediatype.h"
#include <QAbstractTableModel>
#include <QDateTime>
#include <QUrl>

#include <vector>

namespace history
{
struct LinkInfo
{
    QString id;
    QString displayName;
    QUrl url;
    Core::ContentType type;
    QDateTime lastAccess= QDateTime::currentDateTime();
    bool bookmarked= false;
};

class HistoryModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int maxCapacity READ maxCapacity WRITE setMaxCapacity NOTIFY maxCapacityChanged)
public:
    enum HistoryRole
    {
        PathRole= Qt::UserRole + 1,
        NameRole,
        IdRole,
        TypeRole,
        LastAccessRole,
        BookmarkRole
    };
    explicit HistoryModel(QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& data, int role= Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    LinkInfo idToPath(const QString& id) const;
    void refreshAccess(const QString& id);
    void addLink(const QUrl& path, const QString& displayName, Core::ContentType type);

    int maxCapacity() const;
    void setLinks(const QList<LinkInfo>& links);
    void clear();

    const QList<LinkInfo>& data() const;

public slots:
    void setMaxCapacity(int max);
signals:
    void maxCapacityChanged();

private:
    void sortModel();

private:
    QList<LinkInfo> m_links; // must be shorted by bookmarked and last access.
    int m_maxCapacity= 15;
    QStringList m_columns;
};
} // namespace history
#endif // HISTORY_HISTORYMODEL_H
