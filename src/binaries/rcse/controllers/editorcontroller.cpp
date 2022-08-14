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

#include <QDebug>
#include <QGraphicsItem>
#include <QMenu>

#include "canvas.h"
#include "canvasfield.h"
#include "controllers/imagecontroller.h"
#include "itemeditor.h"

#include <algorithm>

// undo
#include "undo/setbackgroundimage.h"

EditorController::EditorController(ImageController* imgCtrl, QObject* parent)
    : QObject(parent), m_imageController(imgCtrl)
{
    /*connect(m_view, &ItemEditor::openContextMenu, this, &EditorController::menuRequestedFromView);
    m_fitInView= new QAction(tr("Fit the view"), m_view);
    m_fitInView->setCheckable(true);

    m_lockItem= new QAction(tr("Lock item"), m_view);
    m_lockItem->setCheckable(true);
    m_alignOnY= new QAction(tr("Align on Y"), m_view);
    m_alignOnX= new QAction(tr("Align on X"), m_view);
    m_sameWidth= new QAction(tr("Same Width"), m_view);
    m_sameHeight= new QAction(tr("Same Height"), m_view);
    m_dupplicate= new QAction(tr("Duplicate"), m_view);
    m_verticalEquaDistance= new QAction(tr("Vertical equidistribution"), m_view);
    m_horizontalEquaDistance= new QAction(tr("Horizontal equidistribution"), m_view);

    connect(m_lockItem, &QAction::triggered, this, &EditorController::lockItem);
    connect(m_fitInView, &QAction::triggered, this, &EditorController::setFitInView);
    connect(m_alignOnY, &QAction::triggered, this, &EditorController::alignOn);
    connect(m_alignOnX, &QAction::triggered, this, &EditorController::alignOn);
    connect(m_sameWidth, &QAction::triggered, this, &EditorController::sameGeometry);
    connect(m_sameHeight, &QAction::triggered, this, &EditorController::sameGeometry);
    connect(m_verticalEquaDistance, &QAction::triggered, this, &EditorController::spreadItemEqualy);
    connect(m_horizontalEquaDistance, &QAction::triggered, this, &EditorController::spreadItemEqualy);*/
}
void EditorController::sameGeometry(bool sameW, QList<FieldController*> ctrls, FieldController* ref)
{

    if(!ref)
        return;

    auto value= sameW ? ref->width() : ref->height();

    for(auto field : ctrls)
    {
        if(sameW)
        {
            field->setWidth(value);
        }
        else
        {
            field->setHeight(value);
        }
    }

    emit dataChanged();
}

void EditorController::alignOn(bool onX, QList<FieldController*> ctrls, FieldController* ref)
{

    if(!ref)
        return;

    qreal value= onX ? ref->x() : ref->y();

    for(auto item : ctrls)
    {
        if(onX)
        {
            item->setX(value);
        }
        else
        {
            item->setY(value);
        }
    }

    emit dataChanged();
}

void EditorController::spreadItemEqualy(QList<FieldController*> ctrls, bool horizon)
{
    // bool horizon= act == m_horizontalEquaDistance ? true : false;
    // auto list= m_view->scene()->selectedItems();

    if(ctrls.size() < 3) // ignore when 2 elements or less have been selected.
        return;

    auto widthCompare= [](FieldController* a, FieldController* b) {
        auto posA= a->x() - a->width() / 2;
        auto posB= b->x() - b->width() / 2;
        return posA < posB;
    };
    auto heightCompare= [](FieldController* a, FieldController* b) {
        auto posA= a->y() - a->height() / 2;
        auto posB= b->y() - b->height() / 2;
        return posA < posB;
    };

    if(horizon)
        std::sort(ctrls.begin(), ctrls.end(), widthCompare);
    else
        std::sort(ctrls.begin(), ctrls.end(), heightCompare);

    auto first= ctrls.begin();
    auto last= ctrls.end() - 1;

    // available distance
    qreal availableDistance;
    int spaceCount= 1;
    qreal beginAvailableSpace;
    qreal endAvailableSpace;
    if(horizon)
    {
        endAvailableSpace= (*last)->x();
        beginAvailableSpace= (*first)->x() + (*first)->width();
    }
    else
    {
        endAvailableSpace= (*last)->y();
        beginAvailableSpace= (*first)->y() + (*first)->height();
    }

    ctrls.erase(ctrls.cbegin());
    ctrls.erase(ctrls.cend() - 1);

    qreal itemSpace= 0.0;
    std::for_each(ctrls.begin(), ctrls.end(), [horizon, &itemSpace](FieldController* item) {
        itemSpace+= horizon ? item->width() : item->height();
    });

    availableDistance= endAvailableSpace - beginAvailableSpace - itemSpace;
    spaceCount= ctrls.size() + 1; // need two spaces when one element between first and last
    auto spaceStep= availableDistance / spaceCount;
    auto pos= beginAvailableSpace;

    if(horizon)
    {
        for(auto item : ctrls)
        {
            item->setX(pos + spaceStep);
            pos= item->x() + item->width();
        }
    }
    else
    {
        for(auto item : ctrls)
        {
            item->setY(pos + spaceStep);
            pos= item->y() + item->height();
        }
    }

    emit dataChanged();
}

