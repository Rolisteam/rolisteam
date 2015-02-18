#ifndef SESSIONITEMMODEL_H
#define SESSIONITEMMODEL_H

#include <QAbstractItemModel>

class SessionItemModel : public QAbstractItemModel
{
public:
    SessionItemModel();

   virtual QModelIndex index( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
   virtual QModelIndex parent( const QModelIndex & index ) const;
   virtual int rowCount(const QModelIndex&) const;
   virtual int columnCount( const QModelIndex&) const;
   virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // SESSIONITEMMODEL_H
