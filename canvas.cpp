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
#include <QDebug>


//#include "charactersheetbutton.h"

Canvas::Canvas(QObject *parent) : QGraphicsScene(parent),m_bg(NULL),m_currentItem(NULL)
{
    setSceneRect(QRect(0,0,800,600));
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
                m_pix = new QPixmap(url.toLocalFile());
                if(NULL==m_bg)
                {
                    m_bg = addPixmap(*m_pix);
                    m_bg->setFlag(QGraphicsItem::ItemIsSelectable,false);
                    m_bg->setFlag(QGraphicsItem::ItemSendsGeometryChanges,false);
                    m_bg->setFlag(QGraphicsItem::ItemIsMovable,false);
                    m_bg->setFlag(QGraphicsItem::ItemIsFocusable,false);
                    m_bg->setAcceptedMouseButtons(Qt::NoButton);
                    emit imageChanged();
                    setSceneRect(m_bg->boundingRect());
                }
                else
                {
                    m_bg->setPixmap(*m_pix);
                    setSceneRect(m_bg->boundingRect());
                }
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
        if(m_currentTool==Canvas::DELETETOOL)
        {
            QList<QGraphicsItem *> itemList = items(mouseEvent->scenePos());
            for(QGraphicsItem* item : itemList)
            {
                if(item!=m_bg)
                {
                    removeItem(item);
                }
            }
        }
        else if((m_currentTool<=Canvas::ADDCHECKBOX)||(m_currentTool==Canvas::BUTTON))
        {
            Field* field = new Field(mouseEvent->scenePos());
            field->setPage(m_currentPage);
            addItem(field->getCanvasField());
            m_model->appendField(field);
            field->setValueFrom(CharacterSheetItem::X,mouseEvent->scenePos().x());
            field->setValueFrom(CharacterSheetItem::Y,mouseEvent->scenePos().y());
            m_currentItem = field;
            //m_currentItem->setFocus();
            switch(m_currentTool)//TEXTINPUT,TEXTFIELD,TEXTAREA,SELECT,CHECKBOX,IMAGE,BUTTON
            {
            case Canvas::ADDCHECKBOX:
                field->setCurrentType(Field::CHECKBOX);
                break;
            case Canvas::ADDINPUT:
                field->setCurrentType(Field::TEXTINPUT);
                break;
            case Canvas::ADDTEXTAREA:
                field->setCurrentType(Field::TEXTAREA);
                break;
            case Canvas::ADDTEXTFIELD:
                field->setCurrentType(Field::TEXTFIELD);
                break;
            case Canvas::ADDIMAGE:
                field->setCurrentType(Field::IMAGE);
                break;
            case Canvas::BUTTON:
                field->setCurrentType(Field::BUTTON);
                field->setBgColor(Qt::red);
                break;
            }
        }
    }
    else if(mouseEvent->button()==Qt::RightButton)
    {
    }
}
void Canvas::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{

    if(m_currentTool==Canvas::MOVE)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
    else if(m_currentItem!=NULL)
    {
        m_currentItem->setNewEnd(m_currentItem->mapFromScene(mouseEvent->scenePos()));
        update();
    }
}
void Canvas::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);

    if(m_currentTool==Canvas::MOVE)
    {      
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}

Canvas::Tool Canvas::currentTool() const
{
    return m_currentTool;
}

int Canvas::currentPage() const
{
    return m_currentPage;
}

void Canvas::setCurrentPage(int currentPage)
{
    m_currentPage = currentPage;
}
FieldModel *Canvas::model() const
{
    return m_model;
}

void Canvas::setModel(FieldModel *model)
{
    m_model = model;
}

QPixmap* Canvas::pixmap()
{
    return m_pix;
}
void Canvas::setPixmap(QPixmap* pix)
{
    m_pix = pix;
    if((NULL!=m_pix)&&(!m_pix->isNull()))
    {
        m_bg = addPixmap(*m_pix);
        setSceneRect(m_bg->boundingRect());
    }
}
