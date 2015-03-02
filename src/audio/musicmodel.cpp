#include "musicmodel.h"


#include <QUrl>
#include <QMediaContent>

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

            return m_data.at(index.row())->canonicalUrl().fileName();
        }
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
        m_data.append(new QMediaContent(QUrl::fromLocalFile(tmp)));
    }
    endInsertRows();
}
QMediaContent* MusicModel::getMediaByModelIndex(QModelIndex index)
{
    return m_data.at(index.row());
}
