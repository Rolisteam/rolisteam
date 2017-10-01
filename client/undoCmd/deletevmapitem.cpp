#include "deletevmapitem.h"
#include "network/networkmessagewriter.h"
#include <QGraphicsScene>

DeleteVmapItemCommand::DeleteVmapItemCommand(VMap* map, VisualItem* item, bool sendToAll, QUndoCommand *parent)
    : QUndoCommand(parent), m_vmap(map), m_currentItem(item), m_sendToAll(sendToAll)
{
    m_visible = m_currentItem->isVisible();
    m_editable = m_currentItem->isEditable();

    setText(QObject::tr("Delete Item From Map %1").arg(m_vmap->getMapTitle()));
}

void DeleteVmapItemCommand::redo()
{
    m_vmap->removeItem(m_currentItem);
    m_vmap->removeItemFromData(m_currentItem);
    m_vmap->update();

    if(m_sendToAll)
    {
        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::DelItem);
        msg.string8(m_vmap->getId());//id map
        msg.string16(m_currentItem->getId());//id item
        msg.sendAll();
    }
}

void DeleteVmapItemCommand::undo()
{
    if(nullptr != m_currentItem)
    {
        m_vmap->setFocusItem(m_currentItem);
        m_vmap->QGraphicsScene::addItem(m_currentItem);
        m_currentItem->setVisible(m_visible);
        m_currentItem->setEditableItem(m_editable);
        m_vmap->addItemFromData(m_currentItem);
        m_vmap->update();


        NetworkMessageWriter msg(NetMsg::VMapCategory,NetMsg::addItem);
        msg.string8(m_vmap->getId());
        msg.uint8(m_currentItem->type());
        m_currentItem->fillMessage(&msg);
        msg.sendAll();
    }
}
