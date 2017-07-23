#include "imagemodel.h"
#include <QIcon>
#include <QDebug>
ImageModel::ImageModel(QObject *parent)
    : QAbstractTableModel(parent),m_imageData(new QList<QPixmap>())
{
    m_column << tr("Thumbnail")<< tr("Key")<< tr("Filename")<< tr("Is Background");
}

QVariant ImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::Horizontal == orientation)
    {
        if(Qt::DisplayRole == role)
        {
           // qDebug() << m_column.at(section);
            return m_column.at(section);
        }
    }
    return QVariant();
}

int ImageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return  m_imageData->size();
}

int ImageModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_column.size();
}
QVariant ImageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {
        switch (index.column())
        {
            case Thumbnails:
                //return m_imageData->at(index.row());
            break;
            case Key:
//                qDebug() << m_keyList.at(index.row());
                return m_keyList.at(index.row());
            break;
            case Filename:
            //    qDebug() << m_filename.at(index.row());
                return m_filename.at(index.row());
            break;
        }
    }
    else if(Qt::DecorationRole == role)
    {
        if(index.column() == 0 )
        {
            QIcon icon(m_imageData->at(index.row()));
            return icon;
        }

    }
    return QVariant();
}

 QList<QPixmap>* ImageModel::imageData() const
{
    return m_imageData;
}

void ImageModel::setImageData( QList<QPixmap>* imageData)
{
    m_imageData = imageData;
}

void ImageModel::insertImage(QPixmap * pix, QString key, QString stuff)
{
    beginInsertRows(QModelIndex(),m_imageData->size(),m_imageData->size());
    m_imageData->append(*pix);
    m_keyList.append(key);
    m_filename.append(stuff);
    endInsertRows();
}

void ImageModel::clear()
{
    beginResetModel();
    m_imageData->clear();
    m_keyList.clear();
    m_filename.clear();
    endResetModel();
}
