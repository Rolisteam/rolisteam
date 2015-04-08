#include "dicealiasmodel.h"

DiceAliasModel::DiceAliasModel()
{
    m_header << tr("Pattern") << tr("Value")<< tr("Regular Expression");
}

DiceAliasModel::~DiceAliasModel()
{

}

QVariant DiceAliasModel::data(const QModelIndex &index, int role) const
{
	if(index.isValid())
	{
        DiceAlias* diceAlias = m_diceAliasList->at(index.row());
        if((Qt::DisplayRole == role)||(Qt::EditRole == role))
		{
            if(NULL!=diceAlias)
            {
                if(index.column()==PATTERN)
                {
                    return diceAlias->getCommand();

                }
                else if(index.column()==VALUE)
                {
                    return diceAlias->getValue();
                }
//                else if(index.column()==METHOD)
//                {
//                    return !diceAlias->isReplace();
//                }
            }
		}
        else if(Qt::CheckStateRole == role)
        {
            if(index.column()==METHOD)
            {
                return diceAlias->isReplace() ? Qt::Unchecked : Qt::Checked ;
            }
        }
        else if(Qt::TextAlignmentRole== role)
        {
            if(index.column()==METHOD)
            {
                return Qt::AlignCenter;
            }
        }
	}
	return QVariant();

}
int DiceAliasModel::rowCount(const QModelIndex &parent) const
{
    return m_diceAliasList->size();
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
void DiceAliasModel::setAliases(QList<DiceAlias*>* lst)
{
    m_diceAliasList = lst;
}
void DiceAliasModel::appendAlias()
{
    addAlias(new DiceAlias(tr("New Alias%1").arg(m_diceAliasList->size()),""));
}
void DiceAliasModel::addAlias(DiceAlias* alias)
{
    beginInsertRows(QModelIndex(),m_diceAliasList->size(),m_diceAliasList->size());
    m_diceAliasList->append(alias);
    endInsertRows();
}
Qt::ItemFlags DiceAliasModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled;
}
bool DiceAliasModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    if(index.isValid())
    {
        DiceAlias* diceAlias = m_diceAliasList->at(index.row());
        if(role==Qt::EditRole)
        {
            switch (index.column())
            {
            case PATTERN:
                diceAlias->setCommand(value.toString());
                result = true;
                break;
            case VALUE:
                diceAlias->setValue(value.toString());
                result = true;
                break;
            }
        }
        else if(role==Qt::CheckStateRole)
        {
            if(value.toInt() == Qt::Checked )
            {
                    diceAlias->setType(DiceAlias::REGEXP);
                    result = true;
            }
            else
            {
                diceAlias->setType(DiceAlias::REPLACE);
                result = true;
            }
        }
    }
    return result;
}
QList<DiceAlias*>* DiceAliasModel::getAliases()
{
    return m_diceAliasList;
}
void DiceAliasModel::deleteAlias(QModelIndex& index)
{
    beginRemoveRows(QModelIndex(),index.row(),index.row());
    m_diceAliasList->removeAt(index.row());
    endRemoveRows();
}
void DiceAliasModel::upAlias(QModelIndex& index)
{
    if(index.row()==0)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),index.row()-1))
    {
        m_diceAliasList->swap(index.row(),index.row()-1);
        endMoveRows();
    }
}

void DiceAliasModel::downAlias(QModelIndex& index)
{
    if(index.row()==m_diceAliasList->size()-1)
        return;

    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),index.row()+2))
    {
        m_diceAliasList->swap(index.row(),index.row()+1);
        endMoveRows();
    }
}

void DiceAliasModel::topAlias(QModelIndex& index)
{
    if(index.row()==0)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),0))
    {
        DiceAlias* dice = m_diceAliasList->takeAt(index.row());
        m_diceAliasList->prepend(dice);
        endMoveRows();
    }
}

void DiceAliasModel::bottomAlias(QModelIndex& index)
{
    if(index.row()==m_diceAliasList->size()-1)
        return;
    if(beginMoveRows(QModelIndex(),index.row(),index.row(),QModelIndex(),m_diceAliasList->size()))
    {
        DiceAlias* dice = m_diceAliasList->takeAt(index.row());
        m_diceAliasList->append(dice);
        endMoveRows();
    }
}
