#include "unitmodel.h"
#include <QDebug>
#include <QSettings>

namespace GMTOOL
{
QHash<Unit::Category, QString> UnitModel::m_cat2Text({{Unit::CURRENCY, QObject::tr("Currency")},
                                                      {Unit::DISTANCE, QObject::tr("Distance")},
                                                      {Unit::TEMPERATURE, QObject::tr("Temperature")},
                                                      {Unit::MASS, QObject::tr("MASS")},
                                                      {Unit::MASS, QObject::tr("OTHER")}});

//{Unit::VOLUME,tr("Volume")},

CategoryModel::CategoryModel(QObject* parent) : QSortFilterProxyModel(parent)
{
    setFilterRole(Qt::UserRole);
    setFilterKeyColumn(0);
    setDynamicSortFilter(true);
}

void CategoryModel::addUnit(Unit* unit)
{
    auto unitModel= dynamic_cast<UnitModel*>(sourceModel());
    if(nullptr == unitModel)
        return;
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    unitModel->insertData(unit);
    endInsertRows();
}

QString CategoryModel::currentCategory() const
{
    return m_currentCategory;
}

void CategoryModel::setCurrentCategory(const QString& currentCategory)
{
    m_currentCategory= currentCategory;
    invalidateFilter();
    setFilterFixedString(m_currentCategory);
}
bool CategoryModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index0= sourceModel()->index(sourceRow, 0, sourceParent);

    return (index0.data(Qt::UserRole).toString() == m_currentCategory);
}

//////////////////////
/// \brief UnitModel::UnitModel
/// \param parent
//////////////////////
UnitModel::UnitModel(QObject* parent) : QAbstractListModel(parent) {}

Unit* UnitModel::indexToUnit(const QModelIndex& index) const
{
    return getUnitByIndex(index.row());
}

QVariant UnitModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();
    auto unit= indexToUnit(index);
    if(nullptr == unit)
        return {};
    if(role == Qt::DisplayRole)
    {
        return unit->name();
    }
    else if(role == Category)
    {
        return getCatNameFromId(unit->currentCat());
    }
    else if(role == UnitRole)
    {
        return QVariant::fromValue(unit);
    }

    return QVariant();
}

int UnitModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    int sum= 0;
    for(auto& list : m_data)
    {
        sum+= list.size();
    }
    return sum;
}

QVariant UnitModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

Unit* UnitModel::insertData(Unit* unit)
{
    int position= 0;
    std::vector<Unit::Category> cats(
        {Unit::TEMPERATURE, Unit::DISTANCE, Unit::CURRENCY, Unit::VOLUME, Unit::MASS, Unit::CUSTOM});

    for(auto cat : cats)
    {
        auto tmplist= m_data[cat];
        if(cat <= unit->currentCat())
            position+= tmplist.size();
    }
    auto& list= m_data[unit->currentCat()];
    beginInsertRows(QModelIndex(), position, position);
    list.append(unit);
    endInsertRows();
    return unit;
}

Unit* UnitModel::getUnitFromIndex(const QModelIndex& i, int currentCat)
{
    if(!i.isValid())
        return nullptr;
    auto list= m_data.value(static_cast<Unit::Category>(currentCat));
    return list.at(i.row());
}

QHash<Unit::Category, QString> UnitModel::cat2Text() const
{
    return m_cat2Text;
}

void UnitModel::setCat2Text(const QHash<Unit::Category, QString>& cat2Text)
{
    m_cat2Text= cat2Text;
}

QString UnitModel::getCatNameFromId(Unit::Category id) const
{
    return m_cat2Text[id];
}

bool UnitModel::insertUnit(Unit::Category cat)
{
    int sum= 0;
    auto const& keys= m_data.keys();
    for(auto& key : keys)
    {
        const auto& list= m_data[key];
        if(key <= cat)
        {
            sum+= list.size();
        }
    }
    auto& list= m_data[cat];

    beginInsertRows(QModelIndex(), sum, sum);
    Unit* unit= new Unit(tr("New Unit"), "", cat);
    list.append(unit);
    endInsertRows();
    emit modelChanged();

    return true;
}

bool UnitModel::removeUnit(Unit* unit)
{
    if(!unit)
        return false;

    int sum= 0;

    auto keys= m_data.keys();
    auto it= std::find_if(std::begin(keys), std::end(keys),
                          [this, unit](const Unit::Category& cat)
                          {
                              auto list= m_data[cat];
                              return list.contains(unit);
                          });

    if(it == std::end(keys))
        return false;
    Unit::Category cat= (*it);

    for(auto& key : keys)
    {
        const auto& list= m_data[key];
        if(key <= cat)
        {
            sum+= list.size();
        }
    }
    auto& list= m_data[cat];

    beginRemoveRows(QModelIndex(), sum, sum);
    list.removeAll(unit);
    endRemoveRows();
    emit modelChanged();

    return true;
}

int UnitModel::getIndex(Unit* unit)
{
    int i= 0;
    for(auto& list : m_data)
    {
        auto pos= list.indexOf(unit);
        if(pos < 0)
        {
            i+= list.size();
        }
        else
        {
            i+= pos;
            return i;
        }
    }
    return -1;
}
Unit* UnitModel::getUnitByIndex(int r) const
{
    if(r < 0)
        return nullptr;

    for(auto& list : m_data)
    {
        if(r >= list.size())
        {
            r-= list.size();
            continue;
        }
        else
        {
            return list.at(r);
        }
    }
    return nullptr;
}
} // namespace GMTOOL
