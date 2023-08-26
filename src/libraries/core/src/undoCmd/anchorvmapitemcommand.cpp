#include "undoCmd/anchorvmapitemcommand.h"
#include "controller/item_controllers/visualitemcontroller.h"


AnchorVMapItemCommand::AnchorVMapItemCommand(vmap::VisualItemController *child, vmap::VisualItemController *newParent)
    : m_child(child)
{
    if(m_child)
        m_previousParent = m_child->parentUuid();

    if(newParent)
        m_parentId = newParent->uuid();
}

void AnchorVMapItemCommand::redo()
{
    if(!m_child)
        return;

    m_child->setParentUuid(m_parentId);
}

void AnchorVMapItemCommand::undo()
{
    if(!m_child)
        return;

    m_child->setParentUuid(m_previousParent);
}
