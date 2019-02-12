#include "sessionview.h"
#include <QDebug>
#include <QMenu>

#include "data/chapter.h"
#include "session/sessionitemmodel.h"
#include <QContextMenuEvent>
#include <QHeaderView>

CleverURI* voidToCleverUri(void* pointer)
{
    auto node= static_cast<ResourcesNode*>(pointer);
    return dynamic_cast<CleverURI*>(node);
}

SessionView::SessionView(QWidget* parent) : QTreeView(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::MoveAction);

    setDragDropMode(QAbstractItemView::InternalMove);
    // setContextMenuPolicy (Qt::CustomContextMenu);
    m_addChapterAction= new QAction(tr("Add Chapter…"), this);
    connect(m_addChapterAction, SIGNAL(triggered()), this, SLOT(onAddChapter()));

    m_removeAction= new QAction(tr("Remove items"), this);
    connect(m_removeAction, SIGNAL(triggered()), this, SIGNAL(removeSelection()));

    m_defineAsCurrent= new QAction(tr("Current Chapter"), this);
    connect(m_defineAsCurrent, SIGNAL(triggered()), this, SIGNAL(defineCurrentChapter()));

    m_switchLoadingMode= new QAction("", this);
    connect(m_switchLoadingMode, &QAction::triggered, this, [=]() {
        auto index= currentIndex();
        if(!index.isValid())
            return;
        auto uri= voidToCleverUri(index.internalPointer());
        // auto uri = dynamic_cast<CleverURI*>(node);
        if(nullptr == uri)
            return;
        if(uri->getCurrentMode() == CleverURI::Internal)
            uri->setCurrentMode(CleverURI::Linked);
        else
            uri->setCurrentMode(CleverURI::Internal);
    });

    m_loadingModeColumn= new QAction(tr("Loading Mode"), this);
    m_loadingModeColumn->setCheckable(true);
    connect(
        m_loadingModeColumn, &QAction::toggled, this, [=]() { setColumnHidden(1, !m_loadingModeColumn->isChecked()); });
    m_loadingModeColumn->setChecked(true);

    m_displayedColumn= new QAction(tr("Displayed Status"), this);
    m_displayedColumn->setCheckable(true);
    connect(m_displayedColumn, &QAction::toggled, this, [=]() { setColumnHidden(2, !m_displayedColumn->isChecked()); });
    m_displayedColumn->setChecked(false);

    m_pathColumn= new QAction(tr("Path"), this);
    m_pathColumn->setCheckable(true);
    connect(m_pathColumn, &QAction::toggled, this, [=]() { setColumnHidden(3, !m_pathColumn->isChecked()); });
    m_pathColumn->setChecked(false);

    m_rename= new QAction(tr("Rename"), this);
    m_rename->setShortcut(QKeySequence(Qt::Key_F2));
    connect(m_rename, &QAction::triggered, this, [=]() { edit(currentIndex()); });

    setEditTriggers(QAbstractItemView::NoEditTriggers);
}
void SessionView::onDefineChapter()
{
    emit defineCurrentChapter();
}
void SessionView::startDrag(Qt::DropActions supportedActions)
{
    QDrag* drag= new QDrag(this);
    QModelIndexList indexes= selectionModel()->selectedRows();

    QMimeData* mimeData= model()->mimeData(indexes);
    if(nullptr == mimeData)
        return;

    drag->setMimeData(mimeData);
    Qt::DropAction defaultDropAction= Qt::MoveAction;

    drag->exec(supportedActions, defaultDropAction);
}
void SessionView::contextMenuEvent(QContextMenuEvent* event)
{
    auto index= indexAt(event->pos());
    auto uri= voidToCleverUri(index.internalPointer());
    ;

    QMenu popMenu(this);

    popMenu.addAction(m_rename);
    if(uri != nullptr)
    {
        auto text= tr("Switch loading mode to %1")
                       .arg((uri->getCurrentMode() == CleverURI::Internal) ? tr("Linked") : tr("Internal"));
        m_switchLoadingMode->setText(text);
        popMenu.addAction(m_switchLoadingMode);
    }
    popMenu.addSeparator();
    popMenu.addAction(m_addChapterAction);
    popMenu.addAction(m_removeAction);
    popMenu.addSeparator();
    popMenu.addSection(tr("Show/hide Column"));
    popMenu.addAction(m_loadingModeColumn);
    popMenu.addAction(m_displayedColumn);
    popMenu.addAction(m_pathColumn);
    m_pointClick= event->pos();
    popMenu.exec(QCursor::pos());
}
void SessionView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index= indexAt(event->pos());
    if(!index.isValid())
        return;

    emit onDoubleClick(index);
    QTreeView::mouseDoubleClickEvent(event);
}

void SessionView::onAddChapter()
{
    QModelIndex index= indexAt(m_pointClick);

    emit addChapter(index);
}
QModelIndexList SessionView::getSelection()
{
    return selectionModel()->selectedRows();
}
