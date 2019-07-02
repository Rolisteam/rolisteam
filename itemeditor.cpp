#include "itemeditor.h"

#include <QMouseEvent>

ItemEditor::ItemEditor(QWidget* parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
}

bool ItemEditor::handle() const
{
    return m_handle;
}

void ItemEditor::setHandle(bool handle)
{
    m_handle= handle;

    if(m_handle)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
    else
    {
        setDragMode(QGraphicsView::NoDrag);
    }
}

void ItemEditor::contextMenuEvent(QContextMenuEvent* event)
{
    emit openContextMenu(event->globalPos());
    QGraphicsView::contextMenuEvent(event);
}
