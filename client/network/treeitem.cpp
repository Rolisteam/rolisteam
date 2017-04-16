#include "treeitem.h"
#include <QUuid>
TreeItem::TreeItem()
    : m_id(QUuid::createUuid().toString())
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

TreeItem *TreeItem::getParent() const
{
    return m_parent;
}

void TreeItem::setParent(TreeItem *parent)
{
    m_parent = parent;
}

QString TreeItem::getName() const
{
    return m_name;
}

void TreeItem::setName(const QString &name)
{
    m_name = name;
}

int TreeItem::rowInParent()
{
    return m_parent->indexOf(this);
}

QString TreeItem::getId() const
{
    return m_id;
}

void TreeItem::setId(const QString &id)
{
    m_id = id;
}

bool TreeItem::addChildInto(QString id, TreeItem *child)
{
    return false;
}
