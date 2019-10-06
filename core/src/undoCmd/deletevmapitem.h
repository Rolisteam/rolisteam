#ifndef ADDMEDIACONTENEUR_H
#define ADDMEDIACONTENEUR_H

#include <QUndoCommand>

#include "vmap/items/visualitem.h"
#include "vmap/vmap.h"

class DeleteVmapItemCommand : public QUndoCommand
{
public:
    DeleteVmapItemCommand(VMap* map, VisualItem* item, bool sendToAll, QUndoCommand* parent= nullptr);

    void redo() override;
    void undo() override;

private:
    VMap* m_vmap= nullptr;
    VisualItem* m_currentItem= nullptr;
    bool m_sendToAll;
    bool m_visible;
};

#endif // ADDMEDIACONTENEUR_H
