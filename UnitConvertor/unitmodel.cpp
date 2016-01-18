#include "unitmodel.h"

UnitModel::UnitModel()
{

}

QVariant UnitModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(role == Qt::DisplayRole)
    {
        return m_data.values().at(index.row())->name();
    }

    return QVariant();
}

int UnitModel::rowCount(const QModelIndex &parent) const
{
    return m_data.values().size();
}

QVariant UnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

void UnitModel::insertData(Unit *unit)
{
    m_data.insertMulti(unit->currentCat(),unit);
}

