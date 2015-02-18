#include "sessionview.h"
#include <QMenu>
#include <QContextMenuEvent>

SessionView::SessionView(QWidget *parent) :
    QTreeView(parent)
{
        //setContextMenuPolicy (Qt::CustomContextMenu);
    m_addChapterAction = new QAction(tr("Add Chapter…"),this);
    connect(m_addChapterAction,SIGNAL(triggered()),this,SLOT(onAddChapter()));

    m_removeAction = new QAction(tr("Remove items"),this);
    connect(m_addChapterAction,SIGNAL(triggered()),this,SIGNAL(removeSelection()));
}
void SessionView::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu popMenu(this);
    popMenu.addAction(m_addChapterAction);
    popMenu.addAction(m_removeAction);
    m_pointClick=event->pos();
    popMenu.exec(QCursor::pos());
}
void SessionView::mouseDoubleClickEvent( QMouseEvent * event)
{

    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
        return;

    emit onDoubleClick(index);



}

void SessionView::onAddChapter()
{
    QModelIndex index = indexAt(m_pointClick);

    emit addChapter(index);
}
