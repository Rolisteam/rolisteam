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
#ifndef CONTENTMODEL_H
#define CONTENTMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <memory>
#include <vector>

#include "media/mediatype.h"

class MediaControllerBase;

class FilteredContentModel : public QSortFilterProxyModel
{
public:
    FilteredContentModel(Core::ContentType type);

    template <class T>
    std::vector<T> contentController() const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:
    Core::ContentType m_type;
};

class ContentModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomRole
    {
        NameRole= Qt::UserRole + 1,
        TitleRole,
        UuidRole,
        PathRole,
        ContentTypeRole,
        ActiveRole,
        ModifiedRole,
        OwnerIdRole,
        ControllerRole
    };
    Q_ENUM(CustomRole)
    explicit ContentModel(QObject* parent= nullptr);
    virtual ~ContentModel();

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    bool appendMedia(MediaControllerBase* media);
    bool removeMedia(const QString& uuid);
    QString activeMediaId() const;
    MediaControllerBase* media(const QString& mediaId);

    std::vector<MediaControllerBase*> controllers() const;

    QHash<int, QByteArray> roleNames() const override;

    void clearData();

signals:
    void mediaControllerAdded(MediaControllerBase* newCtrl);

private:
    std::vector<std::unique_ptr<MediaControllerBase>> m_medias;
};
#endif // CONTENTMODEL_H
