/***************************************************************************
 * Copyright (C) 2019 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "editorcontroller.h"

#include <QGraphicsItem>
#include <QMenu>

#include "canvas.h"
#include "canvasfield.h"
#include "itemeditor.h"

EditorController::EditorController(ItemEditor* view, QObject* parent) : QObject(parent), m_view(view)
{
    connect(m_view, &ItemEditor::openContextMenu, this, &EditorController::menuRequestedFromView);
    m_fitInView= new QAction(tr("Fit the view"), m_view);
    m_fitInView->setCheckable(true);

    m_alignOnY= new QAction(tr("Align on Y"), m_view);
    m_alignOnX= new QAction(tr("Align on X"), m_view);
    m_sameWidth= new QAction(tr("Same Width"), m_view);
    m_sameHeight= new QAction(tr("Same Height"), m_view);
    m_dupplicate= new QAction(tr("Dupplicate"), m_view);
    m_verticalEquaDistance= new QAction(tr("Vertical equidistribution"), m_view);
    m_horizontalEquaDistance= new QAction(tr("Horizontal equidistribution"), m_view);

    connect(m_fitInView, &QAction::triggered, this, &EditorController::setFitInView);
    connect(m_alignOnY, &QAction::triggered, this, &EditorController::alignOn);
    connect(m_alignOnX, &QAction::triggered, this, &EditorController::alignOn);
    connect(m_sameWidth, &QAction::triggered, this, &EditorController::sameGeometry);
    connect(m_sameHeight, &QAction::triggered, this, &EditorController::sameGeometry);
    connect(m_verticalEquaDistance, &QAction::triggered, this, &EditorController::spreadItemEqualy);
    connect(m_horizontalEquaDistance, &QAction::triggered, this, &EditorController::spreadItemEqualy);
}
void EditorController::sameGeometry()
{
    auto action= qobject_cast<QAction*>(sender());
    bool width= false;
    if(m_sameWidth == action)
    {
        width= true;
    }
    QList<QGraphicsItem*> items= m_view->scene()->selectedItems();
    QGraphicsItem* reference= m_view->itemAt(m_posMenu);
    if(nullptr != reference)
    {
        qreal value= reference->boundingRect().height();
        if(width)
        {
            value= reference->boundingRect().width();
        }

        for(auto item : items)
        {
            auto field= dynamic_cast<CanvasField*>(item);
            if(width)
            {
                field->setWidth(value);
            }
            else
            {
                field->setHeight(value);
            }
        }
    }
}

void EditorController::menuRequestedFromView(const QPoint& pos)
{
    Q_UNUSED(pos);
    QMenu menu(m_view);

    auto list= m_view->items(pos);

    for(auto item : list)
    {
        auto field= dynamic_cast<CanvasField*>(item);
        if(nullptr != field)
        {
            field->setMenu(menu);
            menu.addSeparator();
        }
    }
    menu.addAction(m_fitInView);
    menu.addSeparator();
    menu.addAction(m_alignOnX);
    menu.addAction(m_alignOnY);
    menu.addAction(m_sameWidth);
    menu.addAction(m_sameHeight);
    menu.addSeparator();
    menu.addAction(m_dupplicate);

    m_posMenu= pos;
    menu.exec(QCursor::pos());
}

void EditorController::alignOn()
{
    auto action= qobject_cast<QAction*>(sender());
    bool onX= false;
    if(m_alignOnX == action)
    {
        onX= true;
    }

    QList<QGraphicsItem*> items= m_view->scene()->selectedItems();
    QGraphicsItem* reference= m_view->itemAt(m_posMenu);
    if(nullptr != reference)
    {
        qreal value= reference->pos().y();
        if(onX)
        {
            value= reference->pos().x();
        }

        for(auto item : items)
        {
            if(onX)
            {
                item->setPos(value, item->pos().y());
            }
            else
            {
                item->setPos(item->pos().x(), value);
            }
        }
    }
}

void EditorController::spreadItemEqualy()
{
    auto act= qobject_cast<QAction*>(sender());
    bool horizon= act == m_horizontalEquaDistance ? true : false;
}

void EditorController::setFitInView()
{
    if(m_fitInView->isChecked())
    {
        Canvas* canvas= m_canvasList[static_cast<std::size_t>(m_currentPage)].get();
        const QPixmap& pix= canvas->pixmap();
        m_view->fitInView(QRectF(pix.rect()), Qt::KeepAspectRatioByExpanding);
    }
    else
    {
        m_view->fitInView(QRectF(m_view->rect()));
    }
}

int EditorController::currentPage() const
{
    return m_currentPage;
}

void EditorController::clearData(bool defaulCanvas)
{
    setCurrentPage(0);
    m_canvasList.clear();
    Canvas* canvasPtr= nullptr;
    if(defaulCanvas)
    {
        canvasPtr= addPage();
    }
    m_view->setScene(canvasPtr);
}

Canvas* EditorController::addPage()
{
    std::unique_ptr<Canvas> canvas(new Canvas());
    CSItem::resetCount();
    auto can= canvas.get();
    canvas->setPageId(m_currentPage);
    m_canvasList.push_back(std::move(canvas));
    emit pageAdded(can);
    emit pageCountChanged();
    return can;
}

void EditorController::setCurrentPage(int currentPage)
{
    if(m_currentPage == currentPage)
        return;

    m_currentPage= currentPage;
    emit currentPageChanged(m_currentPage);
    updateView();
}

void EditorController::updateView()
{
    m_view->setScene(m_canvasList[m_currentPage].get());
}
void EditorController::setCurrentTool(Canvas::Tool tool)
{
    for(auto& canvas : m_canvasList)
    {
        canvas->setCurrentTool(tool);
    }
}

void EditorController::load(QJsonObject obj) {}
void EditorController::save(QJsonObject obj) {}

void EditorController::insertPage(int i, Canvas* canvas)
{
    m_canvasList.insert(m_canvasList.begin() + i, std::unique_ptr<Canvas>(canvas));
}

Canvas* EditorController::removePage(int i)
{
    auto idx= static_cast<std::size_t>(i);
    if(idx > m_canvasList.size() || i < 0)
        return nullptr;
    auto p= m_canvasList[idx].release();
    m_canvasList.erase(m_canvasList.begin() + i);
    return p;
}
std::size_t EditorController::pageCount() const
{
    return m_canvasList.size();
}

void EditorController::setImageBackground(int idx, const QPixmap& pix)
{
    auto pos= qBound(0, idx, static_cast<int>(m_canvasList.size()));

    if(pos != idx || m_canvasList.empty())
        return;

    auto canvas= m_canvasList[static_cast<std::size_t>(idx)].get();
    if(!canvas)
        return;

    canvas->setPixmap(pix);
}

void EditorController::addItem(int idx, QGraphicsItem* item)
{
    auto pos= qBound(0, idx, static_cast<int>(m_canvasList.size()));

    if(pos != idx || m_canvasList.empty())
        return;

    auto page= m_canvasList[static_cast<std::size_t>(idx)].get();
    if(!page)
        return;
    page->addItem(item);
}

/*void EditorController::addPage()
{
    Canvas* previous= m_canvasList[m_currentPage];
    ui->m_selectPageCb->setModel(AddPageCommand::getPagesModel());
    AddPageCommand* cmd= new AddPageCommand(++m_currentPage, m_canvasList, previous->currentTool());
    auto canvas= cmd->canvas();
    m_undoStack.push(cmd);
    ui->m_selectPageCb->setCurrentIndex(pageCount() - 1);
    emit dataChanged();
}

void EditorController::removePage()
{
    if(m_canvasList.size() > 1)
    {
        DeletePageCommand* cmd= new DeletePageCommand(m_currentPage, m_canvasList, m_model);
        m_undoStack.push(cmd);
    }
}*/
