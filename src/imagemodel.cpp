#include "imagemodel.h"
#include <QBuffer>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QVariant>

#define TOOLTIP_SIZE 256

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
    if(Qt::DisplayRole == role)
    {
        switch(index.column())
        {
        case Key:
            var= QStringLiteral("image://rcs/%1").arg(info.key);
            break;
        case Filename:
            var= info.filename;
            break;
        case Background:
            var= info.isBackground;
            break;
        }
    }
    else if(Qt::EditRole == role)
    {
        switch(index.column())
        {
        case Key:
            var= info.key;
            break;
        case Filename:
            var= info.filename;
            break;
        case Background:
            var= info.isBackground;
            break;
        }
    }
    else if(Qt::ToolTipRole == role)
    {
        auto key= info.key;
        const auto& image= info.pixmap;
        auto pixmap= image.scaledToWidth(TOOLTIP_SIZE);
        QByteArray data;
        QBuffer buffer(&data);
        pixmap.save(&buffer, "PNG", 100);
        var= QVariant::fromValue(QString("<img src='data:image/png;base64, %0'>").arg(QString(data.toBase64())));
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
        case Key:
        {
            // auto formerKey= m_keyList.at(index.row());
            info.key= value.toString();
            //    m_provider->removeImg(formerKey);
            //    QPixmap* pix= m_list.value(formerKey);
            //   m_list.remove(formerKey);

            // m_keyList.replace(index.row(), value.toString());
            // m_provider->insertPix(value.toString(), *pix);
            // m_list.insert(value.toString(), pix);
        }
            val= true;
            break;
        case Background:
            info.isBackground= value.toBool();
            val= true;
            break;
        case Filename:
            info.filename= value.toString();
        default:
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
        it->filename= isBg;
        return true;
    }

    ImageInfo info{pix, isBg, filename, key};

    auto size= static_cast<int>(m_data.size());
    beginInsertRows(QModelIndex(), size, size);
    m_data.push_back(info);
    endInsertRows();
    return true;
}

Qt::ItemFlags ImageModel::flags(const QModelIndex& index) const
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
    if(row < 0 || m_data.size() >= row)
        return;

    auto& info= m_data[static_cast<int>(row)];

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

    removeImage(std::distance(m_data.begin(), it));
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
    if(i < 0 || m_data.size() >= i)
        return;

    beginRemoveRows(QModelIndex(), i, i);
    m_data.erase(m_data.begin() + i);
    endRemoveRows();
}
