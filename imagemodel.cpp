#include "imagemodel.h"
#include <QBuffer>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QVariant>

#define TOOLTIP_SIZE 256

namespace charactersheet
{

ImageModel::ImageModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_column << tr("Key") << tr("Filename") << tr("Is Background");
}

QVariant ImageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::Horizontal == orientation)
    {
        if(Qt::DisplayRole == role)
        {
            return m_column.at(section);
        }
    }
    return QVariant();
}

int ImageModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return static_cast<int>(m_data.size());
}

int ImageModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_column.size();
}
QVariant ImageModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QVariant var;
    const auto& info= m_data[static_cast<std::size_t>(index.row())];

    auto trueRole= role;
    auto col= index.column();

    if(Qt::ToolTipRole == role)
    {
        auto key= info.key;
        const auto& image= info.pixmap;
        auto pixmap= image.scaledToWidth(TOOLTIP_SIZE);
        QByteArray data;
        QBuffer buffer(&data);
        pixmap.save(&buffer, "PNG", 100);
        return QVariant::fromValue(QString("<img src='data:image/png;base64, %0'>").arg(QString(data.toBase64())));
    }

    std::vector<int> ap({Qt::EditRole, Qt::DisplayRole, FilenameRole, BackgroundRole, UrlRole, KeyRole});

    if(std::find(ap.begin(), ap.end(), role) == ap.end())
        return {};

    if(role == Qt::EditRole || role == Qt::DisplayRole)
    {
        if(col == 0)
            trueRole= (Qt::EditRole == role) ? KeyRole : UrlRole;
        else if(col == 1)
            trueRole= FilenameRole;
        else if(col == 2)
            trueRole= BackgroundRole;
    }
    else
        trueRole= role;

    switch(trueRole)
    {
    case KeyRole:
        var= info.key;
        break;
    case UrlRole:
        var= QStringLiteral("image://rcs/%1").arg(info.key);
        break;
    case FilenameRole:
        var= info.filename;
        break;
    case BackgroundRole:
        var= info.isBackground;
        break;
    }

    return var;
}

bool ImageModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool val= false;
    if((Qt::DisplayRole == role) || (Qt::EditRole == role))
    {
        auto& info= m_data[static_cast<std::size_t>(index.row())];
        switch(index.column())
        {
        case 0:
            info.key= value.toString();
            val= true;
            break;
        case 1:
            info.isBackground= value.toBool();
            val= true;
            break;
        case 2:
            info.filename= value.toString();
            break;
        }
    }
    return val;
}

void ImageModel::save(QJsonArray& array) const
{
    for(const auto& info : m_data)
    {
        QJsonObject oj;
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        info.pixmap.save(&buffer, "PNG");
        oj["bin"]= QString(buffer.data().toBase64());
        oj["key"]= info.key;
        oj["isBg"]= info.isBackground;
        oj["filename"]= info.filename;
        array.append(oj);
    }
}

void ImageModel::load(const QJsonArray& array)
{
    for(const auto& imgVal : array)
    {
        auto imgInfo= imgVal.toObject();
        auto imgData= imgInfo["bin"].toString();
        auto imgKey= imgInfo["key"].toString();
        auto imgIsBg= imgInfo["isBg"].toBool();
        auto filename= imgInfo["filename"].toString();

        auto data= QByteArray::fromBase64(imgData.toLocal8Bit());
        QPixmap map;
        map.loadFromData(data, "PNG");

        insertImage(map, imgKey, filename, imgIsBg);
    }
}

/*
void ImageModel::load(const QJsonArray& array)
{
    beginResetModel();
    for(auto ref : array)
    {
        auto obj= ref.toObject();
        ImageInfo info;
        auto bin= obj["bin"].toString();
        QByteArray data= QByteArray::fromBase64(bin.toLocal8Bit());

        info.pixmap.loadFromData(data);

        info.key= obj["key"].toString();
        info.isBackground= obj["isBg"].toBool();
        info.filename= obj["filename"].toString(); // NOTICE hide this field from network
        m_data.push_back(info);
    }
    endResetModel();
}*/
bool ImageModel::insertImage(const QPixmap& pix, const QString& key, const QString& filename, bool isBg)
{
    auto rect= pix.rect();
    if(isBg && !m_data.empty())
    {
        auto b= std::all_of(m_data.begin(), m_data.end(), [rect](const ImageInfo& info) {
            return info.isBackground ? rect == info.pixmap.rect() : true;
        });

        if(!b)
            return false;
    }

    auto it= std::find_if(m_data.begin(), m_data.end(), [key](const ImageInfo& info) { return info.key == key; });
    if(it != m_data.end()) // already set
    {
        it->pixmap= pix;
        it->filename= filename;
        it->isBackground= isBg;
        emit internalDataChanged();
        return true;
    }

    ImageInfo info{pix, isBg, filename, key};

    auto size= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), size, size);
    m_data.push_back(info);
    endInsertRows();
    emit internalDataChanged();
    return true;
}

