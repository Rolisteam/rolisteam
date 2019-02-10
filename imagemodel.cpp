#include "imagemodel.h"
#include <QBuffer>
#include <QDebug>
#include <QIcon>
#include <QVariant>

#define TOOLTIP_SIZE 256

ImageModel::ImageModel(QHash<QString, QPixmap*>& list, QObject* parent) : QAbstractTableModel(parent), m_list(list)
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

    return m_keyList.size();
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
    if(Qt::DisplayRole == role)
    {
        switch(index.column())
        {
        case Key:
            var= QVariant::fromValue("image://rcs/" + m_keyList.at(index.row()));
            break;
        case Filename:
            var= QVariant::fromValue(m_filename.at(index.row()));
            break;
        case Background:
        {
            bool value= m_background.at(static_cast<size_t>(index.row()));
            var= QVariant::fromValue(value);
        }
        break;
        }
    }
    else if(Qt::EditRole == role)
    {
        switch(index.column())
        {
        case Key:
            var= QVariant::fromValue(m_keyList.at(index.row()));
            break;
        case Filename:
            var= QVariant::fromValue(m_filename.at(index.row()));
            break;
        case Background:
        {
            bool value= m_background.at(static_cast<size_t>(index.row()));
            var= QVariant::fromValue(value);
        }
        break;
        }
    }
    else if(Qt::ToolTipRole == role)
    {
        auto key= m_keyList.at(index.row());
        auto image= m_list[key];
        auto pixmap= image->scaledToWidth(TOOLTIP_SIZE);
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
        switch(index.column())
        {
        case Key:
        {
            auto formerKey= m_keyList.at(index.row());
            m_provider->removeImg(formerKey);
            QPixmap* pix= m_list.value(formerKey);
            m_list.remove(formerKey);

            m_keyList.replace(index.row(), value.toString());
            m_provider->insertPix(value.toString(), *pix);
            m_list.insert(value.toString(), pix);
        }
            val= true;
            break;
        case Background:
            m_background[static_cast<size_t>(index.row())]= value.toBool();
            val= true;
            break;
        default:
            break;
        }
    }
    return val;
}

QJsonArray ImageModel::save()
{
    QJsonArray images;
    int i= 0;
    for(auto& key : m_keyList)
    {
        auto pix= m_list[key];
        bool bg= m_background[static_cast<size_t>(i)];
        if(pix != nullptr)
        {
            QJsonObject oj;

            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix->save(&buffer, "PNG");
            oj["bin"]= QString(buffer.data().toBase64());
            oj["key"]= key;
            oj["isBg"]= bg;
            images.append(oj);
        }
        ++i;
    }
    return images;
}

bool ImageModel::insertImage(QPixmap* pix, QString key, QString stuff, bool isBg)
{
    if(m_keyList.contains(key))
        return false;

    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_provider->insertPix(key, *pix);
    m_list.insert(key, pix);
    m_keyList.append(key);
    m_filename.append(stuff);
    m_background.push_back(isBg);
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
    m_provider->cleanData();
    m_list.clear();
    m_keyList.clear();
    m_filename.clear();
    endResetModel();
}

void ImageModel::setImageProvider(RolisteamImageProvider* img)
{
    m_provider= img;
}

void ImageModel::setPixList(QHash<QString, QPixmap*>& list)
{
    m_list= list;
}
bool ImageModel::isBackgroundById(QString id)
{
    int i= m_keyList.indexOf(id);
    return m_background.at(static_cast<size_t>(i));
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
    if(row < 0 || m_keyList.size() <= row)
        return;

    QPixmap pix(path);
    if(pix.isNull())
    {
        qWarning() << "Can't open image: " << path;
        return;
    }
    auto key= m_keyList[row];
    m_provider->insertPix(key, pix);
    m_filename.replace(row, path);
    m_list.insert(key, new QPixmap(pix));
    auto first= index(row, 0);
    auto last= index(row, columnCount());
    emit dataChanged(first, last, QVector<int>() << Qt::DisplayRole);
}

void ImageModel::removeImageByKey(const QString& key)
{
    auto index= m_keyList.indexOf(key);
    removeImage(index);
}

void ImageModel::removeImage(int i)
{
    if(i < 0 || m_keyList.size() <= i)
        return;

    beginRemoveRows(QModelIndex(), i, i);
    auto key= m_keyList.at(i);
    m_list.remove(key);
    m_provider->removeImg(key);
    m_filename.removeAt(i);
    m_keyList.removeAt(i);
    m_background.erase(m_background.begin() + i);
    endRemoveRows();
}
