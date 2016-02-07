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
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>




Canvas::Canvas(QObject *parent) : QGraphicsScene(parent),m_bg(NULL),m_currentItem(NULL)
{

}

void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    const QMimeData* mimeData =  event->mimeData();

    if(mimeData->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->acceptProposedAction();
}


void Canvas::dropEvent ( QGraphicsSceneDragDropEvent * event )
{

    const QMimeData* mimeData =  event->mimeData();
    if(mimeData->hasUrls())
    {
        foreach(QUrl url, mimeData->urls())
        {
            if(url.isLocalFile())
            {
                m_bg = addPixmap(url.toLocalFile());
                emit imageChanged();
                setSceneRect(m_bg->boundingRect());
            }
        }
    }
}
void Canvas::setCurrentTool(Canvas::Tool tool)
{
    m_currentTool = tool;
}
void Canvas::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentTool==Canvas::MOVE)
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
    }
    else if(mouseEvent->button() == Qt::LeftButton)
    {
         if(m_currentTool==Canvas::ADD)
         {
            Field* field = new Field(mouseEvent->scenePos());
            addItem(field);
            m_model->appendField(field);
            m_currentItem = field;
         }

    }
    else if(mouseEvent->button()==Qt::RightButton)
    {
    }
}
void Canvas::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentItem!=NULL)
    {
        m_currentItem->setNewEnd(mouseEvent->scenePos());
        update();
    }
    if(m_currentTool==Canvas::MOVE)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
void Canvas::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);

    m_currentItem = NULL;
    if(m_currentTool==Canvas::MOVE)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}
FieldModel *Canvas::model() const
{
    return m_model;
}

void Canvas::setModel(FieldModel *model)
{
    m_model = model;
}

QPixmap Canvas::pixmap()
{
    if(NULL!=m_bg)
    {
        return m_bg->pixmap();
    }
    return QPixmap();
}
void Canvas::setPixmap(QPixmap pix)
{
    if(!pix.isNull())
    {
        m_bg = addPixmap(pix);
        setSceneRect(m_bg->boundingRect());

    }
}
