/***************************************************************************
 *	Copyright (C) 2014 by Renaud Guezennec                                 *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include <QAbstractTableModel>
#include <QMediaPlayer>
/**
 * @brief The MusicModel class is the model which stores the playlist. Each audioWidget has one instance of this class.
 */
class MusicModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief The COLUMN enum
     */
    enum COLUMN { TITLE };
    /**
     * @brief MusicModel
     * @param parent
     */
    explicit MusicModel(QObject *parent = 0);
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    virtual int columnCount(const QModelIndex & parent= QModelIndex()) const;
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    /**
     * @brief addSong
     */
    void addSong(QStringList );
    /**
     * @brief insertSong
     * @param i
     * @param str
     */
    void insertSong(int i,QString str);
    /**
     * @brief getMediaByModelIndex
     * @return
     */
    QMediaContent* getMediaByModelIndex(QModelIndex);
    /**
     * @brief removeAll
     */
    void removeAll();
    /**
     * @brief removeSong
     * @param list
     */
    void removeSong(QModelIndexList& list);
    /**
     * @brief setCurrentSong
     * @param p
     */
    void setCurrentSong(QModelIndex& p);
    /**
     * @brief getCurrentSong
     * @return
     */
    QModelIndex& getCurrentSong();
    /**
     * @brief saveIn
     * @param file
     */
    void saveIn(QTextStream& file);
    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /**
     * @brief ​mimeTypes
     */
    QStringList mimeTypes() const;
protected:
    Qt::DropActions supportedDropActions() const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
    QStringList m_header;
    QList<QMediaContent*> m_data;
    QMediaPlayer* m_player;
    QModelIndex m_currentSong;
};

#endif // MUSICMODEL_H
