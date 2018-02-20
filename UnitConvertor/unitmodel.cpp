#include "unitmodel.h"
#include <QDebug>
namespace GMTOOL
{
QHash<Unit::Category,QString> UnitModel::m_cat2Text({{Unit::CURRENCY,tr("Currency")},
                                                     {Unit::DISTANCE,tr("Distance")},
                                                     {Unit::TEMPERATURE,tr("Temperature")},
                                                    {Unit::MASS,tr("MASS")}});

                                                     //{Unit::VOLUME,tr("Volume")},

CategoryModel::CategoryModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterRole(Qt::UserRole);
    setFilterKeyColumn(0);
    setDynamicSortFilter(true);
}

QString CategoryModel::currentCategory() const
{
    return m_currentCategory;
}

void CategoryModel::setCurrentCategory(const QString &currentCategory)
{
    m_currentCategory = currentCategory;
    invalidateFilter();
    setFilterFixedString(currentCategory);
}
bool CategoryModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);

    return (index0.data(Qt::UserRole).toString()==m_currentCategory);
}

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
    else if(role == Category)
    {
        return getCatNameFromId(m_data.values().at(index.row())->currentCat());
    }

    return QVariant();
}

int UnitModel::rowCount(const QModelIndex&) const
{
    return m_data.values().size();
}

QVariant UnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)    
    return QVariant();
}

Unit* UnitModel::insertData(Unit *unit)
{
    m_data.insertMulti(unit->currentCat(),unit);
    return unit;
}

Unit *UnitModel::getUnitFromIndex(const QModelIndex &i)
{
    return m_data.values().at(i.row());
}
QHash<Unit::Category, QString> UnitModel::cat2Text() const
{
    return m_cat2Text;
}

void UnitModel::setCat2Text(const QHash<Unit::Category, QString> &cat2Text)
{
    m_cat2Text = cat2Text;
}

QString UnitModel::getCatNameFromId(Unit::Category id) const
{
    return m_cat2Text[id];
}

}

