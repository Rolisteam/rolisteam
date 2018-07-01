#include "customrulemodel.h"

#define PERMANENT_COL_COUNT 2
#include <QDebug>

namespace std {

template <>
pair<GMTOOL::Unit,GMTOOL::Unit>& pair<GMTOOL::Unit,GMTOOL::Unit>::operator=(const pair<GMTOOL::Unit,GMTOOL::Unit>& pairB)
{
    this->first = std::move(pairB.first);
    this->second = std::move(pairB.second);
    return *this;
}

}

namespace GMTOOL
{

using UnitPair = std::pair<Unit,Unit>;
using ConvertionItem = std::pair<UnitPair,ConvertorOperator*>;

CustomRuleModel::CustomRuleModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

QVariant CustomRuleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((orientation == Qt::Horizontal) && (Qt::DisplayRole == role))
    {
         switch(section)
         {
         case 0:
             return tr("Unit");
         case 1:
             return tr("Symbol");
         default:
         {
             if(m_units->rowCount()>section-PERMANENT_COL_COUNT)
             {
                QModelIndex index = createIndex(section-PERMANENT_COL_COUNT,0);
                return m_units->data(mapToSource(index));
             }
         }
         }
    }
    return {};
}

bool CustomRuleModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role))
    {
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

void CustomRuleModel::setCurrentCategory(const QString & cat,int currentCateId)
{
    beginResetModel();
    m_currentCatId = currentCateId;
    m_units->setCurrentCategory(cat);
    endResetModel();
}

QHash<QPair<const Unit *, const Unit *>, ConvertorOperator *> *CustomRuleModel::convertionRules() const
{
    return m_convertionRules;
}

void CustomRuleModel::setConvertionRules(QHash<QPair<const Unit *, const Unit *>, ConvertorOperator *> *convertionRules)
{
    m_convertionRules = convertionRules;
}


int CustomRuleModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_units->rowCount();
}

int CustomRuleModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_units->rowCount()+PERMANENT_COL_COUNT;
}
bool operator==(const Unit& unitA,const Unit& unitB)
{
    return unitA.name() == unitB.name();
}
bool operator==(Unit& unitA,const Unit& unitB)
{
    return unitA.name() == unitB.name();
}

bool sameUnit(QPair<const Unit*,const Unit*> pair)
{
    return (pair.first == pair.second);
}

QPair<const Unit*,const Unit*> CustomRuleModel::makePair(const QModelIndex& indexPair) const
{
    auto idx = index(indexPair.row(),0,QModelIndex());
    auto idxSource = mapToSource(idx);

    qDebug() <<"column:"<< indexPair.column();
    auto idx2 = index(indexPair.column()-PERMANENT_COL_COUNT,0,QModelIndex());
    auto idx2Source = mapToSource(idx2);

    Unit* xUnit = m_units->data(idxSource,UnitModel::UnitRole).value<Unit*>();
    Unit* yUnit = m_units->data(idx2Source,UnitModel::UnitRole).value<Unit*>();
    QPair<const Unit*, const Unit*> pair(xUnit,yUnit);
    return pair;
}

CategoryModel *CustomRuleModel::units() const
{
    return m_units;
}

void CustomRuleModel::setUnits(CategoryModel *units)
{
    m_units = units;
    setSourceModel(units);
}
QVariant CustomRuleModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid())
        return QVariant();

    const auto& pair = makePair(idx);
    if(Qt::DisplayRole == role || Qt::EditRole == role)
    {
        const Unit* xUnit = sourceModel()->data(mapToSource(index(idx.row(),0,QModelIndex())),UnitModel::UnitRole).value<Unit*>();
        if(nullptr != xUnit)
        {
            if(idx.column() == 0)
            {
                return xUnit->name();
            }
            else if(idx.column() == 1)
            {
                return xUnit->symbol();
            }
            else if(m_convertionRules->contains(pair))
            {
                auto convertor = m_convertionRules->value(pair);
                return QString("%1x+%2").arg(convertor->a()).arg(convertor->b());
            }
            else
                return QVariant();
        }
        else
        {
            return QAbstractProxyModel::data(idx,role);
        }
    }
    else if(Qt::BackgroundColorRole == role)
    {
        if(sameUnit(pair))
        {
            return QVariant::fromValue(Qt::red);
        }
    }
    return QVariant();
}

bool CustomRuleModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value)
    {
        const auto& pair = makePair(index);


        auto valueStr = value.toString();
        auto pos = valueStr.indexOf("x+");
        bool ok,ok1;
        auto a = valueStr.left(pos).toDouble(&ok);

        auto b = valueStr.right(valueStr.length()-pos).toDouble(&ok1);

        ConvertorOperator* convertor = nullptr;

        if(m_convertionRules->contains(pair))
        {
            convertor = m_convertionRules->value(pair);
            if(ok)
                convertor->setA(a);
            if(ok1)
                convertor->setB(b);
        }
        else
        {
            if(!sameUnit(pair))
            {
                convertor = new ConvertorOperator(a,b,false,false);
                m_convertionRules->insert(pair,convertor);
            }
        }
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags CustomRuleModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if(sameUnit(makePair(index)))
    {
        return Qt::NoItemFlags;
    }
    else
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex CustomRuleModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(proxyIndex.column() == 0)
        return sourceModel()->index(proxyIndex.row(),0);
    else
        return sourceModel()->index(-1,-1);
}

QModelIndex CustomRuleModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    qDebug() << "mapfromSource" << sourceIndex;
    return index(sourceIndex.row(),0,QModelIndex());
}

QModelIndex CustomRuleModel::parent(const QModelIndex &child) const
{
    return {};
}

QModelIndex CustomRuleModel::index(int row, int column, const QModelIndex &parent) const
{
    if(m_units->rowCount()>row && column<m_units->rowCount()+PERMANENT_COL_COUNT)
        return createIndex(row,column);
    else
        return {};
}

bool CustomRuleModel::insertUnit()
{
    beginInsertRows(QModelIndex(), m_units->rowCount(), m_units->rowCount());
    beginInsertColumns(QModelIndex(), m_units->rowCount()+PERMANENT_COL_COUNT, m_units->rowCount()+PERMANENT_COL_COUNT);


    m_units->addUnit(new Unit(tr("New Unit"),"",static_cast<Unit::Category>(m_currentCatId)));

    endInsertColumns();
    endInsertRows();

    return true;
}



bool CustomRuleModel::removeUnit(const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), m_units->rowCount(),m_units->rowCount());
    beginRemoveColumns(QModelIndex(), m_units->rowCount()+PERMANENT_COL_COUNT,m_units->rowCount()+PERMANENT_COL_COUNT);

    const auto unit = m_units->data(index, UnitModel::UnitRole).value<Unit*>();
    for(auto key : m_convertionRules->keys())
    {
        if((key.first == unit)||(key.second == unit))
        {
            if(!m_convertionRules->value(key)->isReadOnly())
                m_convertionRules->remove(key);
        }
    }


    //m_units.erase(m_units.begin()+index.row());

    endRemoveColumns();
    endRemoveRows();
    return true;
}

qreal CustomRuleModel::convert(QPair<const Unit*,const Unit*> pair,qreal value) const
{
    const auto rule = m_convertionRules->value(pair);
    return rule->convert(value);
}


}
