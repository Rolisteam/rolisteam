#include "campaignview.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QHeaderView>
#include <QMenu>

#include "data/campaign.h"
#include "data/media.h"
#include "model/mediamodel.h"

namespace campaign
{

CampaignView::CampaignView(QWidget* parent) : QTreeView(parent)
{
    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
    setDefaultDropAction(Qt::MoveAction);

    setDragDropMode(QAbstractItemView::InternalMove);

    m_addDirectoryAct= new QAction(tr("Add directory…"), this);
    m_deleteFileAct= new QAction(tr("delete"), this);
    m_defineAsCurrent= new QAction(tr("Current Directory"), this);
    m_openAct= new QAction(tr("Open"), this);
    m_openAsAct= new QAction(tr("Open As…"), this);

    // columns
    m_nameColsAct= new QAction(tr("Name"), this);
    m_sizeColsAct= new QAction(tr("Size"), this);
    m_typeColsAct= new QAction(tr("Type"), this);
    m_addedColsAct= new QAction(tr("Date Added"), this);
    m_modifiedColsAct= new QAction(tr("Date Modified"), this);

    m_nameColsAct->setCheckable(true);
    m_sizeColsAct->setCheckable(true);
    m_typeColsAct->setCheckable(true);
    m_addedColsAct->setCheckable(true);
    m_modifiedColsAct->setCheckable(true);

    m_nameColsAct->setChecked(true);
    m_sizeColsAct->setChecked(true);
    m_typeColsAct->setChecked(true);
    m_addedColsAct->setChecked(true);
    m_modifiedColsAct->setChecked(true);

    m_nameColsAct->setData(0);
    m_sizeColsAct->setData(1);
    m_typeColsAct->setData(2);
    m_addedColsAct->setData(3);
    m_modifiedColsAct->setData(4);

    connect(m_addDirectoryAct, &QAction::triggered, this, &CampaignView::onAddChapter);
    connect(m_deleteFileAct, &QAction::triggered, this, &CampaignView::removeSelection);
    connect(m_defineAsCurrent, &QAction::triggered, this, [this]() {

    });
    connect(m_openAct, &QAction::triggered, this, [this]() {

    });
    connect(m_openAsAct, &QAction::triggered, this, [this]() {

    });

    auto hideshowCol= [this]() {
        auto act= qobject_cast<QAction*>(sender());
        auto col= act->data().toInt();
        act->isChecked() ? showColumn(col) : hideColumn(col);
    };
    connect(m_nameColsAct, &QAction::triggered, this, hideshowCol);
    connect(m_sizeColsAct, &QAction::triggered, this, hideshowCol);
    connect(m_typeColsAct, &QAction::triggered, this, hideshowCol);
    connect(m_addedColsAct, &QAction::triggered, this, hideshowCol);
    connect(m_modifiedColsAct, &QAction::triggered, this, hideshowCol);

    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Init hash

    auto notes= new QAction(tr("Notes"), this);
    notes->setData(QVariant::fromValue(Core::ContentType::NOTES));

    auto sharedNote= new QAction(tr("Shared Notes"), this);
    sharedNote->setData(QVariant::fromValue(Core::ContentType::SHAREDNOTE));

    m_convertionHash.insert(Core::MediaType::TextFile, {notes, sharedNote});
}

void CampaignView::startDrag(Qt::DropActions supportedActions)
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
void CampaignView::contextMenuEvent(QContextMenuEvent* event)
{
    auto index= indexAt(event->pos());

    MediaNode* mediaNode= nullptr;
    if(index.isValid())
        mediaNode= static_cast<MediaNode*>(index.internalPointer());

    QMenu popMenu(this);

    popMenu.addAction(m_addDirectoryAct);
    popMenu.addAction(m_deleteFileAct);
    popMenu.addSeparator();
    auto cols= popMenu.addMenu(tr("Show/hide Column"));
    cols->addAction(m_nameColsAct);
    cols->addAction(m_sizeColsAct);
    cols->addAction(m_typeColsAct);
    cols->addAction(m_addedColsAct);
    cols->addAction(m_modifiedColsAct);

    popMenu.addAction(m_defineAsCurrent);
    popMenu.addAction(m_openAct);
    auto openAs= popMenu.addMenu(tr("Open As…"));
    addOpenAsActs(openAs, mediaNode);

    auto isVoid= (mediaNode == nullptr);
    auto isDirectory= mediaNode ? (mediaNode->nodeType() == MediaNode::NodeType::Directory) : false;

    m_addDirectoryAct->setEnabled(isDirectory || isVoid);
    m_deleteFileAct->setEnabled(!isVoid);
    m_defineAsCurrent->setEnabled(isDirectory);
    m_openAct->setEnabled(!isDirectory && !isVoid);
    openAs->setEnabled(!isDirectory && !isVoid);

    popMenu.exec(QCursor::pos());
}
void CampaignView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QModelIndex index= indexAt(event->pos());
    if(!index.isValid())
        return;

    // emit onDoubleClick(index);
    QTreeView::mouseDoubleClickEvent(event);
}

void CampaignView::onAddChapter()
{
    // QModelIndex index= indexAt(m_pointClick);

    //  emit addChapter(index);
}

void CampaignView::setCurrentChapter(const QString& chapter)
{
    // if()
}

QModelIndexList CampaignView::getSelection() const
{
    return selectionModel()->selectedRows();
}

QString CampaignView::currentChapter() const
{
    return {};
}

void CampaignView::setCampaign(Campaign* campaign)
{
    m_campaign= campaign;
}

void CampaignView::addOpenAsActs(QMenu* menu, MediaNode* node)
{
    menu->setEnabled(false);

    auto id= node->uuid();
    if(id.isEmpty())
        return;

    auto media= m_campaign->mediaFromUuid(id);

    if(!media)
        return;

    auto const& values= m_convertionHash.value(media->type());

    if(values.isEmpty())
        return;

    menu->setEnabled(true);
    for(auto val : values)
    {
        menu->addAction(val);
    }
}
} // namespace campaign
