/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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

#ifndef GRIDITEM_H
#define GRIDITEM_H

#include "characteritem.h"
#include "data/charactervision.h"
#include "visualitem.h"
#include <QConicalGradient>
#include <QGradient>
#include <QRadialGradient>

/**
 * @brief The SightItem class
 */
class GridItem : public VisualItem
{
    Q_OBJECT
public:
    /**
     * @brief GridItem
     * @param characterItemMap
     */
    GridItem();
    /**
     * @brief ~GridItem
     */
    virtual ~GridItem();
    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(QPointF& nend);
    /**
     * @brief writeData
     * @param out
     */
    virtual void writeData(QDataStream& out) const;
    /**
     * @brief readData
     * @param in
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
     * @brief boundingRect
     * @return
     */
    virtual QRectF boundingRect() const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    /**
     * @brief updateChildPosition
     */
    void updateChildPosition();
    /**
     * @brief createActions
     */
    void createActions();
    /**
     * @brief setVisible
     * @param visible
     */
    virtual void setVisible(bool visible);

    void computePattern();
    virtual void updateItemFlags();

private:
    bool m_isGM;
    /**
     * @brief m_computedPattern
     */
    QImage m_computedPattern;
};

#endif // GRIDITEM_H
