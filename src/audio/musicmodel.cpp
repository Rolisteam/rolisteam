#include "musicmodel.h"


#include <QUrl>
#include <QMediaContent>
#include <QFont>

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
                //download.php?downld_file=70_Age_of_Steam.mp3
                QString str = url.toString();
                str = str.right(str.size() - (str.lastIndexOf("=")+1));
                return  str.replace(".mp3","").replace("_"," ");
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
        QUrl tmpUrl(tmp);

        if(tmpUrl.isValid())
        {
            m_data.append(new QMediaContent(tmpUrl));
        }
        else
        {
            m_data.append(new QMediaContent(QUrl::fromLocalFile(tmp)));
        }
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
        file << tmp->canonicalUrl().toString() << "\n";
    }
}
