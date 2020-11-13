#include "characterlist.h"

#include "charactersheetmodel.h"

#include <QDebug>

CharacterList::CharacterList(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

int CharacterList::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
   if (parent.isValid())
        return 0;

   //qDebug() << "Row Count" <<sourceModel()->columnCount(QModelIndex());

   return sourceModel()->columnCount(QModelIndex());
}

int CharacterList::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 1;
}

QHash<int, QByteArray> CharacterList::roleNames() const
{
    return QHash<int,QByteArray>({{NameRole,"name"},{UuidRole,"uuid"}});
}

QVariant CharacterList::data(const QModelIndex &index, int role) const
{
    QVector<int> roles({NameRole,UuidRole, Qt::DisplayRole, Qt::EditRole});
    if (!index.isValid() || !roles.contains(role))
        return QVariant();

    auto r = index.row();

    if(r == 0)
    {
        return (role ==Qt::DisplayRole) ? tr("Mock Data") : "";
    }

    //qDebug() << "Row Count 2";

    auto idx = sourceModel()->index(0, r);

    if(roles.contains(role))
    {
        int realRole = NameRole;

        if(role == UuidRole)
            realRole = role;
        auto var = idx.data(role == NameRole ? CharacterSheetModel::NameRole : CharacterSheetModel::UuidRole).toString();
        //qDebug() << var << role << UuidRole;
        if(var.isEmpty())
            var = tr("Character %1").arg(r);
        return var;
    }
    else
        return QVariant();
}
