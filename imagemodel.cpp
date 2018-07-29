#include "imagemodel.h"
#include <QIcon>
#include <QBuffer>

#define TOOLTIP_SIZE 256

ImageModel::ImageModel(QHash<QString,QPixmap*>& list,QObject *parent)
    : QAbstractTableModel(parent),
      m_imageData(new QList<QPixmap>()),
      m_list(list)
{
    m_column << tr("Key")<< tr("Filename")<< tr("Is Background");
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
            case Key:
                return "image://rcs/"+m_keyList.at(index.row());
            case Filename:
                return m_filename.at(index.row());
            case Background:
                return m_background.at(static_cast<size_t>(index.row()));
        }
    }
    else if(Qt::EditRole == role)
    {
        switch (index.column())
        {
            case Key:
                return m_keyList.at(index.row());
            case Filename:
                return m_filename.at(index.row());
            case Background:
                return m_background.at(static_cast<size_t>(index.row()));
        }
    }
    else if(Qt::ToolTipRole == role)
    {
        auto image =  m_imageData->at(index.row());
        image = image.scaledToWidth(TOOLTIP_SIZE);
        QByteArray data;
        QBuffer buffer(&data);
        image.save(&buffer, "PNG", 100);
        return QString("<img src='data:image/png;base64, %0'>").arg(QString(data.toBase64()));
    }
    return QVariant();
}
bool ImageModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool val = false;
    if((Qt::DisplayRole == role)||(Qt::EditRole == role))
    {
        switch (index.column())
        {
        case Key:
        {
                m_provider->removeImg(m_keyList.at(index.row()));
                QPixmap* pix = m_list.value(m_keyList.at(index.row()));
                m_list.remove(m_keyList.at(index.row()));
                m_keyList.replace(index.row(),value.toString());
                m_provider->insertPix(value.toString(),m_imageData->at(index.row()));
                m_list.insert(value.toString(),pix);
        }
            val = true;
            break;
        case Background:
            m_background[static_cast<size_t>(index.row())]= value.toBool();
            val = true;
            break;
        default:
            break;
        }
    }
    return val;
}
 QList<QPixmap>* ImageModel::imageData() const
{
    return m_imageData;
}

void ImageModel::setImageData( QList<QPixmap>* imageData)
{
    m_imageData = imageData;
}

QJsonArray ImageModel::save()
{
    QJsonArray images;
    int i = 0;
    for(auto key : m_keyList)
    {
        auto pix = m_imageData->at(i);
        bool bg = m_background[static_cast<size_t>(i)];
        if(!pix.isNull())
        {
            QJsonObject oj;

            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            oj["bin"]=QString(buffer.data().toBase64());
            oj["key"]=key;
            oj["isBg"]=bg;
            images.append(oj);
        }
        ++i;
    }
    return images;
}


void ImageModel::insertImage(QPixmap * pix, QString key, QString stuff, bool isBg)
{
    beginInsertRows(QModelIndex(),m_imageData->size(),m_imageData->size());
    m_provider->insertPix(key,*pix);
    m_list.insert(key,pix);
    m_imageData->append(*pix);
    m_keyList.append(key);
    m_filename.append(stuff);
    m_background.push_back(isBg);
    endInsertRows();
}

Qt::ItemFlags ImageModel::flags(const QModelIndex &index) const
{
    if(index.column() == Key || index.column() == Background)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

void ImageModel::clear()
{
    beginResetModel();
    m_imageData->clear();
    m_provider->cleanData();
    m_list.clear();
    m_keyList.clear();
    m_filename.clear();
    endResetModel();
}

void ImageModel::setImageProvider(RolisteamImageProvider *img)
{
    m_provider = img;
}

void ImageModel::setPixList(QHash<QString, QPixmap*> &list)
{
    m_list = list;
}
bool ImageModel::isBackgroundById(QString id)
{
    int i = m_keyList.indexOf(id);
    return m_background.at(static_cast<size_t>(i));
}

void ImageModel::removeImageAt(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    auto i = index.row();
    beginRemoveRows(QModelIndex(), i,i);
    auto key = m_keyList.at(i);
    m_imageData->removeAt(i);
    m_provider->removeImg(key);
    m_filename.removeAt(i);
    m_keyList.removeAt(i);
    m_background.erase(m_background.begin() + i);
    endRemoveRows();

}
