/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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

#ifndef ANCHORITEM_H
#define ANCHORITEM_H

#include "visualitem.h"

#include "vmap/vmap.h"

#include <QPen>

namespace vmap
{
class VisualItemController;
}
/**
 * @brief The AnchorItem class ephemeral item to display rule and measure the distance between two points.
 */
class AnchorItem : public QGraphicsObject
{
public:
    /**
     * @brief AnchorItem
     * @param p
     */
    AnchorItem();
    /**
     *
     */
    virtual ~AnchorItem();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);

    /**
     * @brief gives bounding rect of the line
     */
    virtual QRectF boundingRect() const;

    /**
     * @brief defines new position of the end line.
     */
    virtual void setNewEnd(const QPointF& nend);

    QPointF getStart() const;
    QPointF getEnd() const;

private:
    QPointF m_startPoint= QPointF(0, 0);
    QPointF m_endPoint= QPointF(0, 0);
    QPen m_pen;
};

#endif // ANCHORITEM_H
