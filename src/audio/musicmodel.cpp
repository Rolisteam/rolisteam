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
#include "musicmodel.h"

#include "preferences/preferencesmanager.h"

#include <QUrl>
#include <QMediaContent>
#include <QFont>
#include <QMimeData>


MusicModel::MusicModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_header << tr("Title") /*<< tr("duration")*/;
    m_player = new QMediaPlayer();


}
int MusicModel::rowCount(const QModelIndex & parent) const
{
    return m_data.size();

}

int MusicModel::columnCount(const QModelIndex & parent) const
{
    return m_header.size();
}
QVariant MusicModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QVariant();
    if(Qt::DisplayRole == role)
    {
        return m_header[section];
    }

    return QVariant();
}
QVariant MusicModel::data(const QModelIndex &index, int role) const
{
    if(Qt::DisplayRole == role)
    {
        if(index.column() == TITLE)
        {
            QUrl url = m_data.at(index.row())->canonicalUrl();
            if(url.isLocalFile())
            {
                return url.fileName();
            }
            else if(url.host().contains("tabletopaudio.com"))
            {
                QString str = url.toString();
                str = str.right(str.size() - (str.lastIndexOf("=")+1));
                return  str.replace(".mp3","").replace("_"," ");
            }
            else
            {
                return url.fileName();
            }
        }
    }
    else if((index == m_currentSong)&&(Qt::FontRole == role))
    {
        QFont font;
        font.setBold(true);
        return font;
    }

    return QVariant();
}
void MusicModel::addSong(QStringList list)
{
    if(list.isEmpty())
        return;

    beginInsertRows(QModelIndex(),m_data.size(),m_data.size()+list.size()-1);
    foreach ( QString tmp, list)
    {
        //QMediaContent* tmpMedia = new QMediaContent(tmp);
        QUrl tmpUrl= QUrl::fromUserInput(tmp);//,QUrl::StrictMode
        if((tmpUrl.isValid())&&(!tmpUrl.isLocalFile()))
        {
            m_data.append(new QMediaContent(tmpUrl));
        }
        else if(tmpUrl.isLocalFile())
        {
            m_data.append(new QMediaContent(QUrl::fromLocalFile(tmp)));
        }
    }
    endInsertRows();
}
void MusicModel::insertSong(int i,QString str)
{
    if(str.isEmpty())
        return;
    if(0>i)
    {
        i = m_data.size();
    }

    beginInsertRows(QModelIndex(),i,i);
    QUrl tmpUrl= QUrl::fromUserInput(str);//,QUrl::StrictMode
    if((tmpUrl.isValid())&&(!tmpUrl.isLocalFile()))
    {
        m_data.insert(i,new QMediaContent(tmpUrl));
    }
    else if(tmpUrl.isLocalFile())
    {
        m_data.insert(i,new QMediaContent(QUrl::fromLocalFile(str)));
    }
    endInsertRows();
}
QMediaContent* MusicModel::getMediaByModelIndex(QModelIndex index)
{
    return m_data.at(index.row());
}
void MusicModel::removeAll()
{
    beginResetModel();
    m_data.clear();
    endResetModel();
}
void MusicModel::removeSong(QModelIndexList& list)
{
    if(list.isEmpty())
        return;

    QModelIndex first=list[0];
    QModelIndex end=list.last();
    beginRemoveRows(first.parent(),first.row(),end.row());
    for(int i = list.size()-1;i>=0;--i)
    {
        QModelIndex index=list[i];
        m_data.removeAt(index.row());
    }
    endRemoveRows();
}
void MusicModel::setCurrentSong(QModelIndex& p)
{
    m_currentSong = p;
    dataChanged(p,p);
}
QModelIndex& MusicModel::getCurrentSong()
{
    return m_currentSong;
}
void MusicModel::saveIn(QTextStream& file)
{
    foreach (QMediaContent* tmp, m_data)
    {
        //qDebug() << tmp->canonicalUrl().toString(QUrl::PreferLocalFile) << tmp->canonicalUrl().toLocalFile() << tmp->canonicalUrl().adjusted(QUrl::PreferLocalFile);
        file << tmp->canonicalUrl().toString(QUrl::PreferLocalFile) << "\n";
    }
}
QStringList MusicModel::mimeTypes() const
{
    QStringList types;
    types << "text/uri-list";
    return types;
}
Qt::DropActions MusicModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
bool MusicModel::dropMimeData(const QMimeData *data,Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());


    if(data->hasUrls())
    {
        QList<QUrl> list = data->urls();
        for(int i = 0; i< list.size();++i)
        {
            QString str = list[i].toLocalFile();
            if(str.endsWith(".m3u"))
            {

            }
            else
            {
                QStringList list = PreferencesManager::getInstance()->value("AudioFileFilter","*.wav *.mp2 *.mp3 *.ogg *.flac").toString().split(' ');
                //QStringList list = audioFileFilter.split(' ');
                int i=0;
                while(i<list.size())
                {
                    QString filter = list.at(i);
                    filter.replace("*","");
                    if(str.endsWith(filter))
                    {
                        insertSong(row,str);
                    }
                    ++i;
                }
            }
        }

        return true;
    }
    return false;
}
Qt::ItemFlags MusicModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}
