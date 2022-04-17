#ifndef ADDMEDIACONTENEUR_H
#define ADDMEDIACONTENEUR_H

#include <QPointer>
#include <QUndoCommand>

#include <core_global.h>
class VectorialMapController;
namespace vmap
{
class VisualItemController;
}
class CORE_EXPORT DeleteVmapItemCommand : public QUndoCommand
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
