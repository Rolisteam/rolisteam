#include "item.h"

//////////////////////////////
//Item
/////////////////////////////
Item::Item()
{

}

bool Item::hasChildren()
{
    return false;
}

int Item::getChildrenCount()
{
    return 0;
}

Item *Item::getChildAt(int)
{
    return NULL;
}

QVariant Item::getValue(Item::ColumnId i) const
{
    return QVariant();
}


bool Item::mayHaveChildren()
{
    return false;
}

void Item::appendChild(Item *)
{

}
Item *Item::getParent() const
{
    return m_parent;
}

void Item::setParent(Item *parent)
{
    m_parent = parent;
}
int Item::indexOfChild(Item* itm)
{
    return 0;
}

