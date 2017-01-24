#include "channelmodel.h"

ChannelModel::ChannelModel()
{

}

TreeItem *TreeItem::parent() const
{
    return m_parent;
}

void TreeItem::setParent(TreeItem *parent)
{
    m_parent = parent;
}
