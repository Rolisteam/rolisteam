#include "dicebookmarkmodel.h"
#include "userlist/rolisteammimedata.h"

DiceBookMarkModel::DiceBookMarkModel(std::vector<DiceShortCut>& data, QObject* parent)
    : QAbstractTableModel(parent), m_data(data)
{
}

QVariant DiceBookMarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(orientation == Qt::Horizontal)
        {
            if(section == 0)
                return tr("Name");
            else if(section == 1)
                return tr("Command");
            else if(section == 2)
                return tr("Alias");
        }
    }
    return QVariant();
}

int DiceBookMarkModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_data.size();
}

int DiceBookMarkModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return 3;
}

QVariant DiceBookMarkModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if((Qt::DisplayRole == role) || (Qt::EditRole == role))
    {
        const auto pair= m_data[index.row()];
        if(index.column() == 0)
        {
            return pair.text();
        }
        else if(index.column() == 1)
        {
            return pair.command();
        }
        else
        {
            return pair.alias();
        }
    }
    return QVariant();
}

bool DiceBookMarkModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(data(index, role) != value)
    {
        auto& pair= m_data[index.row()];
        if(index.column() == 0)
        {
            pair.setText(value.toString());
        }
        else if(index.column() == 1)
        {
            pair.setCommand(value.toString());
        }
        else
        {
            pair.setAlias(value.toBool());
        }
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

QStringList DiceBookMarkModel::mimeTypes() const
{
    QStringList types;
    types << "rolisteam/dice-command";
    return types;
}

Qt::ItemFlags DiceBookMarkModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
}

QMimeData* DiceBookMarkModel::mimeData(const QModelIndexList& indexes) const
{
    RolisteamMimeData* mimeData= new RolisteamMimeData();

    for(const QModelIndex& index : indexes)
    {
        if((index.isValid()) && (index.column() == 0))
        {
            DiceShortCut pair= m_data[index.row()];
            mimeData->setAlias(pair.text(), pair.command(), pair.alias());
        }
    }
    return mimeData;
}

bool DiceBookMarkModel::appendRows()
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(DiceShortCut());
    endInsertRows();
    return true;
}

bool DiceBookMarkModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    m_data.erase(m_data.begin() + row);
    endRemoveRows();
    return true;
}
void DiceBookMarkModel::writeSettings(QSettings& settings)
{
    settings.beginWriteArray(QStringLiteral("dicebookmarks"));
    int i= 0;
    for(auto& pair : m_data)
    {
        settings.setArrayIndex(i);
        settings.setValue(QStringLiteral("title"), pair.text());
        settings.setValue(QStringLiteral("command"), pair.command());
        settings.setValue(QStringLiteral("alias"), pair.alias());
        ++i;
    }
    settings.endArray();
}
void DiceBookMarkModel::readSettings(QSettings& settings)
{
    int size= settings.beginReadArray(QStringLiteral("dicebookmarks"));
    beginResetModel();
    m_data.clear();
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        DiceShortCut cut;
        cut.setText(settings.value(QStringLiteral("title")).toString());
        cut.setCommand(settings.value(QStringLiteral("command")).toString());
        cut.setAlias(settings.value(QStringLiteral("alias"), true).toBool());
        m_data.push_back(cut);
    }
    settings.endArray();
    endResetModel();
}