Qt::ItemFlags ImageModel::flags(const QModelIndex& index) const
{
    if(index.column() == 0 || index.column() == 1)
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
    m_data.clear();
    endResetModel();
}

QPixmap ImageModel::pixmapFromKey(QString id)
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const ImageInfo& info) { return id == info.key; });
    if(it == m_data.end())
        return {};
    return it->pixmap;
}

bool ImageModel::isBackgroundById(QString id) const
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [id](const ImageInfo& info) { return id == info.key; });
    return it != m_data.end();
}

void ImageModel::removeImageAt(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    auto i= index.row();
    removeImage(i);
}

void ImageModel::setPathFor(const QModelIndex& idx, const QString& path)
{
    if(!idx.isValid())
        return;

    auto row= idx.row();
    if(row < 0 || static_cast<int>(m_data.size()) >= row)
        return;

    auto& info= m_data[static_cast<std::size_t>(row)];

    QPixmap pix(path);
    if(pix.isNull())
    {
        qWarning() << "Can't open image: " << path;
        return;
    }
    info.pixmap= pix;
    emit dataChanged(idx, idx, QVector<int>() << Qt::DisplayRole);
}

void ImageModel::removeImageByKey(const QString& key)
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [key](const ImageInfo& info) {
        return key == info.key;
    }); // auto index= m_keyList.indexOf(key);

    if(it == m_data.end())
        return; // unfound

    removeImage(static_cast<int>(std::distance(m_data.begin(), it)));
}

QSize ImageModel::backgroundSize() const
{
    auto it= std::find_if(m_data.begin(), m_data.end(), [](const ImageInfo& info) { return info.isBackground; });

    if(it == m_data.end())
        return {};

    return it->pixmap.rect().size();
}

void ImageModel::removeImage(int i)
{
    if(i < 0 || static_cast<int>(m_data.size()) <= i)
        return;

    beginRemoveRows(QModelIndex(), i, i);
    m_data.erase(m_data.begin() + i);
    endRemoveRows();
    emit internalDataChanged();
}

<<<<<<< HEAD
void ImageModel::reloadImage(const QModelIndex& idx)
{
    if(!idx.isValid())
        return;

    auto row= idx.row();
    //qDebug() << "reload image" << row << m_data.size();
    if(row < 0 || row >= m_data.size())
        return;

    //qDebug() << "reload image" << row;
    auto& info= m_data[static_cast<int>(row)];

    QPixmap pix(info.filename);
    if(pix.isNull())
    {
        qWarning() << "Can't open image: " << info.filename;
        return;
    }
   //qDebug() << "reload image" << row;
    info.pixmap= pix;
    emit dataChanged(idx, idx, QVector<int>() << Qt::DisplayRole);
}

/*#ifndef RCSE
void ImageModel::fill(NetworkMessageWriter& msg) const
{
    msg.uint32(static_cast<unsigned int>(m_data.size()));
    for(const auto& info : m_data)
    {
        const auto& pix= info.pixmap;
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        pix.toImage().save(&buffer, "PNG");
        msg.byteArray32(array);
        msg.uint8(info.isBackground);
        msg.string32(info.key);
        msg.string32(info.filename);
    }
}

void ImageModel::read(NetworkMessageReader& msg)
{
    auto size= msg.uint32();
    for(unsigned int i= 0; i < size; ++i)
    {
        ImageInfo info;
        auto array= msg.byteArray32();
        info.pixmap.loadFromData(array);
        info.isBackground= msg.uint8();
        info.key= msg.string32();
        info.filename= msg.string32();
        m_data.push_back(info);
    }
}
#endif*/
