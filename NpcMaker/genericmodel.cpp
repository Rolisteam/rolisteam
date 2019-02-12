#include "genericmodel.h"
#include "data/character.h"

GenericModel::GenericModel(QStringList cols, QObject* parent) : QAbstractTableModel(parent), m_columnList(cols) {}

QVariant GenericModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && Qt::DisplayRole == role)
    {
        return m_columnList[section];
    }
    return QVariant();
}

int GenericModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.size();
}

int GenericModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_columnList.size();
}

QVariant GenericModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto item= m_data.at(index.row());

    return item->getData(index.column(), role);
}

bool GenericModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid())
        return false;

    auto item= m_data.at(index.row());
    if(item->setData(index.column(), value, role))
    {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags GenericModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void GenericModel::addData(CharacterField* item)
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(item);
    endInsertRows();
}

void GenericModel::removeData(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_data.removeAt(index.row());
    endRemoveRows();
}

QList<CharacterField*>::iterator GenericModel::begin()
{
    return m_data.begin();
}

QList<CharacterField*>::iterator GenericModel::end()
{
    return m_data.end();
}
