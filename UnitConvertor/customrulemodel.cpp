#include "customrulemodel.h"

#define PERMANENT_COL_COUNT 2

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
    : CategoryModel(parent)
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
             if(rowCount()>section-PERMANENT_COL_COUNT)
             {
                QModelIndex index = createIndex(section-PERMANENT_COL_COUNT,0);
                return data(index);
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
    CategoryModel::setCurrentCategory(cat);
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

int CustomRuleModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return rowCount()+PERMANENT_COL_COUNT;
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
    auto idx = CategoryModel::index(indexPair.row(),0, QModelIndex());
    auto idx2 = CategoryModel::index(indexPair.column()-PERMANENT_COL_COUNT,0,QModelIndex());

    Unit* xUnit = CategoryModel::data(idx,UnitModel::UnitRole).value<Unit*>();
    Unit* yUnit = CategoryModel::data(idx2,UnitModel::UnitRole).value<Unit*>();
    QPair<const Unit*, const Unit*> pair(xUnit,yUnit);
    return pair;
}

QModelIndex CustomRuleModel::buddy(const QModelIndex &index) const
{
    return index;
}

QVariant CustomRuleModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid())
        return QVariant();

    if(idx.column() < PERMANENT_COL_COUNT)
    {
        if(Qt::DisplayRole == role || Qt::EditRole == role)
        {
            const Unit* xUnit = CategoryModel::data(CategoryModel::index(idx.row(),0,QModelIndex()),UnitModel::UnitRole).value<Unit*>();
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
            }
        }
    }
    else
    {
        const auto& pair = makePair(idx);
        if(Qt::BackgroundColorRole == role)
        {
            if(sameUnit(pair))
            {
                return QVariant::fromValue(Qt::red);
            }
        }
        else if(Qt::DisplayRole == role || Qt::EditRole == role)
        {
                if(m_convertionRules->contains(pair))
                {
                    auto convertor = m_convertionRules->value(pair);
                    return QString("%1x+%2").arg(convertor->a()).arg(convertor->b());
                }
                else
                    return QVariant();
        }
    }
    return QVariant();
}

bool CustomRuleModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (data(idx, role) != value)
    {
        Unit* xUnit = CategoryModel::data(CategoryModel::index(idx.row(),0,QModelIndex()),UnitModel::UnitRole).value<Unit*>();
        if(idx.column() == 0)
        {
            xUnit->setName(value.toString());
        }
        else if(idx.column() == 1)
        {
            xUnit->setSymbol(value.toString());
        }
        else
        {
            const auto& pair = makePair(idx);
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
        }
        emit dataChanged(idx, idx, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags CustomRuleModel::flags(const QModelIndex &idx) const
{

    if (!idx.isValid())
        return Qt::NoItemFlags;

    Unit* xUnit = CategoryModel::data(CategoryModel::index(idx.row(),0,QModelIndex()),UnitModel::UnitRole).value<Unit*>();


    if(nullptr == xUnit)
        return Qt::NoItemFlags;

    auto flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if((idx.column()-PERMANENT_COL_COUNT)==idx.row())
    {
        flag= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else if(!xUnit->readOnly())
    {
        flag = Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else if(idx.column()-PERMANENT_COL_COUNT >= 0)
    {
        Unit* yUnit = CategoryModel::data(CategoryModel::index(idx.column()-PERMANENT_COL_COUNT,0,QModelIndex()),UnitModel::UnitRole).value<Unit*>();

        if(!yUnit->readOnly())
            flag = Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }


    return  flag;

}

QModelIndex CustomRuleModel::parent(const QModelIndex &child) const
{
    return {};
}

QModelIndex CustomRuleModel::index(int row, int column, const QModelIndex &parent) const
{
    if(rowCount()>row && column<rowCount()+PERMANENT_COL_COUNT)
    {
        return createIndex(row,column);
    }
    else
    {
        return {};
    }
}

bool CustomRuleModel::insertUnit()
{

    beginResetModel();

    auto unit = new Unit(tr("New Unit"),"",static_cast<Unit::Category>(m_currentCatId));
    unit->setReadOnly(false);
    addUnit(unit);
    invalidateFilter();
    setFilterFixedString(m_currentCategory);

    endResetModel();

    return true;
}



bool CustomRuleModel::removeUnit(const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(),rowCount(),rowCount());
    beginRemoveColumns(QModelIndex(), rowCount()+PERMANENT_COL_COUNT,rowCount()+PERMANENT_COL_COUNT);

    const auto unit = data(index, UnitModel::UnitRole).value<Unit*>();
    for(auto key : m_convertionRules->keys())
    {
        if((key.first == unit)||(key.second == unit))
        {
            if(!m_convertionRules->value(key)->isReadOnly())
                m_convertionRules->remove(key);
        }
    }

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
