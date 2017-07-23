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

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include "field.h"
#include "fieldmodel.h"

class Canvas : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Tool {NONE,ADDINPUT,ADDTEXTFIELD,ADDTEXTAREA,ADDIMAGE,ADDFUNCBUTTON,ADDCHECKBOX,MOVE,DELETETOOL,BUTTON};
    explicit Canvas(QObject *parent = 0);

    void setCurrentTool(Canvas::Tool);
    FieldModel *model() const;
    void setModel(FieldModel *model);

    QPixmap* pixmap();

    void setPixmap(QPixmap* pix);
    int currentPage() const;
    void setCurrentPage(int currentPage);

   Canvas::Tool currentTool() const;

   void deleteItem(QGraphicsItem *item);
signals:
    void imageChanged();
    void itemDeleted(QGraphicsItem*);
protected:
    void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
    void dropEvent ( QGraphicsSceneDragDropEvent * event );
    void dragMoveEvent( QGraphicsSceneDragDropEvent * event );
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);


private:
    bool forwardEvent();
private:
    QGraphicsPixmapItem* m_bg;
    CSItem* m_currentItem;
    Tool m_currentTool;
    FieldModel* m_model;
    int m_currentPage;
    QPixmap* m_pix;
};

#endif // CANVAS_H
