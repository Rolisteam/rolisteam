#include "sessionview.h"
#include <QMenu>
#include <QDebug>

#include <QHeaderView>
#include <QContextMenuEvent>
#include "data/chapter.h"
#include "session/sessionitemmodel.h"

SessionView::SessionView(QWidget *parent) :
    QTreeView(parent)
{
    //setContextMenuPolicy (Qt::CustomContextMenu);
    m_addChapterAction = new QAction(tr("Add Chapter…"),this);
    connect(m_addChapterAction,SIGNAL(triggered()),this,SLOT(onAddChapter()));
    
    m_removeAction = new QAction(tr("Remove items"),this);
    connect(m_removeAction,SIGNAL(triggered()),this,SIGNAL(removeSelection()));
    
    m_defineAsCurrent = new QAction(tr("Current Chapter"),this);
    connect(m_defineAsCurrent,SIGNAL(triggered()),this,SIGNAL(defineCurrentChapter()));

    m_loadingModeColumn = new QAction(tr("Loading Mode"),this);
    m_loadingModeColumn->setCheckable(true);
    connect(m_loadingModeColumn,&QAction::toggled,[=](){
        setColumnHidden(1,!m_loadingModeColumn->isChecked());
    });
    m_loadingModeColumn->setChecked(false);

    m_displayedColumn= new QAction(tr("Displayed Status"),this);
    m_displayedColumn->setCheckable(true);
    connect(m_displayedColumn,&QAction::toggled,[=](){
        setColumnHidden(2,!m_displayedColumn->isChecked());
    });
    m_displayedColumn->setChecked(false);

    m_pathColumn= new QAction(tr("Path"),this);
    m_pathColumn->setCheckable(true);
    connect(m_pathColumn,&QAction::toggled,[=](){
        setColumnHidden(3,!m_pathColumn->isChecked());
    });
    m_pathColumn->setChecked(false);

    m_rename = new QAction(tr("Rename"),this);
    m_rename->setShortcut(QKeySequence(Qt::Key_F2));
    connect(m_rename,&QAction::triggered,[=](){
        edit(currentIndex());
    });

    //setEditTriggers(QAbstractItemView::AnyKeyPressed);
}
void SessionView::onDefineChapter()
{
    emit defineCurrentChapter();
}
void SessionView::startDrag(Qt::DropActions supportedActions)
{
    QDrag* drag = new QDrag(this);
    QModelIndexList indexes = selectionModel()->selectedRows();

    QMimeData* mimeData = model()->mimeData(indexes);
    if(NULL==mimeData)
        return;

    drag->setMimeData(mimeData);
    Qt::DropAction defaultDropAction = Qt::MoveAction;

    drag->exec(supportedActions, defaultDropAction);
}
void SessionView::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu popMenu(this);
    popMenu.addAction(m_rename);
    popMenu.addSeparator();
    popMenu.addAction(m_addChapterAction);
    popMenu.addAction(m_removeAction);
    popMenu.addSeparator();
    popMenu.addSection(tr("Show/hide Column"));
    popMenu.addAction(m_loadingModeColumn);
    popMenu.addAction(m_displayedColumn);
    popMenu.addAction(m_pathColumn);
    m_pointClick=event->pos();
    popMenu.exec(QCursor::pos());
}
void SessionView::mouseDoubleClickEvent( QMouseEvent * event)
{
    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
        return;
    
    emit onDoubleClick(index);
    QTreeView::mouseDoubleClickEvent(event);
}

void SessionView::onAddChapter()
{
    QModelIndex index = indexAt(m_pointClick);
    
    emit addChapter(index);
}
QModelIndexList SessionView::getSelection()
{
    return selectionModel()->selectedIndexes();
}
