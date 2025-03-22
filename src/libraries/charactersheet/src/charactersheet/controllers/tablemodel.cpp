#include "charactersheet/controllers/tablemodel.h"

#include <QJsonArray>

namespace constants
{
constexpr auto idSeparator{';'};
constexpr auto idDataCount{2};
constexpr auto formulaMark{"="};

} // namespace constants
namespace json
{
constexpr auto valueKey{"value"};
constexpr auto formulaKey{"formula"};
constexpr auto dataTableKey{"dataTable"};
constexpr auto columnsDefinitionKey{"columns"};
} // namespace json

CellData::CellData(QObject* parent) : TreeSheetItem(TreeSheetItem::CellValue, parent) {}

QString CellData::value() const
{
    return m_value;
}

void CellData::setValue(const QString& newValue)
{
    if(m_value == newValue)
        return;
    m_value= newValue;
    emit valueChanged();
}

QString CellData::formula() const
{
    return m_formula;
}

void CellData::setFormula(const QString& newFormula)
{
    if(m_formula == newFormula)
        return;
    m_formula= newFormula;
    emit formulaChanged();
}

void CellData::setFieldInDictionnary(QHash<QString, QString>& dict) const
{
    dict.insert(QString("%1"), m_value);
    // dict.insert(QString("%1.formula"), m_formula);
}

void CellData::setOrig(TreeSheetItem* origin)
{
    Q_UNUSED(origin)
}

QVariant CellData::valueFrom(TreeSheetItem::ColumnId col, int role) const
{
    QVariant res;

    auto const formula= m_formula.isEmpty() ? m_value : m_formula;
    switch(col)
    {
    case TreeSheetItem::ID:
        res= id();
        break;
    case TreeSheetItem::LABEL:
        res= id();
        break;
    case TreeSheetItem::VALUE:
        res= role == Qt::EditRole ? formula : m_value;
        break;
    default:
        break;
    }
    return res;
}

void CellData::setValueFrom(TreeSheetItem::ColumnId col, const QVariant& data)
{
    if(col != TreeSheetItem::VALUE && col != TreeSheetItem::FORMULA)
        return;
    auto text= data.toString();

    if(text.startsWith("=") || col == TreeSheetItem::FORMULA)
        setFormula(text);
    else
        setValue(text);
}

TableModel::TableModel(TreeSheetItem* item) : m_parent(item)
{
    // constructor
}

int TableModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return m_data.size();
}

int TableModel::columnCount(const QModelIndex& parent) const
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
        res= item->value();
        break;
    case Qt::EditRole:
    case FormulaRole:
        res= item->formula().isEmpty() ? item->value() : item->formula();
        break;
    case IdRole:
        res= item->id();
        break;
    }

    return res;
}

QHash<int, QByteArray> TableModel::roleNames() const
{
    auto res= QAbstractTableModel::roleNames();
    res.insert(ValueRole, "value");
    res.insert(FormulaRole, "formula");
    res.insert(IdRole, "cellId");
    return res;
}
void TableModel::addRows(int rCount)
{
    QList<CellData*> cell;
    std::for_each(std::begin(m_columns), std::end(m_columns),
                  [this, &cell](FieldController*) { cell.append(addCellData()); });

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + rCount);
    for(int i= 0; i < rCount; ++i)
        addRowData(cell);
    endInsertRows();
}
void TableModel::addRow()
{
    QList<CellData*> cell;
    std::for_each(std::begin(m_columns), std::end(m_columns),
                  [this, &cell](FieldController*) { cell.append(addCellData()); });

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    addRowData(cell);
    endInsertRows();
}

