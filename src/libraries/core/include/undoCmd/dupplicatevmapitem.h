#ifndef DUPPLICATEVMAPITEM_H
#define DUPPLICATEVMAPITEM_H

#include "controller/item_controllers/visualitemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"
#include <QPointer>
#include <QUndoCommand>

class DupplicateVMapItem : public QUndoCommand
{
public:
    DupplicateVMapItem(const QList<vmap::VisualItemController*>& vitem, VectorialMapController* model);

    void redo();
    void undo();

private:
    QList<vmap::VisualItemController*> m_vitem;
    QPointer<VectorialMapController> m_ctrl;
    QHash<QString, QVariant> m_itemData;
    QSet<QString> m_ids;
};

#endif // DUPPLICATEVMAPITEM_H
