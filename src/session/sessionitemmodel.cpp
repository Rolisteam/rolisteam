#include "sessionitemmodel.h"

SessionItemModel::SessionItemModel()
{

}
QModelIndex SessionItemModel::index( int row, int column, const QModelIndex & parent ) const
{
    return QModelIndex();
}
QModelIndex SessionItemModel::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}
int SessionItemModel::rowCount(const QModelIndex&) const
{
    return 0;
}
int SessionItemModel::columnCount(const QModelIndex&) const
{
    return 0;
}
QVariant SessionItemModel::data(const QModelIndex &index, int role ) const
{
    return QVariant();
}
