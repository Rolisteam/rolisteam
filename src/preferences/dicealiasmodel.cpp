#include "dicealiasmodel.h"

DiceAliasModel::DiceAliasModel()
{
    m_header << tr("Pattern") << tr("Value");
}

DiceAliasModel::~DiceAliasModel()
{

}

QVariant DiceAliasModel::data(const QModelIndex &index, int role) const
{

}
int DiceAliasModel::rowCount(const QModelIndex &parent) const
{
    return m_DiceAliasMap.size();
}
int DiceAliasModel::columnCount(const QModelIndex &parent) const
{
    return m_header.size();
}
QVariant DiceAliasModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::DisplayRole == role)
    {
        if(orientation==Qt::Horizontal)
        {
            return m_header.at(section);
        }
    }
    return QVariant();
}
