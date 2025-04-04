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
#include <QCryptographicHash>

#include "undo/addfieldcommand.h"
#include "undo/deletefieldcommand.h"
#include "undo/movefieldcommand.h"
#include "undo/setbackgroundimage.h"
#include "utils/iohelper.h"
#include "controllers/editorcontroller.h"
#include "controllers/imagecontroller.h"

#include "tablecanvasfield.h"
#include "charactersheet/controllers/tablefield.h"

Canvas::Canvas(EditorController* ctrl, QObject* parent)
    : QGraphicsScene(parent), m_ctrl(ctrl), m_bg(new QGraphicsPixmapItem()), m_currentItem(nullptr), m_model(nullptr)
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
        DeleteFieldCommand* deleteCommand= new DeleteFieldCommand(fieldItem->controller(), this, m_model, m_pageId);
        emit performCommand(deleteCommand);
    }
}
void Canvas::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    qDebug() << "canvas pressed"<< mouseEvent;
    if(mouseEvent->button() == Qt::RightButton)
    {
        mouseEvent->accept();
        return;
    }

    if(mouseEvent->button() != Qt::LeftButton)
        QGraphicsScene::mousePressEvent(mouseEvent);

    if(forwardEvent())
    {
        m_oldPos.clear();
        QPointF mousePos(mouseEvent->buttonDownScenePos(Qt::LeftButton).x(),
                         mouseEvent->buttonDownScenePos(Qt::LeftButton).y());

        const QList<QGraphicsItem*> itemList= items(mousePos);
        qDebug() << "count itemList: "<<itemList.size();// << itemList[0]->boundingRect();
        for(auto item : itemList)
        {
            if(item->flags() & QGraphicsItem::ItemIsMovable)
                m_movingItems.append(item);
        }
        if(!m_movingItems.isEmpty())
        {
            if(m_movingItems.first() != nullptr && mouseEvent->button() == Qt::LeftButton)
            {
                m_oldPos.append(m_movingItems.first()->pos());
            }
        }

        // clearSelection();
        //QGraphicsScene::mousePressEvent(mouseEvent);
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
        mouseEvent->accept();
    }
    else if((m_currentTool <= Canvas::ADDCHECKBOX) || (m_currentTool == Canvas::BUTTON))
    {
        auto imageController= m_ctrl->imageController();

        charactersheet::ImageModel* model= nullptr;
        if(nullptr != imageController)
            model= imageController->model();

        AddFieldCommand* addCommand
            = new AddFieldCommand(m_currentTool, this, m_model, m_pageId, model, mouseEvent->scenePos());

        emit performCommand(addCommand);
        mouseEvent->accept();
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}
void Canvas::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    /*if(forwardEvent())
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
    else */if(m_currentItem != nullptr)
    {
        m_currentItem->setNewEnd(m_currentItem->mapFromScene(mouseEvent->scenePos()));
        update();
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
bool Canvas::forwardEvent()
{
    return (Canvas::MOVE == m_currentTool) || (Canvas::NONE == m_currentTool);
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
                    emit performCommand(moveCmd);
                }
                m_movingItems.clear();
                m_oldPos.clear();
            }
        }
    }
    else
    {
        adjustNewItem(m_currentItem);
        m_currentItem= nullptr;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
void Canvas::adjustNewItem(CanvasField* field)
{
    if(nullptr == field)
        return;

    auto ctrl= field->controller();
    if(ctrl->width() < 0)
    {
        ctrl->setX(ctrl->x() + ctrl->width());
        ctrl->setWidth(fabs(ctrl->width()));
    }

    if(ctrl->height() < 0)
    {
        ctrl->setY(ctrl->y() + ctrl->height());
        ctrl->setHeight(fabs(ctrl->height()));
    }
}

void Canvas::addField(CSItem* itemCtrl)
{
    CanvasField* cf= nullptr;
    switch(itemCtrl->itemType())
    {
    case TreeSheetItem::FieldItem:
        cf= new CanvasField(dynamic_cast<FieldController*>(itemCtrl));
        break;
    case TreeSheetItem::TableItem:
        cf= new TableCanvasField(dynamic_cast<TableFieldController*>(itemCtrl));
        break;
    default:
        break;
    }
    if(cf)
    {
        m_currentItem= cf;
        addItem(m_currentItem);
        m_currentItem->setPos({itemCtrl->x(), itemCtrl->y()});
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
    return m_bg ? m_bg->pixmap() : QPixmap();
}
void Canvas::setPixmap(const QPixmap& pix)
{
    static QString md5;
    auto key = QCryptographicHash::hash(utils::IOHelper::imageToData(pix.toImage()), QCryptographicHash::Md5);

    if(md5 == key)
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
    md5 = key;
}
