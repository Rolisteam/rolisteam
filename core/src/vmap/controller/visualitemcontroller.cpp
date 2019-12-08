#include "visualitemcontroller.h"

#include <QUuid>

#include "controller/view_controller/vectorialmapcontroller.h"

VisualItemController::VisualItemController(VectorialMapController* ctrl, QObject* parent)
    : QObject(parent), m_ctrl(ctrl), m_uuid(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    connect(m_ctrl, &VectorialMapController::layerChanged, this, &VisualItemController::selectableChanged);
    m_layer= m_ctrl->layer();
}

VisualItemController::~VisualItemController() {}

bool VisualItemController::selected() const
{
    return m_selected;
}

bool VisualItemController::editable() const
{
    return (selectable() & m_editable);
}

bool VisualItemController::selectable() const
{
    return (m_ctrl->layer() == m_layer);
}

bool VisualItemController::visible() const
{
    return m_visible;
}

qreal VisualItemController::opacity() const
{
    return m_visible;
}

Core::Layer VisualItemController::layer() const
{
    return m_layer;
}

QPointF VisualItemController::pos() const
{
    return m_pos;
}

QString VisualItemController::uuid() const
{
    return m_uuid;
}

qreal VisualItemController::rotation() const
{
    return m_rotation;
}

int VisualItemController::gridSize() const
{
    return m_ctrl->gridSize();
}

QString VisualItemController::getLayerText(Core::Layer layer) const
{
    return m_ctrl->getLayerToText(layer);
}

void VisualItemController::setSelected(bool b)
{
    if(b == m_selected)
        return;
    m_selected= b;
    emit selectedChanged();
}

void VisualItemController::setUuid(QString uuid)
{
    if(uuid == m_uuid)
        return;
    m_uuid= uuid;
    emit uuidChanged();
}

void VisualItemController::setRotation(qreal rota)
{
    if(qFuzzyCompare(rota, m_rotation))
        return;
    m_rotation= rota;
    emit rotationChanged();
}

void VisualItemController::setEditable(bool b)
{
    if(b == m_editable)
        return;
    m_editable= b;
    emit editableChanged();
}

void VisualItemController::setVisible(bool b)
{
    if(b == m_selected)
        return;
    m_selected= b;
    emit selectedChanged();
}

void VisualItemController::setOpacity(qreal b)
{
    if(qFuzzyCompare(b, m_opacity))
        return;
    m_opacity= b;
    emit opacityChanged();
}

void VisualItemController::setLayer(Core::Layer layer)
{
    if(layer == m_layer)
        return;
    m_layer= layer;
    emit layerChanged();
    emit selectableChanged();
}

void VisualItemController::setPos(QPointF pos)
{
    if(m_pos == pos)
        return;
    m_pos= pos;
    emit posChanged();
}
