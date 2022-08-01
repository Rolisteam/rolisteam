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

#include "mindmap/mindmap_global.h"
#include <QAbstractListModel>
#include <QPixmap>
#include <QString>
#include <QUrl>
namespace mindmap
{
struct MINDMAP_EXPORT ImageInfo
{
    QPixmap m_pixmap;
    QString m_id;
    QUrl m_url;
};

class MINDMAP_EXPORT ImageModel : public QAbstractListModel
{
    Q_OBJECT
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

    void insertPixmap(const QString& id, const QPixmap& map, const QUrl& url= QUrl(), bool network= false);
    int removePixmap(const QString& id);
    bool hasPixmap(const QString& id) const;
    QPixmap pixmapFromId(const QString& id) const;
    const ImageInfo imageInfoFromId(const QString& id) const;

    const std::vector<ImageInfo>& imageInfos() const;
signals:
    void imageAdded(const QString& id);
    void imageRemoved(const QString& id);

private:
    std::vector<ImageInfo> m_data;
};
} // namespace mindmap
#endif // IMAGEMODEL_H
