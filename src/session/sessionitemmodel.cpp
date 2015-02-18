#include "sessionitemmodel.h"
#include "cleveruri.h"
#include "session.h"
#include <QDebug>

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
void ResourcesItem::clean()
{
    m_children->clear();
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
   // qDebug() << "child row="<< row << m_children->size() << m_children->at(row);
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

    qDebug()<< "Index session " <<row << column << parent;
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
    qDebug() << index;
    if(index.isValid())
    {
        ResourcesItem* tmp = static_cast<ResourcesItem*>(index.internalPointer());
        qDebug() << "rowCount tmp"<< tmp->childrenCount();
        return tmp->childrenCount();
    }
    else
    {
        qDebug() << "rowCount root"<<  m_rootItem->childrenCount() << m_session->chapterList().size() << m_session->childrenCount();
        return (m_session->childrenCount());//->childrenCount();
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
               RessourcesNode* t =  tmp->getData();
               qDebug() << tmp;
               return t->getShortName();
             }
        }
    }
    return QVariant();
}
void SessionItemModel::setSession(Session* s)
{
    /// @todo parse the session and build appropriate ressources items.
    m_session = s;
    m_rootItem->clean();
    for(int i =0;i<m_session->chapterList().size();i++)
    {
        Chapter& tmp = m_session->chapterList()[i];
        ResourcesItem* rt = new ResourcesItem(&tmp,false);
        m_rootItem->addChild(rt);
        populateChapter(tmp,rt);
    }
    for(int i = 0;i<m_session->getUnclassedList().size();i++)
    {
        CleverURI& tmp2 = m_session->getUnclassedList()[i];
        ResourcesItem* rt = new ResourcesItem(&tmp2,true);
        m_rootItem->addChild(rt);
    }
}

Chapter* SessionItemModel::addChapter(QString& name)
{
    beginInsertRows(QModelIndex(),m_session->chapterList().size(),m_session->chapterList().size());
    Chapter* t = m_session->addChapter(name);
    endInsertRows();
    return t;
}
/*void SessionItemModel::refresh()
{
  * foreach(Chapter tmp,m_session->chapterList())
    {
        m_rootItemm_parent
    }*
}*/
CleverURI* SessionItemModel::addRessources(QString& urifile, CleverURI::ContentType& type,QModelIndex& parent)
{
     ResourcesItem* parentItem=NULL;
    if(!parent.isValid())
        parentItem=m_rootItem;
    else
    {
        parentItem = static_cast<ResourcesItem*>(parent.internalPointer());
    }
    beginInsertRows(QModelIndex(),parentItem->childrenCount(),parentItem->childrenCount());

    if(parentItem->isLeaf())
    {
        parentItem=parentItem->getParent();//leaf's parent is not a leaf indeed
    }


    Chapter* chap=dynamic_cast<Chapter*>(parentItem->getData());// NULL when it is not a chapter.

    CleverURI* tmp = m_session->addRessource(urifile,type,chap);
    parentItem->addChild(new ResourcesItem(tmp,true));
    endInsertRows();
    return tmp;
}
void SessionItemModel::populateChapter(Chapter& t,ResourcesItem* parentItem)
{

    for(int i =0;i<t.getChapterList().size();i++)
    {
        Chapter& tmp = t.getChapterList()[i];
        ResourcesItem* rt = new ResourcesItem(&tmp,false);
        parentItem->addChild(rt);
        populateChapter(tmp,rt);
    }
    for(int i = 0;i<t.getResourceList().size();i++)
    {
        CleverURI& tmp2 = t.getResourceList()[i];
        ResourcesItem* rt = new ResourcesItem(&tmp2,true);
        parentItem->addChild(rt);
    }
}
