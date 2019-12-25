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
#include "musicmodel.h"

#include "preferences/preferencesmanager.h"

#include <QFont>
#include <QMediaContent>
#include <QMimeData>
#include <QUrl>
#include <QNetworkRequest>

#include <set>

// https://api.soundcloud.com/tracks/293/stream?client_id=59632ff691d8ac46c637c1467d84b6c6

MusicModel::MusicModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_header({tr("Title")})
    , m_player(new QMediaPlayer())
{
}

int MusicModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return m_data.size();
    return 0;
}

int MusicModel::columnCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return COLUMN_COUNT;
    return 0;
}

QVariant MusicModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return {};

    if(Qt::DisplayRole == role)
    {
        return m_header[section];
    }

    return {};
}

namespace {
    QFont boldFont() {
        QFont font;
        font.setBold(true);
        return font;
    }

    QString normalizeUrl(const QUrl& url) {
        if (url.isLocalFile() || url.host().contains("tabletopaudio.com") == false)
            return url.fileName();

        QString str= url.toString();
        str= str.right(str.size() - (str.lastIndexOf("=") + 1));
        return str.replace(".mp3", "").replace("_", " ");
    }
}

QVariant MusicModel::data(const QModelIndex& index, int role) const
{
    // Break early if role is not Diplay or Font.
    if (std::set<int>{Qt::DisplayRole, Qt::FontRole}.count(role) == 0) {
        return {};
    }

    switch(role) {
        case Qt::DisplayRole:
            if (index.column() == TITLE) {
                return normalizeUrl(m_data.at(index.row())->request().url());
            }
        break;
        case Qt::FontRole:
            if(index == m_currentSong) {
                return QVariant(boldFont());
            }
        break;
        default:
        break;
    }

    return {};
}

void MusicModel::addSong(QStringList list)
{
    if(list.isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + list.size() - 1);
    m_data.reserve(list.size());

    for(auto& tmp : list)
    {
        QUrl tmpUrl= QUrl::fromUserInput(tmp);
        Q_ASSERT(tmpUrl.isValid());
        m_data.append(new QMediaContent(tmpUrl.isLocalFile() ? QUrl::fromLocalFile(tmp) : tmpUrl));
    }
    endInsertRows();
}
void MusicModel::insertSong(int i, QString str)
{
    if(str.isEmpty())
        return;
    if(0 > i)
    {
        i= m_data.size();
    }

    beginInsertRows(QModelIndex(), i, i);
    QUrl tmpUrl= QUrl::fromUserInput(str); //,QUrl::StrictMode
    Q_ASSERT(tmpUrl.isValid());
    m_data.insert(i, new QMediaContent(tmpUrl.isLocalFile() ? QUrl::fromLocalFile(str) : tmpUrl));
    endInsertRows();
}
QMediaContent* MusicModel::getMediaByModelIndex(const QModelIndex& index) const
{
    return m_data.at(index.row());
}
void MusicModel::removeAll()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
void MusicModel::removeSong(const QModelIndexList& list)
{
    if(list.isEmpty())
        return;

    QModelIndex first= list[0];
    QModelIndex end= list.last();
    beginRemoveRows(first.parent(), first.row(), end.row());
    for(int i= list.size() - 1; i >= 0; --i)
    {
        QModelIndex index= list[i];
        m_data.removeAt(index.row());
    }
    endRemoveRows();
}
void MusicModel::setCurrentSong(const QModelIndex& p)
{
    m_currentSong= p;
    emit dataChanged(p, p);
}
QModelIndex MusicModel::getCurrentSong()
{
    return static_cast<QModelIndex>(m_currentSong);
}
void MusicModel::saveIn(QTextStream& file)
{
    for(auto& tmp : m_data)
    {
        file << tmp->request().url().toString(QUrl::PreferLocalFile) << "\n";
    }
}
QStringList MusicModel::mimeTypes() const
{
    return {"text/uri-list"};
}

Qt::DropActions MusicModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
bool MusicModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int, const QModelIndex&)
{
    if(action == Qt::IgnoreAction)
        return true;

    if(!data->hasUrls())
        return false;

    QList<QUrl> list= data->urls();
    QStringList filters = PreferencesManager::getInstance()
        ->value("AudioFileFilter", "*.wav *.mp2 *.mp3 *.ogg *.flac")
        .toString()
        .split(' ');


    for (QString& filter : filters) {
        filter.replace("*", "");
    }

    for (QUrl url : list) {
        QString file = url.toLocalFile();
        for (const QString & filter : filters) {
            if (file.endsWith(filter)) {
                insertSong(row, file);
                continue;
            }
        }
    }

    return true;
}
Qt::ItemFlags MusicModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags= QAbstractListModel::flags(index);
    if(!index.isValid())
        defaultFlags |= Qt::ItemIsDropEnabled;

    return  defaultFlags;
}
