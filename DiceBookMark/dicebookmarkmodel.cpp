#include "dicebookmarkmodel.h"
#include "userlist/rolisteammimedata.h"

DiceBookMarkModel::DiceBookMarkModel(QObject *parent)
    : QAbstractTableModel(parent)
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
            else
                return tr("Command");
        }
    }
    return QVariant();
}

int DiceBookMarkModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_data.size();
}

int DiceBookMarkModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant DiceBookMarkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if((Qt::DisplayRole == role)||(Qt::EditRole == role))
    {
        std::pair<QString,QString> pair = m_data[index.row()];
        if(index.column() == 0)
        {
            return pair.first;
        }
        else
        {
            return pair.second;
        }
    }
    return QVariant();
}

bool DiceBookMarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value)
    {
        auto& pair = m_data[index.row()];
        if(index.column() == 0)
        {
            pair.first = value.toString();
        }
        else
        {
            pair.second = value.toString();
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

Qt::ItemFlags DiceBookMarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled ;
}

QMimeData* DiceBookMarkModel::mimeData(const QModelIndexList &indexes) const
{
    RolisteamMimeData* mimeData = new RolisteamMimeData();

    for(const QModelIndex & index : indexes)
    {
        if((index.isValid())&&(index.column()==0))
        {
            std::pair<QString,QString> pair = m_data[index.row()];
            mimeData->setAlias(pair.first,pair.second);
        }
    }
    return mimeData;
}

bool DiceBookMarkModel::appendRows()
{
    beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
    m_data.push_back(std::pair<QString,QString>());
    endInsertRows();
    return true;
}


bool DiceBookMarkModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    m_data.erase(m_data.begin()+row);
    endRemoveRows();
    return true;
}
void DiceBookMarkModel::writeSettings(QSettings& settings)
{
    settings.beginWriteArray(QStringLiteral("dicebookmarks"));
    int i = 0;
    for(auto& pair : m_data)
    {
        settings.setArrayIndex(i);
        settings.setValue(QStringLiteral("title"),pair.first);
        settings.setValue(QStringLiteral("command"),pair.second);
        ++i;
    }
    settings.endArray();

}
void DiceBookMarkModel::readSettings(QSettings& settings)
{
    int size = settings.beginReadArray(QStringLiteral("dicebookmarks"));
    beginResetModel();
    m_data.clear();
    for(int i = 0; i<size; ++i)
    {
        settings.setArrayIndex(i);
        QString title =  settings.value(QStringLiteral("title")).toString();
        QString cmd = settings.value(QStringLiteral("command")).toString();
        m_data.push_back(std::pair<QString,QString>(title,cmd));
    }
    settings.endArray();
    endResetModel();
}

