#include "sessionitemmodel.h"
#include "cleveruri.h"
#include "session.h"
ResourcesItem::ResourcesItem(RessourcesNode* p,bool leaf)
    : m_data(p),m_isLeaf(leaf)
{
    m_children =new QList<ResourcesItem*>;
}
ResourcesItem* ResourcesItem::getParent()
{
    return m_parent;
}

void ResourcesItem::setParent(ResourcesItem* p)
{
    m_parent = p;
}

void ResourcesItem::setData(RessourcesNode* p)
{
    m_data = p;
}
RessourcesNode* ResourcesItem::getData()
{
    return m_data;
}

bool ResourcesItem::isLeaf()
{
    return m_isLeaf;
}

void ResourcesItem::setLeaf(bool leaf)
{
    m_isLeaf=leaf;

}
int ResourcesItem::childrenCount()
{
    return m_children->size();
}

void ResourcesItem::addChild(ResourcesItem* child)
{
    child->setParent(this);
    m_children->append(child);
}

ResourcesItem* ResourcesItem::child(int row)
{
    if(row < m_children->size())
        return m_children->at(row);

    return NULL;
}
int ResourcesItem::row()
{
    return m_parent->indexOfChild(this);
}
int ResourcesItem::indexOfChild(ResourcesItem* itm)
{
    return m_children->indexOf(itm);
}



SessionItemModel::SessionItemModel()
{
     m_rootItem = new ResourcesItem(NULL,false);
}
QModelIndex SessionItemModel::index( int row, int column, const QModelIndex & parent ) const
{
    if(row<0)
        return QModelIndex();

    ResourcesItem* parentItem = NULL;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<ResourcesItem*>(parent.internalPointer());

    ResourcesItem* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}
QModelIndex SessionItemModel::parent( const QModelIndex & index ) const
{

    if (!index.isValid())
        return QModelIndex();

    ResourcesItem *childItem = static_cast<ResourcesItem*>(index.internalPointer());
    ResourcesItem *parentItem = childItem->getParent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);



}
int SessionItemModel::rowCount(const QModelIndex& index) const
{
    if(index.isValid())
    {
        ResourcesItem* tmp = static_cast<ResourcesItem*>(index.internalPointer());
        return tmp->childrenCount();
    }
    else
    {
        return m_rootItem->childrenCount();
    }
}
int SessionItemModel::columnCount(const QModelIndex&) const
{
    return 3;
}
QVariant SessionItemModel::data(const QModelIndex &index, int role ) const
{
    if(role == Qt::DisplayRole)
    {
        if(!index.isValid())
            return QVariant();
        if(index.column()==0)
        {

             ResourcesItem* tmp = static_cast<ResourcesItem*>(index.internalPointer());
             if(tmp)
             {
               return tmp->getData()->getShortName();
             }
        }
    }
    return QVariant();
}
void SessionItemModel::setSession(Session* s)
{
    /// @todo parse the session and build appropriate ressources items.
    m_session = s;
    foreach(Chapter* tmp,m_session->chapterList())
    {
        ResourcesItem* rt = new ResourcesItem(tmp,false);
        m_rootItem->addChild(rt);

    }
}
