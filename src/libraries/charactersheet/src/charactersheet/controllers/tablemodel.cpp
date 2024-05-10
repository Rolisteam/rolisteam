#include "charactersheet/controllers/tablemodel.h"

#include <QJsonArray>

namespace constants
{
constexpr auto idSeparator{';'};
constexpr auto idDataCount{2};
constexpr auto formulaMark{"="};



}
namespace json
{
constexpr auto valueKey{"value"};
constexpr auto formulaKey{"formula"};
constexpr auto dataTableKey{"dataTable"};
constexpr auto columnsDefinitionKey{"columns"};
}
TableModel::TableModel() {}
int TableModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return m_data.size();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_columns.size();
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto item= m_data[index.row()][index.column()];

    static QSet<int> acceptedRole{ValueRole, FormulaRole, Qt::DisplayRole, Qt::EditRole};

    if(!acceptedRole.contains(role))
        return QVariant();

    QVariant res;
    switch(role)
    {
    case Qt::DisplayRole:
    case ValueRole:
        res = item.value;
        break;
    case Qt::EditRole:
    case FormulaRole:
        res = item.formula.isEmpty() ? item.value : item.formula;
        break;
    }

    return res;
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    return {{ValueRole, "value"},{FormulaRole, "formula"}};
}
void TableModel::addRows(int rCount)
{
    QList<CellData> cell;
    std::for_each(std::begin(m_columns), std::end(m_columns), [&cell](FieldController*){
        cell.append(CellData());
    });

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size()+rCount);
    for(int i = 0; i < rCount; ++i)
        m_data.append(cell);
    endInsertRows();
}
void TableModel::addRow()
{
    QList<CellData> cell;
    std::for_each(std::begin(m_columns), std::end(m_columns), [&cell](FieldController*){
        cell.append(CellData());
    });

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.append(cell);
    endInsertRows();
}

void TableModel::addColumn(FieldController *field)
{
    beginInsertColumns(QModelIndex(), m_columns.size(), m_columns.size());
    m_columns.append(field);
    if(m_data.isEmpty())
        addRow();
    for(auto r : m_data)
    {
        r.append(CellData());
    }
    endInsertColumns();
}

void TableModel::clear()
{
    beginResetModel();
    qDeleteAll(m_columns);
    m_columns.clear();
    m_data.clear();
    endResetModel();
}

const CellData* TableModel::cellDataFromId(const QString& id) const
{
    auto ids = id.split(constants::idSeparator);
    if(ids.size()!= constants::idDataCount)
        return nullptr;
    bool a;
    auto r = ids[0].toInt(&a);

    if(!a)
        return nullptr;


    auto c = ids[1].toInt(&a);
    if(!a)
        return nullptr;

    return cellData(r, c);
}

const CellData* TableModel::cellData(int r, int c) const
{
    Q_ASSERT(r<m_data.size() && r >= 0);
    Q_ASSERT(c<m_columns.size() && c >= 0 ) ;

    return &m_data[r][c];
}

void TableModel::save(QJsonObject& json) const
{

    QJsonArray columnJson;

           // TODO
        QJsonArray TableJson;
    for(auto& row : m_data)
    {
        QJsonArray rowJson;
        for(auto& cell : row)
        {
            QJsonObject obj;
            obj[json::valueKey] = cell.value;
            obj[json::formulaKey] = cell.formula;
            rowJson.append(obj);
        }
        TableJson.append(rowJson);
    }

    json[json::dataTableKey] = TableJson;
    json[json::columnsDefinitionKey] = columnJson;
}

void TableModel::saveDataItem(QJsonArray& json)
{
    for(auto& row : m_data)
    {
        QJsonArray rowJson;
        for(auto& cell : row)
        {
            QJsonObject obj;
            obj[json::valueKey] = cell.value;
            obj[json::formulaKey] = cell.formula;
            rowJson.append(obj);
        }
        json.append(rowJson);
    }
}

