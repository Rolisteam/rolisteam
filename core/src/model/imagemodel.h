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
#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QString>
#include <QUrl>

struct ImageInfo
{
    QPixmap m_pixmap;
    QString m_id;
    QUrl m_url;
};

class ImageModel : public QAbstractListModel
{
public:
    enum Role
    {
        PixmapRole= Qt::UserRole + 1,
        IdRole,
        UrlRole,
        LocalFileRole
    };
    ImageModel();

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& index) const override;

    void insertPixmap(const QString& id, const QPixmap& map, const QUrl& url= QUrl());
    int removePixmap(const QString& id);
    QPixmap pixmapFromId(const QString& id);

private:
    std::vector<ImageInfo> m_data;
};

#endif // IMAGEMODEL_H
