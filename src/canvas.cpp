/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
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
#include "canvas.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>
#include <cmath>

#include "undo/addfieldcommand.h"
#include "undo/deletefieldcommand.h"
#include "undo/movefieldcommand.h"
#include "undo/setbackgroundimage.h"

#include "tablecanvasfield.h"

//#include "charactersheetbutton.h"

Canvas::Canvas(QObject* parent)
    : QGraphicsScene(parent)
    , m_bg(new QGraphicsPixmapItem())
    , m_currentItem(nullptr)
    , m_model(nullptr)
    , m_undoStack(nullptr)
{
    setSceneRect(QRect(0, 0, 800, 600));
    m_bg->setFlag(QGraphicsItem::ItemIsSelectable, false);
    m_bg->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
    m_bg->setFlag(QGraphicsItem::ItemIsMovable, false);
    m_bg->setFlag(QGraphicsItem::ItemIsFocusable, false);
    m_bg->setAcceptedMouseButtons(Qt::NoButton);
}

void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mimeData= event->mimeData();
    if(mimeData->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->acceptProposedAction();
}
void Canvas::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mimeData= event->mimeData();
    if(mimeData->hasUrls())
    {
        for(const QUrl& url : mimeData->urls())
        {
            if(url.isLocalFile())
            {
                emit dropFileOnCanvas(url);
                /*SetBackgroundCommand* cmd= new SetBackgroundCommand(this, url);
                m_undoStack->push(cmd);
                emit pixmapChanged();*/
            }
        }
    }
}
void Canvas::setCurrentTool(Canvas::Tool tool)
{
    m_currentTool= tool;
    if(nullptr != m_currentItem)
    {
        m_currentItem= nullptr;
    }
}
void Canvas::deleteItem(QGraphicsItem* item)
{
    auto fieldItem= dynamic_cast<CanvasField*>(item);
    if(nullptr != fieldItem)
    {
        DeleteFieldCommand* deleteCommand= new DeleteFieldCommand(fieldItem->getField(), this, m_model, m_pageId);
        m_undoStack->push(deleteCommand);
    }
}
void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(mouseEvent->button() == Qt::LeftButton)
    {
        if(forwardEvent())
        {
            m_oldPos.clear();
            QPointF mousePos(mouseEvent->buttonDownScenePos(Qt::LeftButton).x(),
                             mouseEvent->buttonDownScenePos(Qt::LeftButton).y());
            const QList<QGraphicsItem*> itemList= items(mousePos);
            m_movingItems.append(itemList.isEmpty() ? nullptr : itemList.first());

            if(m_movingItems.first() != nullptr && mouseEvent->button() == Qt::LeftButton)
            {
                m_oldPos.append(m_movingItems.first()->pos());
            }

            // clearSelection();
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
        if(m_currentTool == Canvas::DELETETOOL)
        {
            QList<QGraphicsItem*> itemList= items(mouseEvent->scenePos());
            for(QGraphicsItem* item : itemList)
            {
                if(item != m_bg)
                {
                    deleteItem(item);
                }
            }
        }
        else if((m_currentTool <= Canvas::ADDCHECKBOX) || (m_currentTool == Canvas::BUTTON))
        {
            AddFieldCommand* addCommand
                = new AddFieldCommand(m_currentTool, this, m_model, m_pageId, m_imageModel, mouseEvent->scenePos());
            m_currentItem= addCommand->getField();
            m_undoStack->push(addCommand);
        }
    }
    else
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
    }
}
void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if(forwardEvent())
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
    else if(m_currentItem != nullptr)
    {
        m_currentItem->setNewEnd(m_currentItem->mapFromScene(mouseEvent->scenePos()));
        update();
    }
}
bool Canvas::forwardEvent()
{
    if((Canvas::MOVE == m_currentTool) || (Canvas::NONE == m_currentTool))
    {
        return true;
    }
    else
        return false;
}

ImageModel* Canvas::getImageModel() const
{
    return m_imageModel;
}

void Canvas::setImageModel(ImageModel* imageModel)
{
    m_imageModel= imageModel;
}

QUndoStack* Canvas::undoStack() const
{
    return m_undoStack;
}

void Canvas::setUndoStack(QUndoStack* undoStack)
{
    m_undoStack= undoStack;
}

void Canvas::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    Q_UNUSED(mouseEvent);

    if(forwardEvent())
    {
        if(!m_movingItems.isEmpty())
        {
            if(m_movingItems.first() != nullptr && mouseEvent->button() == Qt::LeftButton)
            {
                if(m_oldPos.first() != m_movingItems.first()->pos())
                {
                    MoveFieldCommand* moveCmd= new MoveFieldCommand(m_movingItems, m_oldPos);
                    m_undoStack->push(moveCmd);
                }
                m_movingItems.clear();
                m_oldPos.clear();
            }
        }
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
    else
    {
        adjustNewItem(m_currentItem);
        m_currentItem= nullptr;
    }
}
void Canvas::adjustNewItem(CSItem* item)
{
    if(nullptr == item)
        return;
    // qDebug() <<"newitem width: "<< item->getWidth()<<"newitem height:" << item->getHeight();
    if(item->getWidth() < 0)
    {
        item->setX(item->getX() + item->getWidth());
        item->setWidth(fabs(item->getWidth()));
    }

    if(item->getHeight() < 0)
    {
        item->setY(item->getY() + item->getHeight());
        item->setHeight(fabs(item->getHeight()));
    }
}

Canvas::Tool Canvas::currentTool() const
{
    return m_currentTool;
}

int Canvas::pageId() const
{
    return m_pageId;
}

void Canvas::setPageId(int pageId)
{
    if(pageId == m_pageId)
        return;
    m_pageId= pageId;
    emit pageIdChanged();
}
FieldModel* Canvas::model() const
{
    return m_model;
}

void Canvas::setModel(FieldModel* model)
{
    m_model= model;
}

const QPixmap Canvas::pixmap() const
{
    return m_bg->pixmap();
}
void Canvas::setPixmap(const QPixmap& pix)
{
    if(pixmap() == pix)
        return;
    m_bg->setPixmap(pix);
    emit pixmapChanged();

    if(pix.isNull())
        removeItem(m_bg);
    else
    {
        addItem(m_bg);
        setSceneRect(m_bg->boundingRect());
    }
}
