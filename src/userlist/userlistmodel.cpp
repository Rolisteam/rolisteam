#include "userlistmodel.h"

#include <QDebug>


PersonItem::PersonItem(Person* p,bool isLeaf)
    : m_data(p),m_isLeaf(isLeaf)
{
    m_children = new  QList<PersonItem*>;
}

void PersonItem::setPerson(Person* p)
{
    m_data= p;
}

Person* PersonItem::getPerson()
{
    return m_data;
}

bool PersonItem::isLeaf()
{
    return m_isLeaf;
}

void PersonItem::setLeaf(bool leaf)
{
    m_isLeaf = leaf;
}

PersonItem* PersonItem::getParent()
{
    return m_parent;
}

void PersonItem::setParent(PersonItem* p)
{
    m_parent = p;
}
int PersonItem::indexOfPerson(Person* itm)
{
    int i = 0;
    foreach(PersonItem* p, *m_children)
    {

        if(itm == p->getPerson())
            return i;
        i++;
    }
    return -1;
}

void PersonItem::addChild(PersonItem* child)
{
    m_children->append(child);
    child->setParent(this);
}

PersonItem* PersonItem::child(int row)
{
    return m_children->at(row);
}

int PersonItem::row()
{
    return m_parent->indexOfChild(this);
}

int PersonItem::indexOfChild(PersonItem* itm)
{
    return m_children->indexOf(itm);
}
int PersonItem::childrenCount()
{
    return m_children->size();
}

UserListModel::UserListModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_root = new PersonItem(NULL,false);
}
QModelIndex UserListModel::index ( int row, int column, const QModelIndex & parent  ) const
{
    if(row<0)
        return QModelIndex();

    PersonItem* parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<PersonItem*>(parent.internalPointer());

    PersonItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
bool UserListModel::isPlayer(const QModelIndex & index)
{
    if(!index.isValid())
        return false;
    if(!index.parent().isValid())
        return true;

    return false;

}

QModelIndex UserListModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();

    PersonItem *childItem = static_cast<PersonItem*>(index.internalPointer());
    PersonItem *parentItem = childItem->getParent();

    if (parentItem == m_root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
int UserListModel::rowCount ( const QModelIndex & parent  ) const
{
        PersonItem* tmp = static_cast<PersonItem*>(parent.internalPointer());
        if(tmp!=NULL)
            return tmp->childrenCount();
        else
            return m_root->childrenCount();
}
int UserListModel::columnCount ( const QModelIndex & parent  ) const
{
    return 1;
}
QVariant UserListModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();

    if((role == Qt::DisplayRole)||(role == Qt::DecorationRole))
    {
        if(index.column()==0)
        {
            PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
            if(childItem)
            {

                if(role == Qt::DisplayRole)
                {
                    return childItem->getPerson()->getName();
                }
                if(role == Qt::DecorationRole)
                {
                    //return QColor(Qt::blue);
                    //qDebug() << childItem->getPerson()->getColor();
                    return childItem->getPerson()->getColor();
                }
            }
        }
    }

    return QVariant();
}
void UserListModel::addPlayer(Person* p)
{
    beginInsertRows(QModelIndex(),m_root->childrenCount(),m_root->childrenCount());
    m_root->addChild(new PersonItem(p,false));
    endInsertRows();
}
void UserListModel::addCharacter(Person* p,Person* parent)
{
    int row= m_root->indexOfPerson(parent);
    if(row>-1)
    {
        QModelIndex parentIndex = index(row,0);
        PersonItem* childItem = static_cast<PersonItem*>(parentIndex.internalPointer());

        beginInsertRows(parentIndex,childItem->childrenCount(),childItem->childrenCount());
        childItem->addChild(new PersonItem(p,true));
        endInsertRows();
    }
}
void UserListModel::setLocalPlayer(Person* p)
{
    m_player = p;
}

bool UserListModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    if(Qt::EditRole==role)
    {
            PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
            childItem->getPerson()->setName(value.toString());
            return true;
    }
    return false;
}
Qt::ItemFlags UserListModel::flags ( const QModelIndex & index )  const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    PersonItem* childItem = static_cast<PersonItem*>(index.internalPointer());
    if((childItem->getPerson()==m_player)||(childItem->getParent()->getPerson() ==m_player))
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
