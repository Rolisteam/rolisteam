/***************************************************************************
 *	Copyright (C) 2014 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QAbstractListModel>
#include <QUrl>

/**
 * @brief The MusicModel class is the model which stores the playlist. Each audioWidget has one instance of this class.
 */
class QMediaContent;
class MusicModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CustomRole
    {
        TITLE= Qt::UserRole + 1,
        URL,
    };
    Q_ENUM(CustomRole)
    explicit MusicModel(QObject* parent= nullptr);

    QUrl getMediaByModelIndex(const QModelIndex&) const;



    virtual int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void addSong(const QList<QUrl>& urls);
    void insertSong(int i, QUrl str);

    void removeAll();
    void removeSong(const QModelIndexList& list);
    void setCurrentSong(const QModelIndex& p);
    QStringList mimeTypes() const override;

    int indexOfCurrent() const;

    const QList<QUrl>& urls() const;

protected:
    Qt::DropActions supportedDropActions() const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
                      const QModelIndex& index) override;

private:
    QStringList m_header;
    QList<QUrl> m_data;
    QUrl m_currentSong;
};

#endif // MUSICMODEL_H