void TableModel::addColumn(FieldController* field)
{
    beginInsertColumns(QModelIndex(), m_columns.size(), m_columns.size());
    m_columns.append(field);
    for(auto& r : m_data)
    {
        r.append(addCellData());
    }
    if(m_data.isEmpty())
        addRow();
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

FieldController* TableModel::colField(int col) const
{
    Q_ASSERT(col < m_columns.size() && col >= 0);
    return m_columns[col];
}
FieldController* TableModel::colFieldFromId(const QString& id) const
{
    for(auto field : m_columns)
    {
        if(field->id() == id)
            return field;
    }
    return nullptr;
}

QModelIndex TableModel::indexFromCell(TreeSheetItem* data) const
{
    auto cell= dynamic_cast<CellData*>(data);
    int r{0}, c{0};
    for(const auto& row : m_data)
    {
        c= row.indexOf(cell);
        if(c >= 0)
            return index(r, c, QModelIndex());
        ++r;
    }
    return QModelIndex();
}

int TableModel::indexOf(TreeSheetItem* item) const
{

    int res= m_columns.indexOf(item);

    if(res >= 0)
        return res;

    //    QList<QList<CellData*>> m_data;
    res= m_columns.size();
    for(auto line : m_data)
    {
        auto idx= line.indexOf(item);

        if(idx >= 0)
        {
            res+= idx;
            break;
        }
        else
            res+= line.size();
    }

    if(res < (m_columns.size() + m_data.size() * m_columns.size()))
        return res;
    else
        return -1;
}

TreeSheetItem* TableModel::childDataFromId(const QString& id) const
{
    for(auto const& col : m_columns)
    {
        if(col->id() == id)
            return col;
    }

    for(auto const& r : m_data)
    {
        for(auto c : r)
        {
            if(c->id() == id)
                return c;
        }
    }
    return nullptr;
}

CellData* TableModel::cellData(int r, int c) const
{
    Q_ASSERT(r < m_data.size() && r >= 0);
    Q_ASSERT(c < m_columns.size() && c >= 0);

    return m_data[r][c];
}

void TableModel::makeSpace(int row, int cols)
{
    while(m_columns.count() < cols)
    {
        qWarning() << "Wrong number of columns";
        auto field= new FieldController(TreeSheetItem::FieldItem, true);
        beginInsertColumns(QModelIndex(), m_columns.size(), m_columns.size());
        m_columns.append(field);
        endInsertColumns();
    }

    while(m_data.count() < row)
    {
        addRowData(QList<CellData*>());
    }

    for(auto& rowList : m_data)
    {
        while(rowList.size() < cols)
        {
            rowList.append(addCellData());
        }
    }
}

CellData* TableModel::addCellData()
{
    auto data= new CellData(this);
    data->setId(QString("cell_%1").arg(++m_count));
    data->setParent(m_parent);
    return data;
}

void TableModel::save(QJsonObject& json) const
{
    QJsonArray columnJson;
    for(auto& col : m_columns)
    {
        QJsonObject column;
        col->save(column);
        columnJson.append(column);
    }

    QJsonArray TableJson;
    for(auto& row : m_data)
    {
        QJsonArray rowJson;
        for(auto& cell : row)
        {
            QJsonObject obj;
            obj[json::valueKey]= cell->value();
            obj[json::formulaKey]= cell->formula();
            rowJson.append(obj);
        }
        TableJson.append(rowJson);
    }

    json[json::dataTableKey]= TableJson;
    json[json::columnsDefinitionKey]= columnJson;
}

void TableModel::saveDataItem(QJsonArray& json)
{
    for(auto& row : m_data)
    {
        QJsonArray rowJson;
        for(auto& cell : row)
        {
            QJsonObject obj;
            obj[json::valueKey]= cell->value();
            obj[json::formulaKey]= cell->formula();
            rowJson.append(obj);
        }
        json.append(rowJson);
    }
}

void TableModel::load(const QJsonObject& json, TreeSheetItem* parent)
{
    m_count= 0;
    auto dataJson= json[json::dataTableKey].toArray();
    auto columns= json[json::columnsDefinitionKey].toArray();

    beginResetModel();
    m_columns.clear();
    for(auto const& col : std::as_const(columns))
    {
        auto field= new FieldController(TreeSheetItem::FieldItem, true);
        field->setParent(parent);
        field->load(col.toObject());
        m_columns.append(field);
    }

    QList<QList<CellData*>> newData;
    for(auto const& array : std::as_const(dataJson))
    {
        QList<CellData*> rowData;
        QJsonArray row= array.toArray();
        for(auto const& r : std::as_const(row))
        {
            auto cell= r.toObject();
            auto v= cell[json::valueKey].toString();
            auto f= cell[json::formulaKey].toString();
            auto data= addCellData();
            data->setValue(v);
            data->setFormula(f);
            rowData << data;
        }
        newData << rowData;
        // addRowData(rowData);
    }

    auto inBound= [newData](int i, int j)
    {
        if(newData.size() >= 0 && i < newData.size())
        {
            auto line= newData[i];
            return (line.size() >= 0 && j < line.size());
        }
        return false;
    };

    for(int i= 0; i < m_data.size(); ++i)
    {
        auto line= m_data[i];
        for(int j= 0; j < line.size(); ++j)
        {
            auto cell= line[j];
            if(!inBound(i, j))
                continue;

            if(!cell->value().isEmpty())
                newData[i][j]->setValue(cell->value());

            if(!cell->formula().isEmpty())
                newData[i][j]->setFormula(cell->formula());
        }
    }

    m_data= newData;

    endResetModel();
}

void TableModel::loadDataItem(const QJsonArray& json, TreeSheetItem* parent)
{
    Q_UNUSED(parent)
    beginResetModel();
    m_data.clear();
    QList<QList<CellData*>> newData;
    for(auto const& array : json)
    {
        QList<CellData*> rowData;
        QJsonArray row= array.toArray();
        for(auto const& r : std::as_const(row))
        {
            auto cell= r.toObject();
            auto v= cell[json::valueKey].toString();
            auto f= cell[json::formulaKey].toString();
            auto data= addCellData();
            data->setValue(v);
            data->setFormula(f);
            rowData << data;
        }
        addRowData(rowData);
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

const QList<FieldController*>& TableModel::columns() const
{
    return m_columns;
}

void TableModel::setFieldInDictionnary(QHash<QString, QString>& dict, const QString& id, const QString& label) const
{
    if(m_data.isEmpty())
        return;

    // Q_ASSERT(m_data.size() % m_columns.size() == 0);

    auto results= sumColumn();

    for(const auto& line : m_data)
    {
        for(const CellData* cell : line)
        {
            auto realId= QString("%1.%2").arg(id, cell->id());
            qDebug() << "realId" << realId;
            dict[realId]= cell->value();
        }
    }

    int i= 1;
    for(auto const& sum : std::as_const(results))
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
    auto col= m_columns.at(index);
    beginRemoveColumns(QModelIndex(), index, index);
    m_columns.removeAt(index);
    endRemoveColumns();

    col->deleteLater();
}

bool TableModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
    Q_UNUSED(role)
    if(!index.isValid())
        return false;

    auto& cell= m_data[index.row()][index.column()];

    auto text= data.toString();

    text.startsWith(constants::formulaMark) ? cell->setFormula(text) : cell->setValue(text);

    emit dataChanged(index, index, QList<int>{Qt::DisplayRole, Qt::EditRole});

    return true;
}

QList<int> TableModel::sumColumn() const
{
    QList<int> res;
    res.resize(m_columns.size());
    for(auto const& row : m_data)
    {
        int i= 0;
        for(auto const& cell : row)
        {
            res[i]+= cell->value().toInt();
            ++i;
        }
    }
    return res;
}

void TableModel::addRowData(QList<CellData*> cell)
{

    m_data.append(cell);
    for(auto data : std::as_const(cell))
    {
        setupCell(data);
    }
}

void TableModel::setupCell(CellData* cell)
{
    auto v= [this]()
    {
        auto cellData= qobject_cast<CellData*>(sender());
        if(!cellData)
            return;
        auto idx= indexFromCell(cellData);
        emit dataChanged(idx, idx, {Qt::DisplayRole, Qt::EditRole, ValueRole, FormulaRole});
    };

    disconnect(cell, nullptr, this, nullptr);
    connect(cell, &CellData::formulaChanged, this, v);
    connect(cell, &CellData::valueChanged, this, v);
}