void TableModel::load(const QJsonObject& json, TreeSheetItem* parent)
{
    auto dataJson = json[json::dataTableKey].toArray();
    auto columns = json[json::columnsDefinitionKey].toArray();

    beginResetModel();
    for(auto const& array : json)
    {
        QList<CellData> rowData;
        QJsonArray row= array.toArray();
        for(auto const& r : row)
        {
            auto cell = r.toObject();
            auto v = cell[json::valueKey].toString();
            auto f = cell[json::formulaKey].toString();
            CellData data{v, f};
            rowData << data;
        }
        m_data.append(rowData);
    }
    endResetModel();
}

void TableModel::loadDataItem(const QJsonArray& json, TreeSheetItem* parent)
{
    beginResetModel();
    m_data.clear();
    for(auto const& array : json)
    {
        QList<CellData> rowData;
        QJsonArray row= array.toArray();
        for(auto const& r : row)
        {
            auto cell = r.toObject();
            auto v = cell[json::valueKey].toString();
            auto f = cell[json::formulaKey].toString();
            CellData data{v, f};
            rowData << data;
        }
        m_data.append(rowData);
    }
    endResetModel();
}

void TableModel::setChildFieldData(const QJsonObject& json)
{
    /* for(auto& line : m_lines)
     {
         auto field= line->getFieldById(json["id"].toString());
         if(field)
         {
             field->loadDataItem(json);
             return;
         }
      }*/
}

const QList<FieldController *> &TableModel::columns() const
{
    return m_columns;
}

void TableModel::setFieldInDictionnary(QHash<QString, QString>& dict, const QString& id, const QString& label) const
{
    if(m_data.isEmpty())
        return;

    Q_ASSERT(m_data.size() % m_columns.size() == 0);
    /*QList<std::tuple<int,int>> sum;
    int r = 0;
    std::transform(std::begin(m_columns), std::end(m_columns), std::back_inserter(sum), [this,&r](const FieldController* ctrl){
        auto v = sumColumn(ctrl->label());
        return std::make_tuple(r++,v);
    }); */

    auto results = sumColumn();

    int i= 1;
    for(auto const& sum : results)
    {
        QString key= QStringLiteral("%1:sumcol%2").arg(id).arg(i);
        dict[key]= QString::number(sum);
        if(!label.isEmpty())
        {
            key= QStringLiteral("%1:sumcol%2").arg(label).arg(i);
            dict[key]= QString::number(sum);
        }
        ++i;
    }
}

void TableModel::removeLine(int index)
{
    if(m_data.isEmpty())
        return;
    if(m_data.size() <= index)
        return;
    if(index < 0)
        return;
    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
}

void TableModel::removeColumn(int index)
{
    auto col = m_columns.at(index);
    beginRemoveColumns(QModelIndex(), index, index);
    m_columns.removeAt(index);
    endRemoveColumns();

    col->deleteLater();
}

bool TableModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
    if(!index.isValid())
        return false;

    auto cell = m_data[index.row()][index.column()];

    auto text = data.toString();

    text.startsWith(constants::formulaMark)?  cell.formula = text: cell.value = text;

    emit dataChanged(index, index, QList<int>{Qt::DisplayRole, Qt::EditRole});

    return true;
}

QList<int> TableModel::sumColumn() const
{
    /*int sum= 0;
    auto it = std::find_if(std::begin(m_columns), std::end(m_columns), [name](const FieldController* ctrl){
        return ctrl->label() == name;
    });
    if(it == std::end(m_columns))
    {
        qWarning(TableFieldCat) << "no columns with that name" << name;
        return 0;
    }

     int step = std::distance(std::begin(m_columns),it);*/

    QList<int> res;
    res.resize(m_columns.size());
    for(auto const& row : m_data)
    {
        int i = 0;
        for(auto const& cell : row)
        {
            res[i]+= cell.value.toInt();
            ++i;
        }
    }
    return res;
}
