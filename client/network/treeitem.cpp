#include "treeitem.h"
#include <QUuid>
TreeItem::TreeItem(QObject* parent)
    : QObject(parent), m_id(QUuid::createUuid().toString()),m_parentItem(nullptr)
{

}

void TreeItem::addChild()
{

}

bool TreeItem::isLeaf() const
{
    return true;
}

int TreeItem::childCount() const
{
    return 0;
}

int TreeItem::addChild(TreeItem *)
{
    return 0;
}

TreeItem *TreeItem::getChildAt(int row)
{
    return nullptr;
}

TreeItem *TreeItem::getParentItem() const
{
    return m_parentItem;
}

void TreeItem::setParentItem(TreeItem *parent)
{
    if(m_parentItem!=parent)
    {
        m_parentItem=parent;
        itemChanged();
    }
}

QString TreeItem::getName() const
{
    return m_name;
}

void TreeItem::setName(const QString &name)
{
    if(m_name!=name)
    {
        m_name = name;
        itemChanged();
    }
}

int TreeItem::rowInParent()
{
    return m_parentItem->indexOf(this);
}

QString TreeItem::getId() const
{
    return m_id;
}

void TreeItem::setId(const QString &id)
{
    if(m_id!=id)
    {
        m_id=id;
        itemChanged();
    }
}

bool TreeItem::addChildInto(QString id, TreeItem *child)
{
    return false;
}

void TreeItem::clear()
{

}

void TreeItem::kick(QString)
{

}
