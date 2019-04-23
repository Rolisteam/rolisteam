/***************************************************************************
 *      Copyright (C) 2010 by Renaud Guezennec                             *
 *                                                                         *
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
#ifndef PATHITEM_H
#define PATHITEM_H

#include "visualitem.h"
#include <QPen>
/**
 * @brief map item to paint a path on the scene/map
 */
class PathItem : public VisualItem
{
    Q_OBJECT
public:
    PathItem();
    /**
     * @brief constructor with parameters
     * @param nend starting point
     * @param penColor color used to draw the path (path is not filled)
     * @param penSize, width of the pen
     * @param parent
     */
    PathItem(
        const QPointF& nend, const QColor& penColor, int penSize, bool penMode= false, QGraphicsItem* parent= nullptr);

    /**
     * @brief override function to paint itself.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);
    /**
     * @brief defines the  boundrect of path. Useful for mouse selection or collision detection
     */
    virtual QRectF boundingRect() const;
    /**
     * @brief adds new point at the end
     */
    virtual void setNewEnd(QPointF& nend);
    /**
     * @brief accessor to the shape of path, better definition than boudingRect
     */
    virtual QPainterPath shape() const;
    /**
     * @brief writing serialisation method
     */
    virtual void writeData(QDataStream& out) const;
    /**
     * @brief reading serialisation method
     */
    virtual void readData(QDataStream& in);
    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const;
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg);
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg);

    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId, QPointF& pos);
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem();
    /**
     * @brief getItemCopy
     * @return
     */
    virtual VisualItem* getItemCopy();
    /**
     * @brief setPath
     * @param points
     */
    void setPath(QVector<QPointF> points);
    /**
     * @brief setClosed
     */
    void setClosed(bool);
    /**
     * @brief setFilled
     */
    void setFilled(bool);
    /**
     * @brief setStartPoint
     */
    void setStartPoint(QPointF);
    /**
     * @brief release
     */
    void release();

    /**
     * @brief addActionContextMenu
     */
    virtual void addActionContextMenu(QMenu&);
    /**
     * @brief endOfGeometryChange
     */
    virtual void endOfGeometryChange();
    /**
     * @brief readMovePointMsg
     * @param msg
     */
    virtual void readMovePointMsg(NetworkMessageReader* msg);
protected slots:
    void sendPointPosition();
private slots:
    void closePath();
    void fillPath();

private:
    void initRealPoints();
    void createActions();

private:
    bool m_penMode;
    bool m_filled;
    /**
     * @brief path stored in QtClasse
     */
    QPainterPath m_path;
    QPointF m_start;
    QPointF m_end;

    QVector<QPointF> m_pointVector;
    QVector<QPointF> m_pointVectorBary;
    QAction* m_closeAct;
    QAction* m_fillAct;
    bool m_closed;
    QPointF m_changedPointPos;
    qreal m_changedPointId;
};

#endif // PATHITEM_H
