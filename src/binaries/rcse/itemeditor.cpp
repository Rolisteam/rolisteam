#include "itemeditor.h"

#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QMenu>
#include <QMouseEvent>

#include "canvasfield.h"
#include "charactersheet/field.h"
#include "controllers/editorcontroller.h"

QList<FieldController*> cleanSelection(const QList<QGraphicsItem*>& items, bool& locked, bool& allSame)
{
    QList<FieldController*> res;

    auto func= [](QGraphicsItem* item) {
        auto field= dynamic_cast<CanvasField*>(item);
        if(nullptr == field)
            return false;
        return static_cast<bool>(field->flags() & QGraphicsItem::ItemIsMovable);
    };

    allSame= std::all_of(std::begin(items), std::end(items), [](QGraphicsItem* item) {
        auto field= dynamic_cast<CanvasField*>(item);
        if(nullptr == field)
            return false;
        return static_cast<bool>(field->flags() & QGraphicsItem::ItemIsMovable);
    });

    if(!items.isEmpty())
        locked= func(items.first());
    else
        locked= false;

    std::transform(std::begin(items), std::end(items), std::back_inserter(res),
                   [](QGraphicsItem* item) -> FieldController* {
                       auto field= dynamic_cast<CanvasField*>(item);
                       if(!field)
                           return nullptr;

                       return field->controller();
                   });

    res.removeAll(nullptr);

    return res;
}

ItemEditor::ItemEditor(QWidget* parent) : QGraphicsView(parent)
{
    setAcceptDrops(true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    setAlignment((Qt::AlignLeft | Qt::AlignTop));

    m_fitInView.reset(new QAction(tr("Fit the view")));
    m_fitInView->setCheckable(true);

    m_lockItem.reset(new QAction(tr("Lock item")));
    m_lockItem->setCheckable(true);
    m_alignOnY.reset(new QAction(tr("Align on Y")));
    m_alignOnX.reset(new QAction(tr("Align on X")));
    m_sameWidth.reset(new QAction(tr("Same Width")));
    m_sameHeight.reset(new QAction(tr("Same Height")));
    m_dupplicate.reset(new QAction(tr("Dupplicate")));
    m_verticalEquaDistance.reset(new QAction(tr("Vertical equidistribution")));
    m_horizontalEquaDistance.reset(new QAction(tr("Horizontal equidistribution")));
}

void ItemEditor::setController(EditorController* editCtrl)
{
    m_ctrl= editCtrl;

    connect(m_ctrl, &EditorController::dataChanged, this, [this]() { update(); });
    connect(m_lockItem.get(), &QAction::triggered, this, [this](bool checked) {
        std::for_each(std::begin(m_selection), std::end(m_selection), [checked](FieldController* ctrl) {
            if(!ctrl)
                return;
            ctrl->setReadOnly(checked);
        });
    });
    connect(m_fitInView.get(), &QAction::triggered, this, [this](bool checked) {
        if(checked)
        {
            const QPixmap& pix= m_ctrl->backgroundFromIndex(m_ctrl->currentPage());
            fitInView(QRectF(pix.rect()), Qt::KeepAspectRatioByExpanding);
        }
        else
        {
            fitInView(rect());
        }
    });
    connect(m_alignOnY.get(), &QAction::triggered, this,
            [this]() { m_ctrl->alignOn(true, m_selection, m_underCursorItem); });
    connect(m_alignOnX.get(), &QAction::triggered, this,
            [this]() { m_ctrl->alignOn(false, m_selection, m_underCursorItem); });

    connect(m_sameWidth.get(), &QAction::triggered, this,
            [this]() { m_ctrl->sameGeometry(true, m_selection, m_underCursorItem); });
    connect(m_sameHeight.get(), &QAction::triggered, this,
            [this]() { m_ctrl->sameGeometry(false, m_selection, m_underCursorItem); });

    connect(m_verticalEquaDistance.get(), &QAction::triggered, this,
            [this]() { m_ctrl->spreadItemEqualy(m_selection, false); });
    connect(m_horizontalEquaDistance.get(), &QAction::triggered, this,
            [this]() { m_ctrl->spreadItemEqualy(m_selection, true); });

    connect(m_ctrl, &EditorController::currentPageChanged, this, [this]() {
        if(m_ctrl->currentCanvas())
            setScene(m_ctrl->currentCanvas());
    });
    connect(m_ctrl, &EditorController::pageCountChanged, this, [this]() {
        auto c= m_ctrl->currentCanvas();
        if(!c)
            return;

        setScene(c);
    });

    if(m_ctrl->currentCanvas())
        setScene(m_ctrl->currentCanvas());
}

bool ItemEditor::handle() const
{
    return m_handle;
}

void ItemEditor::setHandle(bool handle)
{
    m_handle= handle;
}

void ItemEditor::mousePressEvent(QMouseEvent* event)
{
    if(!m_handle)
    {
        setDragMode(QGraphicsView::NoDrag);
        event->ignore();
        qDebug() << "mousePressEvent inside not handle" << event->isAccepted() << scene();
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
    qDebug() << "mousePressEvent inside end ";

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
    m_selection.clear();
    auto pos= event->pos();
    QMenu menu(this);

    if(!scene())
        return;

    bool allSame;
    bool locked;
    m_selection= cleanSelection(scene()->selectedItems(), locked, allSame);
    auto list= cleanSelection(items(pos), locked, allSame);

    if(!list.isEmpty())
        m_underCursorItem= list.first();

    /*for(auto item : list)
    {
        auto field= dynamic_cast<CanvasField*>(item);
        if(nullptr != field)
        {
            field->setMenu(menu);
            menu.addSeparator();
        }
    }*/
    menu.addAction(m_fitInView.get());
    menu.addSeparator();
    menu.addAction(m_lockItem.get());
    if(allSame)
        m_lockItem->setChecked(locked);
    else
        m_lockItem->setChecked(false);
    menu.addAction(m_alignOnX.get());
    menu.addAction(m_alignOnY.get());
    menu.addAction(m_sameWidth.get());
    menu.addAction(m_sameHeight.get());
    menu.addAction(m_verticalEquaDistance.get());
    menu.addAction(m_horizontalEquaDistance.get());
    menu.addSeparator();
    menu.addAction(m_dupplicate.get());

    menu.exec(QCursor::pos());
}
