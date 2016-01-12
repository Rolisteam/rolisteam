#include "fieldmodel.h"

#include <QDebug>

//////////////////////////////
//Section
/////////////////////////////


Section::Section()
{

}

bool Section::hasChildren()
{
    return !m_children.isEmpty();
}

int Section::getChildrenCount()
{
    return m_children.size();
}

Item *Section::getChildAt(int i)
{
    if((i<getChildrenCount())&&(i>=0))
    {
        return m_children.at(i);
    }
}

QVariant Section::getValue(Item::ColumnId id) const
{
    if(Item::NAME==id)
        return m_name;
    return QVariant();
}

void Section::setValue(Item::ColumnId id, QVariant var)
{
    if(Item::NAME==id)
        m_name = var.toString();
}

bool Section::mayHaveChildren()
{
    return true;
}

void Section::appendChild(Item* item)
{
    m_children.append(item);
    item->setParent(this);
}
int Section::indexOfChild(Item* itm)
{
    return m_children.indexOf(itm);
}
QString Section::getName() const
{
    return m_name;
}

void Section::setName(const QString &name)
{
    m_name = name;
}

//////////////////////////////
//Column
/////////////////////////////
Column::Column(QString name,  Item::ColumnId pos)
    : m_name(name),m_pos(pos)
{

}

QString Column::getName() const
{
    return m_name;
}

void Column::setName(const QString &name)
{
    m_name = name;
}
Item::ColumnId Column::getPos() const
{
    return m_pos;
}

void Column::setPos(const Item::ColumnId &pos)
{
    m_pos = pos;
}




//////////////////////////////
//FieldModel
/////////////////////////////
FieldModel::FieldModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_colunm << new Column(tr("Field"),Item::NAME)           << new Column(tr("x"),Item::X)             << new Column(tr("y"),Item::Y)
             << new Column(tr("Width"),Item::WIDTH)          << new Column(tr("Height"),Item::HEIGHT)   << new Column(tr("Border"),Item::BORDER)
             << new Column(tr("Text-align"),Item::TEXT_ALIGN)<< new Column(tr("Bg Color"),Item::BGCOLOR)<< new Column(tr("Text Color"),Item::TEXTCOLOR);


    m_rootSection = new Section();
}

QVariant FieldModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if((role == Qt::DisplayRole)||(Qt::EditRole == role))
    {

        Item* item = static_cast<Item*>(index.internalPointer());
        if(NULL!=item)
        {
            return item->getValue(m_colunm[index.column()]->getPos());
        }
    }
    return QVariant();
}

QModelIndex FieldModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row<0)
        return QModelIndex();

    Item* parentItem = NULL;

    // qDebug()<< "Index session " <<row << column << parent;
    if (!parent.isValid())
        parentItem = m_rootSection;
    else
        parentItem = static_cast<Item*>(parent.internalPointer());

    Item* childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex FieldModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    Item *childItem = static_cast<Item*>(child.internalPointer());
    Item *parentItem = childItem->getParent();

    if (parentItem == m_rootSection)
        return QModelIndex();

    Item* grandParent = parentItem->getParent();

    return createIndex(grandParent->indexOfChild(parentItem), 0, parentItem);
}

int FieldModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return m_rootSection->getChildrenCount();

    Item *childItem = static_cast<Item*>(parent.internalPointer());
    if (childItem)
        return childItem->getChildrenCount();
    else
        return 0;

}

int FieldModel::columnCount(const QModelIndex &parent) const
{
    return m_colunm.count();
}

QVariant FieldModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role==Qt::DisplayRole)&&(orientation == Qt::Horizontal))
    {
        return m_colunm[section]->getName();
    }
    else
    {
        return QVariant();
    }
}

bool FieldModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    if(Qt::EditRole == role)
    {
        Item* item = static_cast<Item*>(index.internalPointer());
        if(NULL!=item)
        {
            item->setValue(m_colunm[index.column()]->getPos(),value);
            return true;
        }
    }
    return false;
}

void FieldModel::appendField(Field *f)
{
    beginInsertRows(QModelIndex(),m_rootSection->getChildrenCount(),m_rootSection->getChildrenCount());
    m_rootSection->appendChild(f);
    endInsertRows();
}

Qt::ItemFlags FieldModel::flags ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    Item* childItem = static_cast<Item*>(index.internalPointer());
    if(m_colunm[index.column()]->getPos() == Item::NAME)
    {
       return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else if(!childItem->mayHaveChildren())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    }
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;

}




