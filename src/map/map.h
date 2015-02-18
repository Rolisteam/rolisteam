/***************************************************************************
 *   Copyright (C) 2010 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MAP_H
#define MAP_H

#include <QGraphicsScene>
#include <QColor>
#include "ToolBar.h"
class VisualItem;
class Map : public QGraphicsScene
{
    Q_OBJECT
public:
    Map(QObject * parent = 0);
    Map(int width,int height,QString& title,QColor& m_bgColor,QObject * parent = 0);

    void setWidth(int width);
    void setHeight(int height);
    void setTitle(QString title);
    void setBackGroundColor(QColor& bgcolor);
    void setSceneRect();

    int mapWidth() const;
    int mapHeight() const;
    const QString& mapTitle() const;
    const QColor& mapColor() const;

public slots:
    void setCurrentTool(ToolsBar::SelectableTool selectedtool);
    void setCurrentChosenColor(QColor&);
    void setPenSize(int);
    void setNPCSize(int);

protected:
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );

    void addItem();
/*private slots:
    void editingFinished();*/


private:
    int m_width;
    int m_height;
    QString m_title;
    QColor m_bgColor;
    ToolsBar::SelectableTool m_selectedtool;
    QPointF m_first;
    QPointF m_end;
    VisualItem* m_currentItem;
    QColor m_itemColor;
    int m_penSize;
    int m_npcSize;
    QList<VisualItem*>* m_itemList;
};

#endif // MAP_H
