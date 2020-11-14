#include "itemeditor.h"

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>

ItemEditor::ItemEditor(QWidget* parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    setAlignment((Qt::AlignLeft | Qt::AlignTop));
    // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // setInteractive(true);
}

bool ItemEditor::handle() const
{
    return m_handle;
}

void ItemEditor::setHandle(bool handle)
{
    m_handle= handle;
    qDebug() << "setHandle band" << handle;
}

void ItemEditor::mousePressEvent(QMouseEvent* event)
{
    if(!m_handle)
    {
        setDragMode(QGraphicsView::NoDrag);
        QGraphicsView::mousePressEvent(event);
        return;
    }

    if(event->button() == Qt::RightButton)
        return;

    if(event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem*> list= items(event->pos());

        list.erase(std::remove_if(list.begin(), list.end(),
                                  [](QGraphicsItem* item) {
                                      static QGraphicsPixmapItem pix;
                                      return item->type() == pix.type();
                                  }),
                   list.end());

        if(!list.isEmpty())
        {
            setDragMode(QGraphicsView::NoDrag);
        }
        else
        {
            setDragMode(QGraphicsView::RubberBandDrag);
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void ItemEditor::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if(dragMode() == QGraphicsView::ScrollHandDrag)
    {
        setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void ItemEditor::contextMenuEvent(QContextMenuEvent* event)
{
    emit openContextMenu(event->pos());
    QGraphicsView::contextMenuEvent(event);
}
