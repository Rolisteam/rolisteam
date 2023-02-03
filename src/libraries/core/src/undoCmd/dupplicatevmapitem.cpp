#include "undoCmd/dupplicatevmapitem.h"

#include "model/vmapitemmodel.h"
#include "utils/logcategories.h"
#include "worker/vectorialmapmessagehelper.h"
#include <QList>
#include <QUuid>

DupplicateVMapItem::DupplicateVMapItem(const QList<vmap::VisualItemController*>& vitem, VectorialMapController* ctrl)
    : m_vitem{vitem}, m_ctrl{ctrl}
{
    m_itemData= VectorialMapMessageHelper::itemsToHash(vitem);
    auto keys= m_itemData.keys();
    for(auto& k : keys)
    {
        auto p= m_itemData[k];
        auto map= p.toMap().toStdMap();
        auto id= QUuid::createUuid().toString(QUuid::WithoutBraces);
        m_ids << id;
        map[Core::vmapkeys::KEY_UUID]= id;
        m_itemData.insert(k, QVariant::fromValue(map));
    }
}

void DupplicateVMapItem::redo()
{
    if(m_ctrl.isNull())
        return;

    VectorialMapMessageHelper::fetchModelFromMap(m_itemData, m_ctrl);
}

void DupplicateVMapItem::undo()
{
    if(m_ctrl.isNull())
        return;

    auto model= m_ctrl->model();

    if(!model)
        return;

    if(!model->removeItemController(m_ids))
    {
        qCWarning(logCategory::map)
            << QString("Failed to remove item - %1").arg(QStringList{m_ids.begin(), m_ids.end()}.join(", "));
    }
}
