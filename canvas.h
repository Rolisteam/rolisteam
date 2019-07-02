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
#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QUndoStack>

#include "field.h"
#include "fieldmodel.h"

class ImageModel;
class Canvas : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(int pageId READ pageId WRITE setPageId NOTIFY pageIdChanged)
    Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
public:
    enum Tool
    {
        NONE,
        ADDINPUT,
        ADDTEXTFIELD,
        ADDTEXTAREA,
        ADDTABLE,
        ADDIMAGE,
        ADDFUNCBUTTON,
        ADDWEBPAGE,
        NEXTPAGE,
        PREVIOUSPAGE,
        ADDCHECKBOX,
        MOVE,
        DELETETOOL,
        BUTTON
    };
    explicit Canvas(QObject* parent= nullptr);

    void setCurrentTool(Canvas::Tool);
    FieldModel* model() const;
    void setModel(FieldModel* model);

    const QPixmap pixmap() const;

    void setPixmap(const QPixmap& pix);
    int pageId() const;
    void setPageId(int pageId);

    Canvas::Tool currentTool() const;

    void deleteItem(QGraphicsItem* item);

    QUndoStack* undoStack() const;
    void setUndoStack(QUndoStack* undoStack);

    ImageModel* getImageModel() const;
    void setImageModel(ImageModel* imageModel);

signals:
    void pixmapChanged();
    void dropFileOnCanvas(QUrl url);
    void itemDeleted(QGraphicsItem*);
    void pageIdChanged();

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

private:
    void adjustNewItem(CSItem* item);
    bool forwardEvent();

private:
    QGraphicsPixmapItem* m_bg;
    CSItem* m_currentItem;
    Tool m_currentTool;
    FieldModel* m_model;
    int m_pageId;
    QUndoStack* m_undoStack;
    QList<QGraphicsItem*> m_movingItems;
    QList<QPointF> m_oldPos;
    ImageModel* m_imageModel= nullptr;
};

#endif // CANVAS_H
