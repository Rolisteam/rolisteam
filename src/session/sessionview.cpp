#include "sessionview.h"
#include <QMenu>
#include <QContextMenuEvent>

SessionView::SessionView(QWidget *parent) :
    QTreeView(parent)
{
        //setContextMenuPolicy (Qt::CustomContextMenu);
    m_addChapterAction = new QAction(tr("Add Chapter…"),this);
    connect(m_addChapterAction,SIGNAL(triggered()),this,SIGNAL(addChapter()));
}
void SessionView::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu popMenu(this);
    popMenu.addAction(m_addChapterAction);
    popMenu.exec(mapToGlobal(event->pos()));
}