int EditorController::currentPage() const
{
    return m_currentPage;
}

void EditorController::clearData()
{
    setCurrentPage(-1);
    m_canvasList.clear();
    emit dataChanged();
}

int EditorController::addPage()
{
    std::unique_ptr<Canvas> canvas(new Canvas(this)); // not parenting
    int page= -1;
    for(const auto& canvas : m_canvasList)
    {
        page= std::max(page, canvas->pageId());
    }
    auto can= canvas.get();
    canvas->setPageId(page + 1);
    connect(canvas.get(), &Canvas::performCommand, this, &EditorController::performCommand);
    connect(canvas.get(), &Canvas::dropFileOnCanvas, this, &EditorController::loadImageFromUrl);
    m_canvasList.push_back(std::move(canvas));
    if(m_canvasList.size() == 1)
    {
        setCurrentPage(0);
    }
    emit pageAdded(can);
    emit pageCountChanged();
    emit dataChanged();
    return m_canvasList.size() - 1;
}

Canvas* EditorController::currentCanvas() const
{
    return canvas(m_currentPage);
}

Canvas* EditorController::canvas(int i) const
{
    if(i < 0 || i >= static_cast<int>(m_canvasList.size()))
        return nullptr;
    return m_canvasList[i].get();
}

void EditorController::setCurrentPage(int currentPage)
{
    if(m_currentPage == currentPage)
        return;

    m_currentPage= currentPage;
    emit currentPageChanged(m_currentPage);
    emit dataChanged();
}

void EditorController::setCurrentTool(Canvas::Tool tool)
{
    for(auto& canvas : m_canvasList)
    {
        canvas->setCurrentTool(tool);
    }
}

void EditorController::insertPage(int i, Canvas* canvas)
{
    m_canvasList.insert(m_canvasList.begin() + i, std::unique_ptr<Canvas>(canvas));
    emit dataChanged();
}

Canvas* EditorController::removePage(int i)
{
    auto idx= static_cast<std::size_t>(i);
    if(idx > m_canvasList.size() || i < 0 || m_canvasList.size() == 0)
        return nullptr;
    auto p= m_canvasList[idx].release();
    m_canvasList.erase(m_canvasList.begin() + i);
    emit dataChanged();
    return p;
}
std::size_t EditorController::pageCount() const
{
    return m_canvasList.size();
}

void EditorController::setImageBackground(int idx, const QPixmap& pix, const QString& filepath)
{
    auto pos= qBound(0, idx, static_cast<int>(m_canvasList.size()));

    if(pos != idx || m_canvasList.empty())
        return;

    auto canvas= m_canvasList[static_cast<std::size_t>(idx)].get();
    if(!canvas)
        return;

    canvas->setPixmap(pix);
    emit canvasBackgroundChanged(idx, pix, filepath, QString());
    emit dataChanged();
}

void EditorController::loadImageFromUrl(const QUrl& url)
{
    SetBackgroundCommand* cmd= new SetBackgroundCommand(currentPage(), this, url);
    emit performCommand(cmd);
    emit dataChanged();
}

void EditorController::loadImages(const QList<QImage>& imgs)
{
    auto cmd= new AddBackGroundImagesCommand(this, imgs);
    emit performCommand(cmd);
    emit dataChanged();
}

void EditorController::addFieldItem(CSItem* itemCtrl)
{
    auto page= itemCtrl->page();

    auto c= canvas(page);

    if(!c)
        return;

    c->addField(itemCtrl);
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
    emit dataChanged();
}

ImageController* EditorController::imageController() const
{
    return m_imageController;
}

QPixmap EditorController::backgroundFromIndex(int i) const
{
    auto pos= qBound(0, i, static_cast<int>(m_canvasList.size()));

    if(pos != i || m_canvasList.empty())
        return {};

    auto canvas= m_canvasList[static_cast<std::size_t>(i)].get();
    if(!canvas)
        return {};

    return canvas->pixmap();
}
