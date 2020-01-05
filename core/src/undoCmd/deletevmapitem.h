#ifndef ADDMEDIACONTENEUR_H
#define ADDMEDIACONTENEUR_H

#include <QPointer>
#include <QUndoCommand>

#include "vmap/items/visualitem.h"
#include "vmap/vmap.h"

class VectorialMapController;

class DeleteVmapItemCommand : public QUndoCommand
{
public:
    DeleteVmapItemCommand(VectorialMapController* ctrl, const QList<vmap::VisualItemController*>& list,
                          QUndoCommand* parent= nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<VectorialMapController> m_ctrl;
    QList<vmap::VisualItemController*> m_itemCtrls;
};

#endif // ADDMEDIACONTENEUR_H
